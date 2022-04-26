#ifndef __G_LIGHT_H__
#define __G_LIGHT_H__

#include "gf2d_sprite.h"

typedef struct LightData_S
{
	Sprite *unlit_background;
	Sprite *lit_background;
	Sprite *mask;
}LightData;

void light_update();

#endif // !__G_LIGHT_H__
