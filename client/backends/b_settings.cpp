#include "b_settings.h"
#include "helper.h"
#include <sstream>

b_settings::b_settings()
{
	// init values
	
}

void b_settings::err(const char *msg)
{
	std::ostringstream pr;
	pr << "ERROR in b_settings in:" << msg;

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
