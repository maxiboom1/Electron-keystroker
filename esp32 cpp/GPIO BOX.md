Project Characterization: GPIO Box
Overview
The GPIO Box is an ESP32-based device that monitors a GPIO pin (e.g., pin 16) and sends a "tick" message via user-configurable communication interfaces when triggered. It provides a web-based configuration page for setting up its behavior, with options stored in EEPROM for persistence across reboots. The system supports secure communication and admin access control.

Features
Configuration Page:
Minimal Styling: Clean, simple HTML with a footer (e.g., version or status info).
Paragraphs:
Device Address: Already implemented (IP setting).
TCP Settings:
Enabled (checkbox).
IP (text input).
Port (text input).
Secure Mode (checkbox).
User (text input, enabled if Secure Mode checked).
Password (text input, enabled if Secure Mode checked).
HTTP Settings:
Enabled (checkbox).
URL (text input).
Secure Mode (checkbox).
User (text input, enabled if Secure Mode checked).
Password (text input, enabled if Secure Mode checked).
Serial Settings:
Enabled (checkbox, no secure mode due to direct connection).
Admin Access:
Login/Password for config page access (default: admin/admin).
Option to change password (user remains admin).
Communication Modes:
Serial: Sends Serial.println("tick") when enabled.
TCP: Sends "tick" (or JSON with secure credentials) to specified IP/port when enabled.
HTTP: Sends POST request (with secure params if enabled) to specified URL when enabled.
Mode Selection: Only enabled modes activate on GPIO trigger; multiple modes can be enabled simultaneously (e.g., TCP + HTTP).
Security:
Secure Mode: Optional per TCP/HTTP.
If enabled, appends user and password:
HTTP: As URL parameters (e.g., ?user=xxx&password=yyy).
TCP: As JSON fields (e.g., {"event": "tick", "user": "xxx", "password": "yyy"}).
Config Page Access: Requires login with admin user and configurable password.
Persistence:
All config options (IP, modes, settings, credentials) stored in EEPROM.
Restored on reboot to maintain state.
GPIO Trigger:
Monitors pin 16 for LOW state (short to ground).
Debounced to prevent false triggers.
Sends "tick" message via enabled interfaces.
Future Enhancements
GPO (GPIO Output):
Trigger GPIO output pins (e.g., short to ground) via incoming TCP, HTTP, or Serial commands.
Configurable via web page (e.g., pin selection, trigger conditions).