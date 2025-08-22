# QMK Coding Style and Conventions

## C/C++ Code Style (.clang-format)
- Based on Google style guide
- **Indentation**: 4 spaces, no tabs
- **Column limit**: 1000 characters (very long lines allowed)
- **Pointer alignment**: Right-aligned (`int *ptr`)
- **Brace style**: Google style
- **Function style**: Empty functions on single line only
- **Case labels**: Indented
- **Include sorting**: Disabled (manual control)
- **Trailing spaces**: 1 space before trailing comments

## General Conventions (.editorconfig)
- **Line endings**: LF (Unix-style)
- **Character encoding**: UTF-8
- **Final newline**: Required
- **Trailing whitespace**: Trimmed (except Markdown)
- **YAML files**: 2-space indentation
- **Makefiles**: Tab indentation

## Python Code Style (setup.cfg)
- **Formatter**: yapf with extensive configuration
- **Linter**: flake8 with customizations
- **Line length**: 256 characters (very permissive)
- **Indentation**: 4 spaces
- **Maximum complexity**: 16 (should be reduced over time)

## Key Python Style Rules
- Align closing brackets with visual indentation
- Allow multiline dictionary keys and lambdas  
- Use spaces around operators
- Dedent closing brackets on separate lines
- Each dictionary entry on separate line
- 2 blank lines around top-level definitions

## Naming Conventions
- Follow existing patterns in the codebase
- Function names: snake_case
- Constants: UPPER_CASE
- File names: snake_case with appropriate extensions