#ifndef __G_GLOBALS_H__
#define __G_GLOBALS_H__

#include <SDL_ttf.h>

typedef enum
{
	G_MAIN,
	G_EDIT,
	G_RUN,
	G_PAUSE,
	G_STOP
}GameState;

extern Uint8 g_debug;
extern Uint32 g_screen_width;
extern Uint32 g_screen_height;
extern Uint32 g_time;
extern Uint32 g_update_time;
extern Uint32 g_update_time_fixed;

extern GameState g_state;

extern TTF_Font *g_font;
#endif