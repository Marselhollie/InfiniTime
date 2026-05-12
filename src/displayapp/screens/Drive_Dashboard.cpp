#include "displayapp/screens/Drive_Dashboard.h"
#include "displayapp/TouchEvents.h"
#include "displayapp/InfiniTimeTheme.h"
#include <lvgl/lvgl.h>

using namespace Pinetime::Applications::Screens;

static const char* dashboardText =
  "DASHBOARD\n"
  "\n"
  "Getting a sponsor to stay free of porn honestly is an unrealistic expectation. But what I do want a sponsor for, is to get "
  "instructions through the program. I need deeper healing and still need to look at ugly aspects of myself. I'm not a drug addict or alcoholic, "
  "but I want a sponsor to help me become a better version of myself again. More accountability and shadow work but structured through the AA Big Book.\n";

Drive_Dashboard::Drive_Dashboard(DisplayApp* app) : displayApp {app} {
  lv_obj_t* bg = lv_obj_create(lv_scr_act(), nullptr);
  lv_obj_set_size(bg, 240, 240);
  lv_obj_set_pos(bg, 0, 0);
  lv_obj_set_style_local_bg_color(bg, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
  lv_obj_set_style_local_border_width(bg, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);

  label = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_text_color(label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, Colors::lightGray);
  lv_label_set_long_mode(label, LV_LABEL_LONG_SROLL_CIRC);
  lv_label_set_anim_speed(label, 30);
  lv_obj_set_width(label, 228);
  lv_obj_set_height(label, 240);
  lv_obj_align(label, nullptr, LV_ALIGN_IN_TOP_LEFT, 1, 1);
  lv_label_set_text(label, dashboardText);
}

Drive_Dashboard::~Drive_Dashboard() {
  lv_obj_clean(lv_scr_act());
}

bool Drive_Dashboard::OnTouchEvent(TouchEvents event) {
  if (event == TouchEvents::SwipeRight) {
    displayApp->StartApp(Apps::Clock, DisplayApp::FullRefreshDirections::LeftAnim);
    return true;
  }
  return false;
}