cmake_minimum_required(VERSION 3.16)
add_compile_definitions(ARDUINO_XIAO_ESP32S3=1 ARDUINO_USB_MODE=0 ARDUINO_USB_CDC_ON_BOOT=1)
set(EXTRA_COMPONENT_DIRS
  "${CMAKE_CURRENT_LIST_DIR}/vendor/microlink/components"
  "${CMAKE_CURRENT_LIST_DIR}/vendor/arduino"
  "${CMAKE_CURRENT_LIST_DIR}/vendor/lib-builder/components/arduino_tinyusb"
)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(ito_tailscale_monitor)
