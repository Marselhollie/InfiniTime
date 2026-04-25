#include <lvgl/lvgl.h>

#include "displayapp/screens/WatchFaceTerminal.h"
#include "components/battery/BatteryController.h"
#include "components/ble/BleController.h"
#include "components/ble/NotificationManager.h"
#include "components/heartrate/HeartRateController.h"
#include "components/motion/MotionController.h"
#include "components/settings/Settings.h"
#include "components/ble/SimpleWeatherService.h"
#include "displayapp/screens/WeatherSymbols.h"
#include "displayapp/InfiniTimeTheme.h"
#include <string>

using namespace Pinetime::Applications::Screens;

LV_FONT_DECLARE(jetbrains_mono_bold_20)

static const char* mantras[] = {
  "BREATHE 5 DEEP TIMES",
  "HOLD EYE CONTACT REPLYING",
  "BE DIRECT W/O BEING HOSTILE",
  "SPEAK IN OPTIMUM PITCH",
  "SHIFT ATTENTION AND MOOD IN CONVERSATION",
  "REMOVE I · SHIFT TO INVITATIONAL PHRASING",
  "GRATEFUL 3 THINGS",
  "READ PEOPLES SOCIAL AURAS AND DELIVERY",
  "|NOCOFFEE|$TRETCH/WITHOLD_MONEY|SAYNOTOPORN",
  "RE-CALM NON VERBALS"
};
static constexpr uint8_t mantraCount = sizeof(mantras) / sizeof(mantras[0]);

static void MantraTaskCallback(lv_task_t* task) {
  auto* screen = static_cast<WatchFaceTerminal*>(task->user_data);
  screen->NextMantra();
}

WatchFaceTerminal::WatchFaceTerminal(Controllers::DateTime& dateTimeController,
                                     const Controllers::Battery& batteryController,
                                     const Controllers::Ble& bleController,
                                     Controllers::NotificationManager& notificationManager,
                                     Controllers::Settings& settingsController,
                                     Controllers::HeartRateController& heartRateController,
                                     Controllers::MotionController& motionController,
                                     Controllers::SimpleWeatherService& weatherService,
                                     Controllers::CalendarEventService& calendarService)
  : currentDateTime {{}},
    dateTimeController {dateTimeController},
    batteryController {batteryController},
    bleController {bleController},
    notificationManager {notificationManager},
    settingsController {settingsController},
    heartRateController {heartRateController},
    motionController {motionController},
    weatherService {weatherService},
    calendarService {calendarService} {

  container = lv_cont_create(lv_scr_act(), nullptr);
  lv_cont_set_layout(container, LV_LAYOUT_COLUMN_LEFT);
  lv_cont_set_fit(container, LV_FIT_TIGHT);
  lv_obj_set_style_local_pad_inner(container, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, -3);
  lv_obj_set_style_local_bg_opa(container, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_TRANSP);

  notificationIcon = lv_label_create(container, nullptr);

  labelPrompt1 = lv_label_create(container, nullptr);
  lv_obj_set_style_local_text_color(labelPrompt1, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, Colors::lightGray);
  lv_obj_set_style_local_text_font(labelPrompt1, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &jetbrains_mono_extrabold_compressed);
  lv_label_set_text_static(labelPrompt1, "00:00 AM");

  labelTime = lv_label_create(container, nullptr);
  lv_label_set_recolor(labelTime, true);
  lv_obj_set_hidden(labelTime, true);

  labelDate = lv_label_create(container, nullptr);
  lv_label_set_recolor(labelDate, true);

  heartbeatValue = lv_label_create(container, nullptr);
  lv_label_set_recolor(heartbeatValue, true);

  connectState = lv_label_create(container, nullptr);
  lv_label_set_recolor(connectState, true);

  lv_obj_align(container, nullptr, LV_ALIGN_IN_TOP_LEFT, 0, 7);

  // Battery as text label inside the container (default terminal position)
  batteryIcon = lv_label_create(container, nullptr);
  lv_label_set_recolor(batteryIcon, true);
  lv_label_set_text_static(batteryIcon, "#ffffff [PWR]# ---%");

  mantraIndex = xTaskGetTickCount() % mantraCount;
  
  // Top scrolling line: Mantras (always visible)
  labelMantra = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_text_color(labelMantra, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_CYAN);
  lv_obj_set_style_local_text_font(labelMantra, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &jetbrains_mono_bold_20);
  lv_label_set_long_mode(labelMantra, LV_LABEL_LONG_SROLL_CIRC);
  lv_obj_set_width(labelMantra, 240);
  lv_obj_set_height(labelMantra, 44);
  lv_label_set_text(labelMantra, mantras[mantraIndex]);
  lv_obj_align(labelMantra, nullptr, LV_ALIGN_IN_BOTTOM_MID, 0, -44);  // Middle area

  // Bottom scrolling line: Calendar events
  labelCalendar = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_text_color(labelCalendar, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_LIME);
  lv_obj_set_style_local_text_font(labelCalendar, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &jetbrains_mono_bold_20);
  lv_label_set_long_mode(labelCalendar, LV_LABEL_LONG_SROLL_CIRC);
  lv_obj_set_width(labelCalendar, 240);
  lv_obj_set_height(labelCalendar, 44);
  lv_label_set_text_static(labelCalendar, "");  // Empty initially
  lv_obj_align(labelCalendar, nullptr, LV_ALIGN_IN_BOTTOM_LEFT, 0, 0);

  taskRefresh = lv_task_create(RefreshTaskCallback, LV_DISP_DEF_REFR_PERIOD, LV_TASK_PRIO_MID, this);
  taskMantra = lv_task_create(MantraTaskCallback, 24 * 60 * 60 * 1000u, LV_TASK_PRIO_LOW, this);
  Refresh();
}

WatchFaceTerminal::~WatchFaceTerminal() {
  lv_task_del(taskRefresh);
  lv_task_del(taskMantra);
  lv_obj_clean(lv_scr_act());
}

void WatchFaceTerminal::NextMantra() {
  mantraIndex = (mantraIndex + 1) % mantraCount;
  lv_label_set_text(labelMantra, mantras[mantraIndex]);
}

void WatchFaceTerminal::UpdateMantraDisplay() {
  // Mantras rotate every 24 hours - this is handled by taskMantra
  // No update needed per refresh cycle, just keep current mantra visible
}

void WatchFaceTerminal::UpdateCalendarDisplay() {
  // Check if there are upcoming events
  uint64_t currentTime = std::chrono::system_clock::now().time_since_epoch().count() / 1000000000;
  
  Controllers::CalendarEventService::CalendarEvent nextEvent;
  if (calendarService.GetNextEvent(nextEvent, currentTime)) {
    // Format calendar event for display on bottom line
    char calendarText[240];
    calendarService.UpdateDisplayString(calendarText, sizeof(calendarText), currentTime);
    
    // Update bottom line with calendar event
    const char* currentText = lv_label_get_text(labelCalendar);
    if (std::strcmp(currentText, calendarText) != 0) {
      lv_label_set_text(labelCalendar, calendarText);
    }
  } else {
    // No upcoming events - clear bottom line
    const char* currentText = lv_label_get_text(labelCalendar);
    if (std::strcmp(currentText, "") != 0) {
      lv_label_set_text_static(labelCalendar, "");
    }
  }
}

void WatchFaceTerminal::Refresh() {
  notificationState = notificationManager.AreNewNotificationsAvailable();
  if (notificationState.IsUpdated()) {
    if (notificationState.Get()) {
      lv_label_set_text_static(notificationIcon, "[1]+ Notify");
    } else {
      lv_label_set_text_static(notificationIcon, "");
    }
  }

  currentDateTime = std::chrono::time_point_cast<std::chrono::seconds>(dateTimeController.CurrentDateTime());
  if (currentDateTime.IsUpdated()) {
    uint8_t hour = dateTimeController.Hours();
    uint8_t minute = dateTimeController.Minutes();

    if (settingsController.GetClockType() == Controllers::Settings::ClockType::H12) {
      char ampmChar[3] = "AM";
      if (hour == 0) {
        hour = 12;
      } else if (hour == 12) {
        ampmChar[0] = 'P';
      } else if (hour > 12) {
        hour = hour - 12;
        ampmChar[0] = 'P';
      }
      lv_label_set_text_fmt(labelPrompt1, "%02d:%02d %s", hour, minute, ampmChar);
      lv_label_set_text_fmt(labelTime, "#ffffff [TIME]# #11cc55 %02d:%02d %s#", hour, minute, ampmChar);
    } else {
      lv_label_set_text_fmt(labelPrompt1, "%02d:%02d", hour, minute);
      lv_label_set_text_fmt(labelTime, "#ffffff [TIME]# #11cc55 %02d:%02d#", hour, minute);
    }

    currentDate = std::chrono::time_point_cast<std::chrono::days>(currentDateTime.Get());
    if (currentDate.IsUpdated()) {
      Controllers::DateTime::Months month = dateTimeController.Month();
      uint8_t day = dateTimeController.Day();
      lv_label_set_text_fmt(labelDate, "#ffffff [DATE]# #00ffff %02d-%02d#", month, day);
    }
  }

  powerPresent = batteryController.IsPowerPresent();
  batteryPercentRemaining = batteryController.PercentRemaining();
  if (batteryPercentRemaining.IsUpdated() || powerPresent.IsUpdated()) {
    auto pct = batteryPercentRemaining.Get();
    const char* color;
    if (batteryController.IsCharging()) {
      lv_label_set_text_fmt(batteryIcon, "#ffffff [PWR]# #00ff00 CHG %d%%%#", pct);
    } else {
      if (pct > 60) color = "00ff00";
      else if (pct > 30) color = "ffaa00";
      else color = "ff2200";
      lv_label_set_text_fmt(batteryIcon, "#ffffff [PWR]# #%s %d%%%#", color, pct);
    }
  }

  heartbeat = heartRateController.HeartRate();
  heartbeatRunning = heartRateController.State() != Controllers::HeartRateController::States::Stopped;
  if (heartbeat.IsUpdated() || heartbeatRunning.IsUpdated()) {
    if (heartbeatRunning.Get()) {
      lv_obj_set_style_local_text_color(heartbeatValue, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, Colors::deepOrange);
      lv_label_set_text_fmt(heartbeatValue, "#ffffff [<3]# %d bpm", heartbeat.Get());
    } else {
      lv_label_set_text_static(heartbeatValue, "#ffffff [<3]# ---");
      lv_obj_set_style_local_text_color(heartbeatValue, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, Colors::gray);
    }
  }

  bleState = bleController.IsConnected();
  bleRadioEnabled = bleController.IsRadioEnabled();
  if (bleState.IsUpdated() || bleRadioEnabled.IsUpdated()) {
    if (!bleRadioEnabled.Get()) {
      lv_label_set_text_static(connectState, "#ffffff [*]# Disabled");
      lv_obj_set_style_local_text_color(connectState, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, Colors::gray);
    } else {
      if (bleState.Get()) {
        lv_label_set_text_static(connectState, "#ffffff [*]# Connected");
        lv_obj_set_style_local_text_color(connectState, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_CYAN);
      } else {
        lv_label_set_text_static(connectState, "#ffffff [*]# Disconnected");
        lv_obj_set_style_local_text_color(connectState, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, Colors::gray);
      }
    }
  }

  // Update calendar display (runs every refresh cycle)
  UpdateMantraDisplay();
  UpdateCalendarDisplay();
}
