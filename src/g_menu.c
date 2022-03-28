#include "g_menu.h"
#include "simple_logger.h"
#include "simple_json.h"
#include "gf2d_graphics.h"
#include "g_globals.h"
#include "g_func.h"

static void menu_draw_text( MenuText *text );
static void menu_draw_button( MenuButton *button );
static void menu_draw_text_with_offset( MenuText text, Vector2D offset );
static MenuButton *menu_next_button( Menu *self, MenuButton *current );
static MenuButton *menu_prev_button( Menu *self, MenuButton *current );
static void menu_load_images( Menu *menu, SJson *images );

static Uint32 move_cooldown = 200;
static Uint32 move_timer;

typedef struct
{
	Menu	*menu_list;
	Uint32	menu_count;
}MenuManager;

static MenuManager menu_manager = { 0 };

static SDL_Renderer *renderer;
static SDL_Texture *texture;
static SDL_Surface *surface;

void menu_manager_init( Uint32 maxMenus )
{
	menu_manager.menu_list = gfc_allocate_array( sizeof( Menu ), maxMenus );
	if (menu_manager.menu_list == NULL)
	{
		slog( "MenuManagerInit: Failed to allocate array of size: %i", maxMenus );
		return;
	}
	menu_manager.menu_count = maxMenus;
	atexit( menu_manager_close );

	renderer = gf2d_graphics_get_renderer();

	slog( "MenuManager: Initialized" );
}

void menu_manager_close()
{
	int i;
	for (i = 0; i < menu_manager.menu_count; i++)
	{
		menu_free( &menu_manager.menu_list[i] );
	}
	free( menu_manager.menu_list );
	slog( "MenuManager: Closed" );
}

Menu *menu_load( char* filename )
{
	//sj_enable_debug();
	SJson *json, *mjson, *ijson, *ljson, *bjson;
	json = sj_load( filename );

	if (!json)
	{
		slog( "Failed to load json file (%s)", filename );
		return NULL;
	}

	mjson = sj_object_get_value( json, "menu" );
	if (!mjson)
	{
		slog( "Failed to find menu in (%s)", filename );
		sj_free( json );
		return NULL;
	}

	ijson = sj_object_get_value( json, "images" );
	if (!ijson)
	{
		slog( "Failed to find images in (%s)", filename );
		sj_free( json );
		return NULL;
	}

	ljson = sj_object_get_value( json, "labels" );
	if (!ljson)
	{
		slog( "Failed to find labels in (%s)", filename );
		sj_free( json );
		return NULL;
	}

	bjson = sj_object_get_value( json, "buttons" );
	if (!bjson)
	{
		slog( "Failed to find buttons in (%s)", filename );
		sj_free( json );
		return NULL;
	}


	Menu *menu = menu_new();
	if (!menu) return;

	char *background = sj_get_string_value( sj_object_get_value( mjson, "background" ) );
	char *selector_sprite = sj_get_string_value( sj_object_get_value( mjson, "select_icon" ) );

	menu->tag = sj_get_string_value( sj_object_get_value(mjson, "name"));
	menu->background = gf2d_sprite_load_image( background );
	menu->selector_sprite = gf2d_sprite_load_image( selector_sprite );

	menu->selector_position = vector2d( 0, 0 );
	menu->selector_target_pos = vector2d( 0, 0 );

	menu_load_images( menu, ijson );

	return menu;
}

void menu_load_images( Menu *menu, SJson *images )
{
	if (!menu)return;
	if (!images)return;

	Uint32 image_count = sj_array_get_count( images );
	if (image_count == 0) return;

	for (int i = 0; i < image_count; i++)
	{
		MenuImage menuImage;
		Vector2D pos;
		SJson *i_json = sj_array_get_nth( images, i );
		char *filename = sj_get_string_value( sj_object_get_value( i_json, "sprite" ) );

		menuImage.sprite = gf2d_sprite_load_image( filename );
		sj_get_integer_value( sj_object_get_value( i_json, "pos_x" ), &pos.x );
		sj_get_integer_value( sj_object_get_value( i_json, "pos_y" ), &pos.y );

		gfc_list_append( menu->images, &menuImage );
	}
}

void menu_load_labels( Menu *menu, SJson *labels )
{
	if (!menu)return;
	if (!labels)return;

	Uint32 label_count = sj_array_get_count( labels );
	if (label_count == 0) return;

	for (int i = 0; i < label_count; i++)
	{
		MenuText menuText;
		Vector2D pos;
		Uint32 fontSize = 0;
		SJson *l_json = sj_array_get_nth( labels, i );
		char *filename = sj_get_string_value( sj_object_get_value( l_json, "font" ) );

		sj_get_integer_value( sj_object_get_value( l_json, "font_size" ), &fontSize );
		menuText.text = sj_get_string_value( sj_object_get_value( l_json, "text" ) );
		menuText.font = TTF_OpenFont( filename, fontSize );
		
		sj_get_integer_value( sj_object_get_value( l_json, "pos_x" ), &pos.x );		
		sj_get_integer_value( sj_object_get_value( l_json, "pos_y" ), &pos.y );

		gfc_list_append( menu->labels, &menuText );
	}
}

void menu_load_buttons( Menu *menu, SJson *buttons )
{
	if (!menu)return;
	if (!buttons)return;

	Uint32 label_count = sj_array_get_count( buttons );
	if (label_count == 0) return;

	for (int i = 0; i < label_count; i++)
	{
		MenuButton menuButton;
		Vector2D pos;
		Uint32 fontSize = 0;
		SJson *l_json = sj_array_get_nth( buttons, i );
		char *filename = sj_get_string_value( sj_object_get_value( l_json, "font" ) );
		
		menuButton.icon_offset = vector2d( 0, 0 );


		sj_get_integer_value( sj_object_get_value( l_json, "font_size" ), &fontSize );
		menuButton.label.text = sj_get_string_value( sj_object_get_value( l_json, "text" ) );
		menuButton.label.font = TTF_OpenFont( filename, fontSize );

		sj_get_integer_value( sj_object_get_value( l_json, "pos_x" ), &pos.x );
		sj_get_integer_value( sj_object_get_value( l_json, "pos_y" ), &pos.y );

		menuButton.position = pos;
		gfc_list_append( menu->buttons, &menuButton );
	}
}

Menu *menu_new()
{

	int i;
	for (i = 0; i < menu_manager.menu_count; i++)
	{
		if (!menu_manager.menu_list[i]._inuse)
		{
			menu_manager.menu_list[i]._inuse = 1;
			menu_manager.menu_list[i].enabled = false;
			menu_manager.menu_list[i].images = gfc_list_new();
			menu_manager.menu_list[i].buttons = gfc_list_new();
			menu_manager.menu_list[i].labels = gfc_list_new();

			return (&menu_manager.menu_list[i]);
		}
	}
	slog( "EntityNew: No free space" );
	return NULL;
}

void menu_link_all()
{

}

void menu_free( Menu *self )
{
	if (!self)
	{
		slog( "EntityFree: Entity does not exist" );
		return;
	}
	if (self->background)
	{
		gf2d_sprite_free( self->background );
	}
//	if ( self->buttons && gfc_list_get_count( self->buttons ) > 0)
//	{
//		for (int i = 0; i < gfc_list_get_count( self->buttons ); i++)
//		{
//			if (self->buttons->elements[i].data == NULL) continue;
//		}
//	}
	self->_inuse = false;
	memset( self, 0, sizeof( self ) );
}

void menu_update( Menu *self )
{
	if (!self)return;
	if (!self->_inuse) return;
	if (!self->enabled) return;

	if (move_timer < g_time)
	{
		Uint8 moved = false;
		const Uint8 *keys;
		keys = SDL_GetKeyboardState( NULL );

		if (keys[SDL_SCANCODE_DOWN])
		{
			menu_next_button( self, self->current_button );
			moved = true;
		}
		if (keys[SDL_SCANCODE_UP])
		{
			menu_prev_button( self, self->current_button );
			moved = true;
		}
		if (keys[SDL_SCANCODE_RETURN])
		{
			if (self->current_button->action) self->current_button->action( self, self->current_button->data );
			moved = true;
		}
		

		if( moved ) move_timer = g_time + move_cooldown; // Onlu update if we moved to prevent unresponsive clicks
	}

	if (self->update)self->update( self );
}

void menu_update_fixed( Menu *self )
{
	if (!self)return;
	if (!self->_inuse) return;
	if (!self->enabled) return;

	if (self->selector_sprite && self->current_button)
	{
		self->selector_target_pos = self->current_button->position;
		Vector2D offset_target;

		vector2d_add( offset_target, self->current_button->position, self->current_button->icon_offset );
		Vector2D tmp = lerp_vector_2d( self->selector_position, offset_target, 0.15 );

		//vector2d_add( tmp, tmp, tmp_offset );
		self->selector_position = tmp;
	}
}

void menu_draw( Menu *self )
{
	if (!self)return;
	if (!self->_inuse) return;
	if (!self->enabled) return;

	if (self->background)
	{
		gf2d_sprite_draw( self->background,
						  vector2d( 0, 0 ),
						  NULL, 
						  NULL, 
						  NULL, 
						  NULL, 
						  NULL, 
						  0 );
	}


	if (self->images)
	{
		for (int i = 0; i < self->images->count; i++)
		{
			MenuImage *image = (MenuImage *)self->images->elements[i].data;
			if (!image) return;
			gf2d_sprite_draw( image->sprite,
							  image->position,
							  NULL,
							  NULL,
							  NULL,
							  NULL,
							  NULL,
							  0 );
		}
	}

	if (self->labels)
	{
		for (int i = 0; i < self->labels->count; i++)
		{
			MenuText *label = (MenuText *)self->labels->elements[i].data;
			if (!label) return;
			menu_draw_text( label );
		}
	}

	if (self->buttons)
	{
		for (int i = 0; i < self->buttons->count; i++)
		{
			MenuButton *button = (MenuButton *)self->buttons->elements[i].data;
			if (!button) return;
			menu_draw_button( button );
		}
	}

	if (self->selector_sprite)
	{
		gf2d_sprite_draw( self->selector_sprite,
						  self->selector_position,
						  NULL,
						  NULL,
						  NULL,
						  NULL,
						  NULL,
						  0 );
	}

	if (self->update)self->update( self );
}

void menu_manager_update_all()
{
	int i;
	for (i = 0; i < menu_manager.menu_count; i++)
	{
		if (!menu_manager.menu_list[i]._inuse)// not used yet
		{
			continue;// skip this iteration of the loop
		}
		menu_update( &menu_manager.menu_list[i] );
	}
}

void menu_manager_update_fixed_all()
{
	int i;
	for (i = 0; i < menu_manager.menu_count; i++)
	{
		if (!menu_manager.menu_list[i]._inuse)// not used yet
		{
			continue;// skip this iteration of the loop
		}
		menu_update_fixed( &menu_manager.menu_list[i] );
	}
}

void menu_manager_draw_all()
{
	int i;
	for (i = 0; i < menu_manager.menu_count; i++)
	{
		if (!menu_manager.menu_list[i]._inuse)// not used yet
		{
			continue;// skip this iteration of the loop
		}
		menu_draw( &menu_manager.menu_list[i] );
	}
}

void menu_draw_text( MenuText *text )
{
	if (!text) return;
	if (!text->text) return;
	if (!text->font) return;
	SDL_Color c = { 255, 255, 255 };
	SDL_Rect rect = { text->position.x, text->position.y, 0, 0 };

	surface = TTF_RenderText_Blended( text->font, text->text, c );
	texture = SDL_CreateTextureFromSurface( renderer, surface );

	rect.w = surface->w;
	rect.h = surface->h;

	SDL_RenderCopy( renderer, texture, NULL, &rect );

	SDL_FreeSurface( surface );
	SDL_DestroyTexture( texture );
}

void menu_draw_text_with_offset( MenuText text, Vector2D offset )
{
	Vector2D tmp;
	vector2d_add( tmp, text.position, offset );
	text.position = tmp;

	menu_draw_text( &text );
}

void menu_draw_button( MenuButton *button )
{
	if (!button)return;
	gf2d_sprite_draw( button->background,
					  button->position,
					  NULL,
					  NULL,
					  NULL,
					  NULL,
					  NULL,
					  0 );

	//if (button->selected)
	//{
	//	Vector2D tmp;
	//	vector2d_add( tmp, button->position, button->icon_offset );
	//	gf2d_sprite_draw( button->icon,
	//					  tmp,
	//					  NULL,
	//					  NULL,
	//					  NULL,
	//					  NULL,
	//					  NULL,
	//					  0 );
	//}
	menu_draw_text_with_offset( button->label, button->position );
}

MenuButton *menu_next_button( Menu *self, MenuButton* current )
{
	if (!self) return;
	if (!current) return;
	Uint32 index = gfc_list_get_item_index( self->buttons, current ) + 1;
	MenuButton *next;
	if (index < gfc_list_get_count( self->buttons ))
	{
		next = gfc_list_get_nth( self->buttons, index);
	}
	else
	{
		next = gfc_list_get_nth( self->buttons, 0 );
	}
	if (!next) return NULL;

	current->selected = false;
	next->selected = true;

	self->current_button = next;
}

MenuButton *menu_prev_button( Menu *self, MenuButton *current )
{
	if (!self) return;
	if (!current) return;
	Uint32 index = gfc_list_get_item_index( self->buttons, current );
	slog( "%i", index );
	MenuButton *next;
	if (index > 0)
	{
		next = gfc_list_get_nth( self->buttons, index - 1);
	}
	else
	{
		next = gfc_list_get_nth( self->buttons, self->buttons->count - 1);
	}

	if (!next) return NULL;
	current->selected = false;
	next->selected = true;

	self->current_button = next;
}

void menu_close( Menu *self )
{
	if(!self)return;
	self->enabled = false;
	g_state = G_RUN;
}

void menu_open( Menu *self )
{
	if(!self)return;
	self->enabled = true;
	g_state = G_PAUSE;
}

void menu_go_to( Menu *self, Menu *next )
{
	if (!self)return;
	if (!next)return;

	next->prev = self;
	self->enabled = false;
	next->enabled = true;
}

void menu_go_back( Menu *self )
{
	if (!self)return;
	if (!self->prev) return;
	
	self->enabled = false;
	self->prev->enabled = true;
}

void menu_quit( Menu *self )
{
	g_state = G_STOP;
}