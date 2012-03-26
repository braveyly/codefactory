#ifndef _WINDOW_IMP_H_
#define _WINDOW_IMP_H_

class WINDOW_IMP{
	public:
		WINDOW_IMP(){};
		virtual void dev_draw_line()=0;
		virtual void dev_draw_text()=0;
};
#endif
