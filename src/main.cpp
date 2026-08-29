#include <geode/Geode.hpp>
#include <geode/modify/PlayLayer.hpp>
#include <geode/modify/CCDirector.hpp>
#include <geode/utils/web.hpp>
#include "FramePerfectDetector.hpp"
#include "FPUI.hpp"

using namespace geode::prelude;

// Global instances
FramePerfectDetector* g_detector = nullptr;
FramePerfectUI* g_ui = nullptr;
PlayLayer* g_currentPlayLayer = nullptr;

// Mod settings
int g_fpsThreshold = 30;
bool g_soundEnabled = true;
float g_soundVolume = 1.0f;
bool g_resetOnRestart = true;
bool g_detectInPractice = true;
std::string g_displayMode = "Counter";
int g_notificationDuration = 1500;

void loadSettings() {
    auto mod = Mod::get();
    
    g_fpsThreshold = mod->getSettingValue<int64_t>("fps_threshold");
    g_soundEnabled = mod->getSettingValue<bool>("sound_enabled");
    g_soundVolume = mod->getSettingValue<int64_t>("sound_volume") / 100.0f;
    g_resetOnRestart = mod->getSettingValue<bool>("reset_on_restart");
    g_detectInPractice = mod->getSettingValue<bool>("detect_in_practice");
    g_displayMode = mod->getSettingValue<std::string>("display_mode");
    g_notificationDuration = mod->getSettingValue<int64_t>("notification_duration");
}

void playFramePerfectSound() {
    if (!g_soundEnabled) return;
    
    // Play the ding sound from resources
    // Using GameSoundManager for GD sound playback
    try {
        auto soundPath = Mod::get()->getResourcesDir() / "ding.wav";
        if (std::filesystem::exists(soundPath)) {
            // Use FMOD to play sound at specified volume
            // This will be handled by a helper function
            log::info("Playing frame perfect sound at volume: {}", g_soundVolume);
        }
    } catch (const std::exception& e) {
        log::error("Error playing sound: {}", e.what());
    }
}

// Hook into PlayLayer::update to detect frame perfects
class $modify(PlayLayer) {
    void update(float dt) {
        PlayLayer::update(dt);
        
        if (!g_detector || !g_currentPlayLayer) return;
        
        // Update detector with current frame info
        g_detector->onGameUpdate(dt);
        
        // Check if a frame perfect was just triggered
        if (g_detector->hasTriggeredFramePerfect()) {
            playFramePerfectSound();
            
            // Update UI
            if (g_ui) {
                if (g_displayMode == "Counter") {
                    g_ui->setCounter(g_detector->getFramePerfectCount());
                } else {
                    g_ui->showNotification(g_fpsThreshold);
                }
            }
            
            g_detector->clearFramePerfectTrigger();
        }
    }
    
    void onEnter() {
        PlayLayer::onEnter();
        
        g_currentPlayLayer = this;
        
        if (!g_detector) {
            g_detector = new FramePerfectDetector();
        }
        
        // Create or show UI
        if (!g_ui) {
            g_ui = FramePerfectUI::create();
            this->addChild(g_ui, 9999);
        }
        
        // Update UI settings
        DisplayMode mode = (g_displayMode == "Counter") ? DisplayMode::Counter : DisplayMode::Window;
        if (g_ui) {
            g_ui->setDisplayMode(mode);
            g_ui->setCounter(g_detector->getFramePerfectCount());
        }
        
        g_detector->onGameStart();
    }
    
    void onExit() {
        if (g_resetOnRestart && g_detector) {
            g_detector->resetCounter();
        }
        
        g_currentPlayLayer = nullptr;
        PlayLayer::onExit();
    }
};

// Hook into input to detect player actions
class $modify(PlayLayer) {
    void handleButtonDown(PlayerButton button) {
        PlayLayer::handleButtonDown(button);
        
        if (g_detector && (button == PlayerButton::Jump || button == PlayerButton::Rotate)) {
            g_detector->onPlayerInput(button == PlayerButton::Jump);
        }
    }
};

// Initialize mod on load
$on_mod(Loaded) {
    log::info("Frame Perfect Detector loaded!");
    
    loadSettings();
    
    // Watch for setting changes
    Mod::get()->addSettingChangedCallback("fps_threshold", [](const std::string& key) {
        g_fpsThreshold = Mod::get()->getSettingValue<int64_t>("fps_threshold");
        log::info("FPS threshold changed to: {}", g_fpsThreshold);
    });
    
    Mod::get()->addSettingChangedCallback("sound_enabled", [](const std::string& key) {
        g_soundEnabled = Mod::get()->getSettingValue<bool>("sound_enabled");
    });
    
    Mod::get()->addSettingChangedCallback("sound_volume", [](const std::string& key) {
        g_soundVolume = Mod::get()->getSettingValue<int64_t>("sound_volume") / 100.0f;
    });
    
    Mod::get()->addSettingChangedCallback("reset_on_restart", [](const std::string& key) {
        g_resetOnRestart = Mod::get()->getSettingValue<bool>("reset_on_restart");
    });
    
    Mod::get()->addSettingChangedCallback("detect_in_practice", [](const std::string& key) {
        g_detectInPractice = Mod::get()->getSettingValue<bool>("detect_in_practice");
    });
    
    Mod::get()->addSettingChangedCallback("display_mode", [](const std::string& key) {
        g_displayMode = Mod::get()->getSettingValue<std::string>("display_mode");
        if (g_ui) {
            DisplayMode mode = (g_displayMode == "Counter") ? DisplayMode::Counter : DisplayMode::Window;
            g_ui->setDisplayMode(mode);
        }
    });
    
    Mod::get()->addSettingChangedCallback("notification_duration", [](const std::string& key) {
        g_notificationDuration = Mod::get()->getSettingValue<int64_t>("notification_duration");
    });
}
