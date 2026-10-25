#pragma once
#include <Arduino.h>
// External 2k2 pull-ups; open-drain emulation. Bounded clock stretching,
// no Wire library infinite waits. Single master, <=100 kHz.
class Bus {
 uint32_t started=0;
 void low(unsigned p){digitalWrite(p,LOW);pinMode(p,OUTPUT);}
 void release(unsigned p){pinMode(p,INPUT);}
 bool clock(){release(D5);uint32_t t=micros();while(!digitalRead(D5)){
   if(uint32_t(micros()-t)>1000||uint32_t(micros()-started)>20000)return false;
  }delayMicroseconds(3);return uint32_t(micros()-started)<20000;}
 bool start(){started=micros();release(D4);if(!clock()||!digitalRead(D4))return false;low(D4);delayMicroseconds(3);low(D5);return true;}
 void stop(){low(D5);low(D4);delayMicroseconds(3);clock();release(D4);delayMicroseconds(3);}
 bool send(uint8_t v){for(int n=7;n>=0;n--){low(D5);if(v&(1<<n))release(D4);else low(D4);delayMicroseconds(3);if(!clock())return false;}
  low(D5);release(D4);delayMicroseconds(3);if(!clock())return false;bool ack=!digitalRead(D4);low(D5);return ack;}
 bool receive(uint8_t& v,bool last){v=0;release(D4);for(int n=0;n<8;n++){
  low(D5);delayMicroseconds(3);if(!clock())return false;v=(v<<1)|digitalRead(D4);}
  low(D5);if(last)release(D4);else low(D4);delayMicroseconds(3);if(!clock())return false;low(D5);release(D4);return true;}
public:
 void begin(){release(D4);release(D5);}
 bool write(uint8_t addr,const uint8_t* p,unsigned n){bool ok=start()&&send(addr<<1);for(unsigned k=0;ok&&k<n;k++)ok=send(p[k]);stop();return ok;}
 bool read(uint8_t addr,uint8_t* p,unsigned n){bool ok=start()&&send((addr<<1)|1);for(unsigned k=0;ok&&k<n;k++)ok=receive(p[k],k+1==n);stop();return ok;}
 bool reg8(uint8_t addr,uint8_t reg,uint8_t value){uint8_t p[]={reg,value};return write(addr,p,2);}
 bool reg16(uint8_t addr,uint8_t reg,uint16_t value){uint8_t p[]={reg,uint8_t(value>>8),uint8_t(value)};return write(addr,p,3);}
 bool read8(uint8_t addr,uint8_t reg,uint8_t& value){return write(addr,&reg,1)&&read(addr,&value,1);}
 bool read16(uint8_t addr,uint8_t reg,uint16_t& value){uint8_t p[2];if(!write(addr,&reg,1)||!read(addr,p,2))return false;value=(uint16_t(p[0])<<8)|p[1];return true;}
};

