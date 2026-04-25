#pragma once

#include <cstdint>
#include <chrono>
#include "components/datetime/DateTimeController.h"
#include "components/battery/BatteryController.h"
#include "components/ble/BleController.h"
#include "components/ble/NotificationManager.h"
#include "components/heartrate/HeartRateController.h"
#include "components/motion/MotionController.h"
#include "components/settings/Settings.h"
#include "components/ble/SimpleWeatherService.h"
#include "components/ble/CalendarEventService.h"
#include "displayapp/screens/Screen.h"
#include "utility/DirtyValue.h"

// Forward declaration
void MantraTaskCallback(lv_task_t* task);

namespace Pinetime::Applications::Screens {
  class WatchFaceTerminal;
}

namespace Pinetime::Applications::Screens {

  class WatchFaceTerminal : public Screen {
  public:
    WatchFaceTerminal(Controllers::DateTime& dateTimeController,
                     const Controllers::Battery& batteryController,
                     const Controllers::Ble& bleController,
                     Controllers::NotificationManager& notificationManager,
                     Controllers::Settings& settingsController,
                     Controllers::HeartRateController& heartRateController,
                     Controllers::MotionController& motionController,
                     Controllers::SimpleWeatherService& weatherService,
                     Controllers::CalendarEventService* calendarService = nullptr);

    ~WatchFaceTerminal() override;

    void Refresh() override;

    static bool IsAvailable() { return true; }

    void NextMantra();

  private:
    friend void ::MantraTaskCallback(lv_task_t*);
    void UpdateCalendarDisplay();
    void UpdateMantraDisplay();

    Controllers::DateTime& dateTimeController;
    const Controllers::Battery& batteryController;
    const Controllers::Ble& bleController;
    Controllers::NotificationManager& notificationManager;
    Controllers::Settings& settingsController;
    Controllers::HeartRateController& heartRateController;
    Controllers::MotionController& motionController;
    Controllers::SimpleWeatherService& weatherService;
    Controllers::CalendarEventService* calendarService;

    // Display elements
    lv_obj_t* container;
    lv_obj_t* notificationIcon;
    lv_obj_t* labelPrompt1;  // Time
    lv_obj_t* labelTime;
    lv_obj_t* labelDate;
    lv_obj_t* heartbeatValue;
    lv_obj_t* connectState;
    lv_obj_t* batteryIcon;
    lv_obj_t* labelMantra;     // Scrolling mantras (top line - always on)
    lv_obj_t* labelCalendar;   // Scrolling calendar events (bottom line)

    // Tasks
    lv_task_t* taskRefresh;
    lv_task_t* taskMantra;

    // Mantra management
    uint8_t mantraIndex;
    uint32_t lastMantraUpdateTime;

    // State tracking
    Pinetime::Utility::DirtyValue<bool> notificationState;
    Pinetime::Utility::DirtyValue<std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds>> currentDateTime;
    Pinetime::Utility::DirtyValue<std::chrono::time_point<std::chrono::system_clock, std::chrono::days>> currentDate;
    Pinetime::Utility::DirtyValue<uint8_t> batteryPercentRemaining;
    Pinetime::Utility::DirtyValue<bool> powerPresent;
    Pinetime::Utility::DirtyValue<uint8_t> heartbeat;
    Pinetime::Utility::DirtyValue<bool> heartbeatRunning;
    Pinetime::Utility::DirtyValue<bool> bleState;
    Pinetime::Utility::DirtyValue<bool> bleRadioEnabled;
  };
}

namespace Pinetime::Applications {
  template <>
  struct WatchFaceTraits<WatchFace::Terminal> {
    static constexpr WatchFace watchFace = WatchFace::Terminal;
    static constexpr const char* name = "Terminal";

    static Screens::Screen* Create(DisplayApp* app,
                                   Controllers::DateTime& dateTimeController,
                                   const Controllers::Battery& batteryController,
                                   const Controllers::Ble& bleController,
                                   Controllers::NotificationManager& notificationManager,
                                   Controllers::Settings& settingsController,
                                   Controllers::HeartRateController& heartRateController,
                                   Controllers::MotionController& motionController,
                                   Controllers::SimpleWeatherService& weatherService) {
      return new Screens::WatchFaceTerminal(dateTimeController,
                                            batteryController,
                                            bleController,
                                            notificationManager,
                                            settingsController,
                                            heartRateController,
                                            motionController,
                                            weatherService,
                                            nullptr);
    }

    static bool IsAvailable() { return true; }
  };
}
