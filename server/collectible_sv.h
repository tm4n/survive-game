#ifndef __COLLECTIBLE_SV_H__
#define __COLLECTIBLE_SV_H__

#include "collectible.h"

class collectible_sv : public collectible
{
public:

	collectible_sv(level *lvl, char collectible_type, vec *pos);
	~collectible_sv();

protected:
private:
};


#endif