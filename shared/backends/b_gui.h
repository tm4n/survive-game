// Backend gui
#ifndef __B_GUI_H__
#define __B_GUI_H__

#include <windows.h>
/*#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>*/
#include "var.h" // the var class - without it, you'd need _VAR() macros
#include "adll.h"
#include <string>
#include <list>
#include "defs.h"

// The backend gui is only used on the client. its working differently from the other backends

#define B_GUIELEMS_ENTRIES 500
#define B_GUI_TYPE_PANEL 1
#define B_GUI_TYPE_TEXT 2

#define B_GUI_POS_SCALE 1
#define B_GUI_POS_CENTER 2
#define B_GUI_POS_UPLEFT 3
#define B_GUI_POS_DOWNLEFT 4
#define B_GUI_POS_UPCENTER 5
#define B_GUI_POS_DOWNCENTER 6
#define B_GUI_POS_LEFTCENTER 7
#define B_GUI_POS_RIGHTCENTER 8
#define B_GUI_POS_UPRIGHT 9
#define B_GUI_POS_DOWNRIGHT 10

// receiver class for clicks
class gui_button_receiver
{
public:
	virtual void event_click(int, int) {}
	virtual void event_clickup(int, int) {}
	virtual void event_buttonup(int, int) {}
	virtual void event_touch(int, int) {}
	virtual void event_release(int, int) {}
};



// storage for other class members
struct s_key {
	var keycode;
	int panel_id;
	int button_num;
	gui_button_receiver *rcv;
};

// storage for class values from file

struct s_guibuttons {
	int x, y;
	BMAP *bmapOn, *bmapOff, *bmapOver;
	gui_button_receiver *rcv;
};

struct s_guidigits {
	int x, y;
	char *format;
	FONT* font;
	var factor;
	var display;
};

struct s_guilabels {
	int x, y;
	FONT* font;
	STRING* str;
};

struct s_guiwindows {
	int x, y, dx, dy;
	BMAP* bmap;
	var px, py;
};

struct s_guiinputs {
	int x, y, offset_x, offset_y, length, bt_num;
	BMAP *bmapOn, *bmapOff, *bmapOver;
	var *px, *py;
	FONT *font;
	STRING* display_str;
	STRING* mask;
	std::string input_str;
};

struct s_guielems {
	int gui_type; // see B_GUI_TYPE_.. defines
	char bname[32]; // might not be needed
	int group;
	int layer;
	int scale_mode;
	int px, py;
	int size_x, size_y;
	uchar color[3];

	// for panels
	char background[32]; 
	int alpha;
	float scale_x; // original scale ?
	float scale_y;
	
	int num_buttons;
	s_guibuttons* bts;

	int num_digits;
	s_guidigits* digs;

	int num_labels;
	s_guilabels* labs;

	int num_windows;
	s_guiwindows* wins;

	int num_inputs;
	s_guiinputs* inps;

	// for texts
	int num_strings;
	FONT *font;

	void *ptr;
};

// gui class that holds all data
class b_gui
{
public:
	s_guielems b_guielems[B_GUIELEMS_ENTRIES];
	b_gui(ENGINE_VARS*, const char*);
	~b_gui();
	int ready;
	s_guiinputs *current_input;
	int current_input_elem;
	std::list<s_key> keys;
	bool disable_keys;

	ENGINE_VARS *ev;

	void frame(int);
	int lookup_name(const char*);

	// modification
	void key_set_receiver(var keycode, int panel_id, int button_num, gui_button_receiver*);

	bool button_exists(int, int);
	void button_set_receiver(int, int, gui_button_receiver*);
	void button_add(int pan_id, int px, int py, BMAP *bmapOn, BMAP *bmapOff, BMAP *bmapOver);
	void button_change(int pan_id, int but_id, float px, float py, BMAP *bmapOn, BMAP *bmapOff, BMAP *bmapOver);
	
	void txt_set_value(int txt_id, int txt_num, std::string *txt);
	char* txt_get_value(int, int);
	void txt_set_alpha(int txt_id, double alpha);
	void txt_set_color(int txt_id, uchar color_r, uchar color_g, uchar color_b);
	
	void digit_set_double(int, int, double);
	void digit_set_int(int, int, int);

	void label_add(int panel_id, int px, int py, const char* font, const char* str);
	void label_set_caption(int, int, const char*);
	void label_set_color(int, int, uchar red, uchar green, uchar blue);

	void window_set_bmap(int, int, BMAP*);
	void window_set_px(int, int, int);
	void window_set_py(int, int, int);
	void window_set_dx(int, int, int);
	void window_set_dy(int, int, int);

	void input_enable(int pan_id, int input_id);
	void input_disable();

	void resize_elems(int, int);
	void show_group(int);
	void hide_group(int);
	void move_group(int, int, int);
	void toggle_group(int);
private:
	int ct_cursor;
	void err(const char *, int);
};

#endif