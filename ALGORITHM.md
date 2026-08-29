# Frame Perfect Detection Algorithm

## Overview

This document explains the frame perfect detection algorithm used in the Frame Perfect Detector mod. The algorithm detects when a player's input (jump/click) occurs within a timing window of `1/FPS` seconds using **Geometry Dash's internal physics frame counter**.

## Key Principle

> **Do NOT use system time or monitor refresh rate. Use GD's actual game physics frames.**

## Physics Frame System

### How GD Counts Frames

Geometry Dash uses a fixed physics simulation that runs at a deterministic rate:

1. Each game update tick, the physics frame counter increments
2. This is independent of the actual monitor refresh rate
3. The frame counter is consistent across all platforms
4. This allows for frame-perfect calculations

### Frame Time at Different FPS Thresholds

```
Frame Duration = 1 / FPS

30 FPS:  33.33 ms per frame
60 FPS:  16.67 ms per frame
120 FPS: 8.33 ms per frame
240 FPS: 4.17 ms per frame
```

## Detection Process

### Step 1: Frame Accumulation

The detector tracks accumulated time to determine which physics frame we're currently in:

```cpp
m_frameAccumulator += dt;
if (m_frameAccumulator >= m_targetFrameTime) {
    m_frameAccumulator -= m_targetFrameTime;
    m_currentFrame++;
}
```

This converts continuous time (from the game loop) into discrete frames at the target FPS.

### Step 2: Input Detection

When a player input occurs (jump or click), we record the exact frame:

```cpp
void FramePerfectDetector::onPlayerInput(bool isJump) {
    if (isInputFramePerfect(m_currentFrame)) {
        recordFramePerfect(m_currentFrame);
    }
}
```

### Step 3: Frame Perfect Validation

An input qualifies as frame perfect if:

1. **It occurs within a single frame window** at the target FPS
   - Since we're discretizing time into frames, each input naturally falls within one frame at the target FPS

2. **Sufficient gap since last input** (minimum 2 frames)
   - Prevents double-counting from the same tap/click
   - Filters out accidental rapid successive inputs
   - Example: `framesSinceLastInput >= 2`

3. **Not already counted in this frame**
   - Track detected frames in a set to prevent re-triggering
   - Clear on level restart

### Step 4: Counter Update

Once an input passes validation:

```cpp
void FramePerfectDetector::recordFramePerfect(uint32_t frame) {
    m_detectedFrames.insert(frame);  // Record frame to prevent double-count
    m_fpCount++;                      // Increment counter
    m_justTriggered = true;           // Signal UI/sound to trigger
}
```

## Why This Approach Works

### Advantages Over Time-Based Detection

**Problem:** Measuring milliseconds between clicks
```
Click 1 at t=100ms
Click 2 at t=116.67ms
Difference = 16.67ms ≈ 60 FPS window

BUT: This doesn't account for:
- Network jitter
- System scheduling variations
- Monitor refresh rate desync
- Actual GD physics timing
```

**Solution:** Frame-based detection
```
Click 1 at Frame 100
Click 2 at Frame 101
Exactly 1 frame apart = FRAME PERFECT at target FPS

This works because:
- Frames are deterministic in GD
- No timing drift over time
- Works regardless of system load
- Consistent across hardware
```

### Example Walkthrough

**Scenario: 60 FPS threshold, player clicks**

1. **Frame 0-10**: Game running normally
   - Each frame = 16.67ms duration
   - Frame accumulator: increments by dt each update

2. **Frame 11: Player clicks (jump)**
   - Input detected at Frame 11
   - Check: `framesSinceLastInput >= 2`? YES (first click)
   - Check: `not in m_detectedFrames`? YES
   - Add Frame 11 to m_detectedFrames
   - Increment counter: FP = 1
   - Trigger: Play sound, update UI

3. **Frame 11-12: Player holds input**
   - Input might still register
   - Check: Frame 12, but frame already recorded?
   - Might be detected, but prevented by minimum 2-frame gap
   - OR: Already in m_detectedFrames → Skip

4. **Frame 13: Next input**
   - framesSinceLastInput = 13 - 11 = 2
   - Check passes!
   - If frame perfect: FP = 2

## Edge Cases Handled

### 1. Rapid Clicking
```
Click 1 at Frame 100
Click 2 at Frame 101 (only 1 frame later)
Result: Frame 101 skipped (gap < 2 frames required)
Reason: Prevents accidental double-count from rapid clicks
```

### 2. Hold Input
```
Click pressed at Frame 100
Still held at Frames 101, 102, 103
Result: Only counted once
Reason: m_detectedFrames prevents re-triggering
```

### 3. Level Restart
```
Player taps → FP = 5
Player dies and restarts → m_detectedFrames cleared
Next tap → counted normally, FP can increment from 5
(or reset to 0 if auto-reset enabled)
```

### 4. Mode Changes
- Practice mode toggle: Respects detection setting
- Display mode change: Counter ↔ Window
- FPS threshold change: m_targetFrameTime recalculated

## Pseudo-Code Reference

```pseudocode
class FramePerfectDetector:
    targetFrameTime = 1 / FPS_THRESHOLD
    frameAccumulator = 0
    currentFrame = 0
    fpCount = 0
    lastInputFrame = 0
    detectedFrames = {}
    
    function onGameUpdate(dt):
        frameAccumulator += dt
        while frameAccumulator >= targetFrameTime:
            frameAccumulator -= targetFrameTime
            currentFrame++
    
    function onPlayerInput():
        if isInputFramePerfect(currentFrame):
            recordFramePerfect(currentFrame)
    
    function isInputFramePerfect(inputFrame):
        framesSinceLastInput = inputFrame - lastInputFrame
        if framesSinceLastInput < 2:
            return false
        if inputFrame in detectedFrames:
            return false
        lastInputFrame = inputFrame
        return true
    
    function recordFramePerfect(frame):
        detectedFrames.add(frame)
        fpCount++
        triggerEvent()  // Play sound, update UI
```

## Implementation Considerations

### Frame Counter Precision
- Use `uint32_t` for frame counter (won't overflow for hours of gameplay)
- At 60 FPS: overflow after ~280 million frames (109+ days)

### Floating Point Accumulation
- Accumulate dt into frameAccumulator
- Compare against targetFrameTime with sufficient precision
- Reset accumulator after threshold crossed (prevents drift)

### Input Detection Hook
- Hook into PlayLayer's input handling
- Works with both keyboard and touch input
- Detects jump and rotate inputs (rotation timings are also meaningful)

### UI Threading
- Geode handles UI thread safety
- Use `scheduleUpdate()` for per-frame UI updates
- CCLabel for counter display, opacity changes for notifications

## Performance Considerations

- Frame counter increment: O(1)
- Input validation: O(1) set lookup
- Memory: Small (one set of frame numbers, cleared on level restart)
- CPU: Negligible (microseconds per detection)

## Validation & Testing

To verify the algorithm works correctly:

1. **Manual Test: 30 FPS threshold**
   - One input = one frame perfect
   - Multiple inputs spaced 2+ frames = multiple frame perfects
   - No false positives for rapid clicks

2. **Manual Test: 60 FPS threshold**
   - Window is tighter (8.33ms)
   - Should be harder to get frame perfects
   - Test with same input as 30 FPS - fewer should count

3. **Edge Case Test**
   - Hold jump: Count increases only once
   - Rapid tap-tap-tap: Only counts valid frame gaps
   - Level restart: Counter resets if enabled

## Conclusion

The frame perfect detector uses GD's deterministic physics frame system to accurately measure timing windows. By working with discrete frames instead of continuous time, we achieve precision that's independent of system load, monitor refresh rate, and network conditions.
