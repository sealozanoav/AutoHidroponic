/*
 * Copyright 2016 <Admobilize>
 * MATRIX Labs  [http://creator.matrix.one]
 * This file is part of MATRIX Creator firmware for MCU
 * Author: Andrés Calderón [andres.calderon@admobilize.com]
 *
 * MATRIX Creator firmware for MCU is free software: you can redistribute
 * it and/or modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.

 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "board.h"
#include "ch.h"
#include "hal.h"
#include "wdt.h"
#include "soc-hw.h"

#include <math.h>
#include <mcuconf.h>
#include <string.h>

#include "./hts221.h"
#include "./i2c.h"
//#include "./lsm9ds1.h"
//#include "./mpl3115a2.h"
#include "./sensors_data.h"
#include "./veml6070.h"
#include "./rtc.h"
extern "C" {
#include "atmel_psram.h"
//#include "atmel_adc.h"
}

const uint32_t kFirmwareCreatorID = 0x10;
const uint32_t kFirmwareVersion = 0x180403; /* 0xYYMMDD */

/* Global objects */
creator::I2C i2c;  // TODO(andres.calderon@admobilize.com): avoid global objects

void psram_copy(uint8_t mem_offset, char *data, uint8_t len) {
  register char *psram = (char *)PSRAM_BASE_ADDRESS;

  for (int i = 0; i < len; i++) {
    psram[mem_offset + i] = data[i];
  }
}

void psram_read(uint8_t mem_offset, char *data, uint8_t len) {
  register char *psram = (char *)PSRAM_BASE_ADDRESS;

  for (int i = 0; i < len; i++) {
    data[i] = psram[mem_offset + i];
  }
}

static WORKING_AREA(waEnvThread, 256);
static msg_t EnvThread(void *arg) {
  (void)arg;

  //creator::MPL3115A2 mpl3115a2(&i2c);
  creator::HTS221 hts221(&i2c);
  creator::VEML6070 veml6070(&i2c);

  //mpl3115a2.Begin();
  hts221.Begin();
  veml6070.Begin();

  EnvData env;
  MCUData mcu_info;

  mcu_info.ID = kFirmwareCreatorID;
  mcu_info.version = kFirmwareVersion;

  while (true) {
    palSetPad(IOPORT3, 17);
    chThdSleepMilliseconds(1);
//     i2c.WriteByte(0x03,0x00);
     palClearPad(IOPORT3, 17);
 
    hts221.GetData(env.humidity, env.temperature_hts);
 
    //env.altitude = mpl3115a2.GetAltitude();
    //env.pressure = mpl3115a2.GetPressure();
    //env.temperature_mpl = mpl3115a2.GetTemperature();
 
    env.UV = veml6070.GetUV();
  
    psram_copy(mem_offset_mcu, (char *)&mcu_info, sizeof(mcu_info));
    psram_copy(mem_offset_env, (char *)&env, sizeof(env));
  }
  return (0);
}

static 	WORKING_AREA(waRTCThread,256);
static msg_t rtcThread(void *arg){
 (void)arg;
 bool on = false;
creator::RTC1 rtc(&i2c);
 rtc.setTime(10,1,0);
//palClearPad(IOPORT3, 7);
   rtc.createAlarm(10,2);
while(true) {
if(!on){
 on=rtc.checkAlarms();
}
if(on){
//palSetPad(IOPORT3, 7);
}

 }
return (0);
}









//static WORKING_AREA(waIMUThread, 512);
//static msg_t IMUThread(void *arg) {
//  (void)arg;

  //LSM9DS1 imu(&i2c, IMU_MODE_I2C, 0x6A, 0x1C);
  //imu.begin();
  //IMUData data;

  // Getting lasts offsets saved in the imu sensor
  //float current_offset_x = imu.calcMag(imu.getOffset(X_AXIS));
  //float current_offset_y = imu.calcMag(imu.getOffset(Y_AXIS));
  //float current_offset_z = imu.calcMag(imu.getOffset(Z_AXIS));

  //while (true) {
    // Getting new samples from gyro/mag/accel sensors
   // imu.readGyro();
   // data.gyro_x = imu.calcGyro(imu.gx);
   // data.gyro_y = imu.calcGyro(imu.gy);
   // data.gyro_z = imu.calcGyro(imu.gz);

   // imu.readMag();
   // data.mag_x = imu.calcMag(imu.mx);
   // data.mag_y = imu.calcMag(imu.my);
   // data.mag_z = imu.calcMag(imu.mz);

    //imu.readAccel();
    //data.accel_x = imu.calcAccel(imu.ax);
    //data.accel_y = imu.calcAccel(imu.ay);
    //data.accel_z = imu.calcAccel(imu.az);

    //data.yaw = atan2(data.mag_y, -data.mag_x) * 180.0 / M_PI;
    //data.roll = atan2(data.accel_y, data.accel_z) * 180.0 / M_PI;
    //data.pitch = atan2(-data.accel_x, sqrt(data.accel_y * data.accel_y +
    //                                     data.accel_z * data.accel_z)) *
    //          180.0 / M_PI;

    // Saving data to FPGA
    //psram_copy(mem_offset_imu, (char *)&data, sizeof(data));

    //chThdSleepMilliseconds(20);

    //WDT_Restart(WDT);
  //}
  //return (0);
  //}

/*
 * Application entry point.
 */
int main(void) {

  halInit();

  chSysInit();

  /* Configure EBI I/O for psram connection*/
  PIO_Configure(pinPsram, PIO_LISTSIZE(pinPsram));

  /* complete SMC configuration between PSRAM and SMC waveforms.*/
  BOARD_ConfigurePSRAM(SMC);

  i2c.Init();
/*while(){
  i2c.WriteByte(0x03,0x00);
    chThdSleepMilliseconds(1);

}
*/
  /* Creates the imu thread. */
  //chThdCreateStatic(waIMUThread, sizeof(waIMUThread), NORMALPRIO, IMUThread,
    //                NULL);

  /* Creates the hum thread. */
  chThdCreateStatic(waEnvThread, sizeof(waEnvThread), NORMALPRIO, EnvThread,
                    NULL);
   chThdCreateStatic(waRTCThread, sizeof(waRTCThread), NORMALPRIO, rtcThread, NULL);
  return (0);

write_data1(0x01);



}
