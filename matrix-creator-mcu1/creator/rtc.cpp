#include "rtc.h"
#include "./i2c.h"
#include "ch.h"

namespace creator{

uint32_t numAlarms = 1;

RTC1::RTC1(I2C* i2c): i2c_(i2c) {
	reset();
}

//Convierte de decimal a BCD
char RTC1::decimalToBCD(char num) {
	return ((num/10)*16)+(num%10);
	}

//Asigna el registro hora al RTC
void RTC1::rtcSetHour(char hour){
    write(HOURS, decimalToBCD(hour));
	}


//Obtiene la hora del RTC
char RTC1::rtcGetHour() {
	return read(HOURS);
	}


void RTC1::rtcSetMinutes(char minutes){
	write(MINUTES,decimalToBCD(minutes));
	}

char RTC1::rtcGetMinutes() {
	return read(MINUTES);
	}

void RTC1::rtcSetSeconds(char seconds){
	write(SECONDS,decimalToBCD(seconds));
	}

uint32_t RTC1::bcdToDec(uint32_t bcdByte) {
	return (((bcdByte & 0xf0) >> 4)* 10) + (bcdByte & 0x0f);
	}


uint32_t RTC1::rtcGetSeconds() {
	return bcdToDec(read(SECONDS));
	}


uint8_t RTC1::bcd2bin (uint8_t val) { 
	return val - 6 * (val >> 4); 
	}
 
uint8_t RTC1::bin2bcd (uint8_t val) { 
	return val + 6 * (val / 10); 
	}

void RTC1::setTime(char hour, char minutes, char seconds) {
	rtcSetHour(hour);
	rtcSetMinutes(minutes);
	rtcSetSeconds(seconds);	
	}

void RTC1::reset() {
	char i;
	for(i = STARTMEM; i <= ENDMEM; i++) {
		write(i, decimalToBCD(0));
		}
}

void RTC1::createAlarm(char hour, char minutes) {

	write(STARTMEM * numAlarms, decimalToBCD(hour));
	write((STARTMEM * numAlarms) + 1, decimalToBCD(minutes));
	numAlarms += 1;
	}

char RTC1::readMem(char dirReg) {
	return read(dirReg);
	}

bool RTC1::checkAlarms() {
	char i;
	char hour = rtcGetHour();
	char minutes = rtcGetMinutes();
	for(i = STARTMEM; i <= ENDMEM; i+=2) {
		if(readMem(i) == hour) {
			if(readMem(i+1) == minutes) {
				return TRUE;
				}
			}
		}
		return FALSE;
	}

//a : registro
//d: dato
uint8_t RTC1::read(uint8_t a) { return i2c_->ReadByte(RTC_ADD, a); }

void RTC1::write(uint8_t a, uint8_t d) { i2c_->WriteByte(RTC_ADD, a, d); }

uint8_t RTC1::read(uint8_t a, uint8_t* data, uint8_t size) {
  return i2c_->ReadBytes(RTC_ADD, a, data, size);
}
}
