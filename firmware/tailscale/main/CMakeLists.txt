idf_component_register(
  SRCS "main.cpp" "../vendor/pubsubclient/src/PubSubClient.cpp"
  INCLUDE_DIRS "." "../vendor/pubsubclient/src" "../../esp32s3/ito_prototype_esp32s3"
  REQUIRES arduino microlink nvs_flash
)
target_compile_definitions(${COMPONENT_LIB} PRIVATE
  ITO_TAILSCALE_MONITOR=1 ARDUINO_USB_MODE=0 ARDUINO_USB_CDC_ON_BOOT=1
)

# Keep IRAM ISR helper literals before their Xtensa L32R references.
# https://gcc.gnu.org/onlinedocs/gcc/Xtensa-Options.html
set_source_files_properties(main.cpp PROPERTIES COMPILE_OPTIONS "-mtext-section-literals")
