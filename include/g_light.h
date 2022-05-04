#ifndef __G_LIGHT_H__
#define __G_LIGHT_H__

#include "gf2d_sprite.h"

typedef struct LightData_S
{
	SDL_Texture *unlit_background;
	SDL_Texture *lit_background;
	SDL_Texture *mask;

	SDL_Vertex verts[1024];
	Uint32 vertex_count;

	Uint8 debug;
	SDL_Texture *debug_texture;
}LightData;

void light_init();

void light_update();

LightData* light_data_get();

void light_data_enable_debug();

void light_data_disable_debug();

#endif // !__G_LIGHT_H__

