#pragma once
#include <Arduino.h>
#include <math.h>

#ifndef ITO_HOST_TEST
#include <esp_arduino_version.h>
#include <esp_task_wdt.h>
#include <driver/gpio.h>
#if !defined(ARDUINO_XIAO_ESP32S3) || !defined(CONFIG_IDF_TARGET_ESP32S3)
#error Select esp32:esp32:XIAO_ESP32S3, not nRF52840, C3, or S3 Plus.
#endif
#if ESP_ARDUINO_VERSION_MAJOR != 3
#error Use Arduino-ESP32 3.x (verified with 3.3.2).
#endif
#if !ARDUINO_USB_CDC_ON_BOOT || ARDUINO_USB_MODE != 0
#error Enable USB CDC On Boot and select USB-OTG (TinyUSB) USB Mode.
#endif
static_assert(D0==1 && D1==2 && D2==3 && D3==4 && D4==5 && D5==6 &&
              D6==43 && D7==44 && D8==7 && D9==8 && D10==9,
              "Unexpected XIAO ESP32S3 pin mapping");
#endif

namespace platform {
// Preload the hardware latch before enabling output; Arduino digitalWrite alone
// cannot preload an unclaimed ESP32 pin. ROM activity before setup remains possible.
inline void outputLow(unsigned pin){
#ifndef ITO_HOST_TEST
 gpio_set_level(static_cast<gpio_num_t>(pin),0);
#endif
 pinMode(pin,OUTPUT);digitalWrite(pin,LOW);
}
constexpr unsigned WATCHDOG_MS=250;
inline bool calibrationValid(float gain,float offset) {
 return isfinite(gain)&&gain>=0.8f&&gain<=1.2f&&isfinite(offset)&&fabsf(offset)<=0.1f;
}
// ESP32-S3 ADC at 11 dB is characterized up to about 3.1 V, not 3.6 V.
// Reject the upper uncharacterized region instead of treating clipping as a valid NTC.
inline float correctedVoltage(unsigned raw,unsigned mv,float gain,float offset) {
 if(raw>=4090||mv>=3100)return NAN;
 return mv*0.001f*gain+offset;
}
inline void adcBegin(){
 analogReadResolution(12);
 analogSetPinAttenuation(A0,ADC_11db);
 analogSetPinAttenuation(A1,ADC_11db);
}
inline float adcVoltage(unsigned pin,unsigned& raw,unsigned& mv,float gain,float offset){
 raw=analogRead(pin);mv=analogReadMilliVolts(pin);
 return correctedVoltage(raw,mv,gain,offset);
}
#ifndef ITO_HOST_TEST
static esp_task_wdt_user_handle_t watchdogUser=nullptr;
static portMUX_TYPE tachMux=portMUX_INITIALIZER_UNLOCKED;
inline bool watchdogStart(){
 esp_task_wdt_config_t conf={};
 conf.timeout_ms=WATCHDOG_MS;conf.idle_core_mask=0;conf.trigger_panic=true;
 esp_err_t err=esp_task_wdt_init(&conf);
 if(err==ESP_ERR_INVALID_STATE)err=esp_task_wdt_reconfigure(&conf);
 if(err!=ESP_OK)return false;
 return esp_task_wdt_add_user("ito-complete-loop",&watchdogUser)==ESP_OK;
}
inline bool watchdogFeed(){return watchdogUser&&esp_task_wdt_reset_user(watchdogUser)==ESP_OK;}
inline void IRAM_ATTR tachIncrement(volatile uint32_t& pulses){
 portENTER_CRITICAL_ISR(&tachMux);pulses=pulses+1;portEXIT_CRITICAL_ISR(&tachMux);
}
inline uint32_t tachTake(volatile uint32_t& pulses){
 portENTER_CRITICAL(&tachMux);uint32_t n=pulses;pulses=0;portEXIT_CRITICAL(&tachMux);return n;
}
#else
inline bool watchdogStart(){return mockWatchdogOK;}
inline bool watchdogFeed(){return mockWatchdogOK;}
inline void tachIncrement(volatile uint32_t& pulses){pulses=pulses+1;}
inline uint32_t tachTake(volatile uint32_t& pulses){uint32_t n=pulses;pulses=0;return n;}
#endif
}
