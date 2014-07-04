#ifndef __BOX_SV_H__
#define __BOX_SV_H__

#include "box.h"

class box_sv : public box
{

public:
	box_sv(level *, char box_type, vec *, int *box_counter);
	~box_sv();

	virtual void frame(double time_delta);
protected:
	
	int *box_counter;

private:

};



#endif // __BOX_SV_H__
