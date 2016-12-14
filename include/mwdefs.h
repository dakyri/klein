#pragma once
enum {
	ERR_OK = 0,
	ERR_ERROR = 200,
};


typedef long status_t;


inline float
interpolate(float d1, float d2, float p)
{
	return d1 + p*(d2 - d1);
}
