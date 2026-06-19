# Project Aura AQ

[![PlatformIO](https://img.shields.io/badge/platform-PlatformIO-ff7f2a?logo=platformio&logoColor=white)](https://platformio.org/)
[![ESP32-S3](https://img.shields.io/badge/target-ESP32--S3-323330)](https://www.espressif.com/en/products/socs/esp32-s3)
[![LVGL](https://img.shields.io/badge/ui-LVGL-00b0f0)](https://lvgl.io/)
[![License: GPLv3](https://img.shields.io/badge/License-GPLv3-blue.svg)](LICENSE)
[![YouTube Demo](https://img.shields.io/badge/YouTube-Demo-FF0000?logo=youtube&logoColor=white)](https://www.youtube.com/watch?v=TNsyDGNrN-w)
[![YouTube Review](https://img.shields.io/badge/YouTube-Review-FF0000?logo=youtube&logoColor=white)](https://www.youtube.com/watch?v=1pzBqcmbWl8)

Support this project: back the crowdfunding to get detailed build instructions, 3D-printable enclosure models, and wiring guides at:
https://makerworld.com/en/crowdfunding/159-project-aura-make-the-invisible-visible

Project Aura is an open-source ESP32-S3 air-quality station built for makers who want a polished,
reliable device rather than a bare sensor board. It combines a touch-friendly LVGL UI, a full local
web dashboard with browser OTA updates, a Wi-Fi setup portal, optional 0-10V fan control, and MQTT
with Home Assistant discovery.

This repository contains the firmware source code and configuration needed to flash and customize the device.

**Join the community:** [GitHub Discussions](https://github.com/21cncstudio/project_aura/discussions)

## Table of Contents
- [Videos](#videos)
- [Highlights](#highlights)
- [Gallery](#gallery)
- [UI Screens](#ui-screens)
- [Web Dashboard](#web-dashboard)
- [Network Requirements](#network-requirements)
- [Hardware and BOM](#hardware-and-bom)
- [Assembly and Wiring Notice](#assembly-and-wiring-notice)
- [Pin Configuration](#pin-configuration)
- [UI Languages](#ui-languages)
- [Firmware Architecture](#firmware-architecture)
- [Build and Flash](#build-and-flash-platformio)
- [Configuration](#configuration)
- [MQTT + Home Assistant](#mqtt--home-assistant)
- [Contributing](#contributing)
- [AI Assistance](#ai-assistance)
- [License and Commercial Use](#license-and-commercial-use)
- [Tests](#tests)
- [Repo Layout](#repo-layout)

## Videos
Watch the demo:
[![Project Aura demo video](https://img.youtube.com/vi/TNsyDGNrN-w/maxresdefault.jpg)](https://www.youtube.com/watch?v=TNsyDGNrN-w)

Also watch the hands-on review:
[Project Aura review on YouTube](https://www.youtube.com/watch?v=1pzBqcmbWl8)

## Highlights
- Professional telemetry: PM0.5/PM1/PM2.5/PM4/PM10, CO, CO2, VOC, NOx, temperature, humidity, absolute humidity (AH), pressure, HCHO.
- No soldering required: designed for easy assembly using Grove/QT connectors.
- Smooth LVGL UI with night mode, custom themes, and status indicators.
- Integrated web dashboard at `/dashboard` with live state, charts, events, settings sync, DAC page, and OTA firmware update.
- Easy setup: Wi-Fi AP onboarding + mDNS access (`http://<hostname>.local`).
- Home Assistant ready: automatic MQTT discovery and ready-to-use dashboard code.
- Optional DAC control (GP8403, 0-10V): manual levels/timer plus automatic demand mode from air-quality thresholds.
- Hardware autodetect: PCF8523/DS3231 RTC plus BMP58x/BMP3xx/DPS310 pressure support.
- Robust Safe Boot: automatic rollback to the last-known-good config after crashes.

![Project Aura device](docs/assets/device-hero.jpg)

## Gallery
![Assembly overview](docs/assets/device-assembly.jpg)
![Internal wiring](docs/assets/device-internals.jpg)
![Boot and live dashboard](docs/assets/device-boot-ui.jpg)

## UI Screens
<table>
  <tr>
    <td align="center"><img src="docs/assets/ui-dashboard.jpg" alt="Dashboard"/><br/>Dashboard</td>
    <td align="center"><img src="docs/assets/ui-settings.jpg" alt="Settings"/><br/>Settings</td>
    <td align="center"><img src="docs/assets/ui-theme-presets.jpg" alt="Theme Presets"/><br/>Theme Presets</td>
  </tr>
  <tr>
    <td align="center"><img src="docs/assets/ui-mqtt.jpg" alt="MQTT"/><br/>MQTT</td>
    <td align="center"><img src="docs/assets/ui-time-date.jpg" alt="Date & Time"/><br/>Date &amp; Time</td>
    <td align="center"><img src="docs/assets/ui-backlight.jpg" alt="Backlight"/><br/>Backlight</td>
  </tr>
</table>

## Web Dashboard
Project Aura serves two web experiences from the device:

- Setup portal (AP mode): opening `/` in AP mode shows the captive Wi-Fi setup flow (`/wifi`).
- Full dashboard (`/dashboard`): tabs for Sensors, Charts, Events, Settings, and System.
- Dedicated DAC page (`/dac`): live status, manual controls, auto mode, and auto-threshold config.
- OTA from dashboard: upload firmware `.bin` directly via the web UI.
- Dashboard assets are served locally in both AP and STA modes (no CDN dependency).

Useful API routes used by the dashboard:
- `GET /api/state`
- `GET /api/charts?group=core|gases|pm&window=1h|3h|24h`
- `GET /api/events`
- `GET /api/diag` (AP setup mode only)
- `POST /api/settings`
- `POST /api/ota`

## Network Requirements
- AP setup mode (`http://192.168.4.1`) is local-only and works without internet.
- STA mode dashboard (`http://<hostname>.local/dashboard` or `http://<ip>/dashboard`) requires the client and Aura to be in the same L2/L3 network path.
- The hostname is user-friendly and mDNS-safe:
  - Uses **Settings → Device Name** (converted to lowercase `a-z`, `0-9`, `-`) when set.
  - Falls back to `aura-<mac-suffix>` when Device Name is empty or not mDNS-safe.
- If mDNS (`.local`) is blocked on your network, use direct IP from the device screen or router DHCP table.
- Avoid guest networks, client isolation, or VLAN rules that block peer-to-peer LAN traffic.
- Required local traffic:
  - HTTP: TCP `80` to Aura
  - mDNS: UDP `5353` multicast (only for `.local` hostname discovery)
- During OTA, keep one active client tab/session to reduce transfer failures.

Quick diagnostics for support:
- `GET /api/state` should return live JSON with `network.mode`, `network.ip`, and sensor payload.
- `GET /api/diag` (available in AP setup mode) shows Wi-Fi state, IP/hostname, heap, OTA busy state, and recent warnings/errors.

## Hardware and BOM
Project Aura is designed around high-quality components to ensure accuracy. If you are sourcing parts yourself,
look for these specific modules for the reference build:

| Component | Part / Model |
| :--- | :--- |
| Core Board | [Waveshare ESP32-S3-Touch-LCD-4.3 (800x480)](https://www.waveshare.com/esp32-s3-touch-lcd-4.3.htm?&aff_id=144793) |
| Main Sensor | Sensirion SEN66 (via Adafruit breakout) |
| Carbon Monoxide (CO) | DFRobot Fermion SEN0466 (optional) |
| Formaldehyde | Sensirion SFA30 (Grove interface, optional) |
| Pressure | Adafruit BMP580 or DPS310 (recommended) |
| RTC | Adafruit PCF8523 (recommended) |
| DAC Output | GP8403 2-channel 0-10V DAC (optional, VOUT0 used) |

Affiliate note: the Waveshare board link above is an affiliate link and helps support Project Aura at no extra cost.

Pressure note: the reference BOM uses BMP580 or DPS310. Firmware auto-detects BMP580/581, BMP585, BMP388, BMP390, and DPS310.
RTC note: the reference BOM uses PCF8523. Firmware auto-detects PCF8523 and DS3231, and also provides a manual RTC override mode.
Sensor note: the SFA30 is fully supported and widely available. Support for the successor model (SFA40) is on the roadmap.
CO note: the SEN0466 is optional. If not detected at boot, CO is marked unavailable and PM1 telemetry remains active.
Note: SEN66 gas indices (VOC/NOx) require about 5 minutes of warmup for reliable readings; the UI shows WARMUP during this period.

Recommended retailers: Mouser, DigiKey, LCSC, Adafruit, Seeed Studio, [Waveshare (core board)](https://www.waveshare.com/esp32-s3-touch-lcd-4.3.htm?&aff_id=144793).

## Assembly and Wiring Notice
Please pay close attention to the cabling. The pin order on the board is custom and requires modification
of standard off-the-shelf cables (pin swapping).

Backers: please refer to the comprehensive Build Guide included in your reward for the exact wiring diagram
and a trouble-free assembly.

DIY: verify pinouts against the pin table below before powering on to avoid damaging components.

## Pin Configuration
| Component | Pin (ESP32-S3) | Notes |
| :--- | :--- | :--- |
| 3V3 | 3V3 | Power for external I2C sensors |
| GND | GND | Common ground |
| I2C SDA | GPIO 8 | Shared bus: SEN66, SFA30, SEN0466, BMP58x/BMP3xx/DPS310, PCF8523/DS3231, GP8403 |
| I2C SCL | GPIO 9 | Shared bus |

Display and touch are integrated on the board; no external wiring is needed for them.
For DAC control, analog output is generated on the GP8403 module (`VOUT0`), not on a direct ESP32 pin.

## UI Languages
Project Aura speaks your language. You can switch languages in the Settings menu:
- English
- Deutsch
- Espanol
- Francais
- Italiano
- Portugues BR
- Nederlands
- Simplified Chinese

## Firmware Architecture
Data flow and responsibilities are intentionally split into small managers:

```mermaid
graph TD
    subgraph Hardware
        Sensors[Sensors<br/>SEN66, SFA30, SEN0466, BMP58x/BMP3xx/DPS310]
        Touch[Touch<br/>GT911]
        RTC[RTC<br/>PCF8523 or DS3231]
        DAC[DAC<br/>GP8403]
        Actuator[External Fan / Actuator]
        LCD[LCD + Backlight]
    end

    subgraph Core
        SM[SensorManager]
        FC[FanControl]
        NM[NetworkManager]
        MM[MqttManager]
        TM[TimeManager]
        CoreLogic[BootPolicy / Watchdog / MemoryMonitor]
    end

    subgraph Data
        History[PressureHistory]
        Storage[(LittleFS<br/>config/last_good/voc/pressure)]
    end

    subgraph UI
        LVGL[LVGL UI]
        Web[Web Config Portal]
    end

    Sensors -->|I2C| SM
    RTC -->|I2C| TM
    Touch -->|I2C| LVGL
    LCD --> LVGL
    SM -->|Data| LVGL
    SM -->|Data| FC
    SM -->|Data| History
    FC -->|I2C| DAC
    DAC -->|0-10V| Actuator
    History <-->|save/load| Storage
    Storage <-->|config| NM
    Storage <-->|config| MM
    NM --> Web
    NM --> MM
    MM -->|Publish| HA[Home Assistant]

    style CoreLogic fill:#f96,stroke:#333,stroke-width:2px
```

Core modules live in `src/core/` and orchestrate startup (`AppInit`, `BoardInit`).
Feature managers are in `src/modules/`, UI in `src/ui/`, and web pages in `src/web/`.

## Build and Flash (PlatformIO)
Prerequisites: PlatformIO CLI or VSCode + PlatformIO extension.
Built with Arduino ESP32 core 3.1.1 (ESP-IDF 5.3.x).

```powershell
git clone https://github.com/21cncstudio/project_aura.git
cd project_aura
pio run -e project_aura
pio run -e project_aura -t upload
pio run -e project_aura -t uploadfs
pio device monitor -b 115200
```

## Configuration
1. Wi-Fi setup:
   On first boot, the device creates a hotspot: `Aura-XXXXXX-AP` (fallback: `ProjectAura-Setup`).
   Connect to it and open http://192.168.4.1 to configure Wi-Fi credentials.
2. Web portal:
   After saving Wi-Fi, wait about 15 seconds for AP -> STA transition.
   Then access the device at `http://<hostname>.local/` (default hostname format: `aura-XXXXXX`, for example `aura-1a2b3c.local`) or by IP.
   Do not keep using `http://192.168.4.1` after STA connection; that address is AP-setup only.
   In STA mode, `/` opens the dashboard. In AP mode, `/` opens setup and `/dashboard` opens the offline dashboard.
3. Home Assistant:
   MQTT discovery is enabled by default. The device appears in HA via MQTT integration automatically.
   A ready-to-use dashboard YAML is available at `docs/home_assistant/dashboard.yaml`.
   Setup guide: `docs/home_assistant/README.md`.

Optional compile-time defaults belong in `include/secrets.h`, which is ignored by git.
Copy and edit:

```text
copy include/secrets.h.example include/secrets.h
```

## MQTT + Home Assistant
- State topic: `<base>/state`
- Availability topic: `<base>/status`
- Commands: `<base>/command/*` (night_mode, alert_blink, backlight, restart)
- Home Assistant discovery: `homeassistant/*/config`
- Discovery payload includes dedicated sensors for `CO` (`co`) and `PM0.5` (`pm05`).

MQTT stays idle until configured and enabled.

![Home Assistant dashboard](docs/assets/ha-dashboard.jpg)

## Contributing
Contributions are welcome! Please read [`CONTRIBUTING.md`](CONTRIBUTING.md) for details on the process for submitting pull requests and the Contributor License Agreement (CLA).

Found a bug? Open an Issue: https://github.com/21cncstudio/project_aura/issues
Have a question? Ask in Discussions: https://github.com/21cncstudio/project_aura/discussions

## AI Assistance
Parts of this project are developed with AI-assisted workflows.
Primary coding assistance is provided by GPT-5 Codex in a local developer workspace.

## License and Commercial Use
- Firmware in this repository is licensed under GPL-3.0-or-later (see `LICENSE`).
- Commercial use is allowed under GPL. If you distribute firmware (including in devices), you must provide the Corresponding Source under GPL.
- If you need to sell devices while keeping firmware modifications proprietary, obtain a Commercial License (see `COMMERCIAL_LICENSE_SUMMARY.md`).
- Enclosure models and the PDF build guide are not in this repository; they are available to backers on MakerWorld under separate terms.
- Trademark and branding use is covered by `TRADEMARKS.md`.

## Tests
See `TESTING.md` for native host tests and `scripts/run_tests.ps1`.

## Repo Layout
- `src/core/` boot, init, reliability
- `src/modules/` sensors, storage, network, MQTT, time
- `src/ui/` LVGL screens, assets, controllers
- `src/web/` HTML templates and handlers
- `test/` native tests and mocks
