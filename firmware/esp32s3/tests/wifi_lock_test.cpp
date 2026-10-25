#define ITO_HOST_TEST 1
#define ITO_TEST_WIFI_ENABLED 1
#include "../ito_prototype_esp32s3/controller.h"
#include <assert.h>
int main(){
 setup();watchdogOK=true;ready=true;initialized=true;clockMs=5000;lastSensorComplete=clockMs;offTime=0;
 inputs=safety::Inputs{};inputs.sensorsFresh=inputs.fanOk=inputs.usb=inputs.button=true;inputs.busV=24;
 for(auto& t:inputs.temperature)t=25;
 for(auto& a:air){a.valid=true;a.stamp=clockMs;}
 assert(!configurationOK());
 requestPulse(false,1,100,500);requestPulse(true,20,0,500);
 assert(mode==safety::Mode::Idle&&gpio[D3]==0&&gpio[D6]==0&&gpio[D9]==0);
 puts("PASS: Wi-Fi mode refuses all power pulses even with calibrated healthy fixture");
}
