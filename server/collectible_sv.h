#ifndef __COLLECTIBLE_SV_H__
#define __COLLECTIBLE_SV_H__

#include "collectible.h"
#include "level_sv.h"
class level_sv;

class collectible_sv : public collectible
{
public:

	collectible_sv(level_sv *lvl, char collectible_type, vec *pos);
	~collectible_sv();
	
	virtual void frame(double time_frame);

protected:
	
	level_sv *lvl_sv;
	
private:
};


#endif
