#ifndef __G_MENU_H__
#define __G_MENU_H__

#include <SDL.h>
#include <SDL_ttf.h>
#include "gf2d_sprite.h"
#include "gfc_list.h"
#include "gfc_audio.h"

typedef struct MenuText_S
{
	char		*text; /**<char* to display*/
	TTF_Font	*font; /**<A pointer to the font used by the text*/
	Vector2D	position; /**<Where the text is displayed*/
}MenuText;

typedef struct MenuImage_S
{
	Sprite *sprite; /**<A pointer to the sprite used for an image*/
	Vector2D position; /**<Where the image is displayed*/
}MenuImage;

typedef struct MenuButton_S
{
	Uint8			selected; /**<flag to trigger if mouse or user highlights this button*/
	char			*tag; 
	MenuText		label; /**<MenuText of this button*/
	Vector2D		position; /**<Where the button is displayed*/

	Sprite			*background; /**<A pointer to the background sprite*/
	Sprite			*background_selected; /**<A pointer to the selected background sprite*/
	Vector2D		icon_offset; /**<Offset of the menu selector icon*/

	void			(*action)( struct Menu_S *self, void *data); /**<A pointer to the action function*/
	void			*data; /**<A pointer to the parameters for the action function*/

	SDL_Rect		bounds; /**<The bounding box used to trigger mouse clicks/selections*/
}MenuButton;

typedef struct MenuDropdown_S
{
	Uint8		active;
	MenuText	label;
	List		*buttons;
	List		*prev_btn_list;
	MenuButton	*current_button;
	MenuButton	*prev_button;
	Vector2D	offset; // unused... for now...

	MenuImage	underlay; // Used for dimming background
}MenuDropdown;

typedef struct MenuSelector_S
{
	MenuImage	icon; /**<MenuImage of the selector*/
	Vector2D	target_pos; /**<Target position to move the selector to*/
}MenuSelector;

typedef struct MenuSounds_S
{
	Sound *button_hover;
	Sound *button_select;

	Sound *dropdown_open;
	Sound *dropdown_close;

	Sound *menu_open;
	Sound *menu_close;
}MenuSounds;
typedef struct Menu_S
{
	Uint8			_inuse; /**<Flag to check if Menu is being used*/
	Uint8			enabled; /**<Flag to check if Menu is enabled/visible*/

	char			*tag; /**<Unique tag used for searching menus from JSON*/

	MenuSounds		*sfx;

	Sound *hover_sfx;
	Sound *click_sfx;
	Sound *open_sfx;
	Sound *close_sfx;

	List			*labels; /**<A pointer to a list of MenuText*/
	List			*images; /**<A pointer to a list of MenuImage*/
	List			*buttons; /**<A pointer to a list of MenuButton*/
	List			*dropdowns; /**<A pointer to a list of MenuDropdown*/
	MenuButton		*current_button; /**<A pointer to the current MenuButton*/

	Sprite			*selector_sprite; /**<A pointer to the sprite of the selector for this menu*/
	Vector2D		selector_position; /**<Current position of the selector*/
	Vector2D		selector_target_pos; /**<Target position of the selector*/

	Sprite			*background; /**<A pointer to the sprite of the background for this menu*/

	struct Menu_S	*prev; /**<A pointer to the previous menu, null if main*/

	List			*nav_btn_ctx; /**<A pointer to the current button list in the current context*/

	void			(*update)(struct Menu_S *self); /**<A pointer to the update function*/
	void			(*draw)(struct Menu_S *self); /**<A pointer to the draw function*/
	void			(*free)(struct Menu_S *self); /**<A pointer to the free function*/
}Menu;

/**
 * @brief initialize internal menu management system
 * @param maxMenus how many concurrent menus to support
 */
void menu_manager_init( Uint32 maxMenus );

/**
 * @brief Frees all menus and closes the menu_manager
 */
void menu_manager_close();

/**
 * @brief Loads a menu from json
 * @param filename of json menu to load
 * @return NULL on error, or a pointer to a menu
 */
Menu *menu_load( char *filename );

/**
 * @brief allocate and initialize a new menu
 * @return NULL on error, or a pointer to a menu
 */
Menu *menu_new();

/**
 * @brief allocate and initialize a new menu button
 * @return NULL on error, or a pointer to a menu button
 */
MenuButton *menu_button_new();
/**
 * @brief allocate and initialize a new menu text
 * @return NULL on error, or a pointer to a menu text
 */
MenuText *menu_text_new();
/**
 * @brief allocate and initialize a new menu image
 * @return NULL on error, or a pointer to a menu image
 */
MenuImage *menu_image_new();

/**
 * @brief allocate and initialize a new menu dropdown
 * @return NULL on error, or a pointer to a menu dropdown
 */
MenuDropdown *menu_dropdown_new();


/**
 * @brief frees provided menu
 * @param self the menu to free
 */
void menu_free( Menu *self );

/**
 * @brief updates the provided menu
 * @param self the menu to update
 */
void menu_update( Menu *self );

/**
 * @brief updates the provided menu at a fixed interval
 * @param self the menu to update
 */
void menu_update_fixed( Menu *self );

/**
 * @brief draws the provided menu
 * @param self the menu to draw
 */
void menu_draw( Menu *self );

/**
 * @brief run the update function for all active menus
 */
void menu_manager_update_all();

/**
 * @brief run the update function for all active menus at a fixed rate
 */
void menu_manager_update_fixed_all();


/**
 * @brief run the draw function for all active menus
 */
void menu_manager_draw_all();


/**
 * @brief Hide the provided menu
 * @param self the menu to hide
 */
void menu_close( Menu *self );

/**
 * @brief Hide all menus
 */
void menu_manager_close_all();

/**
 * @brief Unhide the provided menu
 * @param self the menu to unhide
 */
void menu_open( Menu *self );

/**
 * @brief switch to the next menu
 * @param self the menu to hide
 * @param next the menu to unhide
 */
void menu_go_to( Menu *self, Menu *next );

/**
 * @brief switch back to the previous menu
 * @param self the menu to hide
 */
void menu_go_back( Menu *self );

/**
* @brief activate dropdown
*/
void menu_activate_dropdown( Menu *self, MenuDropdown *dropdown );

/**
* @brief deactivate dropdown
*/
void menu_deactivate_dropdown( Menu *self, MenuDropdown *dropdown );

/**
 * @brief quits the game
 */
void menu_quit( );

/**
* @brief sets game state to g_run
*/
void menu_g_state_run();

/**
* @brief sets game state to state
*/
void menu_g_state_change( Menu *self, int state );

Menu *menu_manager_get_by_tag( char *tag );
#endif // !__G_MENU_H__
