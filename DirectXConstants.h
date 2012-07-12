#ifndef DX_CONSTANTS_H
#define DX_CONSTANTS_H


// Constants shared between main program and shader program
#define MAX_LIGHTS ( 5 )

const float PI       = 3.14159265358979323f;
const float TWO_PI       = 2 * 3.14159265358979323f;
const float MATH_EPS = 0.0001f;

const float searchAlpha = 3.14159265358979323f / 60;
const float searchBeta  = 3.14159265358979323f / 50;

static const float SHADOW_EPSILON = 0.001f;
static const float SMAP_SIZE = 1024.0f;
static const float SMAP_DX = 1.0f / SMAP_SIZE;



#endif
