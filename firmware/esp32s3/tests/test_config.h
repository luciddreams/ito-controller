// TEST FIXTURE ONLY
#pragma once
// Set these only after completing firmware/README.md with measured values.
namespace cfg {
constexpr bool REQUIRE_AMBIENT = true; // both remote sensors required before test pulses

constexpr bool MEASUREMENTS_VERIFIED = true;
constexpr bool ADC_VERIFIED=true;
constexpr float INPUT_ADC_GAIN=1.0f,INPUT_ADC_OFFSET_V=0.0f,HOT_ADC_GAIN=1.0f,HOT_ADC_OFFSET_V=0.0f;
constexpr unsigned INSTALLED_TEC_MASK=1;
constexpr bool ALLOW_HEATER=true;
constexpr float VDD_V = 3.3f; // measure NTC pull-up supply
constexpr float INPUT_AMPS_PER_VOLT = 1.0f / 0.279f; // calibrate externally
constexpr float INPUT_OFFSET_A = 0.0f;
constexpr float NTC_R25[5] = {10000,10000,10000,10000,10000};
constexpr float NTC_BETA[5] = {3950,3950,3950,3950,3950}; // placeholders: use installed probes
constexpr float TEMP_OFFSET_C[5] = {0,0,0,0,0};
constexpr unsigned TEC_MAX_CODE[4] = {500,500,500,500}; // measured <=2 A incl. overshoot
constexpr float HEATER_RESISTANCE_OHM = 10.0f; // measure between complete busbars
constexpr float INPUT_TRIP_A = 3.8f; // margin below true 4 A maximum
constexpr float INPUT_TRIP_V = 1.00f; // also applies after calibration
constexpr float HEATER_ON_TRIP_A = 3.4f; // measured on-pulse, margin below 3.5 A
constexpr float GLASS_TRIP_C = 40.0f;
constexpr float HOT_TRIP_C = 55.0f;
constexpr unsigned MIN_FAN_RPM = 300; // confirm against actual fan
constexpr unsigned FAN_PULSES_PER_REV = 2; // confirm fan datasheet
constexpr unsigned OFF_SETTLE_MS = 2000; // scope-verify decay; never switch directly
constexpr unsigned MAX_PULSE_MS = 2000;
constexpr float MAX_HEATER_W = 10.0f;
constexpr unsigned HEATER_PWM_HZ = 10; // commissioning only; verify IN-/output on scope
}

