# Start the ITO Wi-Fi dashboard

1. Install Node.js 24 LTS on your PC.
2. In `firmware/esp32s3/ito_prototype_esp32s3`, copy `wifi_local.example.h` to
   `wifi_local.h`. Enter your 2.4 GHz Wi-Fi SSID/password and a random token there.
3. Upload `ito_prototype_esp32s3.ino` using Arduino-ESP32 3.3.2, XIAO_ESP32S3,
   USB-OTG (TinyUSB), USB CDC On Boot Enabled. Attach the antenna. Program off-board.
4. Send `wifi` in Serial Monitor (115200 baud, newline) and note the assigned IP.
5. In the `dashboard` directory, run `npm ci`, copy `.env.example` to `.env.local`,
   and set the IP and the same token. Keep `ITO_DEMO=false` for the real board.
6. Run `npm run dev` and open http://localhost:3000 on that PC.

The full guide, token-generation command, wiring cautions and troubleshooting
are in `dashboard/README.md`. Files containing your credentials stay local.
The dashboard runs on your PC; the XIAO supplies telemetry. This firmware build
is monitor-only and rejects heater/TEC pulses, including those sent over USB.

Builds and automated tests pass. Live interaction in the cloud preview was not
verified; see `dashboard/VERIFICATION.md`. The first local and hardware check is
still required. No dummy readings are substituted when a real device disconnects.
