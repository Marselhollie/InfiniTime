

#include <lvgl/src/lv_core/lv_obj.h>
#include "displayapp/screens/Screen.h"
#include "displayapp/DisplayApp.h"
#include "displayapp/TouchEvents.h"

namespace Pinetime {
  namespace Applications {
    namespace Screens {

      class Drive_Dashboard : public Screen {
      public:
        explicit Drive_Dashboard(DisplayApp* app);
        ~Drive_Dashboard() override;
        bool OnTouchEvent(TouchEvents event) override;

      private:
        void ShowPage(int page);

        DisplayApp* displayApp;
        lv_obj_t* label;
        int currentPage = 0;
        static constexpr int pageCount = 3;
      };
    }
  }
}
