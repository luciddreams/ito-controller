# ITO Thermal Observatory — Next.js dashboard

Local, read-only dashboard for the original revision-L PCB and XIAO ESP32S3.
The Next.js server runs on your Windows PC; the microcontroller exposes telemetry
on your private Wi-Fi network. No cloud service or Internet port forwarding is
needed. Use a trusted home LAN: device HTTP is not encrypted. The token limits
access but is not a replacement for network isolation. Do not expose either
service to the Internet. The dashboard binds to 127.0.0.1 by default.

## 1. Install the Wi-Fi firmware

Use the updated sketch in `../firmware/esp32s3/ito_prototype_esp32s3/`.
Read that folder's hardware cautions first. Upload with the XIAO off the PCB,
USB only. Attach its supplied antenna. Wi-Fi requires a 2.4 GHz network; your PC
can be on Ethernet or 5 GHz if it can reach devices on the same LAN. Guest Wi-Fi
client isolation can prevent access.

Copy `wifi_local.example.h` to `wifi_local.h` in the sketch folder. Fill in:

```cpp
#define ITO_WIFI_SSID "your-network"
#define ITO_WIFI_PASSWORD "your-password"
#define ITO_WIFI_TOKEN "your-random-token-at-least-32-characters"
```

Generate the token locally in PowerShell after installing Node.js:

```powershell
node -e "console.log(require('crypto').randomBytes(24).toString('hex'))"
```

Use the generated value in both firmware and dashboard configuration. Keep it
and your password out of chat and Git. Escape any `"` or `\` in C++ string values.
`wifi_local.h` is ignored by Git; credentials are compiled into the device binary,
so do not distribute a configured firmware binary or your local secrets file.
Without this header, firmware remains USB-only.

Arduino IDE: esp32 by Espressif Systems **3.3.2**, board **XIAO_ESP32S3**,
USB Mode **USB-OTG (TinyUSB)**, USB CDC On Boot **Enabled**. Upload, open Serial
Monitor at 115200 with newline, and send:

```text
wifi
```

After connection it prints `wifi=connected ip=192.168.x.x ...`. Repeat the command
if still connecting. No password or token is printed. The device retries after
Wi-Fi loss. Reserve its IP in your router if you want a stable address.

**This Wi-Fi build is monitor-only:** even USB `tec` and `heat` commands are
rejected. Network-task timing has not been qualified for power operation.
All hardware protections remain necessary, including the GPIO43 boot caveat.
For mounting: remove JP1 when using USB, switch all supplies off first and follow
START-HIER.md. Do not treat the software pulse lock as proof that hardware outputs
cannot glitch before firmware startup. Keep loads disconnected during commissioning.

## 2. Start the dashboard on Windows

Install Node.js **24 LTS** (22.18+ also supports the test runner used here).
Open PowerShell in this `dashboard` folder:

```powershell
npm ci
Copy-Item .env.example .env.local
notepad .env.local
```

Set the actual IP reported above and the same token:

```dotenv
ITO_DEVICE_URL=http://192.168.1.100
ITO_DEVICE_TOKEN=your-generated-token
ITO_DEMO=false
```

Then:

```powershell
npm run dev
```

Open **http://localhost:3000**. Keep the terminal running. Restart the dashboard
when changing `.env.local`. Both configuration files are local and excluded from
Git. No `NEXT_PUBLIC_` secrets are used: the browser calls the Next.js proxy,
which attaches the token on the server. The device has no remote control route.

For a production local build:

```powershell
npm run build
npm start
```

## Dashboard behaviour

- Five NTC temperatures and two minutes of in-browser history.
- Two SHT31 temperatures, humidity and calculated dew points.
- Input current estimate, heater current, switched bus voltage and fan RPM.
- Faults, watchdog, calibration status, logical interlocks and output GPIO levels.
- Raw ADC/millivolt values and installed TEC channels in the technical section.
- Pause/resume updates and export the current snapshot as JSON.

Missing/invalid measurements display `—`, not zero. Disconnected or unchanged
telemetry becomes stale; live cards are hidden. Historical graph values remain
historical and are not proof of current conditions. History is cleared on page
reload and device restart. The dashboard never changes firmware readiness or
clears latched faults. GPIO readback does not prove downstream power is off.
The INA226 heater-current reading is not U11 output current.

`STALE_SENSOR` is expected while your NTCs are disconnected. Temperatures and
currents remain uncalibrated until checked against reference measurements.

## Preview without a board

Set `ITO_DEMO=true` in `.env.local` and restart. A prominent DEMO banner labels
simulated readings. Demo is opt-in and never a fallback for a disconnected device.
Set it back to false to see your actual controller.

## Troubleshooting

- `wifi=disabled`: the local header is missing or an older firmware was uploaded.
- Disconnected Wi-Fi: check SSID/password, antenna, 2.4 GHz and signal strength.
- Dashboard configuration error: verify `.env.local` and restart `npm run dev`.
- Key rejected: make firmware `ITO_WIFI_TOKEN` and server `ITO_DEVICE_TOKEN` match.
- No telemetry: check the current IP with `wifi`, subnet/client isolation, power,
  and the installed L.2-S3 firmware. URLs must be private IPv4 HTTP origins.
- `ready=0`: intentional; the Wi-Fi build is monitor-only even after calibration.

## Verification

`npm test` tests the JSON schema and real local HTTP proxy with valid, unauthorized,
malformed, oversized, redirected and disconnected responses. `npm run build`
checks the production bundle and TypeScript. Firmware host tests are under
`../firmware/esp32s3/tests/`. No physical Wi-Fi connection or hardware timing can
be verified from this workspace; complete those checks on the bench.
