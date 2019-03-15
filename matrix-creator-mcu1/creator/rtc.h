#ifndef RTC_EMB_
#define RTC_EMB_

#include <string.h>
#include "atmel_twid.h"
#include "./i2c.h"


#define RTC_ADD 0x68
#define SECONDS  0x00
#define MINUTES 0X01
#define HOURS 0x02
#define DAY 0x03
#define DATE 0x04
#define MONTH 0x05
#define YEAR 0x06
#define STARTMEM 0x08
#define ENDMEM 0x3F
#define INCREMENT 0x02

namespace creator{
class RTC1{
    public:

    RTC1(I2C* i2c);

    //Asigna el registro hora al RTC
    void rtcSetHour(char hour);
    
    //Obtiene la hora del RTC
    char rtcGetHour();
    
    void rtcSetMinutes(char minutes);
    
    char rtcGetMinutes();
    
    void rtcSetSeconds(char seconds);
    
    uint32_t bcdToDec(uint32_t bcdByte);
    
    uint32_t rtcGetSeconds();
    
    uint8_t bcd2bin (uint8_t val);
    
    uint8_t bin2bcd (uint8_t val);
    
    void setTime(char hour, char minutes, char seconds);
    
    void createAlarm(char hour, char minutes);
    
    char readMem(char dirReg);
    
    bool checkAlarms();
    
    char decimalToBCD(char num);

    void reset();    
    
    private:
    I2C* i2c_;
    void write(uint8_t a, uint8_t d);
    uint8_t read(uint8_t a);
    uint8_t read(uint8_t a, uint8_t* data, uint8_t size);
    

};
}
#endif 
