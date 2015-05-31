#ifndef __B_SETTINGS_H__
#define __B_SETTINGS_H__

#include "defs.h"
#include <iostream>
#include <string>
#include <memory>
#include <mutex>

#define SETTINGS_SHADOW_OFF 0
#define SETTINGS_SHADOW_MID 1
#define SETTINGS_SHADOW_HIGH 2

#define SETTINGS_ANTIALIAS_OFF 0
#define SETTINGS_ANTIALIAS_DEFAULT 1
#define SETTINGS_ANTIALIAS_MID 2
#define SETTINGS_ANTIALIAS_HIGH 3

#define SETTINGS_ANISOTROPY_OFF 0
#define SETTINGS_ANISOTROPY_LOW 1
#define SETTINGS_ANISOTROPY_MID 2
#define SETTINGS_ANISOTROPY_HIGH 3

class b_settings
{
public:
	b_settings();

	static b_settings *instance();

	void save_settings();

	std::string playername;
	std::string server;

	float mouse_sensitivity;
	bool mouse_invert;

	int screenres_x;
	int screenres_y;
	bool fullscreen;
	float screenaspect;
	bool shader;
	int shadow;
	int antialias;
	int anisotropy;
	float sound_volume;

private:

	std::string get_settings_file_path();

	static std::unique_ptr<b_settings> m_instance;
    static std::once_flag m_onceFlag;

	void err(const char *);
};

#endif
