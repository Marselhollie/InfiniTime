#include "displayapp/screens/Drive_Dashboard.h"
#include "displayapp/TouchEvents.h"
#include <lvgl/lvgl.h>

using namespace Pinetime::Applications::Screens;

static const char* const dashText =
  "DASHBOARD"
  "\n"
  " Sometimes opportunities cross my path and I'm too tunnel visioned on art weed, and time management excuses, but I found apartment 733. OR OVER A YEAR, THE BENEFITS OF CO-ED A RESIDENCY IS WORTH PURSUING A PARALLEL OPTION FOR HOUSING.\n"
  "\n"
  "Getting a sponsor to stay free of porn honestly is an unrealistic expectation. But what I do want a sponsor for, is to get instructions through the program. I need deeper healing and still need to look at ugly aspects of myself. I'm not a drug addict or alcoholic,but I want a sponsor to help me become a better version of myself again. More accountability and shadow work but structured through the AA Big Book." 
  "\n"
  ;

Drive_Dashboard::Drive_Dashboard(DisplayApp* app) : displayApp {app} {
  lv_obj_t* page = lv_page_create(lv_scr_act(), nullptr);
  lv_obj_set_size(page, 240, 240);
  lv_obj_set_pos(page, 0, 0);
  lv_obj_set_style_local_bg_color(page, LV_PAGE_PART_BG, LV_STATE_DEFAULT, LV_COLOR_BLACK);
  lv_obj_set_style_local_border_width(page, LV_PAGE_PART_BG, LV_STATE_DEFAULT, 0);

  lv_page_set_scrollbar_mode(page, LV_SCROLLBAR_MODE_ON);
lv_page_set_anim_time(page, 0);
  lv_obj_set_style_local_bg_color(page, LV_PAGE_PART_BG, LV_STATE_DEFAULT, LV_COLOR_BLACK);
  lv_obj_set_style_local_bg_color(page, LV_PAGE_PART_SCROLLBAR, LV_STATE_DEFAULT, LV_COLOR_CYAN);
  lv_obj_set_style_local_size(page, LV_PAGE_PART_SCROLLBAR, LV_STATE_DEFAULT, 4);
  lv_obj_set_style_local_radius(page, LV_PAGE_PART_SCROLLBAR, LV_STATE_DEFAULT, 2);

  label = lv_label_create(page, nullptr);
  lv_obj_set_style_local_text_color(label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_YELLOW);
  lv_label_set_long_mode(label, LV_LABEL_LONG_BREAK);
  lv_obj_set_width(label, 220);
  lv_label_set_text_static(label, dashText);
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