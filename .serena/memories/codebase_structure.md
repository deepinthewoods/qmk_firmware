# QMK Codebase Structure and Important Files

## Root Directory Structure

### Core Directories
- **quantum/**: Core QMK firmware library and features
  - Main firmware logic, action handling, matrix scanning
  - Features: audio, RGB, encoders, split keyboard support
  - Platform-independent code

- **keyboards/**: Individual keyboard definitions
  - One subdirectory per keyboard manufacturer/model
  - Each contains: config.h, rules.mk, keyboard.c, info.json
  - Keymaps in keymaps/ subdirectory

- **platforms/**: Platform-specific code
  - avr/: AVR microcontroller support
  - arm_atsam/: Microchip SAM ARM support  
  - chibios/: ChibiOS RTOS support
  - test/: Testing platform

- **tmk_core/**: Base TMK keyboard firmware
  - Low-level hardware abstraction
  - Protocol implementations (USB, Bluetooth)
  - Matrix scanning and debouncing

### Development and Build
- **util/**: Build utilities and installation scripts
  - qmk_install.sh: Development environment setup
  - Platform-specific installers in install/
  - Various helper scripts

- **builddefs/**: Build system definitions
  - Makefile fragments and build rules
  - Common build configurations

- **lib/**: External libraries and dependencies
  - Third-party code (don't modify directly)
  - Submodules and vendored dependencies

### Documentation and Configuration
- **docs/**: Comprehensive documentation
  - Getting started guides
  - Feature documentation
  - Development guides
  - API documentation

- **data/**: Configuration data and schemas
  - Keyboard database information
  - JSON schemas for validation

### User and Layout Support
- **users/**: Shared user code across keyboards
  - User-specific functions and keymaps
  - Code reuse between different keyboards

- **layouts/**: Community layout definitions
  - Standard layouts (QWERTY, Colemak, etc.)
  - Cross-keyboard layout compatibility

## Important Configuration Files

### Build System
- **Makefile**: Main build system entry point
- **paths.mk**: Path definitions for build system
- **common.mk**: Common build rules (in builddefs/)

### Code Quality
- **.clang-format**: C/C++ formatting rules
- **.editorconfig**: Editor configuration
- **setup.cfg**: Python tool configuration (flake8, yapf)

### Python Environment
- **requirements.txt**: Python runtime dependencies
- **requirements-dev.txt**: Python development dependencies

### Git and Version Control
- **.gitignore**: Files to ignore in version control
- **.gitmodules**: Git submodule definitions
- **.gitattributes**: Git file handling rules

### Development Environment
- **shell.nix**: Nix environment definition
- **.vscode/**: VS Code specific settings

## Key File Types and Purposes

### Keyboard Definition Files
- **config.h**: Hardware configuration (pins, features)
- **rules.mk**: Build rules and feature enables
- **info.json**: Keyboard metadata for configurator
- **keyboard.c/h**: Keyboard-specific code
- **keymap.c**: Individual keymap definitions

### Documentation
- **readme.md**: Project overview and quick start
- **LICENSE**: Licensing information
- **Doxyfile**: Doxygen documentation configuration

### Development
- **nose2.cfg**: Python testing configuration
- **doxygen-todo**: Documentation TODO tracker

## Navigation Tips
- Start with keyboards/ to find specific keyboard support
- Check quantum/ for core feature implementations  
- Look in docs/ for detailed feature documentation
- Use util/ scripts for environment setup
- platforms/ contains hardware-specific low-level code
- tmk_core/ is usually only modified for core protocol changes