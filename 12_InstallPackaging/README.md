# Greeter Packaging Demo

This project implements a small localized greeting tool that demonstrates
packaging workflows: shared library usage, translation catalogs, manpages,
unit tests, documentation extraction, and installation/uninstallation
through CMake.

## Features
- Environment adaptation via CMake with GNUInstallDirs.
- Shared library `libgreeter` used by the `greet_tool` executable.
- gettext-based translations (English and Russian) with runtime locale
  switching.
- Unit tests via CTest.
- Manpage for the CLI tool.
- Documentation generated from header comments plus a narrative guide.
- Install and uninstall targets suitable for system-wide deployment.

## Building
```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
```

## Documentation
`cmake --build build --target docs` generates `docs/generated.md` from
header comments. The source narrative lives in `docs/project.md`.

## Testing
```bash
ctest --test-dir build
```

## Installation
Install locally (use `sudo` for system prefixes):
```bash
cmake --install build
```

Uninstall using the generated manifest:
```bash
cmake --build build --target uninstall
```

## Running
Use `greet_tool` with different locales:
```bash
LANG=en_US.UTF-8 ./build/greet_tool --name Ada
LANG=ru_RU.UTF-8 ./build/greet_tool --name Ada
```
