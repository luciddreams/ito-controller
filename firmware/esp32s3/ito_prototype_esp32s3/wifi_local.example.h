#pragma once
// Copy to wifi_local.h; this file must stay local and out of Git.
#define ITO_WIFI_SSID "YOUR_2_4_GHZ_WIFI"
#define ITO_WIFI_PASSWORD "YOUR_WIFI_PASSWORD"
// Generate with: node -e "console.log(require('crypto').randomBytes(24).toString('hex'))"
#define ITO_WIFI_TOKEN "REPLACE_WITH_RANDOM_TOKEN_AT_LEAST_32_CHARACTERS"
