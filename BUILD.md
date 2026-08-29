# Building Frame Perfect Detector

This guide explains how to build the Frame Perfect Detector mod from source.

## Prerequisites

### Windows
- Visual Studio 2022 or later (with C++ workload)
- CMake 3.21+
- Geometry Dash 2.2 installed
- Geode installed (https://github.com/geode-sdk/geode)

### macOS
- Xcode 13+
- CMake 3.21+
- Geometry Dash 2.2 installed
- Geode installed

### Linux/Android
- GCC or Clang (C++20 support required)
- CMake 3.21+
- Geode SDK for Android
- NDK r21+ (for Android builds)

## Setup

### 1. Install Geode

Follow the official Geode installation guide: https://geode-sdk.github.io/docs/

### 2. Clone or Extract This Repository

```bash
git clone <repository-url>
cd frame-perfect-detector
```

or extract the provided files to a directory.

### 3. Create Build Directory

```bash
mkdir build
cd build
```

## Building

### Windows (Visual Studio)

```bash
cmake -DGEODE_SDK="C:\path\to\geode\sdk" -G "Visual Studio 17 2022" ..
cmake --build . --config Release
```

### macOS

```bash
cmake -DGEODE_SDK=/path/to/geode/sdk -G "Unix Makefiles" ..
make
```

### Linux

```bash
cmake -DGEODE_SDK=/path/to/geode/sdk -G "Unix Makefiles" ..
make
```

### Android

```bash
cmake -DGEODE_SDK=/path/to/geode/sdk \
      -DCMAKE_TOOLCHAIN_FILE=/path/to/ndk/build/cmake/android.toolchain.cmake \
      -DANDROID_ABI=arm64-v8a \
      -DANDROID_PLATFORM=android-21 \
      ..
make
```

## Output

The compiled mod will be in:
- **Windows**: `build/Release/FramePerfectDetector.geode`
- **macOS**: `build/FramePerfectDetector.geode`
- **Linux**: `build/FramePerfectDetector.geode`

## Installation

### Using Geode

```bash
geode install build/FramePerfectDetector.geode
```

or copy the `.geode` file to:
- **Windows**: `%APPDATA%/Geode/mods/`
- **macOS**: `~/Library/Application Support/Geode/mods/`
- **Linux**: `~/.local/share/Geode/mods/`

## Development Build

For development with debug symbols:

```bash
cmake -DGEODE_SDK="<path>" -DCMAKE_BUILD_TYPE=Debug ..
cmake --build . --config Debug
```

## Troubleshooting

### CMake can't find Geode SDK

**Problem**: `GEODE_SDK not defined`

**Solution**: Ensure you pass `-DGEODE_SDK=` with the correct path to Geode's SDK directory.

```bash
cmake -DGEODE_SDK="C:\Users\YourName\AppData\Local\Geode\SDK" ..
```

### Compilation Errors

**Problem**: C++ standard errors

**Solution**: Ensure your compiler supports C++20:
- MSVC: Visual Studio 2022+
- GCC: Version 10+
- Clang: Version 12+

### Geode Bindings Missing

**Problem**: "Cannot find cocos2d-x headers"

**Solution**: Update Geode SDK to latest:

```bash
geode update
```

Then rebuild.

## Code Structure

```
src/
├── main.cpp                 # Entry point, PlayLayer hooks
├── FramePerfectDetector.hpp # Core detection class
├── FramePerfectDetector.cpp # Detection implementation
├── FPUI.hpp                 # UI system
├── FPUI.cpp                 # UI implementation
```

## Making Changes

### Adding a New Setting

1. Add to `mod.json` settings section:
```json
"new_setting": {
    "name": "Display Name",
    "type": "bool",
    "default": true
}
```

2. Load in `main.cpp`:
```cpp
bool g_newSetting = Mod::get()->getSettingValue<bool>("new_setting");

Mod::get()->addSettingChangedCallback("new_setting", [](const std::string& key) {
    g_newSetting = Mod::get()->getSettingValue<bool>("new_setting");
});
```

### Modifying Detection Logic

Edit `src/FramePerfectDetector.cpp`:
- `isInputFramePerfect()`: Change validation rules
- `onGameUpdate()`: Modify frame counting
- `recordFramePerfect()`: Change tracking logic

### Modifying UI

Edit `src/FPUI.cpp`:
- Change label fonts/sizes in `init()`
- Modify positioning in `onEnter()` and `updatePosition()`
- Adjust notification fade timing in `update()`

## Testing

### Unit Test Simulation

Create a test by launching in practice mode:
1. Launch a level in practice mode
2. Tap at various points
3. Observe counter increasing (or notifications showing)
4. Verify it matches your FPS threshold

### Verification Checklist

- [ ] Counter shows "FP: 0" on level start
- [ ] Counter increments on input
- [ ] Sound plays (if enabled)
- [ ] Window notification appears (if in Window mode)
- [ ] Counter doesn't increase on rapid clicks
- [ ] Counter resets on level restart (if enabled)
- [ ] Different FPS thresholds show different counts

## Performance Profiling

To check performance impact:

1. Enable Geode's profiler (if available)
2. Launch a level
3. Monitor CPU usage with detector enabled vs disabled
4. Should be negligible (<1% impact)

## Debugging

### Enable Logging

The mod uses Geode's logging. View logs in:
- **Windows**: `%APPDATA%/Geode/Geode.log`
- **macOS**: `~/Library/Application Support/Geode/Geode.log`
- **Linux**: `~/.local/share/Geode/Geode.log`

### Log Statements Already in Code

```cpp
log::info("Frame Perfect detected! Count: {} ({}FPS)", m_fpCount, threshold);
log::info("Frame Perfect Detector: Game started");
log::error("Error playing sound: {}", e.what());
```

### Add Custom Logging

```cpp
log::debug("Current frame: {}, lastInputFrame: {}", m_currentFrame, m_lastInputFrame);
```

## Releasing

### Create Release Package

```bash
# Build
mkdir build && cd build
cmake -DGEODE_SDK="<path>" -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --config Release

# Package
cd ..
zip -r FramePerfectDetector-v1.0.0.zip build/*.geode mod.json README.md
```

### Update Version

Before releasing, update version in `mod.json`:

```json
"version": "1.1.0"
```

## Support & Issues

For issues or questions:
1. Check Geode documentation: https://geode-sdk.github.io/
2. Review mod.json settings format
3. Ensure Geometry Dash 2.2 is installed
4. Try rebuilding with fresh build directory

## Additional Resources

- Geode SDK: https://github.com/geode-sdk/geode
- Cocos2d-x Docs: https://cocos2d-x.org/reference/
- GD Modding: https://www.boomlings.com/
