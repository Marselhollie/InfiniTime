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

  private:
    void NextMantra();
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

    lv_obj_t* container;
    lv_obj_t* notificationIcon;
    lv_obj_t* labelPrompt1;
    lv_obj_t* labelTime;
    lv_obj_t* labelDate;
    lv_obj_t* heartbeatValue;
    lv_obj_t* connectState;
    lv_obj_t* batteryIcon;
    lv_obj_t* labelMantra;
    lv_obj_t* labelCalendar;

    lv_task_t* taskRefresh;
    lv_task_t* taskMantra;

    uint8_t mantraIndex;
    uint32_t lastMantraUpdateTime;

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