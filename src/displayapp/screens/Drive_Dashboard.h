#pragma once

#include <lvgl/src/lv_core/lv_obj.h>
#include "displayapp/screens/Screen.h"
#include "displayapp/DisplayApp.h"

namespace Pinetime {
  namespace Applications {
    namespace Screens {

      class Drive_Dashboard : public Screen {
      public:
        explicit Drive_Dashboard(DisplayApp* app);
        ~Drive_Dashboard() override;
        bool OnTouchEvent(TouchEvents event) override;

      private:
        DisplayApp* displayApp;
        lv_obj_t* label;
      };
    }
  }
}
