#include "displayapp/screens/Drive_Dashboard.h"
#include "displayapp/TouchEvents.h"
#include "displayapp/InfiniTimeTheme.h"
#include <lvgl/lvgl.h>

using namespace Pinetime::Applications::Screens;

static const char* const pages[] = {
  "DASHBOARD\n"
  "\n"
  "Getting a sponsor to stay free of porn honestly is an unrealistic expectation. But what I do want a sponsor for, is to get instructions through the work. I need deeper healing and still need to look at ugly aspects of myself …I'm not a drug addict or alcoholic, but I want a sponsor to help me become a better version of myself again. "
"\n"
};

Drive_Dashboard::Drive_Dashboard(DisplayApp* app) : displayApp {app} {
  lv_obj_t* bg = lv_obj_create(lv_scr_act(), nullptr);
  lv_obj_set_size(bg, 240, 240);
  lv_obj_set_pos(bg, 0, 0);
  lv_obj_set_style_local_bg_color(bg, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
  lv_obj_set_style_local_border_width(bg, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);

  label = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_text_color(label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, Colors::lightGray);
  lv_label_set_long_mode(label, LV_LABEL_LONG_BREAK);
  lv_obj_set_width(label, 228);
  lv_obj_align(label, nullptr, LV_ALIGN_IN_TOP_LEFT, 1, 1);

  // Manual scrollbar - thin bar on right edge
  scrollBar = lv_obj_create(lv_scr_act(), nullptr);
  lv_obj_set_size(scrollBar, 4, 240 / pageCount);
  lv_obj_set_style_local_bg_color(scrollBar, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_CYAN);
  lv_obj_set_style_local_border_width(scrollBar, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
  lv_obj_set_style_local_radius(scrollBar, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 2);
  lv_obj_set_pos(scrollBar, 236, 0);

  ShowPage(0);
}

Drive_Dashboard::~Drive_Dashboard() {
  lv_obj_clean(lv_scr_act());
}

void Drive_Dashboard::ShowPage(int page) {
  currentPage = page;
  lv_label_set_text_static(label, pages[page]);
  lv_obj_set_pos(scrollBar, 236, page * (240 / pageCount));
}

bool Drive_Dashboard::OnTouchEvent(TouchEvents event) {
  if (event == TouchEvents::SwipeUp) {
    if (currentPage < pageCount - 1) {
      ShowPage(currentPage + 1);
    }
    return true;
  }
  if (event == TouchEvents::SwipeDown) {
    if (currentPage > 0) {
      ShowPage(currentPage - 1);
    }
    return true;
  }
  if (event == TouchEvents::SwipeRight) {
    displayApp->StartApp(Apps::Clock, DisplayApp::FullRefreshDirections::LeftAnim);
    return true;
  }
  return false;
}
