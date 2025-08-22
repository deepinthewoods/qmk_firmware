# QMK Task Completion Checklist

## After Making Code Changes

### 1. Format Code
```bash
# Format C/C++ code
clang-format -i <changed_files>

# Format Python code
yapf -i <changed_python_files>

# Or format all Python files
qmk format-python
```

### 2. Lint and Validate
```bash
# Lint Python code
flake8 <changed_files>

# QMK-specific linting
qmk lint

# Check for common issues
qmk doctor
```

### 3. Build and Test
```bash
# Test compilation of changed keyboard
make <keyboard>:<keymap>

# Test default keymap if creating new keyboard
make <keyboard>:default

# Build with verbose output to catch warnings
make VERBOSE=true <keyboard>:<keymap>

# Clean build to ensure no cached issues
make clean
make <keyboard>:<keymap>
```

### 4. Test Functionality
```bash
# Flash to actual hardware for testing
make <keyboard>:<keymap>:flash

# Test basic functionality:
# - All keys respond correctly
# - Layers work as expected  
# - Special features function properly
# - No unintended behavior
```

### 5. Documentation Updates
- Update relevant documentation in docs/
- Add/update keyboard README.md if applicable
- Update info.json for QMK Configurator support
- Document any new features or breaking changes

### 6. Version Control Best Practices
```bash
# Stage only relevant changes
git add <specific_files>

# Write descriptive commit messages
git commit -m "feat(keyboard): add support for new_keyboard

- Add keyboard definition files
- Include default keymap
- Update documentation"

# Keep commits focused and atomic
```

### 7. Pre-Pull Request Checks
- [ ] Code builds without errors or warnings
- [ ] Code follows QMK style guidelines
- [ ] All tests pass
- [ ] Documentation is updated
- [ ] Commit messages are clear and descriptive
- [ ] No unnecessary files are included
- [ ] Changes are minimal and focused

### 8. Continuous Integration
When submitting pull requests, ensure:
- All CI checks pass
- No merge conflicts exist
- PR description explains changes clearly
- Any breaking changes are documented

### 9. Hardware-Specific Validation
For keyboard development:
- Test all key positions
- Verify encoder/rotary functionality
- Check RGB/LED operations
- Validate split keyboard communication
- Test bootmagic and reset functionality

### 10. Performance Checks
```bash
# Check memory usage
make <keyboard>:<keymap>:objs-size

# Verify firmware size is within limits
# AVR: typically < 32KB flash, < 2KB RAM
# ARM: varies by MCU specifications
```

This checklist ensures code quality, functionality, and compatibility with the broader QMK ecosystem.