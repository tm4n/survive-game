#include "vec.h"
#include <math.h>

vec::vec()
{
    x = 0;
    y = 0;
    z = 0;
}

vec::vec(vec *v)
{
    x = v->x;
    y = v->y;
    z = v->z;
}

vec::vec(float ax, float ay, float az)
{
    x = ax;
    y = ay;
    z = az;
}

float vec::dist2d(vec *v)
{
    float tx = x - v->x;
    float ty = y - v->y;

    return sqrt(tx*tx + ty*ty);
}

float vec::dist(vec *v)
{
    float tx = x - v->x;
    float ty = y - v->y;
    float tz = z - v->z;

    return sqrt(tx*tx + ty*ty + tz*tz);
}

void vec::set(vec *v)
{
    x = v->x;
    y = v->y;
    z = v->z;
}

void vec::zero()
{
    x = 0.f;
    y = 0.f;
    z = 0.f;
}


float vec::length()
{
	return sqrt(x*x + y*y + z*z);
}

float vec::angle(float in)
{
    while (in > 180) in -= 360;
    while (in < -180) in += 360;
    return in;
}
