#ifndef __VEC_H__
#define __VEC_H__


class vec
{
    public:
        float x, y, z;

        vec();
        vec(vec *);
        vec(float, float, float);

        float dist(vec *);
        float dist2d(vec *);
        void set(vec *);
        void zero();
		float length();

		static float angle(float in);

    protected:
    private:
};

#endif // __VEC_H__
