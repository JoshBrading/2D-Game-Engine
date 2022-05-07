#include "g_menu.h"
#include "simple_logger.h"
#include "simple_json.h"
#include "gf2d_graphics.h"
#include "g_globals.h"
#include "g_func.h"
#include "gf2d_draw.h"

static void menu_draw_text( Menu *self, MenuText *text );
static void menu_draw_button( Menu *self, MenuButton *button );
static void menu_draw_text_with_offset( Menu *self, MenuText text, Vector2D offset );
static void menu_draw_dropdown( Menu *self, MenuDropdown *dropdown );
static MenuButton *menu_next_button( Menu *self, MenuButton *current );
static MenuButton *menu_prev_button( Menu *self, MenuButton *current );
static void menu_load_images( Menu *menu, SJson *images );
void menu_button_activate( Menu *self, MenuButton *button );


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
	Uint32 i;
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
	if (!menu) return NULL;

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

	for (Uint32 i = 0; i < image_count; i++)
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

	for (Uint32 i = 0; i < label_count; i++)
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

	for (Uint32 i = 0; i < label_count; i++)
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

	Uint32 i;
	for (i = 0; i < menu_manager.menu_count; i++)
	{
		if (!menu_manager.menu_list[i]._inuse)
		{
			menu_manager.menu_list[i]._inuse = 1;
			menu_manager.menu_list[i].enabled = false;
			menu_manager.menu_list[i].images = gfc_list_new();
			menu_manager.menu_list[i].buttons = gfc_list_new();
			menu_manager.menu_list[i].labels = gfc_list_new();
			menu_manager.menu_list[i].dropdowns = gfc_list_new();

			menu_manager.menu_list[i].nav_btn_ctx = menu_manager.menu_list[i].buttons;

			menu_manager.menu_list[i].floating_menu_offset = vector2d( 0, 0 );

			menu_manager.menu_list[i].hover_sfx = gfc_sound_load( "audio/SFX_button.mp3", 1, 0 );
			menu_manager.menu_list[i].click_sfx = gfc_sound_load( "audio/SFX_button_click.mp3", 1, 1 );
			menu_manager.menu_list[i].open_sfx = gfc_sound_load( "audio/SFX_dropdown.mp3", 1, 2 );
			menu_manager.menu_list[i].close_sfx = gfc_sound_load( "audio/SFX_dropdown_close.mp3", 1, 3 );

			return (&menu_manager.menu_list[i]);
		}
	}
	slog( "MenuNew: No free space" );
	return NULL;
}

MenuButton *menu_button_new()
{
	MenuButton *button = malloc( sizeof( MenuButton ) );
	if (!button)
	{
		slog( "Failed to allocate button" );
		return NULL;
	}
	button->action = NULL;
	button->selected = false;
	button->label.text = "Sample Button";
	button->label.font = TTF_OpenFont( "fonts/arial.ttf", 18 );
	button->label.position = vector2d( 30, 0 );
	button->background = gf2d_sprite_load_image( "images/gui/button.png" );
	button->icon_offset = vector2d( 5, 2 );

	button->bounds.x = button->position.x;
	button->bounds.y = button->position.y;
	button->bounds.w = button->background->frame_w;
	button->bounds.h = button->background->frame_h;

	return button;
}

MenuText *menu_text_new()
{
	MenuText *text = malloc( sizeof( MenuText ) );
	if (!text)
	{
		slog( "Failed to allocate text" );
		return NULL;
	}

	text->text = "Sample Text";
	text->font = TTF_OpenFont( "fonts/FRADMCN.ttf", 36 );
	return text;
}

MenuImage *menu_image_new()
{
	MenuImage *image = malloc( sizeof( MenuImage ) );
	if (!image)
	{
		slog( "Failed to allocate image" );
		return NULL;
	}
	return image;
}

MenuDropdown *menu_dropdown_new()
{
	MenuDropdown *dropdown = malloc( sizeof( MenuDropdown ) );
	if (!dropdown)
	{
		slog( "Failed to allocate dropdown" );
		return NULL;
	}

	dropdown->buttons = gfc_list_new();
	dropdown->offset = vector2d( 0, 0 );

	dropdown->underlay.sprite = gf2d_sprite_load_image( "images/gui/50p_dim.png" );
	dropdown->underlay.position = vector2d( 0, 0 );
	return dropdown;
}

void menu_button_activate( Menu* self, MenuButton *button )
{
	if (self->current_button->action) self->current_button->action( self, self->current_button->data );
}

void menu_free( Menu *self )
{
	if (!self)
	{
		slog( "MenuFree: Menu does not exist" );
		return;
	}
	if (self->background)
	{
		gf2d_sprite_free( self->background );
	}
//	if ( self->buttons && gfc_list_get_count( self->buttons ) > 0)
//	{
//		for (Uint32 i = 0; i < gfc_list_get_count( self->buttons ); i++)
//		{
//			if (self->buttons->elements[i].data == NULL) continue;
//		}
//	}
	self->_inuse = false;
	memset( self, 0, sizeof( self ) );
}

void menu_think( Menu *self )
{
	if (!self)return;
	if (!self->_inuse) return;
	if (!self->enabled) return;
	SDL_PumpEvents();
	SDL_Event e;
	SDL_PollEvent( &e );
	int mx, my;
	SDL_GetMouseState( &mx, &my );

	if (e.type == SDL_MOUSEBUTTONDOWN)
	{
		if (e.button.button == SDL_BUTTON_LEFT)
		{
			if (mx >= self->current_button->bounds.x &&
				 mx <= self->current_button->bounds.x + self->current_button->bounds.w &&
				 my >= self->current_button->bounds.y &&
				 my <= self->current_button->bounds.y + self->current_button->bounds.h)
			{

				menu_button_activate( self, NULL );
			}
		}
	}



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

			gfc_sound_play( self->click_sfx, 0, 1, -1, -1 );
			moved = true;
		}



		if (moved) move_timer = g_time + move_cooldown; // Only update if we moved to prevent unresponsive clicks
	}
	gf2d_draw_rect( self->current_button->bounds, vector4d( 255, 255, 255, 255 ) );



	if (self->think)self->think( self );
}

void menu_update( Menu *self )
{
	if (!self)return;
	if (!self->_inuse) return;
	if (!self->enabled) return;

	SDL_Event e;
	SDL_PollEvent( &e );
	int mx, my;
	SDL_GetMouseState( &mx, &my );

	if (e.type == SDL_MOUSEBUTTONDOWN)
	{
		slog( "Event" );
	}

	if (self->buttons)
	{
		for (Uint32 i = 0; i < self->buttons->count; i++)
		{
			MenuButton *button = (MenuButton *)self->buttons->elements[i].data;
			if (!button) continue;
			if (mx >= button->bounds.x &&
				 mx <= button->bounds.x + button->bounds.w &&
				 my >= button->bounds.y &&
				 my <= button->bounds.y + button->bounds.h)
			{
				self->current_button = button;

				gf2d_draw_fill_rect( button->bounds, vector4d( 0, 255, 0, 100 ) );
			}

			//gf2d_draw_circle( vector2d( mx, my ), 4, vector4d( 0, 0, 255, 255 ) );
			//gf2d_draw_rect( button->bounds, vector4d( 255, 0, 0, 255 ) );
		}
	}

	if (self->dropdowns)
	{
		for (Uint32 i = 0; i < self->dropdowns->count; i++)
		{
			MenuDropdown *dropdown = (MenuDropdown *)self->dropdowns->elements[i].data;
			if (!dropdown) continue;
			if (!dropdown->active) continue;
			if (dropdown->buttons)
			{
				for (Uint32 i = 0; i < dropdown->buttons->count; i++)
				{
					MenuButton *button = (MenuButton *)dropdown->buttons->elements[i].data;
					if (!button) return;
					if (mx >= button->bounds.x &&
							 mx <= button->bounds.x + button->bounds.w &&
							 my >= button->bounds.y &&
							 my <= button->bounds.y + button->bounds.h)
					{
						self->current_button = button;
						gf2d_draw_fill_rect( button->bounds, vector4d( 0, 255, 0, 100 ) );
					}

					gf2d_draw_circle( vector2d( mx, my ), 4, vector4d( 0, 0, 255, 255 ) );
					gf2d_draw_rect( button->bounds, vector4d( 255, 0, 0, 255 ) );
				}
			}
		}
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

	if (self->buttons)
	{
		for (Uint32 i = 0; i < self->buttons->count; i++)
		{
			MenuButton *button = (MenuButton *)self->buttons->elements[i].data;
			if (!button) return;
			button->bounds.x = button->position.x + self->floating_menu_offset.x;
			button->bounds.y = button->position.y + self->floating_menu_offset.y;
		}
	}

	if (self->dropdowns)
	{
		for (Uint32 i = 0; i < self->dropdowns->count; i++)
		{
			MenuDropdown *dropdown = (MenuDropdown *)self->dropdowns->elements[i].data;
			if (!dropdown) return;

			if (dropdown->buttons)
			{
				for (Uint32 i = 0; i < dropdown->buttons->count; i++)
				{
					MenuButton *button = (MenuButton *)dropdown->buttons->elements[i].data;
					if (!button) return;
					button->bounds.x = button->position.x + self->floating_menu_offset.x;
					button->bounds.y = button->position.y + self->floating_menu_offset.y;
				}
			}
		}
	}
}


void menu_draw( Menu *self )
{
	if (!self)return;
	if (!self->_inuse) return;
	if (!self->enabled) return;


	Vector2D pos_plus_offset;

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
		for (Uint32 i = 0; i < self->images->count; i++)
		{
			MenuImage *image = (MenuImage *)self->images->elements[i].data;
			if (!image) return;

			vector2d_add( pos_plus_offset, image->position, self->floating_menu_offset );

			gf2d_sprite_draw( image->sprite,
							  pos_plus_offset,
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
		for (Uint32 i = 0; i < self->labels->count; i++)
		{
			MenuText *label = (MenuText *)self->labels->elements[i].data;
			if (!label) return;
			menu_draw_text( self, label );
		}
	}
	
	if (self->buttons)
	{
		for (Uint32 i = 0; i < self->buttons->count; i++)
		{
			MenuButton *button = (MenuButton *)self->buttons->elements[i].data;
			if (!button) return;
			menu_draw_button( self, button );
		}
	}

	if (self->dropdowns)
	{
		for (Uint32 i = 0; i < self->dropdowns->count; i++)
		{
			MenuDropdown *dropdown = (MenuDropdown *)self->dropdowns->elements[i].data;
			if (!dropdown) return;
			menu_draw_dropdown( self, dropdown );
		}
	}
	
	if (self->selector_sprite)
	{
		vector2d_add( pos_plus_offset, self->selector_position, self->floating_menu_offset );

		gf2d_sprite_draw( self->selector_sprite,
						  pos_plus_offset,
						  NULL,
						  NULL,
						  NULL,
						  NULL,
						  NULL,
						  0 );
	}

	if (self->update)self->update( self );
}

void menu_manager_think_all()
{
	Uint32 i;
	for (i = 0; i < menu_manager.menu_count; i++)
	{
		if (!menu_manager.menu_list[i]._inuse && !menu_manager.menu_list[i].enabled)// not used yet
		{
			continue;// skip this iteration of the loop
		}
		menu_think( &menu_manager.menu_list[i] );
	}
}

void menu_manager_update_all()
{
	Uint32 i;
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
	Uint32 i;
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
	Uint32 i;
	for (i = 0; i < menu_manager.menu_count; i++)
	{
		if (!menu_manager.menu_list[i]._inuse)// not used yet
		{
			continue;// skip this iteration of the loop
		}
		menu_draw( &menu_manager.menu_list[i] );
	}
}

void menu_draw_text( Menu* self, MenuText *text )
{
	if (!text) return;
	if (!text->text) return;
	if (!text->font) return;
	SDL_Color c = { 255, 255, 255 };
	SDL_Rect rect = { text->position.x + self->floating_menu_offset.x, text->position.y + self->floating_menu_offset.y, 0, 0 };
	
	surface = TTF_RenderText_Blended( text->font, text->text, c );
	texture = SDL_CreateTextureFromSurface( renderer, surface );
	
	rect.w = surface->w;
	rect.h = surface->h;
	
	SDL_RenderCopy( renderer, texture, NULL, &rect );
	
	SDL_FreeSurface( surface );
	SDL_DestroyTexture( texture );
}

void menu_draw_text_with_offset( Menu *self, MenuText text, Vector2D offset )
{
	Vector2D tmp;
	vector2d_add( tmp, text.position, offset );
	text.position = tmp;

	menu_draw_text( self, &text );
}


void menu_draw_dropdown( Menu *self, MenuDropdown *dropdown )
{
	if (!dropdown) return;
	if (!dropdown->active) return;
	if (!dropdown->label.text) return;
	if (!dropdown->label.font) return;
//	menu_draw_text_with_offset( dropdown->label, dropdown->offset );
	if (dropdown->underlay.sprite)
	{
		gf2d_sprite_draw( dropdown->underlay.sprite,
						  dropdown->underlay.position,
						  NULL,
						  NULL,
						  NULL,
						  NULL,
						  NULL,
						  0 );
	}
	for (Uint32 i = 0; i < dropdown->buttons->count; i++)
	{
		MenuButton *button = (MenuButton *)dropdown->buttons->elements[i].data;
		if (!button) return;
		menu_draw_button( self, button );
	}


}

void menu_draw_button( Menu *self, MenuButton *button )
{
	if (!button)return;

	if (!button->background) return;

	Vector2D offset;

	vector2d_add( offset, button->position, self->floating_menu_offset );

	gf2d_sprite_draw( button->background,
					  offset,
					  NULL,
					  NULL,
					  NULL,
					  NULL,
					  NULL,
					  0 );

	menu_draw_text_with_offset( self, button->label, button->position );

	//gf2d_draw_rect(button->bounds, vector4d(255, 0, 0, 255));
	//SDL_Rect test = { 100, 100, 50, 50 };
	//gf2d_draw_rect(test, vector4d(255, 0, 0, 255));
}

MenuButton *menu_next_button( Menu *self, MenuButton* current )
{
	if (!self) return NULL;
	if (!current) return NULL;
	Uint32 index = gfc_list_get_item_index( self->nav_btn_ctx, current ) + 1;
	MenuButton *next;
	if (index < gfc_list_get_count( self->nav_btn_ctx ))
	{
		next = gfc_list_get_nth( self->nav_btn_ctx, index );
	}
	else
	{
		next = gfc_list_get_nth( self->nav_btn_ctx, 0 );
	}
	if (!next) return NULL;

	current->selected = false;
	next->selected = true;

	self->current_button = next;

	gfc_sound_play( self->hover_sfx, 0, 1, -1, -1 );

	return next;
}

MenuButton *menu_prev_button( Menu *self, MenuButton *current )
{
	if (!self) return NULL;
	if (!current) return NULL;
	Uint32 index = gfc_list_get_item_index( self->nav_btn_ctx, current );
	MenuButton *next;
	if (index > 0)
	{
		next = gfc_list_get_nth( self->nav_btn_ctx, index - 1);
	}
	else
	{
		next = gfc_list_get_nth( self->nav_btn_ctx, self->nav_btn_ctx->count - 1);
	}

	if (!next) return NULL;
	current->selected = false;
	next->selected = true;

	self->current_button = next;
	gfc_sound_play( self->hover_sfx, 0, 1, -1, -1 );

	return next;
}

void menu_close( Menu *self )
{
	if(!self)return;
	self->enabled = false;
	g_state = G_RUN;

	gfc_sound_play( self->close_sfx, 0, 1, -1, -1 );
}

void menu_manager_close_all()
{
	Uint32 i;
	for (i = 0; i < menu_manager.menu_count; i++)
	{
		if (!menu_manager.menu_list[i]._inuse)// not used yet
		{
			continue;// skip this iteration of the loop
		}
		menu_close( &menu_manager.menu_list[i] );
	}
}

void menu_open( Menu *self )
{
	if(!self)return;
	self->enabled = true;
	g_state = G_PAUSE;

	gfc_sound_play( self->open_sfx, 0, 1, -1, -1 );
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

void menu_activate_dropdown( Menu* self, MenuDropdown *dropdown )
{
	dropdown->active = true;
	
	//self->prev_button = self->current_button;
	dropdown->prev_button = self->current_button;
	self->current_button = dropdown->current_button;
	dropdown->prev_btn_list = self->nav_btn_ctx;
	self->nav_btn_ctx = dropdown->buttons;

	gfc_sound_play( self->open_sfx, 0, 1, -1, -1 );
}

void menu_deactivate_dropdown( Menu* self, MenuDropdown *dropdown )
{
	dropdown->active = false;

	self->current_button = dropdown->prev_button;
	self->nav_btn_ctx = dropdown->prev_btn_list;

	gfc_sound_play( self->close_sfx, 0, 1, -1, -1 );
}

void menu_quit( )
{
	g_state = G_STOP;
}

void menu_g_state_run()
{
	g_state = G_RUN;
}

void menu_g_state_change( Menu *self, int state )
{
	g_state = state;
}

Menu *menu_manager_get_by_tag( char *tag )
{
	Uint32 i;
	for (i = 0; i < menu_manager.menu_count; i++)
	{
		if (!menu_manager.menu_list[i]._inuse)// not used yet
		{
			continue;// skip this iteration of the loop
		}
		if (strcmp( menu_manager.menu_list[i].tag, tag ) == 0)
		{
			return &menu_manager.menu_list[i];
		}
	}

	return NULL;
}