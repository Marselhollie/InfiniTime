#include "displayapp/screens/Drive_Dashboard.h"
#include "displayapp/TouchEvents.h"
#include <lvgl/lvgl.h>

using namespace Pinetime::Applications::Screens;

static const char* const dashText =
  "APARTMENT/HOUSING OPTIONS\n"
  "\n"
  "ARTIST RESIDENCY IS ONE AREA TO REALLY START PLANTING SOME HOUSING SEEDS. I'VE KNOWN WHAT WELDING WITH APARTMENT IS LIKE. AND EVEN THOUGH THE WAIT-LIST FOR RESIDENCIES MAY BE MONTHS OR OVER A YEAR, THE BENEFITS OF CO-ED A RESIDENCY IS WORTH PURSUING A PARALLEL OPTION FOR HOUSING.\n"
  "\n"
  "CONSIDER THAT AN ARTIST RESIDENCY IS A LONG TERM PARALLEL HOUSING SOLUTION TO KEEP THIS IRON IN THE FIRE. IS ONE TYPE OF LIVING SCENARIO I WOULD THRIVE. AN ARTIST RESIDENCY, EVEN MORESO THAN WELDING WITH APARTMENT, I WOULD HONESTLY FIND GREAT PEACE LIVING WITH A BUNCH OF ARTISTS. IT COULD HELP SOLVE A LOT OF MY LONGING. I DEEPLY BELIEVE I CAN EARN A PLACE AT AN ARTIST RESIDENCE LONG TERM. THE CULTURE THERE WOULD BE MORE HAPPY AND IT FEELS LIKE THE CORAL REEF SET ANCHOR AT IN MY PSYCHEDELIC DREAM.\n"
  "\n"
  "ALSO REMEMBER THAT I CAN LIVE AND WORK MORE ON THE OUTSKIRTS/ OTHER CITIES. THERE'S MAYBE MORE LARGER WINDOWS OF OPPORTUNITY FARTHER OUT FROM LA.";

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
  lv_obj_set_style_local_text_font(label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_28);
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
