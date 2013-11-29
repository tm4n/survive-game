#include "b_gui.h"
#include "tinyxml.h"
#include <iostream>
#include <stdio.h>
#include "helper.h"
#include <string.h>

// global button receiver!
b_gui* globgui = NULL;
void gui_global_button_receiver(var vnum, PANEL* pan)
{
	int num = _INT(vnum) - 1;
	// find pan in list
	int i;
	for (i = 0; i < B_GUIELEMS_ENTRIES; i++)
	{
		if (pan == (PANEL*)(globgui->b_guielems[i].ptr))
		{
			// if a button receiver is there
			if (globgui->b_guielems[i].bts[num].rcv != NULL)
			{
				switch((int)*(globgui->ev->event_type))
				{
				case EVENT_CLICK:
					globgui->b_guielems[i].bts[num].rcv->event_click(i, num);
					break;

				case EVENT_CLICKUP:
					globgui->b_guielems[i].bts[num].rcv->event_clickup(i, num);
					break;

				case EVENT_BUTTONUP:
					globgui->b_guielems[i].bts[num].rcv->event_buttonup(i, num);
					break;

				case EVENT_TOUCH:
					globgui->b_guielems[i].bts[num].rcv->event_touch(i, num);
					break;

				case EVENT_RELEASE:
					globgui->b_guielems[i].bts[num].rcv->event_release(i, num);
					break;
				}
			}

			return;
		}
	}
	error("ERROR in global button event receiver: An event was caught from a panel that is not enlisted in b_gui!");
}

// receive clicks from input "buttons"
void gui_global_input_receiver(var internal_num, PANEL* pan)
{
	if (globgui == NULL) return;

	int numnum = _INT(internal_num)-1;
	// find pan in list

	for (int i = 0; i < B_GUIELEMS_ENTRIES; i++)
	{
		if (pan == (PANEL*)(globgui->b_guielems[i].ptr))
		{
			// if the button can be traced to an input
			for (int j = 0; j < globgui->b_guielems[i].num_inputs; j++)
			{
				if (globgui->b_guielems[i].inps[j].bt_num == _INT(internal_num))
				{
					if(*(globgui->ev->event_type) == EVENT_CLICK)
					{
						
						globgui->input_enable(i, j);

					}

					return; // dont iterate until error message
				}
			}
		}
	}
	error("ERROR in global input event receiver: An event was caught from a panel that is not enlisted in b_gui!");
}

// global key receiver
void gui_global_key_receiver(var ack_keycode)
{
	if (globgui == NULL) return;

	// don't react of input is enabled
	if (globgui->disable_keys || globgui->current_input != NULL) {return;}

	// find keycode in list
	for (std::list<s_key>::iterator it = globgui->keys.begin(); it != globgui->keys.end(); it++)
	{
		if (it->keycode == ack_keycode) // found
		{
			if (it->rcv != NULL)
			{
				it->rcv->event_click(it->panel_id, it->button_num);
			}
		}
	}
}


void gui_global_rlclick(int ack_keycode)
{
	if (globgui == NULL) return;
	if (globgui->current_input != NULL)
	{
		globgui->input_disable();
	}
}

// custom windows event handler
long gui_global_WinMessage(UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (globgui == NULL) return(0);

	if (msg == WM_CHAR && globgui->current_input != NULL)
	{
		uchar chrInput = (char)wParam;
		//std::cout << "Input is: " << chrInput << ", als wert: " << (int)chrInput << "\n";

		// if not esc, tab or cr
		if (chrInput != 0x1B && chrInput != 0x09 && chrInput != 0x0D)
		{
			// add or remove characters to string
			if (chrInput >= 32 &&
				globgui->current_input->length > 0 && (uint)globgui->current_input->length >= globgui->current_input->input_str.length())
			{
				globgui->current_input->input_str.append(1, chrInput);
			}
			if (chrInput == 8 && globgui->current_input->input_str.length() > 0) globgui->current_input->input_str.erase(globgui->current_input->input_str.length()-1);

			// display correct display_string
			if (globgui->current_input->mask == NULL) str_cpy(globgui->current_input->display_str, (char*)globgui->current_input->input_str.c_str());
			else
			{
				str_cpy(globgui->current_input->display_str, "");
				for (uint i = 0; i < globgui->current_input->input_str.length(); i++) str_cat(globgui->current_input->display_str, _CHR(globgui->current_input->mask));
			}

			str_cat(globgui->current_input->display_str, "|");
		}
		else
		{
			// cancel input
			globgui->input_disable();
		}
		return(1);
	}

	return(0);
}


// constructor of backend gui
b_gui::b_gui(ENGINE_VARS *aev, const char *filename)
{
	int id;
	const char *name, *background, *bmap, *chr, *flags;
	//char tmp[32]; // TODO: issue with const, is it a problem?
	float flt;
	int integer;
	PANEL *pan;
	TEXT *txt;
	BMAP *map;
	s_guielems* elem;
	char er[500];

	// set me as global gui
	globgui = this;

	// save engine vars
	ev = aev;

	ct_cursor = 0;
	current_input = NULL;
	current_input_elem = -1;
	disable_keys = false;

	// inticate that we are not ready
	ready = 0;

	// make me the event handler:
	ev->ScanMessage = gui_global_WinMessage;

	v(on_anykey) = (EVENT)gui_global_key_receiver;
	v(on_mouse_left) = (EVENT)gui_global_rlclick;
	v(on_mouse_right) = (EVENT)gui_global_rlclick;

	// clear entries
	for (int ct = 0; ct < B_GUIELEMS_ENTRIES; ct++) b_guielems[ct].gui_type = 0;


	// try to load document

	std::string full_filename;
	get_workdir(&full_filename);
	full_filename.append(filename);

	TiXmlDocument doc(full_filename);
	if (!doc.LoadFile())
	{
		error("Couldn't open file for backend gui!");
		error((char *)doc.ErrorDesc());

		return;
	}

	TiXmlHandle hDoc(&doc);
	TiXmlElement *pElem, *pSubElem;
	TiXmlHandle hRoot(0);

	// document base
	pElem=hDoc.FirstChildElement().Element();
	// should always have a valid root but handle gracefully if it does
	if (!pElem) {error("Error in backend gui"); return;}
	if (strcmp(pElem->Value(), "gui_elements") != 0) {error("Error in backend gui"); return;}

	// save this for later
	hRoot=TiXmlHandle(pElem);

	// get all entries
	pElem=hRoot.FirstChild().Element();
	for( pElem; pElem; pElem=pElem->NextSiblingElement())
	{
		///////////////////////////////////////////
		// PANEL
		if (strcmp(pElem->Value(), "panel") == 0)
		{
			// read entries, set values accordingly
			if (pElem->QueryIntAttribute ("id", &id) != TIXML_SUCCESS) {error("Error in backend gui: id of panel missing or invalid"); return;}
			if (id < 0 || id >= B_GUIELEMS_ENTRIES) {error("Error in backend gui: id of panel is invalid"); return;}

			elem = &(b_guielems[id]);

			elem->gui_type = B_GUI_TYPE_PANEL;

			if ((name = pElem->Attribute("name")) == NULL) {error("Error in backend gui: name of panel is missing"); return;}
			strncpy_s(elem->bname, name, 32);

			if (pElem->QueryIntAttribute ("group", &(elem->group)) != TIXML_SUCCESS) {error("Error in backend gui: group of panel missing or invalid"); return;}

			if (pElem->QueryIntAttribute ("layer", &(elem->layer)) != TIXML_SUCCESS) {error("Error in backend gui: layer of panel missing or invalid"); return;}

			if (pElem->QueryIntAttribute ("scale_mode", &(elem->scale_mode)) != TIXML_SUCCESS) {error("Error in backend gui: scale_mode of panel missing or invalid"); return;}

			if (pElem->QueryIntAttribute ("px", &(elem->px)) != TIXML_SUCCESS) {error("Error in backend gui: px of panel missing or invalid"); return;}

			if (pElem->QueryIntAttribute ("py", &(elem->py)) != TIXML_SUCCESS) {error("Error in backend gui: py of panel missing or invalid"); return;}

			// read optional extra entries
			if ((background = pElem->Attribute("background")) != NULL) strncpy_s(elem->background, background, 32);
			else elem->background[0] = '\0';

			flags = pElem->Attribute("flags");

			if (pElem->QueryIntAttribute ("alpha", &(elem->alpha)) != TIXML_SUCCESS) elem->alpha = 100;
			if (pElem->QueryIntAttribute ("size_x", &(elem->size_x)) != TIXML_SUCCESS) elem->size_x = 0;
			if (pElem->QueryIntAttribute ("size_y", &(elem->size_y)) != TIXML_SUCCESS) elem->size_y = 0;

			if (pElem->QueryFloatAttribute ("scale_x", &(elem->scale_x)) != TIXML_SUCCESS) elem->scale_x = 1.0;
			if (pElem->QueryFloatAttribute ("scale_y", &(elem->scale_y)) != TIXML_SUCCESS) elem->scale_y = 1.0;
			
			if (pElem->QueryIntAttribute ("color_r", &(integer)) != TIXML_SUCCESS) elem->color[0] = 255; else elem->color[0] = integer;
			if (pElem->QueryIntAttribute ("color_b", &(integer)) != TIXML_SUCCESS) elem->color[1] = 255; else elem->color[1] = integer;
			if (pElem->QueryIntAttribute ("color_g", &(integer)) != TIXML_SUCCESS) elem->color[2] = 255; else elem->color[2] = integer;

			// create panel in engine!
			pan = pan_create((char*)flags, elem->layer);

			pan->pos_x = elem->px;
			pan->pos_y = elem->py;
			pan->size_x = elem->size_x;
			pan->size_y = elem->size_y;
			pan->scale_x = elem->scale_x;
			pan->scale_y = elem->scale_y;
			pan->alpha = elem->alpha;
			pan->red = elem->color[0]; pan->green = elem->color[1]; pan->blue = elem->color[2];

			if (strnlen(elem->background, 32) > 0)
			{
				// TODO: load bmaps from hashmap to save space!
				map = bmap_create(elem->background);
				if (map == NULL) error("Error in backend gui: background file of a panel couldn't be opened");
				else
				{
					pan->bmap = map;
					if (pan->size_x == 0) pan->size_x = bmap_width(map);
					if (pan->size_y == 0) pan->size_y = bmap_height(map);
				}
			}

			// save panel pointer
			elem->ptr = (void*)pan;

			///////////////////////////////////////////////
			// subelements

			elem->num_buttons = 0;
			elem->num_digits = 0;
			elem->num_labels = 0;
			elem->num_windows = 0;
			elem->num_inputs = 0;

			// count panel elements
			pSubElem=pElem->FirstChildElement();
			for( pSubElem; pSubElem; pSubElem=pSubElem->NextSiblingElement())
			{
				///////////////////////////////////////////
				// BUTTON
				if (strcmp(pSubElem->Value(), "button") == 0) (elem->num_buttons)++;
				if (strcmp(pSubElem->Value(), "digit") == 0) (elem->num_digits)++;
				if (strcmp(pSubElem->Value(), "label") == 0) (elem->num_labels)++;
				if (strcmp(pSubElem->Value(), "window") == 0) (elem->num_windows)++;
				if (strcmp(pSubElem->Value(), "input") == 0) (elem->num_inputs)++;

			}

			
			if (elem->num_buttons > 0) elem->bts = new s_guibuttons[elem->num_buttons];
			if (elem->num_digits > 0) elem->digs = new s_guidigits[elem->num_digits];
			if (elem->num_labels > 0) elem->labs = new s_guilabels[elem->num_labels];
			if (elem->num_windows > 0) elem->wins = new s_guiwindows[elem->num_windows];
			if (elem->num_inputs > 0) elem->inps = new s_guiinputs[elem->num_inputs];

			int ibt = 0, idig = 0, ilab = 0, iwin = 0, iinp = 0;
			// fill arrays and add them to panel
			pSubElem=pElem->FirstChildElement();
			for( pSubElem; pSubElem; pSubElem=pSubElem->NextSiblingElement())
			{
				///////////////////////////////////////////
				// BUTTON

				if (strcmp(pSubElem->Value(), "button") == 0)
				{
					// button definition found
					if (pSubElem->QueryIntAttribute ("x", &(elem->bts[ibt].x)) != TIXML_SUCCESS) {error("Error in backend gui: x of button missing or invalid"); return;}
					if (pSubElem->QueryIntAttribute ("y", &(elem->bts[ibt].y)) != TIXML_SUCCESS) {error("Error in backend gui: y of button missing or invalid"); return;}

					// TODO: use hashmap
					if ((bmap = pSubElem->Attribute("bmapOn")) == NULL) {error("Error in backend gui: bmapOn of button is missing"); return;}
					else
					{
						elem->bts[ibt].bmapOn = bmap_create((char*)bmap);
						if (elem->bts[ibt].bmapOn == NULL) {error("Error in backend gui: bmapOn file of a button couldn't be opened"); return;}
					}
				
					// optional elements
					if ((bmap = pSubElem->Attribute("bmapOff")) == NULL) elem->bts[ibt].bmapOff = elem->bts[ibt].bmapOn;
					else
					{
						elem->bts[ibt].bmapOff = bmap_create((char*)bmap);
						if (elem->bts[ibt].bmapOff == NULL) {error("Error in backend gui: bmapOff file of a button couldn't be opened"); return;}
					}

					if ((bmap = pSubElem->Attribute("bmapOver")) == NULL) elem->bts[ibt].bmapOver = elem->bts[ibt].bmapOn;
					else
					{
						elem->bts[ibt].bmapOver = bmap_create((char*)bmap);
						if (elem->bts[ibt].bmapOver == NULL) {error("Error in backend gui: bmapOver file of a button couldn't be opened"); return;}
					}

					// button can not yet receive clicks
					elem->bts[ibt].rcv = NULL;

					// create button

					pan_setbutton(pan, 0, 1, elem->bts[ibt].x, elem->bts[ibt].y, elem->bts[ibt].bmapOn, elem->bts[ibt].bmapOff, elem->bts[ibt].bmapOver, elem->bts[ibt].bmapOff, gui_global_button_receiver, gui_global_button_receiver, gui_global_button_receiver);

					ibt++;
				}

				///////////////////////////////////////////
				// DIGIT

				if (strcmp(pSubElem->Value(), "digit") == 0)
				{
					if (pSubElem->QueryIntAttribute ("x", &(elem->digs[idig].x)) != TIXML_SUCCESS) {error("Error in backend gui: x of digit missing or invalid"); return;}
					if (pSubElem->QueryIntAttribute ("y", &(elem->digs[idig].y)) != TIXML_SUCCESS) {error("Error in backend gui: y of digit missing or invalid"); return;}

					if ((chr = pSubElem->Attribute("format")) == NULL) {error("Error in backend gui: format string of digit is missing"); return;}
					else
					{
						elem->digs[idig].format = new char[strlen(chr)+1];
						strncpy(elem->digs[idig].format, chr, strlen(chr)+1);
					}

					// TODO: use hashmap
					if ((chr = pSubElem->Attribute("font")) == NULL) {error("Error in backend gui: font string of digit is missing"); return;}
					else
					{
						elem->digs[idig].font = font_create((char*)chr);
						if (elem->digs[idig].font == NULL) {error("Error in backend gui: font creation of a digit failed"); return;}
					}

					// optional elemets
					if (pSubElem->QueryFloatAttribute ("factor", &(flt)) != TIXML_SUCCESS) elem->digs[idig].factor = 1.0;
					else elem->digs[idig].factor = flt;

					elem->digs[idig].display = 0;

					// create digit
					pan_setdigits(pan, 0, elem->digs[idig].x, elem->digs[idig].y, elem->digs[idig].format, elem->digs[idig].font, elem->digs[idig].factor, &elem->digs[idig].display);

					idig++;
				}

				///////////////////////////////////////////
				// LABEL

				if (strcmp(pSubElem->Value(), "label") == 0)
				{
					if (pSubElem->QueryIntAttribute ("x", &(elem->labs[ilab].x)) != TIXML_SUCCESS) {error("Error in backend gui: x of a label missing or invalid"); return;}
					if (pSubElem->QueryIntAttribute ("y", &(elem->labs[ilab].y)) != TIXML_SUCCESS) {error("Error in backend gui: y of a label missing or invalid"); return;}

					// TODO: use hashmap
					if ((chr = pSubElem->Attribute("font")) == NULL) {error("Error in backend gui: font string of a label is missing"); return;}
					else
					{
						elem->labs[ilab].font = font_create((char*)chr);
						if (elem->labs[ilab].font == NULL) {error("Error in backend gui: font creation of a label failed"); return;}
					}

					if ((chr = pSubElem->GetText()) == NULL) {error("Error in backend gui: caption text of a label is missing"); return;}
					else
					{
						elem->labs[ilab].str = str_create((char *)chr);
					}

					// create engine object
					pan_setstring(pan, 0, elem->labs[ilab].x, elem->labs[ilab].y, elem->labs[ilab].font, elem->labs[ilab].str);

					ilab++;
				}

				///////////////////////////////////////////
				// WINDOW

				if (strcmp(pSubElem->Value(), "window") == 0)
				{
					if (pSubElem->QueryIntAttribute ("x", &(elem->wins[iwin].x)) != TIXML_SUCCESS) {error("Error in backend gui: x of a window missing or invalid"); return;}
					if (pSubElem->QueryIntAttribute ("y", &(elem->wins[iwin].y)) != TIXML_SUCCESS) {error("Error in backend gui: y of a window missing or invalid"); return;}
					if (pSubElem->QueryIntAttribute ("dx", &(elem->wins[iwin].dx)) != TIXML_SUCCESS) {error("Error in backend gui: dx of a window missing or invalid"); return;}
					if (pSubElem->QueryIntAttribute ("dy", &(elem->wins[iwin].dy)) != TIXML_SUCCESS) {error("Error in backend gui: dy of a window missing or invalid"); return;}

					// TODO: use hashmap
					if ((bmap = pSubElem->Attribute("bmap")) == NULL) {error("Error in backend gui: bmap of a window is missing"); return;}
					else
					{
						elem->wins[iwin].bmap = bmap_create((char*)bmap);
						if (elem->wins[iwin].bmap == NULL) {error("Error in backend gui: bmap file of a window couldn't be opened"); return;}
					}

					// to be set from somewhere else
					elem->wins[iwin].px = 0;
					elem->wins[iwin].py = 0;

					pan_setwindow(pan, 0, elem->wins[iwin].x, elem->wins[iwin].y, elem->wins[iwin].dx, elem->wins[iwin].dy, elem->wins[iwin].bmap, &elem->wins[iwin].px, &elem->wins[iwin].px);

					iwin++;
				}

				///////////////////////////////////////////
				// INPUT FIELD

				if (strcmp(pSubElem->Value(), "input") == 0)
				{
					if (pSubElem->QueryIntAttribute ("x", &(elem->inps[iinp].x)) != TIXML_SUCCESS) {error("Error in backend gui: x of a input missing or invalid"); return;}
					if (pSubElem->QueryIntAttribute ("y", &(elem->inps[iinp].y)) != TIXML_SUCCESS) {error("Error in backend gui: y of a input missing or invalid"); return;}
	
					if ((bmap = pSubElem->Attribute("bmapOn")) == NULL) elem->inps[iinp].bmapOn = NULL;
					else
					{
						elem->inps[iinp].bmapOn = bmap_create((char*)bmap);
						if (elem->inps[iinp].bmapOn == NULL) {err("Error in backend gui: bmapOn file of a input couldn't be opened", id); return;}
					}

					if ((chr = pSubElem->Attribute("font")) == NULL) {sprintf_s(er, sizeof er, "Error in backend gui: font of input %d is missing", iinp); error(er); return;}
					else
					{
						elem->inps[iinp].font = font_create((char*)chr);
						if (elem->inps[iinp].font == NULL) {error("Error in backend gui: font creation of a input failed"); return;}
					}

					if (pSubElem->QueryIntAttribute ("offset_x", &(elem->inps[iinp].offset_x)) != TIXML_SUCCESS) {error("Error in backend gui: offset_x of a input missing or invalid"); return;}
					if (pSubElem->QueryIntAttribute ("offset_y", &(elem->inps[iinp].offset_y)) != TIXML_SUCCESS) {error("Error in backend gui: offset_y of a input missing or invalid"); return;}
	
				
					// optional elements
					if ((bmap = pSubElem->Attribute("bmapOff")) == NULL) elem->inps[iinp].bmapOff = elem->inps[iinp].bmapOn;
					else
					{
						elem->inps[iinp].bmapOff = bmap_create((char*)bmap);
						if (elem->inps[iinp].bmapOff == NULL) {error("Error in backend gui: bmapOff file of a input couldn't be opened"); return;}
					}

					if ((bmap = pSubElem->Attribute("bmapOver")) == NULL) elem->inps[iinp].bmapOver = elem->inps[iinp].bmapOn;
					else
					{
						elem->inps[iinp].bmapOver = bmap_create((char*)bmap);
						if (elem->inps[iinp].bmapOver == NULL) {error("Error in backend gui: bmapOver file of a input couldn't be opened"); return;}
					}

					if ((chr = pSubElem->Attribute("mask")) == NULL) elem->inps[iinp].mask = NULL;
					else
					{
						elem->inps[iinp].mask = str_create((char*)chr);
					}

					if (pSubElem->QueryIntAttribute ("length", &(elem->inps[iinp].length)) != TIXML_SUCCESS) elem->inps[iinp].length = -1;

					// create button which acts as background
					elem->inps[iinp].bt_num = _INT(pan_setbutton(pan, 0, 1, elem->inps[iinp].x, elem->inps[iinp].y, elem->inps[iinp].bmapOn, elem->inps[iinp].bmapOff, elem->inps[iinp].bmapOver, elem->inps[iinp].bmapOff, gui_global_input_receiver, gui_global_input_receiver, gui_global_input_receiver));

					// create label which displays the text
					elem->inps[iinp].display_str = str_create("");
					// determine x pos of label
					int pos_x = elem->inps[iinp].x + elem->inps[iinp].offset_x;
					if (((PANEL*)elem->ptr)->flags & CENTER_X) pos_x += _INT((var)bmap_width(elem->inps[iinp].bmapOn))/2;

					pan_setstring(pan, 0, pos_x, elem->inps[iinp].y + elem->inps[iinp].offset_y,
						elem->inps[iinp].font, elem->inps[iinp].display_str);


					iinp++;
				}

			}

			// end of panel parsing
		}

		///////////////////////////////////////////
		// TEXT
		if (strcmp(pElem->Value(), "text") == 0)
		{
			// read entries, set values accordingly
			if (pElem->QueryIntAttribute ("id", &id) != TIXML_SUCCESS) {error("Error in backend gui: id of text missing or invalid"); return;}
			if (id < 0 || id >= B_GUIELEMS_ENTRIES) {error("Error in backend gui: id of text is invalid"); return;}

			elem = &(b_guielems[id]);

			elem->gui_type = B_GUI_TYPE_TEXT;

			if ((name = pElem->Attribute("name")) == NULL) {error("Error in backend gui: name of text is missing"); return;}
			strncpy_s(elem->bname, name, 32);

			if (pElem->QueryIntAttribute ("group", &(elem->group)) != TIXML_SUCCESS) {error("Error in backend gui: group of text missing or invalid"); return;}

			if (pElem->QueryIntAttribute ("layer", &(elem->layer)) != TIXML_SUCCESS) {error("Error in backend gui: layer of text missing or invalid"); return;}

			if (pElem->QueryIntAttribute ("scale_mode", &(elem->scale_mode)) != TIXML_SUCCESS) {error("Error in backend gui: scale_mode of text missing or invalid"); return;}

			if (pElem->QueryIntAttribute ("px", &(elem->px)) != TIXML_SUCCESS) {error("Error in backend gui: px of text missing or invalid"); return;}

			if (pElem->QueryIntAttribute ("py", &(elem->py)) != TIXML_SUCCESS) {error("Error in backend gui: py of text missing or invalid"); return;}

			if ((chr = pElem->Attribute("font")) == NULL) {error("Error in backend gui: font of a text is missing"); return;}
			else
			{
				elem->font = font_create((char*)chr);
				if (elem->font == NULL) {error("Error in backend gui: font creation of a text failed"); return;}
			}

			// read optional extra entries
			if (pElem->QueryIntAttribute ("num_strings", &(elem->num_strings)) != TIXML_SUCCESS) elem->num_strings = 1;

			if (pElem->QueryIntAttribute ("alpha", &(elem->alpha)) != TIXML_SUCCESS) elem->alpha = 50;
			if (pElem->QueryIntAttribute ("size_x", &(elem->size_x)) != TIXML_SUCCESS) elem->size_x = 0;
			if (pElem->QueryIntAttribute ("size_y", &(elem->size_y)) != TIXML_SUCCESS) elem->size_y = 0;

			if (pElem->QueryIntAttribute ("color_r", &(integer)) != TIXML_SUCCESS) elem->color[0] = 255; else elem->color[0] = integer;
			if (pElem->QueryIntAttribute ("color_b", &(integer)) != TIXML_SUCCESS) elem->color[1] = 255; else elem->color[1] = integer;
			if (pElem->QueryIntAttribute ("color_g", &(integer)) != TIXML_SUCCESS) elem->color[2] = 255; else elem->color[2] = integer;

			flags = pElem->Attribute("flags");

			elem->scale_x = -1.0; // texts can not be scaled!
			elem->scale_y = -1.0;

			// create engine objects
			txt = txt_create(elem->num_strings, elem->layer);
			txt->pos_x = elem->px;
			txt->pos_y = elem->py;
			if (elem->size_x > 0) txt->size_x = elem->size_x;
			if (elem->size_y > 0) txt->size_y = elem->size_y;
			txt->font = elem->font;
			txt->alpha = elem->alpha;
			txt->red = elem->color[0]; txt->green = elem->color[1]; txt->blue = elem->color[2];

			if (flags != NULL)
			{
				// evaluate flags
				if (strstr(flags, "TRANSLUCENT") != NULL) txt->flags |= TRANSLUCENT;
				if (strstr(flags, "LIGHT") != NULL) txt->flags |= LIGHT;
				if (strstr(flags, "CENTER_X") != NULL) txt->flags |= CENTER_X;
				if (strstr(flags, "CENTER_Y") != NULL) txt->flags |= CENTER_Y;
				if (strstr(flags, "ARIGHT") != NULL) txt->flags |= ARIGHT;
				if (strstr(flags, "WWRAP") != NULL) txt->flags |= WWRAP;
				if (strstr(flags, "REVERSE") != NULL) txt->flags |= REVERSE;
				if (strstr(flags, "SHADOW") != NULL) txt->flags |= SHADOW;
				if (strstr(flags, "OUTLINE") != NULL) txt->flags |= OUTLINE;
			}

			// save text pointer
			elem->ptr = (void*)txt;

			// read actual strings:
			int istr = 0;
			// fill arrays and add them to panel
			pSubElem=pElem->FirstChildElement();
			for( pSubElem; pSubElem; pSubElem=pSubElem->NextSiblingElement())
			{
				if (strcmp(pSubElem->Value(), "string") == 0)
				{
					if (txt->pstring[istr] == NULL) {error("Error in backend gui: tried to write into a string that doesn't exist"); return;}
					str_cpy(txt->pstring[istr], (char*)pSubElem->GetText());
					istr++;
				}
			}

		}
	}

	// no error has happend, set my ready to 1!
	ready = 1;

}

b_gui::~b_gui()
{
	// remove all created stuff!
	s_guielems *elem;

	// remove globgui
	globgui = NULL;

	// make me the event handler:
	ev->ScanMessage = NULL;

	v(on_mouse_left) = (EVENT)NULL;
	v(on_mouse_right) = (EVENT)NULL;

	// clear entries
	for (int id = 0; id < B_GUIELEMS_ENTRIES; id++)
	{
		elem = &(b_guielems[id]);

		if (elem->gui_type == B_GUI_TYPE_PANEL)
		{
			printf("Deleting PANEL %i \n", id);

			PANEL *pan = (PANEL*) elem->ptr;

			// removage of panel deletes bmap
			ptr_remove(pan->bmap);
			pan->bmap = NULL;

			int i;

			for (i = 0; i < elem->num_buttons; i++)
			{
				// delete bmaps
				// TODO: proper deletion, how's that possible?
				ptr_remove(elem->bts[i].bmapOff);
				ptr_remove(elem->bts[i].bmapOn);
				ptr_remove(elem->bts[i].bmapOver);

				pan_setbutton(pan, i+1, 1, 0, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
			}

			for (i = 0; i < elem->num_digits; i++)
			{
				// delete digits
				delete[] elem->digs[i].format;
				ptr_remove(elem->digs[i].font);

			}

			for (i = 0; i < elem->num_labels; i++)
			{
				// delete labels
				ptr_remove(elem->labs[i].font);
				ptr_remove(elem->labs[i].str);
			}

			for (i = 0; i < elem->num_windows; i++)
			{
				// removage of panel deletes windows
				ptr_remove(elem->wins[i].bmap);

				pan_setwindow(pan, i+1, 0, 0, 0, 0, NULL, NULL, NULL);
			}

			for (i = 0; i < elem->num_inputs; i++)
			{
				// delete inputs
				ptr_remove(elem->inps[i].bmapOn);
				ptr_remove(elem->inps[i].font);
				ptr_remove(elem->inps[i].bmapOff);
				ptr_remove(elem->inps[i].bmapOver);
				ptr_remove(elem->inps[i].mask);
				ptr_remove(elem->inps[i].display_str);

				pan_setbutton(pan, elem->num_buttons+i+1, 1, 0, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

			}

						
			if (elem->num_buttons > 0) delete[] elem->bts;
			if (elem->num_digits > 0) delete[] elem->digs;
			if (elem->num_labels > 0) delete[] elem->labs;
			if (elem->num_windows > 0) delete[] elem->wins;
			if (elem->num_inputs > 0) delete[] elem->inps;



			// end of panel subtypes, delete panel

			ptr_remove(pan);
		}

		if (elem->gui_type == B_GUI_TYPE_TEXT)
		{
			// delete text
			printf("Deleting TEXT %i \n", id);

			// delete created font
			ptr_remove(elem->font);

			// delete strings
			TEXT* txt = (TEXT*)elem->ptr;
			for (int i = 0; i < txt->strings; i++) 
			  if ((txt->pstring)[i]) ptr_remove((txt->pstring)[i]);

			// delete text
			ptr_remove(txt);
		}
	}


}


void b_gui::frame(int time_frame)
{
	// make curser blink
	ct_cursor += time_frame;
	if (ct_cursor > 200)
	{
		ct_cursor %= 200;
		if (current_input != NULL)
		{
			int len = strlen(_CHR(current_input->display_str));
			if (len > 0 && _CHR(current_input->display_str)[len-1] == '|') str_trunc(current_input->display_str, 1);
			else str_cat(current_input->display_str, "|");
		}
	}
}




//////////////////////////////////////////////////////////////////////////////
// resize all gui elements depending on the current screen size
void b_gui::resize_elems(int screen_x, int screen_y)
{
	int i;
	var px = 0, py = 0, sx = 1, sy = 1;
	PANEL* pn;
	TEXT* te;
	
	for (i = 0; i < B_GUIELEMS_ENTRIES; i++)
	{
		if (b_guielems[i].gui_type > 0)
		{
			if (b_guielems[i].scale_mode == B_GUI_POS_SCALE)
			{
				// Scale according to screen size
				px = screen_x*(b_guielems[i].px/100);
				py = screen_y*(b_guielems[i].py/100);
				sx = screen_x/800.;
				sy = screen_y/600.;
				
			}
			else
			if (b_guielems[i].scale_mode == B_GUI_POS_CENTER)
			{
				// don't scale, align in center
				px = (screen_x/2)+b_guielems[i].px;
				py = (screen_y/2)+b_guielems[i].py;
				sx = 1;
				sy = 1;
				
			}
			else
			if (b_guielems[i].scale_mode == B_GUI_POS_UPLEFT)
			{
				// don't scale, align in upper left corner
				px = b_guielems[i].px;
				py = b_guielems[i].py;
				sx = 1;
				sy = 1;
				
			}
			else
			if (b_guielems[i].scale_mode == B_GUI_POS_DOWNLEFT)
			{
				// don't scale, align in upper left corner
				px = b_guielems[i].px;
				py = screen_y + b_guielems[i].py;
				sx = 1;
				sy = 1;
				
			}
			else
			if (b_guielems[i].scale_mode == B_GUI_POS_UPCENTER)
			{
				// don't scale, align in upper middle
				px = (screen_x/2) + b_guielems[i].px;
				py = b_guielems[i].py;
				sx = 1;
				sy = 1;
				
			}
			else
			if (b_guielems[i].scale_mode == B_GUI_POS_DOWNCENTER)
			{
				// don't scale, align in lower middle
				px = (screen_x/2) + b_guielems[i].px;
				py = screen_y + b_guielems[i].py;
				sx = 1;
				sy = 1;
				
			}
			else
			if (b_guielems[i].scale_mode == B_GUI_POS_LEFTCENTER)
			{
				// don't scale, align 
				px = b_guielems[i].px;
				py = (screen_y/2) + b_guielems[i].py;
				sx = 1;
				sy = 1;
				
			}
			else
			if (b_guielems[i].scale_mode == B_GUI_POS_RIGHTCENTER)
			{
				// don't scale, align
				px =screen_x + b_guielems[i].px;
				py = (screen_y/2) + b_guielems[i].py;
				sx = 1;
				sy = 1;
				
			}
			else
			if (b_guielems[i].scale_mode == B_GUI_POS_UPRIGHT)
			{
				// don't scale, align
				px = screen_x + b_guielems[i].px;
				py = b_guielems[i].py;
				sx = 1;
				sy = 1;
				
			}
			else
			if (b_guielems[i].scale_mode == B_GUI_POS_DOWNRIGHT)
			{
				// don't scale, align
				px = screen_x + b_guielems[i].px;
				py = screen_y + b_guielems[i].py;
				sx = 1;
				sy = 1;
				
			}
			else {error("B_GUI_POS mode not implemented.");}
			//TODO: Implement other scaling methods
			
			// apply calculations
			if (b_guielems[i].gui_type == B_GUI_TYPE_PANEL)
			{
				pn = (PANEL*)(b_guielems[i]).ptr;
				pn->pos_x = px;
				pn->pos_y = py;
		
				pn->scale_x = sx*b_guielems[i].scale_x;
				pn->scale_y = sy*b_guielems[i].scale_y;
			}
			if (b_guielems[i].gui_type == B_GUI_TYPE_TEXT)
			{
				te = (TEXT*)(b_guielems[i]).ptr;
				te->pos_x = px;
				te->pos_y = py;
			}
		}
	}
}



//////////////////////////////////////////////////////////////////////////////
// find a name in the array
int b_gui::lookup_name(const char* name)
{
	int i;
	
	for (i = 0; i < B_GUIELEMS_ENTRIES; i++)
	{
		if (strncmp(b_guielems[i].bname, name, 32) == 0) return (i);
	}
	std::ostringstream str;
	str << "Did not find panel \"" << name << "\" in backend!" << std::endl;
	err(str.str().c_str(), -1);
	return (-1);
}


//////////////////////////////////////////////////////////////////////////////
// show a group
void b_gui::show_group(int tgroup)
{
	int i;
	
	for (i = 0; i < B_GUIELEMS_ENTRIES; i++)
	{
		if (b_guielems[i].group == tgroup)
		{
			if (b_guielems[i].gui_type == B_GUI_TYPE_PANEL) ((PANEL*)(b_guielems[i]).ptr)->flags |= SHOW;
			if (b_guielems[i].gui_type == B_GUI_TYPE_TEXT) ((TEXT*)(b_guielems[i]).ptr)->flags |= SHOW;
		}
	}
}


//////////////////////////////////////////////////////////////////////////////
// hide a group
void b_gui::hide_group(int tgroup)
{
	int i;
	
	for (i = 0; i < B_GUIELEMS_ENTRIES; i++)
	{
		if (b_guielems[i].group == tgroup)
		{
			if (b_guielems[i].gui_type == B_GUI_TYPE_PANEL) ((PANEL*)(b_guielems[i]).ptr)->flags &= ~SHOW;
			if (b_guielems[i].gui_type == B_GUI_TYPE_TEXT) ((TEXT*)(b_guielems[i]).ptr)->flags &= ~SHOW;
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
// toggle a group
void b_gui::toggle_group(int tgroup)
{
	int i;
	
	for (i = 0; i < B_GUIELEMS_ENTRIES; i++)
	{
		if (b_guielems[i].group == tgroup)
		{
			if (b_guielems[i].gui_type == B_GUI_TYPE_PANEL) ((PANEL*)(b_guielems[i]).ptr)->flags ^= SHOW;
			if (b_guielems[i].gui_type == B_GUI_TYPE_TEXT) ((TEXT*)(b_guielems[i]).ptr)->flags ^= SHOW;
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
// move a group, important function
void b_gui::move_group(int tgroup, int newx, int newy)
{
	int i;
	int master_found = 0, diff_x, diff_y;
	PANEL* pan; TEXT* txt;
	
	for (i = 0; i < B_GUIELEMS_ENTRIES; i++)
	{
		if (b_guielems[i].group == tgroup)
		{
			if (master_found == 0)
			{
				master_found = 1;
				if (b_guielems[i].gui_type == B_GUI_TYPE_PANEL)
				{
					pan = (PANEL*)(b_guielems[i].ptr);
					diff_x = newx - pan->pos_x;
					diff_y = newy - pan->pos_y;
					pan->pos_x = newx;
					pan->pos_y = newy;
				}
				if (b_guielems[i].gui_type == B_GUI_TYPE_TEXT)
				{
					txt = (TEXT*)(b_guielems[i].ptr);
					diff_x = newx - txt->pos_x;
					diff_y = newy - txt->pos_y;
					txt->pos_x = newx;
					txt->pos_y = newy;
				}
			}
			else
			{
				if (b_guielems[i].gui_type == B_GUI_TYPE_PANEL)
				{
					pan = (PANEL*)(b_guielems[i].ptr);
					pan->pos_x += diff_x;
					pan->pos_y += diff_y;
				}
				if (b_guielems[i].gui_type == B_GUI_TYPE_TEXT)
				{
					txt = (TEXT*)(b_guielems[i].ptr);
					txt->pos_x += diff_x;
					txt->pos_y += diff_y;
				}
			}
		}
	}
}

///////////////////////////////////////////////////////////
// register a button receiver for a button
void b_gui::button_set_receiver(int panel_id, int button_id, gui_button_receiver* rcv)
{
	if (panel_id >= B_GUIELEMS_ENTRIES || panel_id < 0) {error("ERROR in button_set_receiver: invalid panel_id given!"); return;}
	if (button_id >= b_guielems[panel_id].num_buttons || button_id < 0) {error("ERROR in button_set_receiver: invalid button_id given!"); return;}

	b_guielems[panel_id].bts[button_id].rcv = rcv;
}

void b_gui::key_set_receiver(var keycode, int panel_id, int button_num, gui_button_receiver* r)
{
	s_key k;
	k.keycode = keycode;
	k.panel_id = panel_id;
	k.button_num = button_num;
	k.rcv = r;

	keys.push_back(k);
}




bool b_gui::button_exists(int panel_id, int button_id)
{
	if (panel_id >= B_GUIELEMS_ENTRIES || panel_id < 0) return false;
	if (button_id >= b_guielems[panel_id].num_buttons || button_id < 0) return false;
	return true;
}

// create a single button outside of this

void b_gui::button_add(int panel_id, int px, int py, BMAP *bmapOn, BMAP *bmapOff, BMAP *bmapOver)
{
	if (panel_id >= B_GUIELEMS_ENTRIES || panel_id < 0) {error("ERROR in add_button: invalid panel_id given!"); return;}

	s_guielems *elem = &(b_guielems[panel_id]);

	int ibt = elem->num_buttons; // slot to use

	// resize button struct
	s_guibuttons *b = new s_guibuttons[elem->num_buttons+1];
	for (int i = 0; i < elem->num_buttons; i++) b[i] = elem->bts[i];
	if (elem->num_buttons > 0) delete[] elem->bts;
	elem->bts = b;

	elem->bts[ibt].x = px;
	elem->bts[ibt].y = py;
	elem->bts[ibt].bmapOn = bmapOn;
	elem->bts[ibt].bmapOff = bmapOff;
	elem->bts[ibt].bmapOver = bmapOver;

	// now create the button
	pan_setbutton((PANEL*)elem->ptr, 0, 1, elem->bts[ibt].x, elem->bts[ibt].y, elem->bts[ibt].bmapOn, elem->bts[ibt].bmapOff, elem->bts[ibt].bmapOver, elem->bts[ibt].bmapOff, gui_global_button_receiver, gui_global_button_receiver, gui_global_button_receiver);

	(elem->num_buttons)++;
}

// modify a button
void b_gui::button_change(int panel_id, int but_id, float px, float py, BMAP *bmapOn, BMAP *bmapOff, BMAP *bmapOver)
{
	// get panel pointer
	if (panel_id >= B_GUIELEMS_ENTRIES || panel_id < 0) {error("ERROR in change_button: invalid panel_id given!"); return;}

	s_guielems *elem = &(b_guielems[panel_id]);

	if (but_id >= elem->num_buttons || but_id < 0) {error("ERROR in change_button: invalid button_id given!"); return;}

	elem->bts[but_id].x = (int)px;
	elem->bts[but_id].y = (int)py;
	elem->bts[but_id].bmapOn = bmapOn;
	elem->bts[but_id].bmapOff = bmapOff;
	elem->bts[but_id].bmapOver = bmapOver;

	pan_setbutton((PANEL*)elem->ptr, but_id+1, 1, (int)px, (int)py, bmapOn, bmapOff, bmapOver, bmapOff, gui_global_button_receiver, gui_global_button_receiver, gui_global_button_receiver);

}

////////////////////////////////////////////
// Label modifier

void b_gui::label_add(int panel_id, int px, int py, const char* font, const char* str)
{
	if (panel_id >= B_GUIELEMS_ENTRIES || panel_id < 0) {error("ERROR in add_label: invalid panel_id given!"); return;}

	s_guielems *elem = &(b_guielems[panel_id]);

	int ilb = elem->num_labels; // slot to use

	// resize label struct
	s_guilabels *l = new s_guilabels[ilb+1];
	for (int i = 0; i < elem->num_labels; i++) l[i] = elem->labs[i];
	if (elem->num_labels > 0) delete[] elem->labs;
	elem->labs = l;

	elem->labs[ilb].x = px;
	elem->labs[ilb].y = py;
	elem->labs[ilb].font = font_create((char*)font);
	elem->labs[ilb].str = str_create((char*)str);

	// now create the label
	pan_setstring((PANEL*)elem->ptr, 0, (int)px, (int)py, elem->labs[ilb].font, elem->labs[ilb].str);

	(elem->num_labels)++;
}





///////////////////////////////////////////////////////////
// get the STRING* of a text element
char* b_gui::txt_get_value(int id, int num_str)
{
	TEXT *txt = (TEXT*)b_guielems[id].ptr;
	if (txt == NULL) return NULL;
	return _CHR(txt->pstring[num_str]);
}

// modify a text
void b_gui::txt_set_value(int txt_id, int txt_num, std::string *txt)
{
	// get text pointer
	if (txt_id >= B_GUIELEMS_ENTRIES || txt_id < 0) {error("ERROR in txt_set_value: invalid txt_id given!"); return;}

	s_guielems *elem = &(b_guielems[txt_id]);

	if (elem->gui_type != B_GUI_TYPE_TEXT)  {error("ERROR in txt_set_value: item on txt_id is not a TEXT!"); return;}

	if (txt_num >= elem->num_strings || txt_num < 0) {error("ERROR in txt_set_value: invalid txt_num given"); return;}

	TEXT* t = (TEXT *)elem->ptr;
	str_cpy(t->pstring[txt_num], (char *)txt->c_str());
}

void b_gui::txt_set_alpha(int txt_id, double alpha)
{
	// get text pointer
	if (txt_id >= B_GUIELEMS_ENTRIES || txt_id < 0) {error("ERROR in txt_set_alpha: invalid txt_id given!"); return;}
	s_guielems *elem = &(b_guielems[txt_id]);

	if (elem->gui_type != B_GUI_TYPE_TEXT)  {error("ERROR in txt_set_alpha: item on txt_id is not a TEXT!"); return;}

	TEXT* t = (TEXT *)elem->ptr;
	t->alpha = alpha;
}

void b_gui::txt_set_color(int txt_id, uchar color_r, uchar color_g, uchar color_b)
{
	// get text pointer
	if (txt_id >= B_GUIELEMS_ENTRIES || txt_id < 0) {error("ERROR in txt_set_alpha: invalid txt_id given!"); return;}
	s_guielems *elem = &(b_guielems[txt_id]);

	if (elem->gui_type != B_GUI_TYPE_TEXT)  {error("ERROR in txt_set_alpha: item on txt_id is not a TEXT!"); return;}

	TEXT* t = (TEXT *)elem->ptr;
	t->red = color_r;
	t->green = color_g;
	t->blue = color_b;
}


/////////////////////////////////////////////////////////////
// modify a label

void b_gui::label_set_caption(int panel_id, int label_id, const char* caption)
{
	if (panel_id >= B_GUIELEMS_ENTRIES || panel_id < 0) {error("ERROR in label_set_caption: invalid panel_id given!"); return;}
	if (label_id >= b_guielems[panel_id].num_labels || label_id < 0) {error("ERROR in label_set_caption: invalid label_id given!"); return;}

	s_guielems *elem = &(b_guielems[panel_id]);

	str_cpy(elem->labs[label_id].str, (char *)caption);
}

void b_gui::label_set_color(int panel_id, int label_id, uchar red, uchar green, uchar blue)
{
	if (panel_id >= B_GUIELEMS_ENTRIES || panel_id < 0) {error("ERROR in label_set_color: invalid panel_id given!"); return;}
	if (label_id >= b_guielems[panel_id].num_labels || label_id < 0) {error("ERROR in label_set_color: invalid label_id given!"); return;}

	s_guielems *elem = &(b_guielems[panel_id]);

	PANEL *pan = (PANEL*)elem->ptr;

	COLOR c;
	c.red = red;
	c.green = green;
	c.blue = blue;

	pan_setcolor(pan, 1, label_id+1, &c);
}

/////////////////////////////////////////////////////////////
// digits
void b_gui::digit_set_double(int panel_id, int digit_id, double value)
{
	if (panel_id >= B_GUIELEMS_ENTRIES || panel_id < 0) {error("ERROR in digit_set_double: invalid panel_id given!"); return;}
	if (digit_id >= b_guielems[panel_id].num_digits || digit_id < 0) {error("ERROR in digit_set_double: invalid digit_id given!"); return;}

	s_guielems *elem = &(b_guielems[panel_id]);

	elem->digs[digit_id].display = _VAR(value);
}

void b_gui::digit_set_int(int panel_id, int digit_id, int value)
{
	if (panel_id >= B_GUIELEMS_ENTRIES || panel_id < 0) {error("ERROR in digit_set_int: invalid panel_id given!"); return;}
	if (digit_id >= b_guielems[panel_id].num_digits || digit_id < 0)
	{error("ERROR in digit_set_int: invalid digit_id given!"); return;}

	s_guielems *elem = &(b_guielems[panel_id]);

	elem->digs[digit_id].display = _VAR(value);
}

/////////////////////////////////////////////////////////////
// window

void b_gui::window_set_bmap(int panel_id, int window_id, BMAP* bitmap)
{
	if (panel_id >= B_GUIELEMS_ENTRIES || panel_id < 0) {error("ERROR in window_set_bmap: invalid panel_id given!"); return;}
	if (window_id >= b_guielems[panel_id].num_windows || window_id < 0) {error("ERROR in window_set_bmap: invalid window_id given!"); return;}

	s_guielems *elem = &(b_guielems[panel_id]);

	elem->wins[window_id].bmap = bitmap;

	pan_setwindow((PANEL*)elem->ptr, window_id+1, elem->wins[window_id].x, elem->wins[window_id].y, elem->wins[window_id].dx, elem->wins[window_id].dy, bitmap, &elem->wins[window_id].px, &elem->wins[window_id].py);
}

void b_gui::window_set_px(int panel_id, int window_id, int value)
{
	if (panel_id >= B_GUIELEMS_ENTRIES || panel_id < 0) {error("ERROR in window_set_px: invalid panel_id given!"); return;}
	if (window_id >= b_guielems[panel_id].num_windows || window_id < 0) {error("ERROR in window_set_px: invalid window_id given!"); return;}

	s_guielems *elem = &(b_guielems[panel_id]);

	elem->wins[window_id].px = _VAR(value);
}

void b_gui::window_set_py(int panel_id, int window_id, int value)
{
	if (panel_id >= B_GUIELEMS_ENTRIES || panel_id < 0) {error("ERROR in window_set_py: invalid panel_id given!"); return;}
	if (window_id >= b_guielems[panel_id].num_windows || window_id < 0) {error("ERROR in window_set_py: invalid window_id given!"); return;}

	s_guielems *elem = &(b_guielems[panel_id]);

	elem->wins[window_id].py = _VAR(value);
}

void b_gui::window_set_dx(int panel_id, int window_id, int value)
{
	if (panel_id >= B_GUIELEMS_ENTRIES || panel_id < 0) {error("ERROR in window_set_dx: invalid panel_id given!"); return;}
	if (window_id >= b_guielems[panel_id].num_windows || window_id < 0) {error("ERROR in window_set_dx: invalid window_id given!"); return;}

	s_guielems *elem = &(b_guielems[panel_id]);

	elem->wins[window_id].dx = _VAR(value);

	pan_setwindow((PANEL*)elem->ptr, window_id+1, elem->wins[window_id].x, elem->wins[window_id].y, elem->wins[window_id].dx, elem->wins[window_id].dy, elem->wins[window_id].bmap, &elem->wins[window_id].px, &elem->wins[window_id].py);
}

void b_gui::window_set_dy(int panel_id, int window_id, int value)
{
	if (panel_id >= B_GUIELEMS_ENTRIES || panel_id < 0) {error("ERROR in window_set_dy: invalid panel_id given!"); return;}
	if (window_id >= b_guielems[panel_id].num_windows || window_id < 0) {error("ERROR in window_set_dy: invalid window_id given!"); return;}

	s_guielems *elem = &(b_guielems[panel_id]);

	elem->wins[window_id].dy = _VAR(value);

	pan_setwindow((PANEL*)elem->ptr, window_id+1, elem->wins[window_id].x, elem->wins[window_id].y, elem->wins[window_id].dx, elem->wins[window_id].dy, elem->wins[window_id].bmap, &elem->wins[window_id].px, &elem->wins[window_id].py);
}



void b_gui::input_enable(int pan_id, int input_id)
{
	if (current_input == &b_guielems[pan_id].inps[input_id]) return; // already active again.. do nothing

	if (current_input != NULL) // remove | from last input if any
	{

	}

	s_guiinputs *inp = &b_guielems[pan_id].inps[input_id];

	// set new input
	current_input = inp;
	current_input_elem = pan_id;

	// change button
	pan_setbutton((PANEL*)b_guielems[pan_id].ptr, inp->bt_num, 1, inp->x, inp->y, inp->bmapOn, inp->bmapOn, inp->bmapOn, inp->bmapOn, gui_global_input_receiver, gui_global_input_receiver, gui_global_input_receiver);
}

void b_gui::input_disable()
{
	if (current_input == NULL) return;

	// remove curser
	int len = strlen(_CHR(current_input->display_str));
	if (len > 0 && _CHR(current_input->display_str)[len-1] == '|') str_trunc(current_input->display_str, 1);

	// change button back
	PANEL* pan = (PANEL*)b_guielems[current_input_elem].ptr;
	pan_setbutton(pan, current_input->bt_num, 1, current_input->x, current_input->y, current_input->bmapOn, current_input->bmapOff, current_input->bmapOver, current_input->bmapOn, gui_global_input_receiver, gui_global_input_receiver, gui_global_input_receiver);
	
	current_input = NULL;
	current_input_elem = -1;
}

/////////////////////////////////////////////////////
// errors

void b_gui::err(const char *msg, int id)
{
	std::ostringstream pr;
	pr << "ERROR in b_gui in id=" << id << " :" << msg;

	error((char *)pr.str().c_str());
}