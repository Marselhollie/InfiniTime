#pragma once

#include <lvgl/src/lv_core/lv_obj.h>
#include "displayapp/screens/Screen.h"
#include "UserApps.h"
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
        lv_obj_t* label;
      };
    }

    template <>
    struct AppTraits<Apps::DriveDashboard> {
      static constexpr Apps app = Apps::DriveDashboard;
      static constexpr const char* icon = "D";

      static Screens::Screen* Create(AppControllers& controllers) {
        return new Screens::Drive_Dashboard(controllers.displayApp);
      }
    };
  }
}
