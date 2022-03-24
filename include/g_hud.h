#ifndef __G_HUD_H__
#define __G_HUD_H__

#include <SDL.h>
#include <SDL_ttf.h>

typedef struct{
	SDL_Renderer *renderer;
	TTF_Font *font;

	SDL_Surface *surface;
	SDL_Texture *texture;
}HUD;

void HUD_init();

void HUD_draw();

void HUD_draw_message( char *text, Vector2D position, Vector3D color, Uint32 w, Uint32 h );

#endif // !__G_HUD_H__
