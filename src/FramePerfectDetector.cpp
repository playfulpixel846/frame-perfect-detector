#include "FramePerfectDetector.hpp"
#include <geode/Geode.hpp>

using namespace geode::prelude;

FramePerfectDetector::FramePerfectDetector()
    : m_currentFrame(0),
      m_lastInputFrame(0),
      m_frameAccumulator(0.0f),
      m_targetFrameTime(1.0f / 60.0f),
      m_fpCount(0),
      m_justTriggered(false),
      m_isInLevel(false),
      m_isAlive(false),
      m_gameFrameCounter(0) {
    updateFPSThreshold();
}

FramePerfectDetector::~FramePerfectDetector() {
}

void FramePerfectDetector::onGameStart() {
    m_isInLevel = true;
    m_isAlive = true;
    m_currentFrame = 0;
    m_lastInputFrame = 0;
    m_frameAccumulator = 0.0f;
    m_gameFrameCounter = 0;
    m_detectedFrames.clear();
    updateFPSThreshold();
    
    log::info("Frame Perfect Detector: Game started");
}

void FramePerfectDetector::onLevelRestart() {
    m_currentFrame = 0;
    m_lastInputFrame = 0;
    m_frameAccumulator = 0.0f;
    m_gameFrameCounter = 0;
    m_detectedFrames.clear();
    m_isAlive = true;
    
    log::info("Frame Perfect Detector: Level restarted");
}

void FramePerfectDetector::onLevelEnd() {
    m_isInLevel = false;
    m_isAlive = false;
    
    log::info("Frame Perfect Detector: Level ended");
}

void FramePerfectDetector::onGameUpdate(float dt) {
    if (!m_isInLevel || !m_isAlive) return;
    
    // Accumulate frame time
    m_frameAccumulator += dt;
    
    // Increment frame counter based on target frame time
    // GD typically runs at 60 physics frames per second
    while (m_frameAccumulator >= m_targetFrameTime) {
        m_frameAccumulator -= m_targetFrameTime;
        m_currentFrame++;
        m_gameFrameCounter++;
    }
}

void FramePerfectDetector::onPlayerInput(bool isJump) {
    if (!m_isInLevel || !m_isAlive) return;
    
    // Check if this input triggers a frame perfect
    if (isInputFramePerfect(m_currentFrame)) {
        recordFramePerfect(m_currentFrame);
    }
}

bool FramePerfectDetector::isDetectionActive() {
    return m_isInLevel && m_isAlive;
}

void FramePerfectDetector::updateFPSThreshold() {
    auto mod = Mod::get();
    int fpsThreshold = mod->getSettingValue<int64_t>("fps_threshold");
    
    // Calculate the target frame time for the specified FPS
    // This defines our timing window
    // For 60 FPS detection: window = 1/60 = 0.01667 seconds
    // For 120 FPS detection: window = 1/120 = 0.00833 seconds
    m_targetFrameTime = 1.0f / static_cast<float>(fpsThreshold);
}

int FramePerfectDetector::getCurrentFPSThreshold() {
    return static_cast<int>(1.0f / m_targetFrameTime);
}

bool FramePerfectDetector::isInputFramePerfect(uint32_t inputFrame) {
    // Avoid double-counting the same frame
    if (m_detectedFrames.count(inputFrame) > 0) {
        return false;
    }
    
    // A frame perfect occurs when the input lands in a single frame window
    // at the specified FPS threshold.
    // 
    // The timing window is defined by:
    // - Frame starts at: (currentFrame - 1) * (targetFrameTime)
    // - Frame ends at: (currentFrame) * (targetFrameTime)
    //
    // Since we're working with discrete frames, an input is "frame perfect"
    // if it occurs in the current game frame at our target FPS.
    
    // Calculate the frame window (in game frames at target FPS)
    uint32_t framesSinceLastInput = inputFrame - m_lastInputFrame;
    
    // Require at least 2 frames since last input to avoid accidental double-detection
    // This prevents rapid successive clicks from being counted multiple times
    if (framesSinceLastInput < 2) {
        return false;
    }
    
    // The input is frame perfect if it falls within a single target-FPS frame
    // Since we're using discrete frame counting, each frame is exactly "perfect"
    // at the target FPS. To avoid false positives, we check that this is a
    // meaningful input, not just rapid clicking.
    
    m_lastInputFrame = inputFrame;
    return true;
}

void FramePerfectDetector::recordFramePerfect(uint32_t frame) {
    m_detectedFrames.insert(frame);
    m_fpCount++;
    m_justTriggered = true;
    
    int threshold = getCurrentFPSThreshold();
    log::info("Frame Perfect detected! Count: {} ({}FPS)", m_fpCount, threshold);
}
