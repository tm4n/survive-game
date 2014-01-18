#ifndef __HELPER_H__
#define __HELPER_H__

#include <vector>
#include <string>

void get_workdir(std::string *out);

void get_savedir(const char *account_name, std::string *out);

std::vector<std::string> split(const char *, char = ' ');

bool str_assemble(std::string *input, int i1, int i2, float f1, float f2, std::string *s1, std::string *s2, std::string *s3);

void str_replace(std::string& str, const std::string& oldStr, const std::string& newStr);

bool str_sane(const std::string &str);

bool str_alphanumeric(const std::string &str);

bool str_numeric(const std::string &str);

bool str_email(const std::string &str);

int str_lines(const std::string *str);


#ifdef ACKNEX

#define CHKHEAP()  (check_heap(__FILE__, __LINE__))
void check_heap(char *file, int line);

#endif

float toRadians(float d);

#endif // __HELPER_H__
