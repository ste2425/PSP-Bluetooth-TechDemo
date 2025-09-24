# CMFileManager PSP [![C/C++ CI](https://github.com/ste2425/PSP-Bluetooth-TechDemo/actions/workflows/c-cpp.yml/badge.svg?branch=next)](https://github.com/ste2425/PSP-Bluetooth-TechDemo/actions/workflows/c-cpp.yml) ![Github latest downloads](https://img.shields.io/github/downloads/ste2425/PSP-Bluetooth-TechDemo/total.svg)

PSP Bluetooth Tech Demo is a PSP application built using the unofficial PSPSDK and [glib2d (modded)](https://github.com/joel16/gLib2D). The code base for this project is based on the brilliant [CMFileManager](https://github.com/joel16/CMFileManager-PSP) project by *joel16*. It leverages the equally good SIO library from the [Operation Ditto](https://github.com/operation-ditto) project.

The purpose of this homebrew is to demonstrate the `PSP Bluetooth` project and how the PSP can interface with bluetooth controllers.

**Features:**

- PONG demo demonstrating the use of two controllers
- LED Colour demo demonstrating setting the colour of controllers which support LEDs (DualSense etc)
- Vibration demo. Demonstrating the use of setting rumble including controllers which support srong and weak rumble.
- Player LED demo. Demonstrating setting the player LED of conrollers.
- Ping. Demonstratings pinging the ESP32 to be sure if the `PSP Bluetooth Dongle` is connected.
- Controller type detection. Demonstrates requesting controller type and bettery information.
- Enabling/disabling new connections. Demonstrates enabling new controller connections when pairing.

TODO add pictures and achnowledgements and setup instructions
