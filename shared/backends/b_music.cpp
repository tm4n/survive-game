#include "b_music.h"
#include "tinyxml.h"
#include "helper.h"

#include <iostream>
#include <sstream>
#include <algorithm>

b_music *b_mu;


b_music::b_music(const char *filename)
{
	int id;
	const char *chr;
	s_score *sc;

	wait_timer = -1;
	next_score = 0;
	current_score = 0;
	current_track = -1;
	current_volume = 0;
	fade_fac = 0.;
	current_danger_level = 0;

	snd_handle_l = 0; snd_handle_r = 0;
	current_l = NULL; current_r = NULL;

	// clear all entries
	for (int i = 0; i < B_MUSIC_ENTRIES; i++)
	{
		scores[i].valid = false;
	}


	// load file
    std::string full_filename;
	get_workdir(&full_filename);
	full_filename.append(filename);

	TiXmlDocument doc(full_filename);
	if (!doc.LoadFile()) {err("Couldn't open file for music backend!", -1);}

	TiXmlHandle hDoc(&doc);
	TiXmlElement *pElem, *pSubElem;
	TiXmlHandle hRoot(0);

	// document base
	pElem=hDoc.FirstChildElement().Element();
	// should always have a valid root but handle gracefully if it does
	if (!pElem) {err("backend music: Root Element not found, file empty?", -1); return;}
	if (strcmp(pElem->Value(), "backend_music") != 0) {err("backend_music definition not found", -1); return;}

	// save this for later
	hRoot=TiXmlHandle(pElem);

	// get all entries
	pElem=hRoot.FirstChild().Element();
	for( ; pElem; pElem=pElem->NextSiblingElement())
	{
		///////////////////////////////////////////
		// class definition
		if (strcmp(pElem->Value(), "score") == 0)
		{

			if (pElem->QueryIntAttribute ("id", &id) != TIXML_SUCCESS) {err("id of score or invalid", id); return;}
			if (id < 0 || id >= B_MUSIC_ENTRIES) {err("id of score is invalid", id); return;}

			sc = &(scores[id]);

			if ((chr = pElem->Attribute("name")) == NULL) {err("name of score is missing", id); return;}
			sc->name.assign(chr);

			sc->valid = true;

            // query subelements
            s_track tr;

			for(pSubElem=pElem->FirstChildElement(); pSubElem; pSubElem=pSubElem->NextSiblingElement())
			{

				////////////////////////////////////////
				// TRACK

				if (strcmp(pSubElem->Value(), "track") == 0)
				{
					if ((chr = pSubElem->Attribute("file_l")) == NULL) {err("file_l of track is missing", id); return;}
					tr.file_l.assign(chr);

					if ((chr = pSubElem->Attribute("file_r")) == NULL) {err("file_r of track is missing", id); return;}
					tr.file_r.assign(chr);

					if (pSubElem->QueryIntAttribute("volume", &(tr.volume)) != TIXML_SUCCESS) tr.volume = 100;

					if (pSubElem->QueryFloatAttribute("delay_after", &(tr.delay_after)) != TIXML_SUCCESS) tr.delay_after = 0;
					if (pSubElem->QueryIntAttribute("danger_level", &(tr.danger_level)) != TIXML_SUCCESS) tr.danger_level = 0;

					sc->tracks.push_back(tr);
				}

			}

		}
	}
}




void b_music::play(uint score_id)
{
	if (score_id >= B_MUSIC_ENTRIES) {err("Invalid score_id given from level", score_id); return;}

	// get score
	s_score *sc = &scores[score_id];

	stop();

	// load all score sounds
	for (std::vector<s_track>::iterator it = sc->tracks.begin(); it != sc->tracks.end(); it++)
	{
		s_sndptrs sp;

		sp.snd_l = snd_create((char*)it->file_l.c_str());
		sp.snd_r = snd_create((char*)it->file_r.c_str());
		if (sp.snd_l == NULL || sp.snd_r == NULL) {err("Could not load sound files from score", score_id);}

		printf("loaded %s \n", it->file_l.c_str());

		sndbuffer.push_back(sp);
	}

	// mark for next usage

	next_score = score_id;
	wait_timer = 2;
}

void b_music::stop()
{
	printf("stopping music, deleting soundbuffer\n");
	// unload old score sounds except currently playing
	for (std::vector<s_sndptrs>::iterator it = sndbuffer.begin(); it != sndbuffer.end(); it++)
	{
		if (it->snd_l != current_l)
		{
			if (it->snd_l != NULL)
			{
				ptr_remove(it->snd_l);
				ptr_remove(it->snd_r);

				printf("deleted a sound \n");
			}
		}
	}
	sndbuffer.clear();

	wait_timer = -1;

	next_score = 0;
	fade_fac = 4.;
}



int b_music::get_next_song(uint score_id, int danger_level)
{
	if (score_id >= B_MUSIC_ENTRIES) {err("get_next_song: Invalid score_id given", score_id); return -1;}

	// get score
	s_score *sc = &scores[score_id];

	std::vector<uint> possible_results;

	// try to find a fitting song
	uint ct = 0;
	for (std::vector<s_track>::iterator it = sc->tracks.begin(); it != sc->tracks.end(); it++)
	{
		if (it->danger_level == danger_level) possible_results.push_back(ct);
		ct++;
	}

	if (possible_results.size() == 0)
	{
		// no fitting track found, take any song
		for (std::vector<s_track>::iterator it = sc->tracks.begin(); it != sc->tracks.end(); it++)
		{
			possible_results.push_back(ct);
			ct++;
		}
		

		if (possible_results.size() == 0) return -1;
	}

	random_shuffle(possible_results.begin(), possible_results.end());

	return (int)possible_results.at(0);
}


void b_music::frame(double time_step, int danger_level)
{
	///////////////////////////////////
	// play music part

	// if music stopped
	if (snd_playing(snd_handle_l) == 0 || snd_playing(snd_handle_r) == 0
				|| current_volume <= 0)
	{
		if (wait_timer >= 0)
		{
			wait_timer -= time_step*0.064;
			//printf("wait_timer=%lf \n", wait_timer);
		}
		else
		{
			snd_stop(snd_handle_l); snd_stop(snd_handle_r);

			if (next_score != current_score)
			{
				// unload songs that are playing
				if (current_l) log(LOG_DEBUG, "DELETING current_l and r");
				if (current_l) {ptr_remove(current_l); current_l = NULL;}
				if (current_r) {ptr_remove(current_r); current_r = NULL;}

				current_score = next_score;
			}

			// start next song

			if (current_score > 0)
			{

				// choose song
				current_track = get_next_song(current_score, danger_level);

				if (current_track < 0)
				{
					err("ERROR in b_music: no fitting track in score found", current_score);
					stop();
				}
				else
				{
					printf("Starting Song=%i, score=%u \n", current_track, current_score);
					// set song data and play
					current_l = sndbuffer.at(current_track).snd_l;
					current_r = sndbuffer.at(current_track).snd_r;
					snd_handle_l = snd_play(current_l, scores[current_score].tracks.at(current_track).volume, -100);
					snd_handle_r = snd_play(current_r, scores[current_score].tracks.at(current_track).volume, 100);
					current_volume = scores[current_score].tracks.at(current_track).volume;
					wait_timer = scores[current_score].tracks.at(current_track).delay_after;

					current_danger_level = danger_level;
					fade_fac = 0.;
				}
			}

		}
	}
	else
	{
		// fade out if fade_fac is set
		current_volume -= fade_fac*(double)(v(time_step));
		snd_tune(snd_handle_l, current_volume, 0, 0);
		snd_tune(snd_handle_r, current_volume, 0, 0);
	}




	///////////////////////////////////
	// decide to change on danger_level

	if (current_danger_level != danger_level)
	{
		// make current track fade out. will automatically start one with correct danger level
		if (current_danger_level > danger_level)
		{
			// slow fade out
			fade_fac = 1.;
			wait_timer = 10;
		}
		else
		{
			// nearly immediate change
			fade_fac = 7.;
			wait_timer = -1;
		}
		current_danger_level = danger_level;
	}

}



void b_music::err(const char *msg, uint id)
{
	std::ostringstream pr;
	pr << "ERROR in b_music in id=" << id << " :" << msg;

#ifdef ACKNEX
	error((char*)pr.str().c_str());
#else
	std::cout << pr.str() << std::endl;
#endif
}
