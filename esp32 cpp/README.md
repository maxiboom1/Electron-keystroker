# GPIO Box Project Characterization

## Overview

The GPIO Box is an ESP32-based device designed to monitor 8 GPIO input pins (GPI-1 to GPI-8) and send their state changes as structured JSON messages via user-configurable communication interfaces: Serial, TCP, and HTTP. It supports 8 GPIO output pins (GPO-1 to GPO-8) reserved for future remote triggering. A minimal web-based configuration page allows users to set device behavior, communication modes, and security options, with all configurations stored persistently in EEPROM. The device includes a reset button to restore factory defaults, ensuring robust operation and ease of recovery.

## General Purpose

The GPIO Box serves as a versatile, network-enabled input/output controller for IoT applications. It monitors GPIO inputs for state changes, reporting them via Serial, TCP, or HTTP based on user configuration, with optional secure authentication. It’s designed for integration into automation systems, providing a simple web interface for setup, persistent storage, and future expansion to control outputs (GPOs) remotely. The device is ideal for home automation, industrial monitoring, or custom sensor networks, focusing solely on configuration without a dashboard or status feedback.

## Features

### 1. GPIO Mapping and Trigger Logic
- **Inputs**: 8 GPIO inputs (GPI-1 to GPI-8), monitored for state changes (HIGH to LOW or LOW to HIGH).
- **Outputs**: 8 GPIO outputs (GPO-1 to GPO-8), reserved for future remote triggering via TCP, HTTP, or Serial commands.
- **Trigger**: Fires on state change, sending a message with the pin identifier, state, user, and password (e.g., `{"event": "GPI-1", "state": "HIGH", "user": "", "password": ""}`).

### 2. Communication Modes
- **Serial**: Sends JSON messages (e.g., `{"event": "GPI-1", "state": "HIGH", "user": "", "password": ""}`) via `Serial.println()` when enabled.
- **TCP**: Sends JSON messages (`{"event": "GPI-1", "state": "HIGH", "user": "xxx", "password": "yyy"}`) to a specified IP and port while maintaining a persistent connection. If the server is unreachable, it retries every 5 seconds. .
- **HTTP**: Sends POST requests with JSON in the body (e.g., `{"event": "GPI-1", "state": "HIGH", "user": "xxx", "password": "yyy"}` with `Content-Type: application/json`) to a specified URL when enabled.
- **Mode Selection**: Only enabled modes activate on GPIO trigger; multiple modes can be enabled simultaneously (e.g., TCP + HTTP). All interfaces are disabled by default.

### 3. Security
- **Secure Mode**: Optional for TCP and HTTP.
  - If enabled, includes user and password in JSON messages; if disabled, includes empty strings for consistency.
  - **TCP**: `{"event": "GPI-1", "state": "HIGH", "user": "xxx", "password": "yyy"}`.
  - **HTTP**: POST body `{"event": "GPI-1", "state": "HIGH", "user": "xxx", "password": "yyy"}`.
- **Config Page Access**: Requires basic authentication with fixed user `admin` and configurable password (default `admin`).

### 4. Configuration Page
- **Style**: Minimal HTML with CSS, featuring a footer (e.g., "GPIO Box v1.0").
- **Sections**:
  - **Device Address**: 
    - IP address (text input, default `10.168.0.177`).
    - Subnet mask (text input, default `255.255.255.0`).
    - Gateway (text input, default `10.168.0.1`).
  - **TCP Settings**:
    - Enabled (checkbox, off by default).
    - IP (text input, default `""`).
    - Port (text input, default `""`).
    - Secure Mode (checkbox, off by default).
    - User (text input, blank by default, enabled if Secure Mode checked).
    - Password (text input, blank by default, enabled if Secure Mode checked).
  - **HTTP Settings**:
    - Enabled (checkbox, off by default).
    - URL (text input, default `""`).
    - Secure Mode (checkbox, off by default).
    - User (text input, blank by default, enabled if Secure Mode checked).
    - Password (text input, blank by default, enabled if Secure Mode checked).
  - **Serial Settings**:
    - Enabled (checkbox, off by default).
  - **Admin Access**:
    - Login: Fixed `admin`, password (configurable, default `admin`).
    - Password change option via POST.

### 5. Persistence and Reset
- **EEPROM Storage**: All configuration options stored in EEPROM, restored on boot.
- **Reset Button**: Physical button (GPIO 0) resets to factory defaults (e.g., all interfaces disabled, IP/URL/ports empty, admin password `admin`).

## ESP32 Wiring and GPIO Mapping

### Hardware Configuration
- **Development Board**: ESP32-WROOM-32 (30-pin layout).
- **Current Usage**:
  - **LEDs**: GPIO 21 (SDA, I2C for 1602 LCD), GPIO 22 (SCL, I2C for 1602 LCD).
  - **W5500 Ethernet Module**:
    - GPIO 15 (CS, chip select, defined as W5500_CS).
    - GPIO 18 (SCK, SPI clock).
    - GPIO 19 (MISO, SPI data in).
    - GPIO 23 (MOSI, SPI data out).
    - GPIO 4 (RST, reset, confirmed unused after disconnection—freed for GPIO).
  - **Reset Button**: GPIO 0 (preferred, check if tied to EN; fallback GPIO 26 if needed).
- **Available GPIO Pins** (after freeing GPIO 4): GPIO 2, 5, 12, 13, 14, 16, 17, 25, 32, 33, 34, 35, 4, 26, 27 (15 pins total).
  - **RX0/TX0** (GPIO 1, 3): UART0, reserved for Serial debugging unless repurposed.
  - **VN/VP** (GPIO 36, 39): ADC-only, not usable as GPIOs.

### Assigned Pins:
| **GPI Inputs (GPI-1 to GPI-8)** | **GPO Outputs (GPO-1 to GPO-8)** |
|---------------------------------|----------------------------------|
| GPI-1: GPIO 4                   | GPO-1: GPIO 32                   |
| GPI-2: GPIO 5                   | GPO-2: GPIO 33                   |
| GPI-3: GPIO 12                  | GPO-3: GPIO 34                   |
| GPI-4: GPIO 13                  | GPO-4: GPIO 35                   |
| GPI-5: GPIO 14                  | GPO-5: GPIO 26  |
| GPI-6: GPIO 16                  | GPO-6: GPIO 27                   |
| GPI-7: GPIO 17                  | GPO-7: GPIO 0 (May conflict with reset)                  |
| GPI-8: GPIO 25                  | GPO-8: n/a |

- **Reset Button**: GPIO 0 (preferred, check if tied to EN; fallback GPIO 26).

#### Notes:
- Verify GPIO 0 isn’t the EN pin (typically tied to EN on some boards, requiring pull-up). If it is, use GPIO 26 for reset.
- RX0/TX0 (GPIO 1, 3) can be repurposed later if Serial isn’t needed, but keep for debugging now.
- All GPI/GPO pins support input/output, PWM, interrupt—adequate for our needs.

## EEPROM Structure

| **Field**           | **Size (Bytes)** | **Offset** | **Default Value**    |
|---------------------|------------------|------------|----------------------|
| Device IP           | 4                | 0-3        | `10.168.0.177`       |
| Gateway             | 4                | 4-7        | `10.168.0.1`         |
| Subnet Mask         | 4                | 8-11       | `255.255.255.0`      |
| TCP Enabled         | 1                | 12         | 0 (off)              |
| TCP IP              | 4                | 13-16      | `0.0.0.0` (empty)    |
| TCP Port            | 2                | 17-18      | 0 (empty)            |
| TCP Secure          | 1                | 19         | 0 (off)              |
| TCP User            | 32               | 20-51      | `""` (empty)         |
| TCP Password        | 32               | 52-83      | `""` (empty)         |
| HTTP Enabled        | 1                | 84         | 0 (off)              |
| HTTP URL            | 64               | 85-148     | `""` (empty)         |
| HTTP Secure         | 1                | 149        | 0 (off)              |
| HTTP User           | 32               | 150-181    | `""` (empty)         |
| HTTP Password       | 32               | 182-213    | `""` (empty)         |
| Serial Enabled      | 1                | 214        | 0 (off)              |
| Admin Password      | 32               | 215-246    | `"admin"`            |
| Config Flag         | 1                | 247        | 0xAA (configured)    |
| **Total**           | **248 bytes**    |            |                      |

#### Notes:
- Fits within ESP32’s 512-byte EEPROM.
- **Config Flag** ensures valid config (reset to `0x00` for defaults).
- Empty IP/URL/port defaults to 0.0.0.0, `""`, or 0, requiring user configuration.
- URL length (64 bytes, offset 77-140) allows up to 64 characters (e.g., `http://10.168.0.10/api` fits, max length for URLs in EEPROM).

## Future Enhancements

- **GPO Control**: Remotely trigger GPO-1 to GPO-8 via TCP, HTTP, or Serial commands (e.g., `{"event": "GPO-1", "state": "HIGH", "user": "", "password": ""}`).
- **Error Handling**: Define behavior for unreachable servers or invalid configs.

## Future Enhancements

#### V 0.03:
- Implemented MessageBuilder.h module to construct messages.
- Implemented 8 GPI pins handling logic.

#### V 0.04:
- Implemented a persistent TCP connection instead of reconnecting per message.
- Added automatic reconnection every 5 seconds if the server is unreachable.
- Managed connection state to avoid unnecessary reconnections.
- Stopped TCP communication when `tcpEnabled` is disabled in the config.
- Integrated `TcpClient.h` into `Gpio_Box_V0.04.ino` for seamless TCP messaging.

#### V 0.05:
- Refactored message handling into separate functions for TCP, HTTP, and Serial.
- Fixed Secure Mode handling, ensuring correct user/password usage for TCP and HTTP.
- Updated `HttpClient.h` to retrieve the HTTP URL from EEPROM instead of using a hardcoded value.
- Modified `sendHttpPost()` to accept the JSON message as a parameter.
- The only issue that the http url cannot pars port from url in current version. We will fix it in 0.06

#### V 0.06:
- Fixed HTTP URL handling to correctly support custom ports (e.g., `10.168.0.10:5000/api`).
- Increased JSON parsing buffer size from `512` to `2048` to prevent `NoMemory` errors.
- Verified that URLs with and without ports are stored and parsed correctly.
- Added error print in Json parser.