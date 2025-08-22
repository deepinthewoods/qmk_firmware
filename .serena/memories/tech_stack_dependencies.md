# QMK Technology Stack and Dependencies

## Core Technologies
- **Primary Language**: C (embedded firmware)
- **Build System**: GNU Make with custom makefiles
- **Python Tools**: CLI tools and utilities (Python 3.7+)
- **Version Control**: Git
- **Documentation**: Markdown with Docsify

## Supported Platforms
- **AVR**: Atmel AVR microcontrollers (Arduino-compatible)
- **ARM**: ARM Cortex-M microcontrollers
- **ChibiOS**: Real-time operating system for ARM
- **STM32**: STMicroelectronics ARM processors
- **RP2040**: Raspberry Pi Pico microcontroller

## Development Environment Requirements

### Python Dependencies (requirements.txt)
- **appdirs**: Application directory utilities
- **argcomplete**: Bash tab completion
- **colorama**: Cross-platform colored terminal text
- **dotty-dict**: Dictionary access with dot notation
- **hid**: USB HID device communication
- **hjson**: Human JSON parser
- **jsonschema**: JSON schema validation (v4+)
- **milc**: CLI framework (v1.4.2+)
- **pygments**: Syntax highlighting
- **pyserial**: Serial port communication
- **pyusb**: USB device communication
- **pillow**: Image processing library

### Development Dependencies (requirements-dev.txt)
- **nose2**: Testing framework
- **flake8**: Python linting
- **pep8-naming**: PEP 8 naming conventions
- **pyflakes**: Python error detection
- **yapf**: Python code formatter

## Build Toolchain
- **GCC**: GNU Compiler Collection for cross-compilation
- **AVR-GCC**: AVR-specific GCC toolchain
- **ARM-GCC**: ARM-specific GCC toolchain
- **Make**: GNU Make build system
- **Clang**: Alternative compiler with formatting tools

## Code Quality Tools
- **clang-format**: C/C++ code formatting
- **flake8**: Python linting
- **yapf**: Python formatting
- **EditorConfig**: Consistent editor settings

## Documentation Tools
- **Docsify**: Documentation site generator
- **Doxygen**: C/C++ documentation generation
- **Markdown**: Documentation format

## Hardware Interface
- **USB HID**: Human Interface Device protocol
- **Serial**: UART communication
- **DFU**: Device Firmware Update protocol
- **Bootloaders**: Various keyboard-specific bootloaders

## Optional Features
- **Audio**: Sound and music capabilities
- **RGB**: LED lighting control
- **OLED**: Display support
- **Encoders**: Rotary encoder support
- **Split keyboards**: Inter-half communication