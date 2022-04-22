#include "g_editor.h"
#include "g_static_entity.h"
#include "gf2d_sprite.h"
#include <SDL_ttf.h>
#include "simple_logger.h"
#include "gfc_list.h"
#include "g_globals.h"

Menu* editor_load()
{
	entity_manager_clear();
    Menu *menu = menu_new();

    menu_g_state_change( menu, G_EDIT );

    menu->tag = "editor";

    menu->selector_sprite = gf2d_sprite_load_image( "images/gui/selected.png" );
    menu->selector_target_pos = vector2d( 128, 317 );
    menu->selector_position = vector2d( 128, 317 );

  //  menu->background = gf2d_sprite_load_image( "images/gui/main_bg.png" );
    menu->enabled = true;

    MenuText *title = menu_text_new();
    title->text = "EDITOR";
    title->font = TTF_OpenFont( "fonts/FRADMCN.ttf", 36 );
    title->position = vector2d( 32, 16 );
    gfc_list_append( menu->labels, title );

    MenuImage *gradi = menu_image_new();
    gradi->position = vector2d( 0, 0 );
    gradi->sprite = gf2d_sprite_load_image( "images/gui/background.png" );
    gfc_list_append( menu->images, gradi );
    MenuDropdown *drp_create_entity = menu_dropdown_new();

    MenuDropdown *drp_create = menu_dropdown_new();
    drp_create->active = false;
    drp_create->label.position = vector2d( 0, 128 );

    MenuButton *btn_create_ent = menu_button_new();
    btn_create_ent->label.text = "ENTITIES";
    btn_create_ent->position = vector2d( 232, 64 );
    gfc_list_append( drp_create->buttons, btn_create_ent );
    drp_create->current_button = btn_create_ent;

    MenuButton *btn_create_sEnt = menu_button_new();
    btn_create_sEnt->label.text = "S_ENTITIES";
    btn_create_sEnt->action = menu_activate_dropdown;
    btn_create_sEnt->data = drp_create_entity;
    btn_create_sEnt->position = vector2d( 232, 90 );
    gfc_list_append( drp_create->buttons, btn_create_sEnt );

    MenuButton *btn_create_obj = menu_button_new();
    btn_create_obj->label.text = "OBJECTIVES";
    btn_create_obj->position = vector2d( 232, 116 );
    gfc_list_append( drp_create->buttons, btn_create_obj );

    MenuButton *btn_create_rtrn = menu_button_new();
    btn_create_rtrn->label.text = "BACK";
    btn_create_rtrn->action = menu_deactivate_dropdown;
    btn_create_rtrn->data = drp_create;
    btn_create_rtrn->position = vector2d( 232, 142 );
    gfc_list_append( drp_create->buttons, btn_create_rtrn );

    drp_create->label.text = "TEST";
    gfc_list_append( menu->dropdowns, drp_create );

    drp_create_entity->active = false;
    drp_create_entity->label.position = vector2d( 0, 128 );

    MenuButton *btn_create_ent_ex = menu_button_new();
    btn_create_ent_ex->label.text = "WALL_100x20";
    btn_create_ent_ex->position = vector2d( 442, 90 );
    gfc_list_append( drp_create_entity->buttons, btn_create_ent_ex );
    drp_create_entity->current_button = btn_create_ent_ex;

    MenuButton *btn_create_sEnt_ex = menu_button_new();
    btn_create_sEnt_ex->label.text = "WALL_100x20 (Destructible)";
    btn_create_sEnt_ex->position = vector2d( 442, 116 );
    gfc_list_append( drp_create_entity->buttons, btn_create_sEnt_ex );

    MenuButton *btn_create_obj_ex = menu_button_new();
    btn_create_obj_ex->label.text = "WALL_20x100_90D";
    btn_create_obj_ex->position = vector2d( 442, 142 );
    gfc_list_append( drp_create_entity->buttons, btn_create_obj_ex );

    MenuButton *btn_create_rtrn_ex = menu_button_new();
    btn_create_rtrn_ex->label.text = "BACK";
    btn_create_rtrn_ex->action = menu_deactivate_dropdown;
    btn_create_rtrn_ex->data = drp_create_entity;
    btn_create_rtrn_ex->position = vector2d( 442, 168 );
    gfc_list_append( drp_create_entity->buttons, btn_create_rtrn_ex );

    drp_create_entity->label.text = "TEST";
    gfc_list_append( menu->dropdowns, drp_create_entity );

    MenuButton *btn_game_select = menu_button_new();
    btn_game_select->selected = true;
    btn_game_select->action = menu_activate_dropdown;
    btn_game_select->data = drp_create;
    btn_game_select->label.text = "CREATE";
    btn_game_select->position = vector2d( 16, 64 );
    gfc_list_append( menu->buttons, btn_game_select );
    menu->current_button = btn_game_select;

    MenuButton *btn_edit = menu_button_new();
    btn_edit->label.text = "MODIFY";
    btn_edit->position = vector2d( 16, 90 );
    gfc_list_append( menu->buttons, btn_edit );

    MenuButton *btn_settings = menu_button_new();
    btn_settings->label.text = "RESET";
    btn_settings->position = vector2d( 16, 116 );
    gfc_list_append( menu->buttons, btn_settings );

    MenuButton *btn_quit = menu_button_new();
    btn_quit->action = menu_quit;
    btn_quit->label.text = "QUIT";
    btn_quit->position = vector2d( 16, 142 );
    gfc_list_append( menu->buttons, btn_quit );

}

void editor_instantiate_entity( Menu* self, void* data )
{
       
}