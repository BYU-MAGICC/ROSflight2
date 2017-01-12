#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include <breezystm32/breezystm32.h>
#include <turbotrig/turbovec.h>

#include "estimator.h"
#include "mavlink.h"
#include "mavlink_param.h"
#include "mavlink_receive.h"
#include "mavlink_stream.h"
#include "mavlink_util.h"
#include "mode.h"
#include "param.h"
#include "sensors.h"
#include "controller.h"
#include "mixer.h"
#include "rc.h"

extern void SetSysClock(bool overclock);

serialPort_t *Serial1;

int main(void)
{
  // Configure clock, this figures out HSE for hardware autodetect
  SetSysClock(0);
  systemInit();

  // Perform Setup Operations
  // Make sure all the perhipherals are done booting up before starting
  delay(500);

  // Read EEPROM to get initial params
  init_params();

  /***********************/
  /***  Hardware Setup ***/
  /***********************/

  //  // Initialize I2c
  i2cInit(I2CDEV_2);

  // Initialize PWM and RC
  init_PWM();
  init_rc();

  // Initialize MAVlink Communication
  init_mavlink();

  // Initialize Sensors
  init_sensors();


  /***********************/
  /***  Software Setup ***/
  /***********************/

  // Initialize Motor Mixing
  init_mixing();

  // Initizlie Controller
  init_controller();

  // Initialize Estimator
  // mat_exp <- greater accuracy, but adds ~90 us
  // quadratic_integration <- some additional accuracy, adds ~20 us
  // accelerometer correction <- if using angle mode, this is required, adds ~70 us
  init_estimator(false, false, true);
  init_mode();

  // Initialize Serial ports
  Serial1 = uartOpen(USART1, NULL, get_param_int(PARAM_BAUD_RATE), MODE_RXTX);

  while (1)
  {
    // Main loop
    loop();
  }
}


void setup(void)
{

}


void loop(void)
{
  /*********************/
  /***  Pre-Process ***/
  /*********************/


  /*********************/
  /***  Control Loop ***/
  /*********************/
  if (update_sensors()) // 595 | 591 | 590 us
  {
    // If I have new IMU data, then perform control
    run_estimator(micros()); //  212 | 195 us (acc and gyro only, not exp propagation no quadratic integration)
    run_controller(); // 278 | 271
    mix_output(); // 16 | 13 us
  }


  /*********************/
  /***  Post-Process ***/
  /*********************/
  // internal timers figure out what and when to send
  mavlink_stream(micros()); // 165 | 27 | 2

  // receive mavlink messages
  mavlink_receive(); // 159 | 1 | 1

  // update the armed_states, an internal timer runs this at a fixed rate
  check_mode(micros()); // 108 | 1 | 1

  // get RC, an internal timer runs this every 20 ms (50 Hz)
  receive_rc(micros()); // 42 | 2 | 1

  // update commands (internal logic tells whether or not we should do anything or not)
  mux_inputs(); // 6 | 1 | 1
}
