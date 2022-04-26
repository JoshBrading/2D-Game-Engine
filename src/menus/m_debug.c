#include "m_debug.h"
#include "g_globals.h"
#include "g_editor.h"
#include "g_manager.h"
#include "g_light.h"
#include "g_entity.h"

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
    
    MenuText* light_txt = menu_text_new();
    light_txt->font = TTF_OpenFont("fonts/FRADMCN.ttf", 16);
    light_txt->position = vector2d(16, 75);
    light_txt->text = "Lighting";
    gfc_list_append(menu->labels, light_txt);

    MenuButton* resume_btn = menu_button_new();
    resume_btn->label.text = "RESUME";
    resume_btn->position = vector2d(16, 50);
    resume_btn->action = menu_close;
    gfc_list_append(menu->buttons, resume_btn);
    menu->current_button = resume_btn;

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



    MenuText* title_obj = menu_text_new();
    title_obj->text = "OBJECTIVES";
    title_obj->font = TTF_OpenFont("fonts/FRADMCN.ttf", 36);
    title_obj->position = vector2d(743.8, 270);
    gfc_list_append(menu->labels, title_obj);

    MenuImage* bg_obj = menu_image_new();
    bg_obj->sprite = gf2d_sprite_load_image("images/gui/obj_grad.png");
    bg_obj->position = vector2d(696, 314);
    gfc_list_append(menu->images, bg_obj);

    MenuText* obj = menu_text_new();
    obj->text = "Kill all enemies.";
    obj->font = TTF_OpenFont("fonts/arial.ttf", 18);
    obj->position = vector2d(710, 320);
    gfc_list_append(menu->labels, obj);

    MenuText* obj2 = menu_text_new();
    obj2->text = "Locate all intel.";
    obj2->font = TTF_OpenFont("fonts/arial.ttf", 18);
    obj2->position = vector2d(710, 340);
    gfc_list_append(menu->labels, obj2);

    MenuText* obj3 = menu_text_new();
    obj3->text = "Rescue the hostage.";
    obj3->font = TTF_OpenFont("fonts/arial.ttf", 18);
    obj3->position = vector2d(710, 360);
    gfc_list_append(menu->labels, obj3);

    return menu;
}