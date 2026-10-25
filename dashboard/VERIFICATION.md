# Verification — 25 October 2026

- Next.js 16.4.3 / React 19.3.1, Node.js 24.22.1.
- Production `next build --webpack`: PASS, including TypeScript.
- Four Node test cases: PASS. Includes real local HTTP requests through the route
  for correct/wrong keys, invalid/oversized JSON, redirect rejection and offline
  state; schema checks cover missing values and nonfinite readings.
- Six firmware host suites: PASS, including JSON serialization and explicit Wi-Fi
  pulse rejection with an otherwise calibrated, healthy fixture.
- Arduino-ESP32 3.3.2 Wi-Fi target build: PASS, 949087 bytes flash / 66200 bytes RAM.
- USB-only target build: PASS, 340563 bytes flash / 41752 bytes RAM.
- Core library warnings occurred on full Arduino compilation; final incremental
  Wi-Fi compile has no warnings.
- Cloud browser: layout rendered and `/api/telemetry` returned the labelled demo
  payload. The page stayed in Connecting in this preview, so browser hydration,
  live updates, pause/resume and export are NOT verified end-to-end. This needs
  a local-browser check; successful builds/API tests do not establish UI operation.
- No physical XIAO, radio connection, sensor calibration or network/control-loop
  timing measured here. Wi-Fi-enabled firmware therefore blocks power pulses.
