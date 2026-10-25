# MQTT dashboard on luciddreams

The ESP32 publishes to the existing MQTT broker through its native Tailscale
connection. This container only subscribes and displays fresh telemetry. It does
not create, replace, or reconfigure the broker.

1. Confirm the broker accepts MQTT 3.1.1 on the server's Tailscale address/port
   (default firmware port 1883). Restrict access with tailnet policy and broker
   credentials. Create separate publish-only device and subscribe-only dashboard
   accounts for the exact topic `ito/controller-01/telemetry`.
2. Copy `.env.mqtt.example` to `.env.mqtt`. Set the actual broker URL and subscriber
   credentials locally. For a broker on the Docker host, the example uses
   `host.docker.internal`; its listener/firewall must permit the Docker bridge.
   If it is a container, attach both containers to its existing Docker network
   and use that broker's service name instead. Do not publish a broker port to
   the internet for this integration.
3. From `dashboard/`, run `docker compose up -d --build`.
4. Check `curl http://127.0.0.1:3100/api/telemetry` on the server. A 503 is expected
   until a fresh, non-retained sample arrives. Missing sensors remain null.
5. Inspect `tailscale serve status` first. If the root Serve route is unused,
   use `tailscale serve --bg 3100` to expose this dashboard privately at
   `https://luciddreams.tail8decad.ts.net/`. Preserve any existing Serve setup:
   choose a separate HTTPS port or coordinate its existing proxy if occupied.

The Compose mapping binds port 3100 only to server localhost. The container runs
as a non-root user with a read-only root filesystem. It needs no SSH key or
Tailscale enrollment key. The host already runs Tailscale; the ESP has its own
node identity. This setup does not use Funnel or publish the dashboard publicly.

The server rejects retained, invalid, oversized (>8192-byte), or non-monitor
samples. Duplicate samples do not refresh freshness. Readings expire after five
seconds without progress and are cleared when MQTT disconnects. MQTT transport
selection is explicit (`ITO_TRANSPORT=mqtt`), so a broker failure never switches
to the LAN HTTP device or demo readings. History remains in-browser, not durable.

Verification in this workspace: seven Node tests and the Next.js production build
pass. A local MQTT broker delivered data to the production Next.js API; stale
duplicates were rejected and new samples restored service. Docker execution and
real luciddreams broker/server access have not been verified.
The firmware's native ESP-IDF build and physical connection remain unverified.
