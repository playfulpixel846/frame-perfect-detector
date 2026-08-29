#pragma once

#include <geode/Geode.hpp>
#include <cocos2d.h>

using namespace geode::prelude;
using namespace cocos2d;

enum class DisplayMode {
    Counter,  // Shows "FP: 12"
    Window    // Shows "30 FPS PERFECT" temporarily
};

class FramePerfectUI : public CCNode {
public:
    static FramePerfectUI* create();
    
    bool init() override;
    
    // Update counter display
    void setCounter(int count);
    
    // Show window notification
    void showNotification(int fpsThreshold);
    
    // Set display mode
    void setDisplayMode(DisplayMode mode);
    
    // Update every frame
    void update(float dt) override;
    
    // Called when layer is added to scene
    void onEnter() override;
    void onExit() override;
    
private:
    CCLabelBMFont* m_counterLabel;
    CCLabelBMFont* m_windowLabel;
    CCSprite* m_background;
    
    DisplayMode m_displayMode;
    float m_notificationTimer;
    float m_notificationDuration;
    int m_currentCount;
    
    bool m_isCounterVisible;
    bool m_isWindowVisible;
    
    // Positioning helpers
    void updatePosition();
    void updateLabelScale();
    
    CREATE_FUNC(FramePerfectUI);
};
