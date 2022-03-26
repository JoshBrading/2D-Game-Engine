#ifndef __G_GLOBALS_H__
#define __G_GLOBALS_H__

#include <SDL_ttf.h>

typedef enum
{
	G_RUN,
	G_PAUSE,
	G_STOP
}GameState;

extern Uint8 g_debug;
extern Uint32 g_screen_width;
extern Uint32 g_screen_height;
extern Uint32 g_time;

extern GameState g_state;

extern TTF_Font *g_font;
#endif