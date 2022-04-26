#include "m_main.h"
#include "g_globals.h"
#include "g_editor.h"
#include "g_manager.h"

void main_menu_level_start( Menu *self, char* level_name );
MenuDropdown* main_load_misison_drp(Menu* self);
MenuDropdown* main_load_loadout_drp(Menu* self);

Menu *menu_main_load()
{
	Menu *menu = menu_new();

    menu->tag = "main";

    menu->selector_sprite = gf2d_sprite_load_image( "images/gui/selected.png" );
    menu->selector_target_pos = vector2d( 128, 317 );
    menu->selector_position = vector2d( 128, 317 );

    menu->background = gf2d_sprite_load_image( "images/gui/main_bg.png" );
	menu->enabled = true;

	MenuText *title = menu_text_new();
	title->text = "MAIN MENU";
	title->font = TTF_OpenFont( "fonts/FRADMCN.ttf", 36 );
	title->position = vector2d( 128, 264 );
	gfc_list_append( menu->labels, title );

    MenuImage *gradi = menu_image_new();
    gradi->position = vector2d( 0, 0 );
    gradi->sprite = gf2d_sprite_load_image( "images/gui/background.png" );
    gfc_list_append( menu->images, gradi );

    MenuButton *btn_game_select = menu_button_new();
    btn_game_select->selected = true;
    btn_game_select->action = menu_activate_dropdown;
    btn_game_select->data = main_load_misison_drp(menu);
    btn_game_select->label.text = "MISSION SELECT";
    btn_game_select->position = vector2d( 128, 317 );
    gfc_list_append( menu->buttons, btn_game_select );
    menu->current_button = btn_game_select;

    MenuButton *btn_loadout = menu_button_new();
    btn_loadout->label.text = "LOADOUT";
    btn_loadout->action = menu_activate_dropdown;
    btn_loadout->data = main_load_loadout_drp(menu);
    btn_loadout->position = vector2d( 128, 343 );
    gfc_list_append( menu->buttons, btn_loadout );

    MenuButton *btn_edit = menu_button_new();
    btn_edit->label.text = "EDITOR";
    btn_edit->action = game_load_editor;
    btn_edit->position = vector2d( 128, 369 );
    gfc_list_append( menu->buttons, btn_edit );

    MenuButton *btn_quit = menu_button_new();
    btn_quit->action = menu_quit;
    btn_quit->label.text = "QUIT";
    btn_quit->position = vector2d( 128, 395 );
    gfc_list_append( menu->buttons, btn_quit );

	return menu;
}

void main_menu_level_start( Menu* self, char* level_name )
{
    self->enabled = false;
    menu_g_state_change( self, G_RUN);

    game_load_mission( level_name );
}

MenuDropdown *main_load_misison_drp(Menu* self)
{

    MenuDropdown* drp_level_select = menu_dropdown_new();
    drp_level_select->active = false;
    drp_level_select->label.position = vector2d(0, 128);

    MenuButton* btn_drp_lvl1 = menu_button_new();
    btn_drp_lvl1->label.text = "LEVEL 1";
    btn_drp_lvl1->action = main_menu_level_start;
    btn_drp_lvl1->data = "config/asset_list.json";
    btn_drp_lvl1->position = vector2d(332, 317);
    gfc_list_append(drp_level_select->buttons, btn_drp_lvl1);
    drp_level_select->current_button = btn_drp_lvl1;

    MenuButton* btn_drp_lvl2 = menu_button_new();
    btn_drp_lvl2->label.text = "LEVEL 2";
    btn_drp_lvl2->position = vector2d(332, 343);
    gfc_list_append(drp_level_select->buttons, btn_drp_lvl2);

    MenuButton* btn_drp_lvl3 = menu_button_new();
    btn_drp_lvl3->label.text = "LEVEL 3";
    btn_drp_lvl3->position = vector2d(332, 369);
    gfc_list_append(drp_level_select->buttons, btn_drp_lvl3);

    MenuButton* btn_drp_rtrn = menu_button_new();
    btn_drp_rtrn->label.text = "BACK";
    btn_drp_rtrn->action = menu_deactivate_dropdown;
    btn_drp_rtrn->data = drp_level_select;
    btn_drp_rtrn->position = vector2d(332, 395);
    gfc_list_append(drp_level_select->buttons, btn_drp_rtrn);

    drp_level_select->label.text = "TEST";
    gfc_list_append(self->dropdowns, drp_level_select);

    return drp_level_select;
}

MenuDropdown *main_load_loadout_drp(Menu* self)
{

    MenuDropdown* drop = menu_dropdown_new();
    drop->active = false;
    drop->label.position = vector2d(0, 128);

    MenuButton* btn_drp_lvl1 = menu_button_new();
    btn_drp_lvl1->label.text = "RIFLE";
    btn_drp_lvl1->action = main_menu_level_start;
    btn_drp_lvl1->position = vector2d(332, 343);
    gfc_list_append(drop->buttons, btn_drp_lvl1);
    drop->current_button = btn_drp_lvl1;

    MenuButton* btn_drp_lvl2 = menu_button_new();
    btn_drp_lvl2->label.text = "PISTOL";
    btn_drp_lvl2->position = vector2d(332, 369);
    gfc_list_append(drop->buttons, btn_drp_lvl2);

    MenuButton* btn_drp_lvl3 = menu_button_new();
    btn_drp_lvl3->label.text = "FLASHLIGHT";
    btn_drp_lvl3->position = vector2d(332, 395);
    gfc_list_append(drop->buttons, btn_drp_lvl3);

    MenuButton* btn_drp_rtrn = menu_button_new();
    btn_drp_rtrn->label.text = "CONFIRM";
    btn_drp_rtrn->action = menu_deactivate_dropdown;
    btn_drp_rtrn->data = drop;
    btn_drp_rtrn->position = vector2d(332, 421);
    gfc_list_append(drop->buttons, btn_drp_rtrn);

    drop->label.text = "TEST";
    gfc_list_append(self->dropdowns, drop);

    return drop;
}

