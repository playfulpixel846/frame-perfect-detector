#include "FPUI.hpp"
#include <geode/Geode.hpp>

using namespace geode::prelude;

FramePerfectUI* FramePerfectUI::create() {
    auto ret = new FramePerfectUI();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool FramePerfectUI::init() {
    if (!CCNode::init()) return false;
    
    m_displayMode = DisplayMode::Counter;
    m_notificationTimer = 0.0f;
    m_notificationDuration = 1.5f;
    m_currentCount = 0;
    m_isCounterVisible = true;
    m_isWindowVisible = false;
    
    this->setAnchorPoint({1.0f, 0.0f}); // Bottom-right
    this->setPosition({-10.0f, 10.0f});  // Margin from bottom-right
    
    // Create counter label (default display)
    m_counterLabel = CCLabelBMFont::create("FP: 0", "goldFont.fnt");
    if (m_counterLabel) {
        m_counterLabel->setAnchorPoint({1.0f, 0.0f});
        m_counterLabel->setPosition({0.0f, 0.0f});
        m_counterLabel->setScale(0.8f);
        this->addChild(m_counterLabel, 1);
    }
    
    // Create window label (notification display)
    m_windowLabel = CCLabelBMFont::create("", "goldFont.fnt");
    if (m_windowLabel) {
        m_windowLabel->setAnchorPoint({1.0f, 0.0f});
        m_windowLabel->setPosition({0.0f, 0.0f});
        m_windowLabel->setScale(0.7f);
        m_windowLabel->setOpacity(0);
        this->addChild(m_windowLabel, 1);
    }
    
    // Create background for visibility
    m_background = CCSprite::create("GJ_button_01.png");
    if (m_background) {
        m_background->setAnchorPoint({1.0f, 0.0f});
        m_background->setPosition({5.0f, 0.0f});
        m_background->setScale(0.5f);
        m_background->setOpacity(150);
        this->addChild(m_background, 0);
    }
    
    this->scheduleUpdate();
    
    return true;
}

void FramePerfectUI::setCounter(int count) {
    m_currentCount = count;
    
    if (m_counterLabel && m_displayMode == DisplayMode::Counter) {
        std::string text = "FP: " + std::to_string(count);
        m_counterLabel->setString(text.c_str());
    }
}

void FramePerfectUI::showNotification(int fpsThreshold) {
    if (m_displayMode != DisplayMode::Window) return;
    
    if (m_windowLabel) {
        std::string text = std::to_string(fpsThreshold) + " FPS\nPERFECT";
        m_windowLabel->setString(text.c_str());
        m_windowLabel->setOpacity(255);
    }
    
    // Reset timer
    m_notificationTimer = 0.0f;
}

void FramePerfectUI::setDisplayMode(DisplayMode mode) {
    m_displayMode = mode;
    
    if (mode == DisplayMode::Counter) {
        if (m_counterLabel) {
            m_counterLabel->setOpacity(255);
            std::string text = "FP: " + std::to_string(m_currentCount);
            m_counterLabel->setString(text.c_str());
        }
        if (m_windowLabel) {
            m_windowLabel->setOpacity(0);
        }
    } else {
        if (m_counterLabel) {
            m_counterLabel->setOpacity(0);
        }
    }
}

void FramePerfectUI::update(float dt) {
    CCNode::update(dt);
    
    if (m_displayMode == DisplayMode::Window) {
        // Update notification timer
        m_notificationTimer += dt;
        
        auto mod = Mod::get();
        float notifDuration = mod->getSettingValue<int64_t>("notification_duration") / 1000.0f;
        
        if (m_notificationTimer > notifDuration) {
            if (m_windowLabel) {
                // Fade out
                float fadeProgress = (m_notificationTimer - notifDuration) / 0.3f;
                if (fadeProgress >= 1.0f) {
                    m_windowLabel->setOpacity(0);
                } else {
                    m_windowLabel->setOpacity(static_cast<GLubyte>(255 * (1.0f - fadeProgress)));
                }
            }
        }
    }
}

void FramePerfectUI::onEnter() {
    CCNode::onEnter();
    
    // Position at bottom-right of the screen
    auto director = CCDirector::sharedDirector();
    auto screenSize = director->getWinSize();
    
    this->setPosition(screenSize.width - 10.0f, 10.0f);
}

void FramePerfectUI::onExit() {
    this->unscheduleUpdate();
    CCNode::onExit();
}
