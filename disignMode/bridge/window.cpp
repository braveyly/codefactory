#include "window.h"
#include "window_imp.h"
void WINDOW::draw_rect()
{
    win_imp->dev_draw_line();
    win_imp->dev_draw_line();
    win_imp->dev_draw_line();
    win_imp->dev_draw_line();
}

void WINDOW::draw_text()
{
    win_imp->dev_draw_text();
}

void WINDOW::set_window_imp ( WINDOW_IMP* new_imp )
{
    win_imp = new_imp;
}

WINDOW_IMP* WINDOW::get_window_imp()
{
    return win_imp;
}
