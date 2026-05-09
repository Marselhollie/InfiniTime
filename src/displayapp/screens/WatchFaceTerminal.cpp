#include <lvgl/lvgl.h>
#include "displayapp/screens/WatchFaceTerminal.h"
#include "displayapp/screens/BatteryIcon.h"
#include "components/battery/BatteryController.h"
#include "components/ble/BleController.h"
#include "components/ble/NotificationManager.h"
#include "components/heartrate/HeartRateController.h"
#include "components/motion/MotionController.h"
#include "components/settings/Settings.h"
#include "components/ble/SimpleWeatherService.h"
#include "displayapp/InfiniTimeTheme.h"
#include "displayapp/screens/Symbols.h"
#include "displayapp/DisplayApp.h"
#include <cstdlib>

extern lv_font_t jetbrains_mono_42;

using namespace Pinetime::Applications::Screens;

static const char* mantras[] = {
  "BREATHE 5 DEEP TIMES",
  "HOLD EYE CONTACT WHEN SPEAKING",
  "BE DIRECT W/O BEING HOSTILE",
  "PRACTICE OPTIMUM PITCH W/O DRAGGING MOOD",
  "REMOVING I IN COMMENTS",
  "GRATEFUL 3 THINGS",
  "RD AURAS & FX of delivery",
  "|NOCOFFEE BETTER SOCIALS|",
  "ASKING GOD. KEEPING GOD ON MY MIND.",
  "LONGERTERM ACTIVITIES SAVES MONEY",
  "NTLOOKNG@PORN = MOVING CLOSER 2GF",
  "Speak Audibly. or not at all."
};
static const int mantraCount = 12;

WatchFaceTerminal::WatchFaceTerminal(DisplayApp* app,
                                     Controllers::DateTime& dateTimeController,
                                     const Controllers::Battery& batteryController,
                                     const Controllers::Ble& bleController,
                                     Controllers::NotificationManager& notificationManager,
                                     Controllers::Settings& settingsController,
                                     Controllers::HeartRateController& heartRateController,
                                     Controllers::MotionController& motionController,
                                     Controllers::SimpleWeatherService& weatherService)
  : displayApp {app},
    currentDateTime {{}},
    dateTimeController {dateTimeController},
    batteryController {batteryController},
    bleController {bleController},
    notificationManager {notificationManager},
    settingsController {settingsController},
    heartRateController {heartRateController},
    motionController {motionController},
    weatherService {weatherService} {

  container = lv_cont_create(lv_scr_act(), nullptr);
  lv_cont_set_layout(container, LV_LAYOUT_COLUMN_LEFT);
  lv_cont_set_fit(container, LV_FIT_TIGHT);
  lv_obj_set_style_local_pad_inner(container, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 2);
  lv_obj_set_style_local_bg_opa(container, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_TRANSP);

  notificationIcon = lv_label_create(container, nullptr);
  lv_obj_set_style_local_text_color(notificationIcon, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_YELLOW);
  if (notificationManager.AreNewNotificationsAvailable()) {
    lv_label_set_text_fmt(notificationIcon, "[%d]", notificationManager.NbNotifications());
  } else {
    lv_label_set_text_static(notificationIcon, "");
  }

  statusIcons = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_text_color(statusIcons, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
  lv_label_set_recolor(statusIcons, true);
  lv_obj_align(statusIcons, nullptr, LV_ALIGN_IN_TOP_RIGHT, -2, 2);

  labelTime = lv_label_create(container, nullptr);
  lv_label_set_recolor(labelTime, true);
  lv_obj_set_style_local_text_font(labelTime, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &jetbrains_mono_42);

  labelDate = lv_label_create(container, nullptr);
  lv_label_set_recolor(labelDate, true);

  heartbeatValue = lv_label_create(container, nullptr);
  lv_label_set_recolor(heartbeatValue, true);

  connectState = lv_label_create(container, nullptr);
  lv_label_set_recolor(connectState, true);

  batteryValue = lv_label_create(container, nullptr);
  lv_label_set_recolor(batteryValue, true);

  labelMantra = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_text_color(labelMantra, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_YELLOW);
  lv_obj_set_style_local_text_font(labelMantra, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &jetbrains_mono_bold_20);
  lv_label_set_long_mode(labelMantra, LV_LABEL_LONG_SROLL_CIRC);
  lv_label_set_anim_speed(labelMantra, 60);
  lv_obj_set_width(labelMantra, 240);
  lv_obj_align(labelMantra, nullptr, LV_ALIGN_IN_BOTTOM_LEFT, 0, 0);

  srand(dateTimeController.CurrentDateTime().time_since_epoch().count());
  int idx = rand() % mantraCount;
  lv_label_set_text(labelMantra, mantras[idx]);

  lv_obj_align(container, nullptr, LV_ALIGN_IN_TOP_LEFT, 0, 7);

  taskRefresh = lv_task_create(RefreshTaskCallback, LV_DISP_DEF_REFR_PERIOD, LV_TASK_PRIO_MID, this);
  taskChargeAnim = lv_task_create(ChargeAnimCallback, 750, LV_TASK_PRIO_LOW, this);
  lv_task_set_repeat_count(taskChargeAnim, -1);
  Refresh();
}

WatchFaceTerminal::~WatchFaceTerminal() {
  lv_task_del(taskRefresh);
  lv_task_del(taskChargeAnim);
  lv_obj_clean(lv_scr_act());
}

bool WatchFaceTerminal::OnTouchEvent(TouchEvents event) {
  if (event == TouchEvents::SwipeLeft) {
    displayApp->StartApp(Apps::DriveDashboard, DisplayApp::FullRefreshDirections::LeftAnim);
    return true;
  }
  return false;
}

void WatchFaceTerminal::ChargeAnimCallback(lv_task_t* task) {
  auto* screen = static_cast<WatchFaceTerminal*>(task->user_data);
  screen->UpdateChargeAnim();
}

void WatchFaceTerminal::UpdateChargeAnim() {
  if (!batteryController.IsCharging()) return;
  static const char* bars[] = {"#***", "##**", "###*", "####"};
  chargeAnimStep = (chargeAnimStep + 1) % 4;
  lv_obj_set_style_local_text_color(batteryValue, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xADFF2F));
  lv_label_set_text_fmt(batteryValue, "[BATT] %s", bars[chargeAnimStep]);
}

void WatchFaceTerminal::Refresh() {
  notificationState = notificationManager.AreNewNotificationsAvailable();
  if (notificationState.IsUpdated()) {
    if (notificationState.Get()) {
      lv_label_set_text_fmt(notificationIcon, "[%d]", notificationManager.NbNotifications());
    } else {
      lv_label_set_text_static(notificationIcon, "");
    }
  }

  currentDateTime = std::chrono::time_point_cast<std::chrono::seconds>(dateTimeController.CurrentDateTime());
  if (currentDateTime.IsUpdated()) {
    uint8_t hour = dateTimeController.Hours();
    uint8_t minute = dateTimeController.Minutes();

    static const char* dayNames[] = {"Su", "Mo", "Tu", "We", "Th", "Fr", "Sa"};
    const char* dayStr = dayNames[static_cast<int>(dateTimeController.DayOfWeek())];

    if (settingsController.GetClockType() == Controllers::Settings::ClockType::H12) {
      if (hour == 0) {
        hour = 12;
      } else if (hour > 12) {
        hour = hour - 12;
      }
      if (hour < 10) {
        lv_label_set_text_fmt(labelTime, "#adff2f _%d:%02d %s#", hour, minute, dayStr);
      } else {
        lv_label_set_text_fmt(labelTime, "#adff2f %02d:%02d %s#", hour, minute, dayStr);
      }
    } else {
      if (hour < 10) {
        lv_label_set_text_fmt(labelTime, "#adff2f _%d:%02d %s#", hour, minute, dayStr);
      } else {
        lv_label_set_text_fmt(labelTime, "#adff2f %02d:%02d %s#", hour, minute, dayStr);
      }
    }

    currentDate = std::chrono::time_point_cast<std::chrono::days>(currentDateTime.Get());
    if (currentDate.IsUpdated()) {
      Controllers::DateTime::Months month = dateTimeController.Month();
      uint8_t day = dateTimeController.Day();
      uint16_t year = dateTimeController.Year();
      static const char* monthNames[] = {
        "JAN", "FEB", "MAR", "APR", "MAY", "JUN",
        "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"
      };
      const char* monthStr = monthNames[static_cast<int>(month) - 1];
      lv_label_set_text_fmt(labelDate, "#ffff00 [DATE] %s_%d_%d#", monthStr, day, year);
    }
  }

  powerPresent = batteryController.IsPowerPresent();
  batteryPercentRemaining = batteryController.PercentRemaining();
  if (batteryPercentRemaining.IsUpdated() || powerPresent.IsUpdated()) {
    int pct = batteryPercentRemaining.Get();
    lv_color_t batColor;
    if (batteryController.IsCharging()) {
      // handled by charge anim task
      return;
    } else if (pct <= 25) {
      batColor = LV_COLOR_RED;
    } else if (pct <= 50) {
      batColor = LV_COLOR_YELLOW;
    } else if (pct <= 74) {
      batColor = LV_COLOR_GREEN;
    } else {
      batColor = lv_color_hex(0xADFF2F);
    }
    lv_obj_set_style_local_text_color(batteryValue, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, batColor);
    lv_label_set_text_fmt(batteryValue, "#ffffff [BATT]# %d%%", pct);
  }

  heartbeat = heartRateController.HeartRate();
  heartbeatRunning = heartRateController.State() != Controllers::HeartRateController::States::Stopped;
  if (heartbeat.IsUpdated() || heartbeatRunning.IsUpdated()) {
    if (heartbeatRunning.Get()) {
      lv_obj_set_style_local_text_color(heartbeatValue, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, Colors::deepOrange);
      lv_label_set_text_fmt(heartbeatValue, "#ffffff [HR]# %d bpm", heartbeat.Get());
    } else {
      lv_label_set_text_static(heartbeatValue, "#ffffff [HR]# ---");
      lv_obj_set_style_local_text_color(heartbeatValue, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, Colors::gray);
    }
  }

  bleState = bleController.IsConnected();
  bleRadioEnabled = bleController.IsRadioEnabled();
  if (bleState.IsUpdated() || bleRadioEnabled.IsUpdated()) {
    if (!bleRadioEnabled.Get()) {
      lv_label_set_text_static(connectState, "#ff0000 [BLE]#");
      lv_obj_set_style_local_text_color(connectState, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, Colors::gray);
    } else {
      if (bleState.Get()) {
        lv_label_set_text_static(connectState, "#0000ff [BLE^]#");
        lv_obj_set_style_local_text_color(connectState, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, Colors::blue);
      } else {
        lv_label_set_text_static(connectState, "#ff0000 [BLE]#");
        lv_obj_set_style_local_text_color(connectState, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, Colors::gray);
      }
    }
    bool connected = bleRadioEnabled.Get() && bleState.Get();
    lv_label_set_text(statusIcons, connected ? Symbols::bluetooth : "");
  }
}
