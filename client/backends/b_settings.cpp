#include "b_settings.h"
#include "helper.h"
#include <sstream>
#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <string>
#include <exception>

#ifdef _WIN32
#include <direct.h>
#include <tchar.h>
#include <shlwapi.h>
#pragma comment(lib,"shlwapi.lib")
#include "shlobj.h"
#else
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#endif

b_settings::b_settings()
{
#ifdef ANDROID
	goto default_settings;
#endif
	// load values
	std::string path = get_settings_file_path();
	if (path.size() > 0)
	{
		#ifdef _WIN32
		path.append("\\settings.cfg");
		#else
		path.append("/settings.cfg");
		#endif
		
		std::cout << "opening file " << path << std::endl;

		std::ifstream sfile;
		sfile.open(path);
		
		if (sfile.is_open())
		{
			std::cout << "\nLoading settings: " << path << "\n";

			std::string l;
			std::vector<std::string> lines;
			while (std::getline(sfile, l))
			{
				lines.push_back(l);
			}
			sfile.close();

			if (lines.size() == 13)
			{
				playername.assign(lines.at(0));
				server.assign(lines.at(1));

				try
				{
					mouse_sensitivity = std::stof(lines.at(2));
					mouse_invert = (std::stoi(lines.at(3)) == 1);
					screenres_x = std::stoi(lines.at(4));
					screenres_y = std::stoi(lines.at(5));
					fullscreen = (std::stoi(lines.at(6)) == 1);
					screenaspect = std::stof(lines.at(7));
					shader = (std::stoi(lines.at(8)) == 1);
					shadow = std::stoi(lines.at(9));
					antialias = std::stoi(lines.at(10));
					anisotropy = std::stoi(lines.at(11));
					sound_volume = std::stof(lines.at(12));
				}
				catch (std::exception& e)
				{
					err("Failed to parse settings file.");
					err(e.what());
					goto default_settings;
				}

				return;
			}
			else std::cout << "\n Settings file had invalid number of lines \n";
		}
	}
	
	std::cout << "\n Could not load settings, using default \n";

	// Default settings
default_settings:
#ifdef ANDROID
	playername.assign("Android Player");;
	server.assign("Survive! Server");

	mouse_sensitivity = 2.0f;
	mouse_invert = false;

	screenres_x = 1920;
	screenres_y = 1080;
	fullscreen = true;
	screenaspect = 16.f / 9.f;
	shader = true;
	shadow = SETTINGS_SHADOW_MID;
	antialias = SETTINGS_ANTIALIAS_OFF;
	anisotropy = SETTINGS_ANISOTROPY_OFF;
	sound_volume = 70.f;
	return;
#endif
	// Other systems default settings
	playername.assign("Player");;
	server.assign("Survive! Server");

	mouse_sensitivity = 2.0f;
	mouse_invert = false;

	screenres_x = 1366;
	screenres_y = 768;
	fullscreen = false;
	screenaspect = 16.f/9.f;
	shader = true;
	shadow = SETTINGS_SHADOW_MID;
	antialias = SETTINGS_ANTIALIAS_OFF;
	anisotropy = SETTINGS_ANISOTROPY_OFF;
	sound_volume = 70.f;
}

void b_settings::save_settings()
{
#ifdef ANDROID 
	return;
#endif
	// load values
	std::string path = get_settings_file_path();
	if (path.length() > 0)
	{
		// Create directory if needed
		struct stat st;
		if (stat(path.c_str(), &st) != 0)
		{
			#ifdef _WIN32
			int mkdirResult = _mkdir(path.c_str());
			#else
			int mkdirResult = mkdir(path.c_str(), 0777);
			#endif
			if (mkdirResult != 0)
			{
				err("Directory creation failed");
				return;
			}
		}
		
		#ifdef _WIN32
		path.append("\\settings.cfg");
		#else
		path.append("/settings.cfg");
		#endif

		std::ofstream sfile;
		sfile.open(path, std::fstream::out | std::fstream::trunc);

		if (!sfile.fail())
		{
			std::cout << "\nSaving settings: " << path << "\n";

			sfile << playername << '\n';
			sfile << server << '\n';

			sfile << mouse_sensitivity << '\n';
			sfile << (int)mouse_invert << '\n';

			sfile << screenres_x << '\n';
			sfile << screenres_y << '\n';
			sfile << (int)fullscreen << '\n';
			sfile << screenaspect << '\n';
			sfile << (int)shader << '\n';
			sfile << shadow << '\n';
			sfile << antialias << '\n';
			sfile << anisotropy << '\n';

			sfile << sound_volume;

			sfile.close();
			return;
		}
	}
	err("Error saving settings file");
}

std::string b_settings::get_settings_file_path()
{
#ifdef _WIN32
	TCHAR szPath[MAX_PATH];
	// Get path for each computer, non-user specific and non-roaming data.
	if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, szPath)))
	{
		// Append product-specific path
		PathAppend(szPath, _T("\\Survive"));
		return std::string(szPath);
	}
	else
	{
		err("Cold not get Appdata Path from Windows");
	}
#else
	struct passwd *pw = getpwuid(getuid());
	const char *homedir = pw->pw_dir;
	
	std::string res(homedir);
	res.append("/.Survive");
	return res;
#endif
	return "";
}

void b_settings::err(const char *msg)
{
	std::ostringstream pr;
	pr << "ERROR in b_settings in: " << msg;

	log(LOG_ERROR, pr.str().c_str());
}

// Thread safe singleton!
std::unique_ptr<b_settings> b_settings::m_instance;
std::once_flag b_settings::m_onceFlag;

b_settings * b_settings::instance()
{
    std::call_once(m_onceFlag,
        [] {
            m_instance.reset(new b_settings);
    });
    return m_instance.get();
}
