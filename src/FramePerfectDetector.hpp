#pragma once

#include <geode/Geode.hpp>
#include <geode/utils/math.hpp>
#include <cstdint>
#include <unordered_set>

using namespace geode::prelude;

/**
 * Frame Perfect Detection Algorithm:
 * 
 * A frame perfect occurs when a player's input falls within a timing window
 * of exactly 1/FPS seconds (one game frame at the specified FPS).
 * 
 * For example:
 * - 30 FPS: window = 33.33ms
 * - 60 FPS: window = 16.67ms
 * - 120 FPS: window = 8.33ms
 * - 240 FPS: window = 4.17ms
 * 
 * This detector:
 * 1. Monitors the game's physics frame counter
 * 2. Detects player input events (jumps/clicks)
 * 3. Determines if the input occurred within a valid frame window
 * 4. Uses GD's internal frame timing, not system time
 */

class FramePerfectDetector {
public:
    FramePerfectDetector();
    ~FramePerfectDetector();
    
    // Called when gameplay starts or level loads
    void onGameStart();
    
    // Called when level restarts
    void onLevelRestart();
    
    // Called when player dies/level ends
    void onLevelEnd();
    
    // Called every game physics frame
    void onGameUpdate(float dt);
    
    // Called when player input is detected
    void onPlayerInput(bool isJump);
    
    // Check if we're in a valid detection state
    bool isDetectionActive();
    
    // Get current counter value
    int getFramePerfectCount() const { return m_fpCount; }
    
    // Reset counter
    void resetCounter() { m_fpCount = 0; }
    
    // Check if a frame perfect was just triggered
    bool hasTriggeredFramePerfect() const { return m_justTriggered; }
    void clearFramePerfectTrigger() { m_justTriggered = false; }
    
private:
    // Frame tracking
    uint32_t m_currentFrame;
    uint32_t m_lastInputFrame;
    float m_frameAccumulator;
    float m_targetFrameTime; // Time per frame at target FPS
    
    // Detection state
    int m_fpCount;
    bool m_justTriggered;
    bool m_isInLevel;
    bool m_isAlive;
    
    // Input tracking to prevent double-counting
    std::unordered_set<uint32_t> m_detectedFrames;
    
    // Physics frame data
    uint32_t m_gameFrameCounter; // GD's internal frame counter
    
    // Threshold calculation
    void updateFPSThreshold();
    int getCurrentFPSThreshold();
    
    // Detection logic
    bool isInputFramePerfect(uint32_t inputFrame);
    void recordFramePerfect(uint32_t frame);
};
