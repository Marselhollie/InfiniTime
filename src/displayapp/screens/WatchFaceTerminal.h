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
                     Controllers::CalendarEventService& calendarService);

    ~WatchFaceTerminal() override;

    void Refresh() override;

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
    Controllers::CalendarEventService& calendarService;

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
    Pinetime::Utilities::DirtyValue<bool> notificationState;
    Pinetime::Utilities::DirtyValue<std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds>> currentDateTime;
    Pinetime::Utilities::DirtyValue<std::chrono::time_point<std::chrono::system_clock, std::chrono::days>> currentDate;
    Pinetime::Utilities::DirtyValue<uint8_t> batteryPercentRemaining;
    Pinetime::Utilities::DirtyValue<bool> powerPresent;
    Pinetime::Utilities::DirtyValue<uint8_t> heartbeat;
    Pinetime::Utilities::DirtyValue<bool> heartbeatRunning;
    Pinetime::Utilities::DirtyValue<bool> bleState;
    Pinetime::Utilities::DirtyValue<bool> bleRadioEnabled;
  };
}
