#ifndef __B_SETTINGS_H__
#define __B_SETTINGS_H__

#include "defs.h"
#include <iostream>
#include <string>
#include <memory>
#include <mutex>


class b_settings
{
public:
	b_settings();

	static b_settings *instance();
private:

	static std::unique_ptr<b_settings> m_instance;
    static std::once_flag m_onceFlag;

	void err(const char *);
};

#endif
