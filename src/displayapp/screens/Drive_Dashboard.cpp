#include "displayapp/screens/Drive_Dashboard.h"
#include "displayapp/TouchEvents.h"
#include <lvgl/lvgl.h>

using namespace Pinetime::Applications::Screens;

static const char* const dashText =
  "Mantras Entries"
  "\n"
  " Sometimes opportunities cross my path and I'm too tunnel visioned on art weed, and time management excuses, but I found apartment 733. OR OVER A YEAR, THE BENEFITS OF CO-ED A RESIDENCY IS WORTH PURSUING A PARALLEL OPTION FOR HOUSING.\n"
  "\n"
  "JOURNAL ENTRY: Eye Contact versus Eye Blocking Making eye contact: if it feels weird, I will do excessive eye-blocking as a reaction because I don't want awkward unexpected eye contact for other people,/ JOURNAL ENTRY: Eye Contact versus Eye Blocking ___ Making eye contact: if it feels weird, I will do excessive eye-blocking as a reaction because I don't want awkward unexpected eye contact for other people,/ (other reasons include I don't want to risk any of my toxic/angry/dark/energy getting to in their field” don't make eye contact. Eyes are made of brain tissue and eyes and sharing facial expressions are brief windows into a person's state of mind! I feel vulnerable sometimes when I am expressive.  Well in a way yeah, it feels like mind reading to empathize deeply with people so sometimes I get psyched out andious from how intimate an exchange of expressions can be,) I do eye blocking as a default and I want to arrive at some neutral way to make more eye contact. Why? How can I be recovering friends if I'm still breaking eye contact so much? I need to not look at porn, it changes my eye patterns. The not even decaf technique has helped calm my eye sight and demeanor! Not smoking pot also helps to form a new habit of this. < Notice unconscious eye blocking and work through those hardest times> I do eye blocking when I look out of my peripherals as situational awareness. When I see ladies, when I smoke weed too often much.examples of when my BODY LANGUAGE FOLDS, MY VOICE FOLDS.My posture and chest sometimes caves in. < Notice unconscious eye blocking and work through those hardest times> I do eye blocking when I look out of my peripherals as situational awareness. When I see ladies, when I smoke weed too often much.i break eye contact too fast: examples of when my body language folds, my voice folds. My posture and chest sometimes caves in" 
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