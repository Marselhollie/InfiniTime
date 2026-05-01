#include "displayapp/screens/Drive_Dashboard.h"
#include "displayapp/TouchEvents.h"
#include <lvgl/lvgl.h>

using namespace Pinetime::Applications::Screens;

static const char* const dashText =
  "DASHBOARD\n"
  "\n"
  "Journaling how the mantras work for myself, and journaling how other people handle similar situations, is really powerful for integration. When I recognize and write down qualities in others that I want to practice, the change gets rooted deeper. The watch mantras, the journaling how it's been working, and the seeing deliveries and effect in 3rd person from others.";

Drive_Dashboard::Drive_Dashboard(DisplayApp* app) : displayApp {app} {
  lv_obj_t* page = lv_page_create(lv_scr_act(), nullptr);
  lv_obj_set_size(page, 240, 240);
  lv_obj_set_pos(page, 0, 0);
  lv_obj_set_style_local_bg_color(page, LV_PAGE_PART_BG, LV_STATE_DEFAULT, LV_COLOR_BLACK);
  lv_obj_set_style_local_border_width(page, LV_PAGE_PART_BG, LV_STATE_DEFAULT, 0);

  lv_page_set_scrollbar_mode(page, LV_SCROLLBAR_MODE_ON);
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
