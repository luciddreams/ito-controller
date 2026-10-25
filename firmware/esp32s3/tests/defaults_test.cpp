#define ITO_HOST_TEST 1
#define ITO_TEST_DEFAULT_CONFIG 1
#include "../ito_prototype_esp32s3/controller.h"
#include <assert.h>
int main(){
 setup();assert(watchdogOK);assert(initialized);assert(!ready);
 assert(!cfg::ALLOW_HEATER && cfg::INSTALLED_TEC_MASK==1);
 for(unsigned c:cfg::TEC_MAX_CODE)assert(c==0);
 assert(gpio[D3]==0&&gpio[D6]==0&&gpio[D9]==0);
 // Even a healthy test fixture cannot enable power with delivered settings.
 inputs.sensorsFresh=inputs.fanOk=inputs.button=inputs.usb=true;
 for(auto& t:inputs.temperature)t=25;
 lastSensorComplete=clockMs;ready=true;offTime=0;
 for(auto& a:air){a.valid=true;a.stamp=clockMs;}
 requestPulse(false,1,100,500);requestPulse(true,20,0,500);
 assert(mode==safety::Mode::Idle&&gpio[D3]==0&&gpio[D6]==0&&gpio[D9]==0);
 assert(isnan(platform::correctedVoltage(4095,3000,1,0)));
 assert(isnan(platform::correctedVoltage(3500,3100,1,0)));
 assert(fabsf(platform::correctedVoltage(2000,1650,1.01f,-0.01f)-1.6565f)<0.0001f);
 assert(!platform::calibrationValid(NAN,0));assert(!platform::calibrationValid(1,1));
 Serial.capacity=64;status();for(int i=0;i<16;i++)serialFlush();
 assert(Serial.output.find("FW=L.1-S3")!=std::string::npos);
 mockWatchdogOK=false;setup();assert(mode==safety::Mode::Fault);
 assert(gpio[D3]==0&&gpio[D6]==0&&gpio[D9]==0);
 puts("PASS: delivered config blocks power, startup off, ADC clipping/calibration, USB queue, watchdog failure");
}
