#include "m_pause.h"
#include "g_globals.h"
#include "g_editor.h"
#include "g_manager.h"

Menu *menu_pause_load()
{
    Menu *menu = menu_new();

    menu->tag = "pause";

    menu->selector_sprite = gf2d_sprite_load_image( "images/gui/selected.png" );
    menu->selector_target_pos = vector2d( 279, 317 );
    menu->selector_position = vector2d( 279, 317 );

    menu->background = gf2d_sprite_load_image( "images/gui/background.png" );
    menu->enabled = false;

    MenuText *title = menu_text_new();
    title->text = "PAUSED";
    title->font = TTF_OpenFont( "fonts/FRADMCN.ttf", 36 );
    title->position = vector2d( 323.6, 270 );
    gfc_list_append( menu->labels, title );

    MenuButton *resume_btn = menu_button_new();
    resume_btn->label.text = "RESUME";
    resume_btn->position = vector2d( 279, 317 );
    resume_btn->action = menu_close;
    gfc_list_append( menu->buttons, resume_btn );
    menu->current_button = resume_btn;

    MenuButton *restart_btn = menu_button_new();
    restart_btn->label.text = "RESTART";
    restart_btn->position = vector2d( 279, 343 );
    gfc_list_append( menu->buttons, restart_btn );

    MenuButton *settings_btn = menu_button_new();
    settings_btn->label.text = "SETTINGS";
    settings_btn->position = vector2d( 279, 369 );
    gfc_list_append( menu->buttons, settings_btn );

    MenuButton *leave_btn = menu_button_new();
    leave_btn->label.text = "LEAVE";
    leave_btn->action = game_mission_quit;
    leave_btn->position = vector2d( 279, 395 );
    gfc_list_append( menu->buttons, leave_btn );

    MenuButton *quit_btn = menu_button_new();
    quit_btn->label.text = "QUIT";
    quit_btn->action = menu_quit;
    quit_btn->position = vector2d( 279, 421 );
    gfc_list_append( menu->buttons, quit_btn );



    MenuText *title_obj = menu_text_new();
    title_obj->text = "OBJECTIVES";
    title_obj->font = TTF_OpenFont( "fonts/FRADMCN.ttf", 36 );
    title_obj->position = vector2d( 743.8, 270 );
    gfc_list_append( menu->labels, title_obj );

    MenuImage *bg_obj = menu_image_new();
    bg_obj->sprite = gf2d_sprite_load_image( "images/gui/obj_grad.png" );
    bg_obj->position = vector2d( 696, 314 );
    gfc_list_append( menu->images, bg_obj );

    MenuText *obj = menu_text_new();
    obj->text = "Kill all enemies.";
    obj->font = TTF_OpenFont( "fonts/arial.ttf", 18 );
    obj->position = vector2d( 710, 320 );
    gfc_list_append( menu->labels, obj );

    MenuText *obj2 = menu_text_new();
    obj2->text = "Locate all intel.";
    obj2->font = TTF_OpenFont( "fonts/arial.ttf", 18 );
    obj2->position = vector2d( 710, 340 );
    gfc_list_append( menu->labels, obj2 );

    MenuText *obj3 = menu_text_new();
    obj3->text = "Rescue the hostage.";
    obj3->font = TTF_OpenFont( "fonts/arial.ttf", 18 );
    obj3->position = vector2d( 710, 360 );
    gfc_list_append( menu->labels, obj3 );

    return menu;
}