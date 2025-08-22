# QMK Firmware Project Overview

## Purpose
QMK (Quantum Mechanical Keyboard) is an open-source keyboard firmware framework based on tmk_keyboard firmware. It provides advanced features for various microcontrollers including Atmel AVR and ARM controllers, with specific focus on OLKB, ErgoDox EZ, and Clueboard keyboards.

## Key Features
- Supports hundreds of keyboards (in keyboards/ directory)
- Advanced keymap features including layers, macros, tap dance, etc.
- RGB lighting and LED matrix support
- Split keyboard support
- Audio/MIDI capabilities
- VIA/VIAL configurator support
- Encoder/rotary support
- Extensive customization options

## Project Structure
- **keyboards/**: Individual keyboard definitions and keymaps
- **quantum/**: Core QMK firmware library
- **platforms/**: Platform-specific code (AVR, ARM, ChibiOS, etc.)
- **tmk_core/**: Base TMK keyboard firmware
- **docs/**: Comprehensive documentation
- **util/**: Development and build utilities
- **lib/**: External libraries and dependencies
- **users/**: User-specific code sharing across keyboards

## Official Resources
- Website: https://qmk.fm
- Documentation: https://docs.qmk.fm
- Discord: https://discord.gg/Uq7gcHh