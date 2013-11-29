#include "b_effects.h"

#include "tinyxml.h"
#include <iostream>
#include <stdio.h>
#include "helper.h"
#include <string.h>


b_effects *b_ef;



//////////////////////////////////////////////////////////////
// globally called particles function

// events
void parev_alphafade(PARTICLE *p)
{
	p->skill[0] -= v(time_step);
	if (p->skill[0] <= 0)
	{
		p->alpha -= p->skill[1]*v(time_step);
		if (p->alpha <= 0) p->lifespan = 0;
	}
}


// fncs
void par_vel_random(PARTICLE *p, s_instr_particles *ip, ENTITY* ent)
{
	// RANDOMIZE vel_x with values given in skill_x, skill_y, skill_z

	p->vel_x = p->vel_x*(((var)1)-p->skill_x) + ((float)rand()/(float)RAND_MAX)*p->vel_x*p->skill_x;
	if (((float)rand()/(float)RAND_MAX) > 0.5)  p->vel_x = -(float)(p->vel_x);
	p->vel_y = p->vel_y*(((var)1)-p->skill_y) + ((float)rand()/(float)RAND_MAX)*p->vel_y*p->skill_y;
	if (((float)rand()/(float)RAND_MAX) > 0.5) p->vel_y = -(float)(p->vel_y);
	p->vel_z = p->vel_z*(((var)1)-p->skill_z) + ((float)rand()/(float)RAND_MAX)*p->vel_z*p->skill_z;
}

void par_vertex_random(PARTICLE *p, s_instr_particles *ip, ENTITY* ent)
{
	// get a random vertex from model
	VECTOR vtemp;
	vec_for_vertex(&vtemp, ent, (int)(rand() % (int)((var)ent_status(ent, 0)+1)));

	p->x = vtemp.x;
	p->y = vtemp.y;
	p->z = vtemp.z;
}

void par_size_random(PARTICLE *p, s_instr_particles *ip, ENTITY* ent)
{
	// get a random position within size
	VECTOR vtemp;
	//vec_for_vertex(&vtemp, ent, (int)(rand() % (int)((var)ent_status(ent, 0)+1)));

	p->x = ((float)ent->x + (float)ent->min_x) +  ((float)ent->max_x + -(float)ent->min_x)*((double) rand() / (RAND_MAX));
	p->y = ((float)ent->y + (float)ent->min_y) +  ((float)ent->max_y + -(float)ent->min_y)*((double) rand() / (RAND_MAX));
	p->z = ent->z;
}




void parev_general(PARTICLE *p)
{
	// get data out of vel_..
	ENTITY* caster = (ENTITY*)(void*)p->vel_x;

	// get backend info
	s_effect *e = b_ef->at((int)p->vel_y);
	if (e == NULL) {error("FATAL ERROR: invalid id in effect parev_general"); return;}

	// get instruction
	s_instr *instr = e->commands.at((int)p->vel_z);
	if (instr->type != EFFECT_TYPE_PARTICLES) {error("FATAL ERROR: invalid instruction type in effect parev_general"); return;}

	s_instr_particles *ip = (s_instr_particles*)instr;

	p->vel_x = ip->vel_x;
	p->vel_y = ip->vel_y;
	p->vel_z = ip->vel_z;

	if (ip->bmap != NULL) p->bmap = ip->bmap;
	p->alpha = ip->alpha;
	p->red = ip->color[0];
	p->green = ip->color[1];
	p->blue = ip->color[2];
	p->size = ip->size;
	p->lifespan = ip->lifespan;
	p->gravity = ip->gravity;
	
	p->skill[0] = ip->skill_a;
	p->skill[1] = ip->skill_b;
	p->skill[2] = ip->skill_c;
	p->skill[3] = ip->skill_d;
	p->skill_x = ip->skill_x;
	p->skill_y = ip->skill_y;
	p->skill_z = ip->skill_z;

	p->flags = ip->flags;

	if (ip->fnc_type > 0)
	{
		switch (ip->fnc_type)
		{
			case PAR_FNC_TYPE_VEL_RANDOM:
				par_vel_random(p, ip, caster); break;
			case PAR_FNC_TYPE_VERTEX_RANDOM:
				par_vertex_random(p, ip, caster); break;
			case PAR_FNC_TYPE_SIZE_RANDOM:
				par_size_random(p, ip, caster); break;
		}
	}

	if (ip->event_type > 0)
	{
		switch(ip->event_type)
		{
			case PAR_EVENT_TYPE_ALPHAFADE:
				p->event = (EVENT)parev_alphafade; break;
		}
	}
	else p->event = NULL;
}








////////////////////////////////////////////////////////////////////
// class function


b_effects::b_effects(b_gui* gui, const char *filename)
{
	int id;
	const char *chr;
	s_effect *ef;

	this->gui = gui;
	this->font = font_create("Times#14");

	// get stuff from gui backend
	txt_message_id[0] = gui->lookup_name("txt_message1");
	if (txt_message_id[0] < 0) {err("Could not find txt_message1 in backend gui!", -1); return;}
	txt_message_group[0] = 15;
	txt_message_use[0] = false;
	txt_message_id[1] = gui->lookup_name("txt_message2");
	if (txt_message_id[1] < 0) {err("Could not find txt_message2 in backend gui!", -1); return;}
	txt_message_group[1] = 16;
	txt_message_use[1] = false;
	txt_message_id[2] = gui->lookup_name("txt_message3");
	if (txt_message_id[2] < 0) {err("Could not find txt_message3 in backend gui!", -1); return;}
	txt_message_group[2] = 17;
	txt_message_use[2] = false;


	// clear all entries
	for (int i = 0; i < B_EFFECTS_ENTRIES; i++)
	{
		effects[i].valid = false;
	}

	// load file
    std::string full_filename;
	get_workdir(&full_filename);
	full_filename.append(filename);

	TiXmlDocument doc(full_filename);
	if (!doc.LoadFile()) {err("Couldn't open file for effects backend!", -200);}

	TiXmlHandle hDoc(&doc);
	TiXmlElement *pElem, *pSubElem;
	TiXmlHandle hRoot(0);

	// document base
	pElem=hDoc.FirstChildElement().Element();
	// should always have a valid root but handle gracefully if it does
	if (!pElem) {err("backend effects: Root Element not found, file empty?", -1); return;}
	if (strcmp(pElem->Value(), "backend_effects") != 0) {err("backend_effects definition not found", -1); return;}

	// save this for later
	hRoot=TiXmlHandle(pElem);

	// get all entries
	pElem=hRoot.FirstChild().Element();
	for( ; pElem; pElem=pElem->NextSiblingElement())
	{
		///////////////////////////////////////////
		// class definition
		if (strcmp(pElem->Value(), "effect") == 0)
		{

			if (pElem->QueryIntAttribute ("id", &id) != TIXML_SUCCESS) {err("id of effect missing or invalid", id); return;}
			if (id < 0 || id >= B_EFFECTS_ENTRIES) {err("id of effect is invalid", id); return;}

			ef = &(effects[id]);

			ef->name.assign("unnamed");
			if ((chr = pElem->Attribute("name")) == NULL) ef->name.assign(chr);			

			ef->valid = true;
            // query subelements

			for(pSubElem=pElem->FirstChildElement(); pSubElem; pSubElem=pSubElem->NextSiblingElement())
			{

				////////////////////////////////////////
				// SOUND

				if (strcmp(pSubElem->Value(), "sound") == 0)
				{
					s_instr_sound *es = new s_instr_sound;
					es->type = EFFECT_TYPE_SOUND;

					if ((chr = pSubElem->Attribute("file")) == NULL) {err("file of sound instruction is missing", id); return;}
					es->snd = snd_create((char*)chr); // TODO: hash
					if (es->snd == NULL)
					{
						// on error
						std::ostringstream s;
						s << "Could not open effect sound file: \"" << chr << "\"";
						err(s.str().c_str(), id); 
						return;
					}

					if (pSubElem->QueryIntAttribute("volume", &(es->volume)) != TIXML_SUCCESS) {err("volume of a sound instruction missing or invalid", id); return;}
					if (pSubElem->QueryIntAttribute("balance", &(es->balance)) != TIXML_SUCCESS) es->balance = 0;
					if (pSubElem->QueryIntAttribute("frequency", &(es->frequency)) != TIXML_SUCCESS) es->frequency = 0;

					if (pSubElem->QueryIntAttribute("snd3d", &(es->snd3d)) != TIXML_SUCCESS) es->snd3d = 0;

					ef->commands.push_back(es);
				}


				/////////////////////////////////////////
				// DELAY

                if (strcmp(pSubElem->Value(), "delay") == 0)
				{
					s_instr_delay *ed = new s_instr_delay;
					ed->type = EFFECT_TYPE_DELAY;

					if (pSubElem->QueryFloatAttribute("duration", &(ed->duration)) != TIXML_SUCCESS) {err("duration of a delay instruction missing or invalid", id); return;}

					ef->commands.push_back(ed);
				}


				/////////////////////////////////////////
				// MESSAGE

                if (strcmp(pSubElem->Value(), "message") == 0)
				{
					s_instr_message *em = new s_instr_message;
					em->type = EFFECT_TYPE_MESSAGE;

					if ((chr = pSubElem->Attribute("text")) == NULL) {err("text of message is missing", id); return;}
					em->msg.assign(chr);

					int c;
					if (pSubElem->QueryIntAttribute("color_r", &(c)) != TIXML_SUCCESS) c = 255;
					em->color[0] = (uchar)c;
					if (pSubElem->QueryIntAttribute("color_g", &(c)) != TIXML_SUCCESS) c = 255;
					em->color[1] = (uchar)c;
					if (pSubElem->QueryIntAttribute("color_b", &(c)) != TIXML_SUCCESS) c = 255;
					em->color[2] = (uchar)c;

					if (pSubElem->QueryFloatAttribute("duration", &(em->duration)) != TIXML_SUCCESS) {err("duration of a message instruction missing or invalid", id); return;}
					
					if (pSubElem->QueryFloatAttribute("flash", &(em->flash)) != TIXML_SUCCESS) em->flash = 0.f;
				
					ef->commands.push_back(em);
				}


				/////////////////////////////////////////
				// PARTICLE
				if (strcmp(pSubElem->Value(), "particle") == 0)
				{
					s_instr_particles *ep = new s_instr_particles;
					ep->type = EFFECT_TYPE_PARTICLES;

					if (pSubElem->QueryIntAttribute("number", &(ep->number)) != TIXML_SUCCESS) {err("number of a particle instruction missing or invalid", id); return;}

					if ((chr = pSubElem->Attribute("bmap")) == NULL) ep->bmap = NULL;
					else
					{
						ep->bmap = bmap_create((char*)chr);
						if (ep->bmap == NULL) {err("bmap file of a particle instruction could not be opened", id); return;}
					}
					

					if (pSubElem->QueryFloatAttribute("alpha", &(ep->alpha)) != TIXML_SUCCESS) ep->alpha = 100;

					int c;
					if (pSubElem->QueryIntAttribute("color_r", &(c)) != TIXML_SUCCESS) c = 255;
					ep->color[0] = (uchar)c;
					if (pSubElem->QueryIntAttribute("color_g", &(c)) != TIXML_SUCCESS) c = 255;
					ep->color[1] = (uchar)c;
					if (pSubElem->QueryIntAttribute("color_b", &(c)) != TIXML_SUCCESS) c = 255;
					ep->color[2] = (uchar)c;

					if (pSubElem->QueryFloatAttribute("size", &(ep->size)) != TIXML_SUCCESS) ep->size = 4;
					if (pSubElem->QueryFloatAttribute("lifespan", &(ep->lifespan)) != TIXML_SUCCESS) ep->lifespan = 80;
					if (pSubElem->QueryFloatAttribute("gravity", &(ep->gravity)) != TIXML_SUCCESS) ep->gravity = 0;

					if (pSubElem->QueryFloatAttribute("vel_x", &(ep->vel_x)) != TIXML_SUCCESS) ep->vel_x = 0;
					if (pSubElem->QueryFloatAttribute("vel_y", &(ep->vel_y)) != TIXML_SUCCESS) ep->vel_y = 0;
					if (pSubElem->QueryFloatAttribute("vel_z", &(ep->vel_z)) != TIXML_SUCCESS) ep->vel_z = 0;

				
					if ((chr = pSubElem->Attribute("fnc_type")) == NULL) ep->fnc_type = 0;
					else
					{
						ep->fnc_type = -1;
						if (strcmp(chr, "vel_random") == 0) ep->fnc_type = PAR_FNC_TYPE_VEL_RANDOM;
						if (strcmp(chr, "vertex_random") == 0) ep->fnc_type = PAR_FNC_TYPE_VERTEX_RANDOM;
						if (strcmp(chr, "size_random") == 0) ep->fnc_type = PAR_FNC_TYPE_SIZE_RANDOM;

						if (ep->fnc_type == -1) {ep->fnc_type = 0; err("Invalid fnc_type in a particle found", id);}
					}
						
					if ((chr = pSubElem->Attribute("event_type")) == NULL) ep->event_type = 0;
					else
					{
						ep->event_type = -1;
						if (strcmp(chr, "alphafade") == 0) ep->event_type = PAR_EVENT_TYPE_ALPHAFADE;

						if (ep->event_type == -1) {ep->event_type = 0; err("Invalid event_type in a particle found", id);}
					}

					if (pSubElem->QueryFloatAttribute("skill_a", &(ep->skill_a)) != TIXML_SUCCESS) ep->skill_a = 0;
					if (pSubElem->QueryFloatAttribute("skill_b", &(ep->skill_b)) != TIXML_SUCCESS) ep->skill_b = 0;
					if (pSubElem->QueryFloatAttribute("skill_c", &(ep->skill_c)) != TIXML_SUCCESS) ep->skill_c = 0;
					if (pSubElem->QueryFloatAttribute("skill_d", &(ep->skill_d)) != TIXML_SUCCESS) ep->skill_d = 0;

					if (pSubElem->QueryFloatAttribute("skill_x", &(ep->skill_x)) != TIXML_SUCCESS) ep->skill_x = 0;
					if (pSubElem->QueryFloatAttribute("skill_y", &(ep->skill_y)) != TIXML_SUCCESS) ep->skill_y = 0;
					if (pSubElem->QueryFloatAttribute("skill_z", &(ep->skill_z)) != TIXML_SUCCESS) ep->skill_z = 0;

					// flags
					ep->flags = 0;
					if ((chr = pSubElem->Attribute("flags")) != NULL)
					{
						if (strstr(chr, "MOVE") != NULL) ep->flags |= MOVE;
						if (strstr(chr, "BEAM") != NULL) ep->flags |= BEAM;
						if (strstr(chr, "STREAK") != NULL) ep->flags |= STREAK;
						if (strstr(chr, "OVERLAY") != NULL) ep->flags |= OVERLAY;
						if (strstr(chr, "TRANSLUCENT") != NULL) ep->flags |= TRANSLUCENT;
						if (strstr(chr, "NOFILTER") != NULL) ep->flags |= NOFILTER;
						if (strstr(chr, "BRIGHT") != NULL) ep->flags |= BRIGHT;
					}

					ef->commands.push_back(ep);
				}

				/////////////////////////////////////////
				// SPRITE
				if (strcmp(pSubElem->Value(), "sprite") == 0)
				{
					s_instr_sprite *es = new s_instr_sprite;
					es->type = EFFECT_TYPE_SPRITE;

					if ((chr = pSubElem->Attribute("file")) == NULL) {err("file definition of a sprite instruction could not be found or is invalid", id); return;}
					es->file.assign(chr);
					
					int bb;
					if (pSubElem->QueryIntAttribute("billboard", &bb) != TIXML_SUCCESS) es->billboard = false;
					else es->billboard = (bb == 1);

					int c;
					if (pSubElem->QueryIntAttribute("color_r", &(c)) != TIXML_SUCCESS) c = 255;
					es->color[0] = (uchar)c;
					if (pSubElem->QueryIntAttribute("color_g", &(c)) != TIXML_SUCCESS) c = 255;
					es->color[1] = (uchar)c;
					if (pSubElem->QueryIntAttribute("color_b", &(c)) != TIXML_SUCCESS) c = 255;
					es->color[2] = (uchar)c;

					if (pSubElem->QueryFloatAttribute("scale_start", &(es->scale_start)) != TIXML_SUCCESS) es->scale_start = 100;
					if (pSubElem->QueryFloatAttribute("scale_change", &(es->scale_change)) != TIXML_SUCCESS) es->scale_change = 0;
					if (pSubElem->QueryFloatAttribute("fade_in_fac", &(es->fade_in_fac)) != TIXML_SUCCESS) es->fade_in_fac = 50;
					if (pSubElem->QueryFloatAttribute("fade_out_fac", &(es->fade_out_fac)) != TIXML_SUCCESS) es->fade_out_fac = 50;
					if (pSubElem->QueryFloatAttribute("fade_in_start", &(es->fade_in_start)) != TIXML_SUCCESS) es->fade_in_start = 0;
					if (pSubElem->QueryFloatAttribute("fade_out_end", &(es->fade_out_end)) != TIXML_SUCCESS) es->fade_out_end = 0;
					if (pSubElem->QueryFloatAttribute("fade_max", &(es->fade_max)) != TIXML_SUCCESS) es->fade_max = 100;

					if (pSubElem->QueryFloatAttribute("duration", &(es->duration)) != TIXML_SUCCESS) {err("duration of a sprite instruction could not be found or is invalid", id); return;}

					if (pSubElem->QueryFloatAttribute("lightrange", &(es->lightrange)) != TIXML_SUCCESS) es->lightrange = 0;


					if ((chr = pSubElem->Attribute("pos_type")) == NULL) es->pos_type = SPRITE_POS_TYPE_START;
					else
					{
						es->pos_type = -1;
						if (strcmp(chr, "start") == 0) es->pos_type = SPRITE_POS_TYPE_START;
						if (strcmp(chr, "end") == 0) es->pos_type = SPRITE_POS_TYPE_END;
						if (strcmp(chr, "between") == 0) es->pos_type = SPRITE_POS_TYPE_BETWEEN;
						if (strcmp(chr, "line") == 0) es->pos_type = SPRITE_POS_TYPE_LINE;

						if (es->pos_type == -1) {es->pos_type = SPRITE_POS_TYPE_START; err("Invalid pos_Type in a sprite found", id);}
					}

					// flags
					es->flags = 0;
					if ((chr = pSubElem->Attribute("flags")) != NULL)
					{
						if (strstr(chr, "UNLIT") != NULL) es->flags |= UNLIT;
						if (strstr(chr, "SHADOW") != NULL) es->flags |= SHADOW;
						if (strstr(chr, "CAST") != NULL) es->flags |= CAST;
						if (strstr(chr, "OVERLAY") != NULL) es->flags |= OVERLAY;
						if (strstr(chr, "TRANSLUCENT") != NULL) es->flags |= TRANSLUCENT;
						if (strstr(chr, "NOFILTER") != NULL) es->flags |= NOFILTER;
						if (strstr(chr, "BRIGHT") != NULL) es->flags |= BRIGHT;
						if (strstr(chr, "LIGHT") != NULL) es->flags |= LIGHT;
						if (strstr(chr, "ANIMATE") != NULL) es->flags |= ANIMATE;
					}

					ef->commands.push_back(es);

				}


				/////////////////////////////////////////
				// MESSAGE_3D

                if (strcmp(pSubElem->Value(), "message_3d") == 0)
				{
					s_instr_message_3d *e3 = new s_instr_message_3d;
					e3->type = EFFECT_TYPE_MESSAGE_3D;

					if ((chr = pSubElem->Attribute("text")) == NULL) {err("text of message_3d is missing", id); return;}
					e3->msg.assign(chr);

					if (pSubElem->QueryIntAttribute("fontsize", &(e3->fontsize)) != TIXML_SUCCESS) e3->fontsize = 12;

					int c;
					if (pSubElem->QueryIntAttribute("color_r", &(c)) != TIXML_SUCCESS) c = 255;
					e3->color[0] = (uchar)c;
					if (pSubElem->QueryIntAttribute("color_g", &(c)) != TIXML_SUCCESS) c = 255;
					e3->color[1] = (uchar)c;
					if (pSubElem->QueryIntAttribute("color_b", &(c)) != TIXML_SUCCESS) c = 255;
					e3->color[2] = (uchar)c;

					if (pSubElem->QueryFloatAttribute("duration", &(e3->duration)) != TIXML_SUCCESS) {err("duration of a message instruction missing or invalid", id); return;}
					
					if (pSubElem->QueryFloatAttribute("move_z", &(e3->move_z)) != TIXML_SUCCESS) e3->move_z = 0.f;
					if (pSubElem->QueryFloatAttribute("offset_z", &(e3->offset_z)) != TIXML_SUCCESS) e3->offset_z = 0.f;

					if (pSubElem->QueryFloatAttribute("flash", &(e3->flash)) != TIXML_SUCCESS) e3->flash = 0.f;
				
					ef->commands.push_back(e3);
				}


				/////////////////////////////////////////
				// REPEAT

                if (strcmp(pSubElem->Value(), "repeat") == 0)
				{
					s_instr_repeat *er = new s_instr_repeat;
					er->type = EFFECT_TYPE_REPEAT;

					if (pSubElem->QueryIntAttribute("goto", &(er->go_to)) != TIXML_SUCCESS) er->go_to = 0;

					ef->commands.push_back(er);
				}


			}

		}
	}
}


void b_effects::frame(double time_delta)
{
	// do for all message_displays
	for (uint m = 0; m < 3; m++)
	{
		if (msg_duration[m] > 0)
		{
			// fade in message
			msg_alpha[m] = min(100, msg_alpha[m] + time_delta*10.);
			msg_duration[m] -= time_delta*64.f;
		}
		else
		{
			// fade out
			msg_alpha[m] = max(0, msg_alpha[m] - time_delta*5.);
		}
		gui->txt_set_alpha(txt_message_id[m], msg_alpha[m]);
		if (msg_alpha[m] <= 0)
		{
			txt_message_use[m] = false;
			gui->hide_group(txt_message_group[m]);
		}
	}


	// handle sprites
	std::list<ENTITY*>::iterator sps = active_sprites.begin();
	while(sps != active_sprites.end())
	{
		if ((*sps)->skill[69] == 1)
		{
			ent_remove((*sps));
			active_sprites.erase(sps++);
		}
		else
		{
			handle_sprite(*sps, time_delta);
			sps++;
		}
	}

	// handle message_3d
	std::list<s_message_3d_sched>::iterator i3s = active_msg3d.begin();
	while(i3s != active_msg3d.end())
	{
		if (i3s->duration <= 0 && i3s->txt->alpha <= 0)
		{
			// delete everything
			for (int i = 0; i < i3s->txt->strings; i++) 
			  if ((i3s->txt->pstring)[i]) ptr_remove((i3s->txt->pstring)[i]);
			ptr_remove(i3s->txt);
			active_msg3d.erase(i3s++);
		}
		else
		{
			handle_msg3d(&(*i3s), time_delta);
			i3s++;
		}
	}


	///////////////////////////////////////////////
	// iterate through schedules, do their actions
	std::list<s_effsched>::iterator it = schedule.begin();
	while(it != schedule.end())
	{
		if (it->wait_time > 0)
		{
			it->wait_time -= (float)(time_delta*64.);
			it++;
		}
		else
		{
			// go through schedule
			s_effect *e = at(it->id);
			if (e == NULL) {error("FATAL ERROR: invalid id in effect schedule"); return;}

			bool delayed = false;
			while (it->instr < e->commands.size() && delayed == false)
			{
				// get instruction
				s_instr *instr = e->commands.at(it->instr);
				it->instr++;

				switch(instr->type)
				{
					case EFFECT_TYPE_SOUND:
					{
						s_instr_sound *is = (s_instr_sound*)instr;
						long handle = 0;
						if (is->snd3d == 0)
						{
							handle = snd_play(is->snd, is->volume, is->balance);
						}
						else
						{
							if (it->caster == NULL) log(LOG_ERROR, "ERROR in b_effects::frame: called 3d sound without entity!");
							else handle = ent_playsound(it->caster, is->snd, is->volume);
						}
						if (is->frequency != 0) snd_tune(handle, 0, is->frequency, 0);
					}
					break;

					case EFFECT_TYPE_DELAY:
					{
						s_instr_delay *id = (s_instr_delay*)instr;
						it->wait_time = id->duration;
						// stop right here
						delayed = true;
					}
					break;

					case EFFECT_TYPE_MESSAGE:
					{
						s_instr_message *im = (s_instr_message*)instr;

						// find free message txt
						int m;
						for (m = 0; m < 3; m++) if (txt_message_use[m] == false) break;
						if (m < 3)
						{
							// assemble string
							std::string msg(im->msg);
							str_assemble(&msg, it->i1, it->i2, it->f1, it->f2, &it->s1, &it->s2, &it->s3);
							
							txt_message_use[m] = true;
							gui->txt_set_value(txt_message_id[m], 0, &msg);
							msg_duration[m] = im->duration;
							msg_alpha[m] = 0.;
							gui->txt_set_color(txt_message_id[m], im->color[0], im->color[1], im->color[2]);
							gui->show_group(txt_message_group[m]);
						} 
						else log(LOG_ERROR, "ERROR in b_effect: creating message failed, no free display text!");
					}
					break;

					case EFFECT_TYPE_PARTICLES:
					{
						if (it->caster == NULL || !(it->caster->eflags & CLIPPED))
						{
							s_instr_particles *ip = (s_instr_particles*)instr;

							VECTOR vpos;
							vpos.x = it->pos_start.x;
							vpos.y = it->pos_start.y;
							vpos.z = it->pos_start.z;

							// use this to smuggle data into the function!
							VECTOR vvel;
							vvel.x = (void*)it->caster;
							vvel.y = (int)it->id;
							vvel.z = (int)(it->instr-1);

							effect((EVENT)parev_general, ip->number, &vpos, &vvel);
						}
					}
					break;

					case EFFECT_TYPE_SPRITE:
					{
						s_instr_sprite *is = (s_instr_sprite*)instr;

						VECTOR vpos;
						if (is->pos_type == SPRITE_POS_TYPE_END)
						{
							vpos.x = it->pos_end.x;
							vpos.y = it->pos_end.y;
							vpos.z = it->pos_end.z+0.1;
						}
						if (is->pos_type == SPRITE_POS_TYPE_START)
						{
							vpos.x = it->pos_start.x;
							vpos.y = it->pos_start.y;
							vpos.z = it->pos_start.z+0.1;
						}
						if (is->pos_type == SPRITE_POS_TYPE_BETWEEN || is->pos_type == SPRITE_POS_TYPE_LINE)
						{
							vec_lerp(&vpos, vector(it->pos_start.x, it->pos_start.y, it->pos_start.z), vector(it->pos_end.x, it->pos_end.y, it->pos_end.z) ,0.5);
							vpos.z = 10;
						}


						ENTITY* sprite = ent_create((char*)is->file.c_str(), &vpos, NULL);
						sprite->flags2 |= UNTOUCHABLE;

						if (!is->billboard) sprite->roll = 0.01;
						else {sprite->roll = 0.01; sprite->tilt = 270;}

						sprite->lightrange = is->lightrange;
						sprite->flags = is->flags;
						sprite->flags |= PASSABLE;

						if (is->pos_type != SPRITE_POS_TYPE_LINE)
						{
							sprite->scale_x = _VAR(is->scale_start);
							sprite->scale_y = _VAR(is->scale_start);
							sprite->scale_z = _VAR(is->scale_start);
						}
						else
						{
							sprite->scale_x = vec_dist(vector(it->pos_start.x, it->pos_start.y, it->pos_start.z), vector(it->pos_end.x, it->pos_end.y, it->pos_end.z))/190000.;
							std::cout << "scale_X = " << (float)sprite->scale_x << std::endl;
							sprite->scale_y = sprite->scale_x;

							// turn
							VECTOR vtemp;
							vec_set(&vtemp,vector(it->pos_start.x, it->pos_start.y, it->pos_start.z)); 
							vec_sub(&vtemp,vector(it->pos_end.x, it->pos_end.y, it->pos_end.z));
							vec_to_angle((ANGLE*)&sprite->pan,&vtemp); // now MY looks at YOU
							sprite->pan += 90;
							sprite->tilt = 270.;
							sprite->roll = 0.01;

							
						}

						sprite->alpha = is->fade_in_start;
						sprite->red = is->color[0]; sprite->green = is->color[1]; sprite->blue = is->color[2];

						sprite->skill[69] = 0; // if 1 then delete me!
						sprite->skill[70] = (var)(int)it->id;
						sprite->skill[71] = (var)(int)(it->instr-1);
						sprite->skill[72] = 0; // saves fade stat

						active_sprites.push_back(sprite);
					}
					break;


					case EFFECT_TYPE_MESSAGE_3D:
					{
						s_instr_message_3d *i3 = (s_instr_message_3d*)instr;
						
						s_message_3d_sched s3;

						// assemble string
						std::string msg(i3->msg);
						str_assemble(&msg, it->i1, it->i2, it->f1, it->f2, &it->s1, &it->s2, &it->s3);

						// create text for message
						s3.txt = txt_create(1, 1);
						// TODO: several fonts
						s3.txt->font = font;
						s3.txt->red = i3->color[0];
						s3.txt->green = i3->color[1];
						s3.txt->blue = i3->color[2];
						str_cpy(s3.txt->pstring[0], (char*)msg.c_str());
						s3.txt->alpha = 0;
						s3.txt->flags |= CENTER_X | OUTLINE | TRANSLUCENT;

						// get 2d position
						VECTOR vtemp;
						vec_set(&vtemp, vpos(it->caster));
						if (vec_to_screen(&vtemp, ev->camera))
						{
							s3.txt->flags |= SHOW;
							s3.txt->pos_x = vtemp.x;
							s3.txt->pos_y = vtemp.y;
						}


						s3.msg3d_data = i3;
						s3.duration = i3->duration;
						vec_set(&s3.pos, vpos(it->caster));
						s3.pos.z += i3->offset_z;

						active_msg3d.push_back(s3);
					}
					break;


					case EFFECT_TYPE_REPEAT:
					{
						s_instr_repeat *ir = (s_instr_repeat*)instr;

						it->instr = ir->go_to;
					}
					break;
				}
			}

			if (it->instr >= e->commands.size())
			{
				// finished all instructions, now delete this
				schedule.erase(it++);
			}
			else
			{
				it++;
			}
		}
	}

}

// end all effects
void b_effects::disable_all()
{
	// TODO: hide sounds played by this backend
	//snd_stopall(4);

	// hide message
	for (int m = 0; m < 3; m++) {gui->hide_group(txt_message_group[m]); txt_message_use[m] = false;}
	
	// hide particles (is that even neccessary?)

	// hide sprites (are removed by level)
	active_sprites.clear();

	// delete active_msg3d's
	for(std::list<s_message_3d_sched>::iterator it = active_msg3d.begin(); it != active_msg3d.end(); it++)
	{
		// delete everything
		for (int i = 0; i < it->txt->strings; i++) 
		  if ((it->txt->pstring)[i]) ptr_remove((it->txt->pstring)[i]);
		ptr_remove(it->txt);
	}
	active_msg3d.clear();


	// clear scheduling
	schedule.clear();
}



// start effect

void b_effects::play(uint id)
{
	// see if id exists
	if (at(id) == NULL) return;

	s_effsched sch;

	sch.wait_time = 0.f;
	sch.id = id;
	sch.instr = 0;
	sch.pos_start.zero();
	sch.pos_end.zero();
	sch.caster = NULL;
	sch.target = NULL;

	schedule.push_back(sch);
}

void b_effects::play(uint id, vec *pos)
{
	// see if id exists
	if (at(id) == NULL) return;

	s_effsched sch;

	sch.wait_time = 0.f;
	sch.id = id;
	sch.instr = 0;
	sch.pos_start.set(pos);
	sch.pos_end.zero();
	sch.caster = NULL;
	sch.target = NULL;
	
	schedule.push_back(sch);
}

void b_effects::play(uint id, vec *pos_start, vec *pos_end)
{
	// see if id exists
	if (at(id) == NULL) return;

	s_effsched sch;

	sch.wait_time = 0.f;
	sch.id = id;
	sch.instr = 0;
	sch.pos_start.set(pos_start);
	sch.pos_end.set(pos_end);
	sch.caster = NULL;
	sch.target = NULL;
	
	schedule.push_back(sch);
}

void b_effects::play(uint id, ENTITY *cast, vec *pos_start, ENTITY* target, vec *pos_end)
{
	// see if id exists
	if (at(id) == NULL) return;

	s_effsched sch;

	sch.wait_time = 0.f;
	sch.id = id;
	sch.instr = 0;
	sch.pos_start.set(pos_start);
	sch.pos_end.set(pos_end);
	sch.caster = cast;
	sch.target = target;
	
	schedule.push_back(sch);
}

void b_effects::play(uint id, ENTITY *ent, vec *pos)
{
	// see if id exists
	if (at(id) == NULL) return;

	s_effsched sch;

	sch.wait_time = 0.f;
	sch.id = id;
	sch.instr = 0;
	sch.pos_start.set(pos);
	sch.pos_end.zero();
	sch.caster = ent;
	sch.target = NULL;
	
	schedule.push_back(sch);
}

void b_effects::play(uint id, ENTITY* ent)
{
	// see if id exists
	if (at(id) == NULL) return;

	s_effsched sch;

	sch.wait_time = 0.f;
	sch.id = id;
	sch.instr = 0;
	sch.pos_start.x = _FLOAT(ent->x);
	sch.pos_start.y = _FLOAT(ent->y);
	sch.pos_start.z = _FLOAT(ent->z + ent->max_z/_VAR(2));
	sch.pos_end.zero();
	sch.caster = ent;
	sch.target = NULL;
	
	schedule.push_back(sch);
}

void b_effects::play(uint id, ENTITY* cast, ENTITY *target)
{
	// see if id exists
	if (at(id) == NULL) return;

	s_effsched sch;

	sch.wait_time = 0.f;
	sch.id = id;
	sch.instr = 0;
	if (cast != NULL)
	{
		sch.pos_start.x = _FLOAT(cast->x);
		sch.pos_start.y = _FLOAT(cast->y);
		sch.pos_start.z = _FLOAT(cast->z + cast->max_z/_VAR(2));
	}
	else sch.pos_start.zero();
	sch.pos_end.x = _FLOAT(target->x);
	sch.pos_end.y = _FLOAT(target->y);
	sch.pos_end.z = _FLOAT(target->z + target->max_z/_VAR(2));
	sch.caster = cast;
	sch.target = target;
	
	schedule.push_back(sch);
}

void b_effects::play(uint id, ENTITY* ent, int i1, int i2)
{
	// see if id exists
	if (at(id) == NULL) return;

	s_effsched sch;

	sch.wait_time = 0.f;
	sch.id = id;
	sch.instr = 0;
	sch.pos_start.x = _FLOAT(ent->x);
	sch.pos_start.y = _FLOAT(ent->y);
	sch.pos_start.z = _FLOAT(ent->z);
	sch.pos_end.zero();
	sch.caster = ent;
	sch.target = NULL;
	sch.i1 = i1;
	sch.i2 = i2;
	
	schedule.push_back(sch);
}

void b_effects::play(uint id, int i1, int i2, float f1, float f2, std::string *s1, std::string *s2, std::string *s3)
{
	// see if id exists
	if (at(id) == NULL) return;

	s_effsched sch;

	sch.wait_time = 0.f;
	sch.id = id;
	sch.instr = 0;
	sch.pos_start.zero();
	sch.pos_end.zero();
	sch.caster = NULL;
	sch.target = NULL;
	sch.i1 = i1;
	sch.i2 = i2;
	sch.f1 = f1;
	sch.f2 = f2;
	if (s1) sch.s1.assign(*s1);
	if (s2) sch.s2.assign(*s2);
	if (s3) sch.s3.assign(*s3);
	
	schedule.push_back(sch);
}


void b_effects::stop(uint id, ENTITY* ent)
{
	// find in schedule
	std::list<s_effsched>::iterator it = schedule.begin();
	while(it != schedule.end())
	{
		if (it->caster == ent && it->id == id)
		{
			// stop me
			schedule.erase(it++);
		}
		else it++;
	}
	
}

void b_effects::stop_ent(ENTITY* ent)
{
	// find in schedule
	std::list<s_effsched>::iterator it = schedule.begin();
	while(it != schedule.end())
	{
		if (it->caster == ent)
		{
			// stop me
			schedule.erase(it++);
		}
		else it++;
	}
	
}



void b_effects::handle_sprite(ENTITY *sprite, double time_delta)
{
	// get backend info
	s_effect *e = at((int)sprite->skill[70]);
	if (e == NULL) {error("FATAL ERROR: invalid id in effect handle_sprite"); return;}

	// get instruction
	s_instr *instr = e->commands.at((int)sprite->skill[71]);
	if (instr->type != EFFECT_TYPE_SPRITE) {error("FATAL ERROR: invalid instruction type in effect handle_sprite"); return;}

	s_instr_sprite *is = (s_instr_sprite*)instr;


	sprite->scale_x += is->scale_change*(float)time_delta;
	sprite->scale_y += is->scale_change*(float)time_delta;

	if (sprite->skill[72] < 0 && is->duration >= 0)
	{
		sprite->alpha -= is->fade_out_fac*(float)time_delta;
		if (sprite->alpha <= is->fade_out_end)
		{
			// signal deletion
			sprite->skill[69] = 1;
		}
	}
	if (sprite->skill[72] == 0) // fadeing in
	{
		sprite->alpha += is->fade_in_fac*(float)time_delta;
		if (sprite->alpha >= is->fade_max)
		{
			sprite->alpha = is->fade_max;
			sprite->skill[72] = is->duration;
		}
	}
	if (sprite->skill[72] > 0)
	{
		// count down duration
		sprite->skill[72] -= (var)(time_delta*64.);
	}

}



void b_effects::handle_msg3d(s_message_3d_sched *s3, double time_delta)
{
	s3->pos.z += s3->msg3d_data->move_z * time_delta;

	VECTOR vtemp;
	vec_set(&vtemp, &s3->pos);
	if (vec_to_screen(&vtemp, ev->camera))
	{
		s3->txt->flags |= SHOW;
		s3->txt->pos_x = vtemp.x;
		s3->txt->pos_y = vtemp.y;
	}
	else
	{
		s3->txt->flags &= ~SHOW;
	}


	if (s3->duration > 0.f)
	{
		// fade in:
		s3->txt->alpha += 30*time_delta;
		s3->txt->alpha = min(100, s3->txt->alpha);

		s3->duration -= (float)(time_delta*64.);
	}
	else
	{
		// fade out:
		s3->txt->alpha -= 10*time_delta;
	}
}


s_effect *b_effects::at(uint id)
{
	if (id > B_EFFECTS_ENTRIES || effects[id].valid == false) {err("b_effects::at was called with invalid id", id); return NULL;}
	return &effects[id];
}


void b_effects::err(const char *msg, int id)
{
	// silent errors in this backend!

	std::cout << "ERROR in b_effects in id=" << id << " :" << msg << std::endl;
}
