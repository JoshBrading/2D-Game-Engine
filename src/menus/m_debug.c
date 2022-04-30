#include "m_debug.h"
#include "g_globals.h"
#include "g_editor.h"
#include "g_manager.h"
#include "g_light.h"
#include "g_entity.h"
#include "g_collision.h"
#include "g_globals.h"

Menu* menu_debug_load()
{
    Menu* menu = menu_new();

    menu->tag = "debug";

    menu->selector_sprite = gf2d_sprite_load_image("images/gui/selected.png");
    menu->selector_target_pos = vector2d(16, 317);
    menu->selector_position = vector2d(16, 317);

    menu->background = gf2d_sprite_load_image("images/gui/background.png");
    menu->enabled = false;

    MenuText* title = menu_text_new();
    title->text = "DEBUG";
    title->font = TTF_OpenFont("fonts/FRADMCN.ttf", 36);
    title->position = vector2d(16, 12);
    gfc_list_append(menu->labels, title);

    MenuButton *resume_btn = menu_button_new();
    resume_btn->label.text = "RESUME";
    resume_btn->position = vector2d( 16, 50 );
    resume_btn->action = menu_close;
    gfc_list_append( menu->buttons, resume_btn );
    menu->current_button = resume_btn;

    ////////////////////////////////////////////////////////////////////

    MenuText* light_txt = menu_text_new();
    light_txt->font = TTF_OpenFont("fonts/FRADMCN.ttf", 16);
    light_txt->position = vector2d(16, 75);
    light_txt->text = "Lighting";
    gfc_list_append(menu->labels, light_txt);

    MenuButton* light_enable_btn = menu_button_new();
    light_enable_btn->label.text = "ENABLE LIGHT";
    light_enable_btn->action = light_data_enable_debug;
    light_enable_btn->position = vector2d(16, 100);
    gfc_list_append(menu->buttons, light_enable_btn);

    MenuButton* light_disable_btn = menu_button_new();
    light_disable_btn->label.text = "DISABLE LIGHT";
    light_disable_btn->action = light_data_disable_debug;
    light_disable_btn->position = vector2d(16, 125);
    gfc_list_append(menu->buttons, light_disable_btn);

    ////////////////////////////////////////////////////////////////////

    MenuText* entity_txt = menu_text_new();
    entity_txt->font = TTF_OpenFont("fonts/FRADMCN.ttf", 16);
    entity_txt->position = vector2d(16, 150);
    entity_txt->text = "Entity";
    gfc_list_append(menu->labels, entity_txt);

    MenuButton* ent_enable_box = menu_button_new();
    ent_enable_box->label.text = "ENABLE";
    ent_enable_box->action = entity_manager_enable_debug;
    ent_enable_box->position = vector2d(16, 175);
    gfc_list_append(menu->buttons, ent_enable_box);

    MenuButton* ent_disable_box = menu_button_new();
    ent_disable_box->label.text = "DISABLE";
    ent_disable_box->action = entity_manager_disable_debug;
    ent_disable_box->position = vector2d(16, 200);
    gfc_list_append(menu->buttons, ent_disable_box);

    ////////////////////////////////////////////////////////////////////

    MenuText* collision_txt = menu_text_new();
    collision_txt->font = TTF_OpenFont("fonts/FRADMCN.ttf", 16);
    collision_txt->position = vector2d(16, 225);
    collision_txt->text = "Collision";
    gfc_list_append(menu->labels, collision_txt);

    MenuButton* col_enable_btn = menu_button_new();
    col_enable_btn->label.text = "ENABLE";
    col_enable_btn->action = collision_system_enable_debug;
    col_enable_btn->position = vector2d(16, 250);
    gfc_list_append(menu->buttons, col_enable_btn);

    MenuButton* col_disable_btn = menu_button_new();
    col_disable_btn->label.text = "DISABLE";
    col_disable_btn->action = collision_system_disable_debug;
    col_disable_btn->position = vector2d(16, 275);
    gfc_list_append(menu->buttons, col_disable_btn);

    ////////////////////////////////////////////////////////////////////

    MenuText* timing_txt = menu_text_new();
    timing_txt->font = TTF_OpenFont("fonts/FRADMCN.ttf", 16);
    timing_txt->position = vector2d(16, 300);
    timing_txt->text = "Draw Timings";
    gfc_list_append(menu->labels, timing_txt);

    MenuButton* timing_enable_btn = menu_button_new();
    timing_enable_btn->label.text = "ENABLE";
    timing_enable_btn->action = game_timing_enable_debug;
    timing_enable_btn->position = vector2d(16, 325);
    gfc_list_append(menu->buttons, timing_enable_btn);

    MenuButton* timing_disable_btn = menu_button_new();
    timing_disable_btn->label.text = "DISABLE";
    timing_disable_btn->action = game_timing_disable_debug;
    timing_disable_btn->position = vector2d(16, 350);
    gfc_list_append(menu->buttons, timing_disable_btn);


    return menu;
}