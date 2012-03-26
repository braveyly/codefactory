#include <iostream>
#include <string>
#include "xwindow_imp.h"
#include "pmwindow_imp.h"
#include "icon_window.h"
#include "transient_window.h"

using namespace::std;

int main()
{
    ICON_WINDOW icon_win;
    TRANSIENT_WINDOW transient_win;
    XWINDOW_IMP xwin_imp;
    PMWINDOW_IMP pmwin_imp;
    icon_win.set_window_imp ( &xwin_imp );
    icon_win.draw_border();
    icon_win.set_window_imp ( &pmwin_imp );
    icon_win.draw_border();

    transient_win.set_window_imp ( &xwin_imp );
    transient_win.draw_close_window();
}
