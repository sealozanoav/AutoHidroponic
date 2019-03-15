/**
    Purpose: Sets the main logic of IEDD project

    @author Alexis Cuero Losada
    @version 1.1 18/01/10
*/

#define _GLIBCXX_USE_CXX11_ABI 0

#include "board.h"
#include "ch.h"
#include "chprintf.h"
#include "hal.h"
#include "pio.h"
#include "pmc.h"
#include "rtc.h"
#include "psram.h"
#include "i2c.h"


I2C i2c;


static WORKING_AREA(waRTCThread, 256);
static msg_t rtcThread(void *arg) {
  (void)arg;  
  bool on = false;
  RTC1 rtc(&i2c);
  rtc.setTime(10,1,0);
  palClearPad(IOPORT3, BOARD_LED);
	rtc.createAlarm(10,2);
  while (true) {
    if(!on){
      on = rtc.checkAlarms();
    }
    if(on) {
      palSetPad(IOPORT3, BOARD_LED);
    }
  }
  return (0);
}


/*
* Application entry point.
*/
int main(void) {
  // Initialize ChibiOS HAL
  halInit();
  chSysInit();

  //Start Serial.
  sdStart(&SD2, NULL);
  i2c.Init();

  /* Complete SMC configuration between PSRAM and SMC waveforms.*/
  BOARD_ConfigurePSRAM(SMC) ;

    /* Creates the RTC thread. */
  chThdCreateStatic(waRTCThread, sizeof(waRTCThread), NORMALPRIO, rtcThread,NULL);

/*
  while (TRUE) {
    chprintf((BaseChannel *) &SD2, "Hello World.\r\n");
    palClearPad(IOPORT3, BOARD_LED);
    chThdSleepMilliseconds(500);
    palSetPad(IOPORT3, BOARD_LED);
    chThdSleepMilliseconds(500);
  }
*/
  return(0);
}
