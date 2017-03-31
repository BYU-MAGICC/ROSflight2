#include <stdbool.h>

#include "board.h"
#include "mavlink.h"
#include "mavlink_param.h"
#include "mixer.h"
#include "sensors.h"
#include "estimator.h"
#include "param.h"
#include "mode.h"
#include "rc.h"

#include "mavlink_stream.h"
#include "mavlink_util.h"
#include "mavlink_log.h"

// Declarations of local function definitions
static void mavlink_send_heartbeat(void);
static void mavlink_send_attitude(void);
static void mavlink_send_imu(void);
static void mavlink_send_rosflight_output_raw(void);
static void mavlink_send_rc_raw(void);
static void mavlink_send_diff_pressure(void);
static void mavlink_send_baro(void);
static void mavlink_send_sonar(void);
static void mavlink_send_mag(void);
static void mavlink_send_low_priority(void);

// typedefs
typedef struct
{
  uint32_t period_us;
  uint64_t next_time_us;
  void (*send_function)(void);
} mavlink_stream_t;

// local variable definitions
static mavlink_stream_t mavlink_streams[MAVLINK_STREAM_COUNT] =
{
  { .period_us = 0, .next_time_us = 0, .send_function = mavlink_send_heartbeat },

  { .period_us = 0,  .next_time_us = 0, .send_function = mavlink_send_attitude },
  { .period_us = 0,  .next_time_us = 0, .send_function = mavlink_send_imu },
  { .period_us = 0,  .next_time_us = 0, .send_function = mavlink_send_diff_pressure },
  { .period_us = 0,  .next_time_us = 0, .send_function = mavlink_send_baro },
  { .period_us = 0,  .next_time_us = 0, .send_function = mavlink_send_sonar },
  { .period_us = 0,  .next_time_us = 0, .send_function = mavlink_send_mag },
  { .period_us = 0,  .next_time_us = 0, .send_function = mavlink_send_rosflight_output_raw },
  { .period_us = 0,  .next_time_us = 0, .send_function = mavlink_send_rc_raw },

  { .period_us = 5000,   .next_time_us = 0, .send_function = mavlink_send_low_priority }
};

// local function definitions
static void mavlink_send_heartbeat(void)
{
  MAV_MODE armed_mode = 0;
  if (_armed_state & ARMED)
    armed_mode = MAV_MODE_MANUAL_ARMED;
  else
    armed_mode = MAV_MODE_MANUAL_DISARMED;

  MAV_STATE failsafe_state = 0;
  if (_armed_state & FAILSAFE)
    failsafe_state = MAV_STATE_CRITICAL;
  else
    failsafe_state = MAV_STATE_STANDBY;

  uint8_t control_mode = 0;
  if (get_param_int(PARAM_FIXED_WING))
  {
    control_mode = MODE_PASS_THROUGH;
  }
  else
  {
    if(rc_switch(get_param_int(PARAM_RC_ATT_CONTROL_TYPE_CHANNEL)))
      control_mode = MODE_ROLL_PITCH_YAWRATE_THROTTLE;
    else
      control_mode = MODE_ROLLRATE_PITCHRATE_YAWRATE_THROTTLE;
  }

  mavlink_msg_heartbeat_send(MAVLINK_COMM_0,
                             get_param_int(PARAM_FIXED_WING) ? MAV_TYPE_FIXED_WING : MAV_TYPE_QUADROTOR,
                             MAV_AUTOPILOT_GENERIC,
                             armed_mode,
                             control_mode,
                             failsafe_state);
}

static void mavlink_send_attitude(void)
{
  mavlink_msg_attitude_quaternion_send(MAVLINK_COMM_0,
                                        clock_millis(),
                                        _current_state.q.w,
                                        _current_state.q.x,
                                        _current_state.q.y,
                                        _current_state.q.z,
                                        _current_state.omega.x,
                                        _current_state.omega.y,
                                        _current_state.omega.z);
}

static void mavlink_send_imu(void)
{
  // If we haven't sent this IMU measurement yet, send it
  if (!_imu_sent)
  {
    mavlink_msg_small_imu_send(MAVLINK_COMM_0,
                               _imu_time,
                               _accel.x,
                               _accel.y,
                               _accel.z,
                               _gyro.x,
                               _gyro.y,
                               _gyro.z,
                               _imu_temperature);
  }
  else
  {
    // Otherwise, wait and signal that we still need to send IMU
    mavlink_streams[MAVLINK_STREAM_ID_IMU].next_time_us -= mavlink_streams[MAVLINK_STREAM_ID_IMU].period_us;
  }

}

static void mavlink_send_rosflight_output_raw(void)
{
  mavlink_msg_rosflight_output_raw_send(MAVLINK_COMM_0,
                                        clock_millis(),
                                        _outputs);
}

static void mavlink_send_rc_raw(void)
{
  mavlink_msg_rc_channels_send(MAVLINK_COMM_0,
                               clock_millis(),
                               0,
                               pwm_read(0),
                               pwm_read(1),
                               pwm_read(2),
                               pwm_read(3),
                               pwm_read(4),
                               pwm_read(5),
                               pwm_read(6),
                               pwm_read(7),
                               0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 0);
}

static void mavlink_send_diff_pressure(void)
{
  if (diff_pressure_present())
  {
    mavlink_msg_diff_pressure_send(MAVLINK_COMM_0, _diff_pressure_velocity, _diff_pressure, _diff_pressure_temp);
  }
}

static void mavlink_send_baro(void)
{
  if (baro_present())
  {
    mavlink_msg_small_baro_send(MAVLINK_COMM_0, _baro_altitude, _baro_pressure, _baro_temperature);
  }
}

static void mavlink_send_sonar(void)
{
  if (sonar_present())
  {
    mavlink_msg_small_sonar_send(MAVLINK_COMM_0,
                                 _sonar_range,
                                 8.0,
                                 0.25);
  }
}

static void mavlink_send_mag(void)
{
  if (mag_present())
  {
    mavlink_msg_small_mag_send(MAVLINK_COMM_0,
                               _mag.x,
                               _mag.y,
                               _mag.z);
  }
}

static void mavlink_send_low_priority(void)
{
  mavlink_send_next_param();
}

// function definitions
void mavlink_stream(uint64_t time_us)
{
  for (int i = 0; i < MAVLINK_STREAM_COUNT; i++)
  {
    if (time_us >= mavlink_streams[i].next_time_us)
    {
      mavlink_streams[i].next_time_us += mavlink_streams[i].period_us;
      mavlink_streams[i].send_function();
    }
  }
}

void mavlink_stream_set_rate(mavlink_stream_id_t stream_id, uint32_t rate)
{
  mavlink_streams[stream_id].period_us = (rate == 0 ? 0 : 1000000/rate);
}

void mavlink_stream_set_period(mavlink_stream_id_t stream_id, uint32_t period_us)
{
  mavlink_streams[stream_id].period_us = period_us;
}
