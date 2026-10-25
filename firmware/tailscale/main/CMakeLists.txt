idf_component_register(
  SRCS "main.cpp" "../vendor/pubsubclient/src/PubSubClient.cpp"
  INCLUDE_DIRS "." "../vendor/pubsubclient/src" "../../esp32s3/ito_prototype_esp32s3"
  REQUIRES arduino microlink nvs_flash
)
target_compile_definitions(${COMPONENT_LIB} PRIVATE
  ITO_TAILSCALE_MONITOR=1 ARDUINO_USB_MODE=0 ARDUINO_USB_CDC_ON_BOOT=1
)
