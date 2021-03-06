#ifndef __HELPER_H__
#define __HELPER_H__

#include <vector>
#include <string>
#include <glm/glm.hpp>
#include "vec.h"

#define clamp(x, a, b) (x > b ? b : (x < a ? a : x))

void get_workdir(std::string *out);

void get_savedir(const char *account_name, std::string *out);

std::string get_settings_file_path();

std::vector<std::string> split(const char *, char = ' ');

bool str_assemble(std::string *input, int i1, int i2, float f1, float f2, std::string *s1, std::string *s2, std::string *s3);

void str_replace(std::string& str, const std::string& oldStr, const std::string& newStr);

bool str_sane(const std::string &str);

bool str_alphanumeric(const std::string &str);

bool str_numeric(const std::string &str);

bool str_email(const std::string &str);

int str_lines(const std::string *str);


// enum defines
#define LOG_DEBUG_VERBOSE 1
#define LOG_DEBUG 2
#define LOG_INFO 3
#define LOG_WARNING 4
#define LOG_ERROR 5

extern int log_output_level;
void log(int, const char *);

#ifdef ACKNEX

#define CHKHEAP()  (check_heap(__FILE__, __LINE__))
void check_heap(char *file, int line);

#endif

float toRadians(float d);
double toRadians(double d);

float random_range(float mx);
int random_int_range(int mx);
void random_seed(int seed);

void move_dir(glm::vec3 &tomove, const glm::vec3 &direction, float byx, float byy, float byz);
void move_dir(vec &tomove, const vec &direction, float byx, float byy, float byz);

#endif // __HELPER_H__
