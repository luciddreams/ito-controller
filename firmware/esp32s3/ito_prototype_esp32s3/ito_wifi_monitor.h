#pragma once
#include "telemetry.h"
#if defined(ITO_TAILSCALE_MONITOR) && !defined(ITO_HOST_TEST)
#include "ito_tailnet_monitor.h"
#elif !defined(ITO_HOST_TEST) && __has_include("wifi_local.h")
#include "wifi_local.h"
#include <WiFi.h>
#include <WebServer.h>
namespace monitor {
constexpr bool enabled=true;
static_assert(sizeof(ITO_WIFI_TOKEN)>32,"Use a telemetry token of at least 32 characters");
static portMUX_TYPE mux=portMUX_INITIALIZER_UNLOCKED;
static telemetry::Snapshot latest;
static uint32_t published=0;
static bool hasSnapshot=false;
static char connection[100]="wifi=starting\n";
static WebServer server(80);
inline void publish(const telemetry::Snapshot& s){
 portENTER_CRITICAL(&mux);latest=s;published=millis();hasSnapshot=true;portEXIT_CRITICAL(&mux);
}
inline void status(char* out,size_t n){portENTER_CRITICAL(&mux);snprintf(out,n,"%s",connection);portEXIT_CRITICAL(&mux);}
inline void task(void*){
 WiFi.persistent(false);WiFi.mode(WIFI_STA);WiFi.setHostname("ito-controller");
 WiFi.setAutoReconnect(true);WiFi.begin(ITO_WIFI_SSID,ITO_WIFI_PASSWORD);
 const char* headers[]={"X-ITO-Key"};server.collectHeaders(headers,1);
 server.on("/api/status",HTTP_GET,[]{
  server.sendHeader("Cache-Control","no-store");
  if(server.header("X-ITO-Key")!=ITO_WIFI_TOKEN){server.send(401,"application/json","{\"error\":\"unauthorized\"}");return;}
  telemetry::Snapshot copy;uint32_t stamp;bool valid;
  portENTER_CRITICAL(&mux);copy=latest;stamp=published;valid=hasSnapshot;portEXIT_CRITICAL(&mux);
  if(!valid||uint32_t(millis()-stamp)>2000){server.send(503,"application/json","{\"error\":\"stale_snapshot\"}");return;}
  char json[2048];if(!telemetry::encode(copy,json,sizeof(json))){server.send(503,"application/json","{}");return;}
  server.send(200,"application/json",json);
 });
 server.onNotFound([]{server.send(404,"application/json","{\"error\":\"read_only\"}");});
 server.begin();uint32_t retry=millis();
 for(;;){
  char text[100];
  if(WiFi.status()==WL_CONNECTED){
   snprintf(text,sizeof(text),"wifi=connected ip=%s rssi=%d monitor_only=1\n",WiFi.localIP().toString().c_str(),WiFi.RSSI());
   server.handleClient();
  }else{
   snprintf(text,sizeof(text),"wifi=disconnected status=%d monitor_only=1\n",int(WiFi.status()));
   if(uint32_t(millis()-retry)>30000){WiFi.reconnect();retry=millis();}
  }
  portENTER_CRITICAL(&mux);memcpy(connection,text,sizeof(text));connection[sizeof(connection)-1]=0;portEXIT_CRITICAL(&mux);
  vTaskDelay(pdMS_TO_TICKS(10));
 }
}
inline void begin(){
 // Networking never calls the control loop, ADC, I2C or GPIO. Core 0 only.
 if(xTaskCreatePinnedToCore(task,"ito-telemetry",8192,nullptr,1,nullptr,0)!=pdPASS){
  snprintf(connection,sizeof(connection),"wifi=task_failed monitor_only=1\n");
 }
}
}
#else
namespace monitor {
#if defined(ITO_HOST_TEST) && defined(ITO_TEST_WIFI_ENABLED)
constexpr bool enabled=true;
#else
constexpr bool enabled=false;
#endif
inline void publish(const telemetry::Snapshot&){}
inline void begin(){}
inline void status(char* out,size_t n){snprintf(out,n,"wifi=disabled copy wifi_local.example.h to wifi_local.h\n");}
}
#endif

