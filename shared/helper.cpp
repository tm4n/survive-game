#include "helper.h"
#include "defs.h"
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <sstream>
#include <random>

using namespace std;

#ifndef _WIN32

#include "unistd.h"

void get_workdir(std::string *out)
{
    char *tmp = get_current_dir_name();

    if (tmp!= NULL)
    {
        out->assign(tmp);

		out->append("/");

        free(tmp);
    }

}

void get_savedir(std::string *out) { } // dummy

#else

#include <windows.h>
#include <Shlobj.h>
#include <Shlwapi.h>

void get_workdir(std::string *out)
{
    char buf[500];

    if (GetCurrentDirectory(500, buf) != NULL)
    {
        out->assign(buf);

		out->append("\\");

    }
}

/*void get_savedir(const char *account_name, std::string *out)
{
	TCHAR appDataPath[MAX_PATH];

	if(SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA | CSIDL_FLAG_CREATE, NULL, SHGFP_TYPE_CURRENT, appDataPath)))
	{

		PathAppend(appDataPath, b_ga->game_name.c_str());

		// create folder if needed 1
		DWORD dwAttrib = GetFileAttributes(appDataPath);
		if (dwAttrib == INVALID_FILE_ATTRIBUTES ||
			 !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY))
		{
			log(LOG_DEBUG, "Creating game name directory");
			CreateDirectory(appDataPath, NULL);
		}

		PathAppend(appDataPath, account_name);

		dwAttrib = GetFileAttributes(appDataPath);
		if (dwAttrib == INVALID_FILE_ATTRIBUTES ||
			 !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY))
		{
			log(LOG_DEBUG, "Creating account name directory");
			CreateDirectory(appDataPath, NULL);
		}

		out->assign(appDataPath);
	}
}*/

#endif

vector<string> split(const char *str, char c)
{
    vector<string> result;

    while(1)
    {
        const char *begin = str;

        while(*str != c && *str)
                str++;

        result.push_back(string(begin, str));

        if(0 == *str++)
                break;
    }

    return result;
}


void str_replace(std::string& str, const std::string& oldStr, const std::string& newStr)
{
  size_t pos = 0;
  while((pos = str.find(oldStr, pos)) != std::string::npos)
  {
     str.replace(pos, oldStr.length(), newStr);
     pos += newStr.length();
  }
}



bool str_assemble(std::string *input, int i1, int i2, float f1, float f2, std::string *s1, std::string *s2, std::string *s3)
{
	std::ostringstream out;

	for (size_t i = 0; i < input->length(); i++)
	{
		if (input->at(i) == '%' && i < input->length()-2)
		{
			// read next 2 chars
			char b1 = input->at(++i);
			char b2 = input->at(++i);

			if (b1 == 'i')
			{
				if (b2 == '1') out << i1;
				if (b2 == '2') out << i2;
			}
			if (b1 == 'f')
			{
				if (b2 == '1') out << f1;
				if (b2 == '2') out << f2;
			}
			if (b1 == 's')
			{
				if (b2 == '1') out << *s1;
				if (b2 == '2') out << *s2;
				if (b2 == '3') out << *s3;
			}
		}
		else out << input->at(i);
	}

	input->assign(out.str());

	return true;

}

bool str_sane(const std::string &str)
{
	for (size_t i = 0; i < str.length(); i++)
	{
		if (str.at(i) < 32 || str.at(i) == 59) return false;
	}

	return true;
}

bool str_alphanumeric(const std::string &str)
{
    for (size_t i = 0; i < str.length(); i++)
	{
		if (str.at(i) < 48 || str.at(i) == 59) return false;
	}

	return true;
}

bool str_numeric(const std::string &str)
{
    for (size_t i = 0; i < str.length(); i++)
	{
		if (str.at(i) < 48 || str.at(i) > 57) return false;
	}

	return true;
}

bool str_email(const std::string &str)
{
	bool at_found = false;

	for (size_t i = 0; i < str.length(); i++)
	{
		if (str.at(i) == '@') at_found = true;
		if (at_found == true && str.at(i) == '.') return true;
	}
	return false;
}

int str_lines(const std::string *str)
{
	int ct = 1;

	for (size_t i = 0; i < str->length(); i++)
	{
		if (str->at(i) == '\n') ct++;
	}

	return ct;
}


#ifdef _WIN32

#include <malloc.h>
void check_heap(char *file, int line)
{
    static char *lastOkFile = "here";
    static int lastOkLine = 0;
    static int heapOK = 1;

    if (!heapOK) return;

    if (_heapchk() == _HEAPOK)
    {
        lastOkFile = file;
        lastOkLine = line;
       return;
    }

    heapOK = 0;
    printf("Heap corruption detected at %s (%d)\n", file, line);
    printf("Last OK at %s (%d)\n", lastOkFile, lastOkLine);
}



#endif

void log(int prop, char const* str)
{
	puts(str);
}


float toRadians(float d)
{
	return (d * (float)M_PI / 180.f);
}

// global generator
std::minstd_rand *gen = NULL;

void random_seed(int seed)
{
	if (gen != NULL) delete gen;
	gen = new std::minstd_rand(seed);
}

int random_range(int mx)
{
	if (gen == NULL) gen = new std::minstd_rand();
	
	std::uniform_int_distribution<> d(0, mx);
	
	return d(*gen);
}

float random(int mx)
{
	return rand() % mx;
}