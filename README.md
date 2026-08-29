# Frame Perfect Detector - Geometry Dash Geode Mod

A Geode mod for Geometry Dash 2.2 that detects and counts frame perfect timings based on your chosen FPS threshold.

## Features

- **Frame Perfect Detection**: Detects when you perform inputs with timing that qualifies as frame perfect at your selected FPS threshold
- **Customizable FPS Threshold**: Choose from 30, 60, 120, 240 FPS, or any custom value
- **Dual Display Modes**:
  - **Counter Mode**: Shows running count of frame perfects (e.g., "FP: 12")
  - **Window Mode**: Shows brief notification when frame perfect is detected (e.g., "120 FPS PERFECT")
- **Audio Feedback**: Optional ding sound with adjustable volume
- **Smart Counter**: Doesn't double-count inputs, works in normal and practice modes
- **Auto-Reset**: Optional automatic counter reset on level restart

## How It Works

### Frame Perfect Detection Algorithm

The mod uses **Geometry Dash's internal physics frame counter**, not system time or monitor refresh rate. This is crucial for accurate detection.

**Timing Window Calculation:**
- The game runs at a fixed physics rate
- At any given FPS threshold, the timing window = `1 / FPS` seconds
  - 30 FPS: 33.33ms window
  - 60 FPS: 16.67ms window
  - 120 FPS: 8.33ms window
  - 240 FPS: 4.17ms window

**Detection Process:**
1. Each game frame is tracked using a frame counter that increments based on the physics update
2. When player input is detected (jump/click), the mod records the exact frame it occurred
3. The input is validated to be within a single frame at the target FPS
4. A minimum 2-frame gap is required between inputs to avoid false positives from rapid clicking
5. Once validated, the frame perfect is counted and cannot be counted again

**Why This Works:**
- Uses GD's actual game frames, not estimated timing
- Each input is tied to a specific physics frame
- Frame duration at target FPS defines the perfect window
- Prevents double-counting through frame memory and input validation

## Settings

- **FPS Threshold**: Minimum FPS for detection (30-360)
- **Display Mode**: Counter or Window
- **Sound Enabled**: Toggle audio feedback
- **Sound Volume**: 0-100%
- **Reset on Restart**: Auto-reset counter when restarting level
- **Detect in Practice Mode**: Enable/disable detection in practice mode
- **Notification Duration**: How long window notifications appear (500-5000ms)

## Architecture

```
Input Detection (PlayLayer Hooks)
         ↓
FramePerfectDetector (Core Logic)
         ↓
Frame Validation & Counting
         ├──→ FramePerfectUI (Display)
         ├──→ Sound Manager (Audio)
         └──→ Geode Settings (Config)
```

### File Structure

- `mod.json` - Mod manifest and settings definition
- `CMakeLists.txt` - Build configuration
- `src/main.cpp` - Entry point and PlayLayer hooks
- `src/FramePerfectDetector.hpp/cpp` - Core detection logic
- `src/FPUI.hpp/cpp` - UI system for counter/notification display
- `resources/ding.wav` - Notification sound file

## Building

```bash
geode build
```

## Installing

1. Build the mod using Geode
2. Install via Geode mod manager

## Troubleshooting

**Counter not increasing:**
- Ensure you're in a normal level (not menu)
- Check that your input is actually frame perfect for the chosen threshold
- Try with a lower FPS threshold (30 or 60) first
- Verify detection is enabled for your mode (practice mode toggle)

**Sound not playing:**
- Check Sound Enabled setting
- Adjust Volume to 100%
- Ensure Geometry Dash sound effects are enabled

**UI not visible:**
- Check Display Mode setting
- In Window mode, ensure you're getting frame perfects to trigger notifications
- Try Counter mode for constant visibility

## Development Notes

The detection uses frame-based timing to ensure accuracy. The physics frame is incremented every update call, and inputs are timestamped to this frame. This approach is significantly more accurate than time-based detection because:

1. GD's physics are deterministic and frame-based
2. No reliance on system clock or monitor refresh rate
3. Works consistently across different hardware
4. Accounts for game speed/slow motion modifiers

## Compatibility

- Geometry Dash 2.2 (Windows, Mac, Android)
- Geode 3.0+

## Credits

Created as a Geode mod to help players analyze and improve their precision timing.
