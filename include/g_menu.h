#ifndef __G_MENU_H__
#define __G_MENU_H__

#include <SDL.h>
#include <SDL_ttf.h>
#include "gf2d_sprite.h"
#include "gfc_list.h"

typedef struct
{
	char		*text;
	TTF_Font	*font;
	Vector2D	position;
}MenuText;

typedef struct
{
	Uint8			selected;
	char			*tag;
	MenuText		label;
	Vector2D		position;

	Sprite			*background;
	Sprite			*background_selected;
	Sprite			*icon;
	Vector2D		icon_offset;

	void			(*action)( struct Menu_S *self, void *data);
	void			*data;

	SDL_Rect		bounds;
}MenuButton;

typedef struct
{
	Sprite *sprite;
	Vector2D position;
}MenuImage;

typedef struct Menu_S
{
	Uint8			_inuse;
	Uint8			enabled;

	char			*tag;

	List			*labels;
	List			*images;
	List			*buttons;
	MenuButton		*current_button;

	Sprite			*background;

	struct Menu_S	*prev;
	void			(*update)(struct Menu_S *self);
}Menu;

void menu_manager_init( Uint32 maxMenus );

void menu_manager_close();

Menu *menu_new();

void menu_free( Menu *self );

void menu_update( Menu *self );

void menu_draw( Menu *self );

void menu_manager_update_all();

void menu_manager_draw_all();

void menu_close( Menu *self );

void menu_go_to( Menu *self, Menu *next );

void menu_go_back( Menu *self );

void menu_quit( Menu *self );
#endif // !__G_MENU_H__
