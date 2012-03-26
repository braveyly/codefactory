#ifndef _WINDOW_H_
#define _WINDOW_H_
class WINDOW_IMP;

class WINDOW{
	public:
		virtual void draw_rect();
		virtual void draw_text();
		void set_window_imp(WINDOW_IMP* new_imp);
		WINDOW_IMP* get_window_imp();
	protected:
		WINDOW_IMP* win_imp;
};
#endif
