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
#include "displayapp/screens/WeatherSymbols.h"
#include "displayapp/InfiniTimeTheme.h"
#include "displayapp/LittleVgl.h"
#include <string>

using namespace Pinetime::Applications::Screens;

LV_FONT_DECLARE(jetbrains_mono_bold_20)

static const char* mantras[] = {
  "Breathe 5 deep times",
  "Hold eye contact · look away less",
  "Visualize outcomes you DO want",
  "Speak in Optimum Pitch",
  "Shift attention to fun · remember to smile",
  "Remove I · shift to invitational questions",
  "Subconscious visual check-in",
  "Grateful mentality · daydream gratitude",
  "Read peoples social auras and delivery",
  "Fasting · self control · pot · coffee · food",
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
                                     Pinetime::Components::LittleVgl& lglDriver)
  : currentDateTime {{}},
    dateTimeController {dateTimeController},
    batteryController {batteryController},
    bleController {bleController},
    notificationManager {notificationManager},
    settingsController {settingsController},
    heartRateController {heartRateController},
    motionController {motionController},
    weatherService {weatherService},
    lglDriver {lglDriver} {

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

  // Battery icon pinned to top-right corner
  batteryIcon = lv_label_create(lv_scr_act(), nullptr);
  lv_label_set_text_static(batteryIcon, Symbols::batteryHalf);
  lv_obj_align(batteryIcon, nullptr, LV_ALIGN_IN_TOP_RIGHT, -4, 4);

  // Mantra scrolling label — pinned to bottom, yellow, larger font, one mantra per day
  mantraIndex = xTaskGetTickCount() % mantraCount;
  labelPrompt2 = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_text_color(labelPrompt2, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_YELLOW);
  lv_obj_set_style_local_text_font(labelPrompt2, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &jetbrains_mono_bold_20);
  lv_label_set_long_mode(labelPrompt2, LV_LABEL_LONG_SROLL_CIRC);
  lv_obj_set_width(labelPrompt2, 240);
  lv_label_set_text(labelPrompt2, mantras[mantraIndex]);
  lv_obj_align(labelPrompt2, nullptr, LV_ALIGN_IN_BOTTOM_LEFT, 0, 0);

#ifndef MONITOR_ZOOM
  // Rotate display 90° clockwise via ST7789 MADCTL hardware register.
  // MX (0x40) | MV (0x20) = 90° CW. Restored in destructor.
  lglDriver.SetOrientation(0x60);
#endif

  taskRefresh = lv_task_create(RefreshTaskCallback, LV_DISP_DEF_REFR_PERIOD, LV_TASK_PRIO_MID, this);
  taskMantra = lv_task_create(MantraTaskCallback, 24 * 60 * 60 * 1000u, LV_TASK_PRIO_LOW, this);
  Refresh();
}

WatchFaceTerminal::~WatchFaceTerminal() {
  lv_task_del(taskRefresh);
  lv_task_del(taskMantra);
#ifndef MONITOR_ZOOM
  // Restore default orientation
#ifdef DRIVER_DISPLAY_MIRROR
  lglDriver.SetOrientation(0b01000000);
#else
  lglDriver.SetOrientation(0x00);
#endif
#endif
  lv_obj_clean(lv_scr_act());
}

void WatchFaceTerminal::NextMantra() {
  mantraIndex = (mantraIndex + 1) % mantraCount;
  lv_label_set_text(labelPrompt2, mantras[mantraIndex]);
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
      lv_label_set_text_fmt(labelDate, "#ffffff [DATE]# #007fff %02d-%02d#", month, day);
    }
  }

  powerPresent = batteryController.IsPowerPresent();
  batteryPercentRemaining = batteryController.PercentRemaining();
  if (batteryPercentRemaining.IsUpdated() || powerPresent.IsUpdated()) {
    lv_obj_set_style_local_text_color(batteryIcon,
                                      LV_LABEL_PART_MAIN,
                                      LV_STATE_DEFAULT,
                                      BatteryIcon::ColorFromPercentage(batteryPercentRemaining.Get()));
    lv_label_set_text_static(batteryIcon, Symbols::batteryHalf);
  }

  heartbeat = heartRateController.HeartRate();
  heartbeatRunning = heartRateController.State() != Controllers::HeartRateController::States::Stopped;
  if (heartbeat.IsUpdated() || heartbeatRunning.IsUpdated()) {
    if (heartbeatRunning.Get()) {
      lv_obj_set_style_local_text_color(heartbeatValue, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, Colors::deepOrange);
      lv_label_set_text_fmt(heartbeatValue, "#ffffff [L_HR]# %d bpm", heartbeat.Get());
    } else {
      lv_label_set_text_static(heartbeatValue, "#ffffff [L_HR]# ---");
      lv_obj_set_style_local_text_color(heartbeatValue, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, Colors::gray);
    }
  }

  bleState = bleController.IsConnected();
  bleRadioEnabled = bleController.IsRadioEnabled();
  if (bleState.IsUpdated() || bleRadioEnabled.IsUpdated()) {
    if (!bleRadioEnabled.Get()) {
      lv_label_set_text_static(connectState, "#ffffff [STAT]# Disabled");
      lv_obj_set_style_local_text_color(connectState, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, Colors::gray);
    } else {
      if (bleState.Get()) {
        lv_label_set_text_static(connectState, "#ffffff [STAT]# Connected");
        lv_obj_set_style_local_text_color(connectState, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, Colors::blue);
      } else {
        lv_label_set_text_static(connectState, "#ffffff [STAT]# Disconnected");
        lv_obj_set_style_local_text_color(connectState, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, Colors::gray);
      }
    }
  }
}
