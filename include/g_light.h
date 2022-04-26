#ifndef __G_LIGHT_H__
#define __G_LIGHT_H__

#include "gf2d_sprite.h"

typedef struct LightData_S
{
	Sprite *unlit_background;
	Sprite *lit_background;
	Sprite *mask;

	Uint8 debug;
}LightData;

void light_init();

void light_update();

LightData* light_data_get();

void light_data_enable_debug();

void light_data_disable_debug();

#endif // !__G_LIGHT_H__
