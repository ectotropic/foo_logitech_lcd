//--------------------------------------
//
#include "foobar/foobar_sdk.h"
#include <helpers/atl-misc.h>
//--------------------------------------

//--------------------------------------
//
#include "foobar/UI/foobar_preferences.h"
//--------------------------------------

namespace foobar::UI {
    //**************************************************************************
    // foo_logitech_lcd_preferences
    //**************************************************************************
    class foo_logitech_lcd_preferences :
        public preferences_page_impl<CFoobarPreferencesDlg> {
    protected:
        ~foo_logitech_lcd_preferences() = default;

    public:
        virtual const char* get_name() noexcept override {
            return "Logitech LCD";
        }

        virtual GUID get_guid() noexcept override {
            static const GUID s_PrefGuid{ 0xb02fcec2, 0xaf1, 0x43ac, { 0x85, 0x59, 0x29, 0x71, 0x4c, 0xc7, 0x24, 0x32 } };
            return s_PrefGuid;
        };

        virtual GUID get_parent_guid() noexcept override {
            return preferences_page::guid_display;
        };
    }; // class foo_logitech_lcd_initquit

    //----------------------------------

    preferences_page_factory_t<foo_logitech_lcd_preferences> g_PreferencesFactory;
}
