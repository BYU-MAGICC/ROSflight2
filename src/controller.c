#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#include <turbotrig/turbotrig.h>

#include "param.h"
#include "mixer.h"
#include "mux.h"
#include "estimator.h"
#include "sensors.h"
#include "mode.h"

#include "controller.h"

#include "mavlink_log.h"
#include "mavlink_util.h"

typedef struct
{
  param_id_t kp_param_id;
  param_id_t ki_param_id;
  param_id_t kd_param_id;

  float *current_x;
  float *current_xdot;
  float *commanded_x;
  float *output;

  float max;
  float min;

  float integrator;
  float prev_time;
  float prev_x;
  float differentiator;
  float tau;
} pid_t;

static pid_t pid_roll;
static pid_t pid_roll_rate;
static pid_t pid_pitch;
static pid_t pid_pitch_rate;
static pid_t pid_yaw_rate;

static void init_pid(pid_t *pid, param_id_t kp_param_id, param_id_t ki_param_id, param_id_t kd_param_id, float *current_x,
              float *current_xdot, float *commanded_x, float *output, float max, float min)
{
  pid->kp_param_id = kp_param_id;
  pid->ki_param_id = ki_param_id;
  pid->kd_param_id = kd_param_id;
  pid->current_x = current_x;
  pid->current_xdot = current_xdot;
  pid->commanded_x = commanded_x;
  pid->output = output;
  pid->max = max;
  pid->min = min;
  pid->integrator = 0.0;
  pid->prev_time = clock_micros()*1e-6;
  pid->differentiator = 0.0;
  pid->prev_x = 0.0;
  pid->tau = get_param_float(PARAM_PID_TAU);
}


static void run_pid(pid_t *pid, float dt)
{
  if (dt > 0.010 || !(_armed_state & ARMED))
  {
    // This means that this is a ''stale'' controller and needs to be reset.
    // This would happen if we have been operating in a different mode for a while
    // and will result in some enormous integrator, and weird behavior on the derivative term.
    // Obiously, it could also mean we are disarmed and shouldn't integrate
    // Setting dt to zero for this loop will mean that the integrator and dirty derivative
    // doesn't do anything this time but will keep it from exploding.
    dt = 0.0;
    pid->differentiator = 0.0;
  }

  // Calculate Error (make sure to de-reference pointers)
  float error = (*pid->commanded_x) - (*pid->current_x);

  // Initialize Terms
  float p_term = error * get_param_float(pid->kp_param_id);
  float i_term = 0.0;
  float d_term = 0.0;

  // If there is a derivative term
  if (pid->kd_param_id < PARAMS_COUNT)
  {
    // calculate D term (use dirty derivative if we don't have access to a measurement of the derivative)
    // The dirty derivative is a sort of low-pass filtered version of the derivative.
    // (Be sure to de-reference pointers)
    if (pid->current_xdot == NULL)
    {
      if (dt > 0.0f)
      {
        pid->differentiator = (2.0f*pid->tau-dt)/(2.0f*pid->tau+dt)*pid->differentiator + 2.0f/(2.0f*pid->tau+dt)*((
                                                                                                                     *pid->current_x) - pid->prev_x);
        pid->prev_x = *pid->current_x;
        d_term = get_param_float(pid->kd_param_id)*pid->differentiator;
      }
      else
      {
        d_term = 0;
      }
    }
    else
    {
      d_term = get_param_float(pid->kd_param_id) * (*pid->current_xdot);
    }
  }
  else
  {
    d_term = get_param_float(pid->kd_param_id) * (*pid->current_xdot);
  }

  // If there is an integrator, we are armed, and throttle is high
  /// TODO: better way to figure out if throttle is high
  if ( (pid->ki_param_id < PARAMS_COUNT) && (_armed_state == ARMED) && (pwm_read(get_param_int(PARAM_RC_F_CHANNEL) > 1200)))
  {
    if (get_param_float(pid->ki_param_id) > 0.0)
    {
      // integrate
      pid->integrator += error*dt;
      // calculate I term (be sure to de-reference pointer to gain)
      i_term = get_param_float(pid->ki_param_id) * pid->integrator;
    }
  }

  // sum three terms
  float u = p_term + i_term - d_term;

  // Integrator anti-windup
  float u_sat = (u > pid->max) ? pid->max : (u < pid->min) ? pid->min : u;
  if (u != u_sat && fabs(i_term) > fabs(u - p_term + d_term))
    pid->integrator = (u_sat - p_term + d_term)/get_param_float(pid->ki_param_id);

  // Set output
  (*pid->output) = u_sat;

  return;
}


void init_controller()
{
  init_pid(&pid_roll,
           PARAM_PID_ROLL_ANGLE_P,
           PARAM_PID_ROLL_ANGLE_I,
           PARAM_PID_ROLL_ANGLE_D,
           &_current_state.roll,
           &_current_state.omega.x,
           &_combined_control.x.value,
           &_command.x,
           get_param_int(PARAM_MAX_COMMAND)/2.0f,
           -1.0f*get_param_int(PARAM_MAX_COMMAND)/2.0f);

  init_pid(&pid_pitch,
           PARAM_PID_PITCH_ANGLE_P,
           PARAM_PID_PITCH_ANGLE_I,
           PARAM_PID_PITCH_ANGLE_D,
           &_current_state.pitch,
           &_current_state.omega.y,
           &_combined_control.y.value,
           &_command.y,
           get_param_int(PARAM_MAX_COMMAND)/2.0f,
           -1.0f*get_param_int(PARAM_MAX_COMMAND)/2.0f);

  init_pid(&pid_roll_rate,
           PARAM_PID_ROLL_RATE_P,
           PARAM_PID_ROLL_RATE_I,
           PARAM_PID_ROLL_RATE_D,
           &_current_state.omega.x,
           NULL,
           &_combined_control.x.value,
           &_command.x,
           get_param_int(PARAM_MAX_COMMAND)/2.0f,
           -1.0f*get_param_int(PARAM_MAX_COMMAND)/2.0f);

  init_pid(&pid_pitch_rate,
           PARAM_PID_PITCH_RATE_P,
           PARAM_PID_PITCH_RATE_I,
           PARAM_PID_PITCH_RATE_D,
           &_current_state.omega.y,
           NULL,
           &_combined_control.y.value,
           &_command.y,
           get_param_int(PARAM_MAX_COMMAND)/2.0f,
           -1.0f*get_param_int(PARAM_MAX_COMMAND)/2.0f);

  init_pid(&pid_yaw_rate,
           PARAM_PID_YAW_RATE_P,
           PARAM_PID_YAW_RATE_I,
           PARAM_PID_YAW_RATE_D,
           &_current_state.omega.z,
           NULL,
           &_combined_control.z.value,
           &_command.z,
           get_param_int(PARAM_MAX_COMMAND)/2.0f,
           -1.0f*get_param_int(PARAM_MAX_COMMAND)/2.0f);
}


void run_controller()
{
  // Time calculation
  static float prev_time = 0.0f;

  if (prev_time < 0.0000001)
  {
    prev_time = _current_state.now_us * 1e-6;
    return;
  }

  float now = _current_state.now_us * 1e-6;
  float dt = now - prev_time;
  prev_time = now;

  // ROLL
  if (_combined_control.x.type == RATE)
    run_pid(&pid_roll_rate, dt);
  else if (_combined_control.x.type == ANGLE)
    run_pid(&pid_roll, dt);
  else // PASSTHROUGH
    _command.x = _combined_control.x.value;

  // PITCH
  if (_combined_control.y.type == RATE)
    run_pid(&pid_pitch_rate, dt);
  else if (_combined_control.y.type == ANGLE)
    run_pid(&pid_pitch, dt);
  else // PASSTHROUGH
    _command.y = _combined_control.y.value;

  // YAW
  if (_combined_control.z.type == RATE)
    run_pid(&pid_yaw_rate, dt);
  else// PASSTHROUGH
    _command.z = _combined_control.z.value;


  // Add feedforward torques
  _command.x += get_param_float(PARAM_X_EQ_TORQUE);
  _command.y += get_param_float(PARAM_Y_EQ_TORQUE);
  _command.z += get_param_float(PARAM_Z_EQ_TORQUE);


  _command.F = _combined_control.F.value;

  static uint32_t counter = 0;
  if (counter > 100)
  {
    mavlink_send_named_command_struct("RC", _rc_control);
    mavlink_send_named_command_struct("offboard", _offboard_control);
    _combined_control.x.value += get_param_float(PARAM_X_EQ_TORQUE);
    _combined_control.y.value += get_param_float(PARAM_Y_EQ_TORQUE);
    _combined_control.z.value += get_param_float(PARAM_Z_EQ_TORQUE);
    mavlink_send_named_command_struct("combined", _combined_control);
    counter = 0;
  }
  counter++;
}

void calculate_equilbrium_torque_from_rc()
{
  // Make sure we are disarmed
  if (!(_armed_state & ARMED))
  {
    // Tell the user that we are doing a equilibrium torque calibration
    mavlink_log_warning("Capturing equilbrium offsets from RC");

    // pass the rc_control through the controller
    _combined_control.x = _rc_control.x;
    _combined_control.y = _rc_control.y;
    _combined_control.z = _rc_control.z;

    run_controller();

    // the output from the controller is going to be the static offsets
    set_param_float(PARAM_X_EQ_TORQUE, _command.x);
    set_param_float(PARAM_Y_EQ_TORQUE, _command.y);
    set_param_float(PARAM_Z_EQ_TORQUE, _command.z);

    mavlink_log_warning("Equilibrium torques found and applied.");
    mavlink_log_warning("Please zero out trims on your transmitter");
  }
  else
  {
    mavlink_log_warning("Cannot perform equilbirum offset calibration while armed");
  }
}
