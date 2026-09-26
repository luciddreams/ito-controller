#pragma once
#include <stdint.h>
#include <stddef.h>
#include <string>
#include <string.h>
#define ARDUINO_Seeed_XIAO_nRF52840 1
#define LOW 0
#define HIGH 1
#define INPUT 0
#define OUTPUT 1
#define FALLING 2
#define AR_INTERNAL 0
constexpr unsigned D0=0,D1=1,D2=2,D3=3,D4=4,D5=5,D6=6,D7=7,D8=8,D9=9,D10=10,A0=0,A1=1;
static uint32_t clockMs=5000;
static int gpio[11]={};
inline uint32_t millis(){return clockMs;}
inline uint32_t micros(){return clockMs*1000;}
inline void delay(unsigned n){clockMs+=n;}
inline void delayMicroseconds(unsigned){}
inline void digitalWrite(unsigned p,int v){gpio[p]=v;}
inline int digitalRead(unsigned p){return gpio[p];}
inline void pinMode(unsigned,int){}
inline void noInterrupts(){}
inline void interrupts(){}
inline unsigned digitalPinToInterrupt(unsigned p){return p;}
inline void attachInterrupt(unsigned,void(*)(),int){}
inline void analogReference(unsigned){}
inline void analogReadResolution(unsigned){}
inline unsigned analogRead(unsigned p){return p==A0?1877:0;}
struct SerialMock{
 bool connected=true;std::string output;
 operator bool()const{return connected;}
 void begin(unsigned){}
 int availableForWrite(){return 1024;}
 size_t write(const uint8_t* p,size_t n){output.append((const char*)p,n);return n;}
 int available(){return 0;}int read(){return -1;}
};
static SerialMock Serial;
struct WdtMock{uint32_t CONFIG=0,CRV=0,RREN=0,TASKS_START=0,RR[1]={};};
static WdtMock wdt;
#define NRF_WDT (&wdt)
