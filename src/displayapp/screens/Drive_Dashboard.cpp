#include "displayapp/screens/Drive_Dashboard.h"
#include "displayapp/TouchEvents.h"
#include "displayapp/UserApps.h"
#include <lvgl/lvgl.h>

using namespace Pinetime::Applications::Screens;

static const char* const dashText =
  "DASHBOARD\n"
  "\n"
  "Focus on Keeping my grades higher to keep up the grants and money for a Car when I start going full time. ARTIST RESIDENCY- is a branch to land at. I know it's not gonna be a bunch of hot girls, but I think it could be a place of a lot of fun and genuine friendships and soulcial Ive wanted to commit myself to.\n"
  "\n"
  "I deeply believe I can earn a living and stay happier in simple ways. >>> Book of enoch, emerald tablets, sumerian or pre-bible books, atomic habit, mussolini; A LIFE OF LISTENING i like reading biographies about strategic events they moved through, the power of positive thinking Norman Peele. Ego is the enemy Ryan Holiday.\n"
  "\n"
  "ALSO REMEMBER THAT I CAN LIVE AND WORK MORE ON THE OUTSKIRTS/ OTHER CITIES. THERE'S MAYBE MORE LARGER WINDOWS OF OPPORTUNITY FARTHER OUT FROM LA.";

Drive_Dashboard::Drive_Dashboard(DisplayApp* app) : displayApp {app} {
  lv_obj_t* page = lv_page_create(lv_scr_act(), nullptr);
  lv_obj_set_size(page, 240, 240);
  lv_obj_set_pos(page, 0, 0);
  lv_obj_set_style_local_bg_color(page, LV_PAGE_PART_BG, LV_STATE_DEFAULT, LV_COLOR_BLACK);
  lv_obj_set_style_local_border_width(page, LV_PAGE_PART_BG, LV_STATE_DEFAULT, 0);

  // Enable scrollbar, snap page by page
  lv_page_set_scrlbar_mode(page, LV_SCRLBAR_MODE_ON);
  lv_page_set_edge_flash(page, false);
  lv_obj_set_style_local_bg_color(page, LV_PAGE_PART_SCRL, LV_STATE_DEFAULT, LV_COLOR_BLACK);

  // Scrollbar style - aqua
  lv_obj_set_style_local_bg_color(page, LV_PAGE_PART_SCRLBAR, LV_STATE_DEFAULT, LV_COLOR_CYAN);
  lv_obj_set_style_local_size(page, LV_PAGE_PART_SCRLBAR, LV_STATE_DEFAULT, 4);
  lv_obj_set_style_local_radius(page, LV_PAGE_PART_SCRLBAR, LV_STATE_DEFAULT, 2);

  label = lv_label_create(page, nullptr);
  lv_obj_set_style_local_text_font(label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_14);
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
