#include "m_main.h"
#include "g_globals.h"
#include "g_editor.h"
#include "g_manager.h"
#include "simple_json.h"
#include "simple_logger.h"

#include <windows.h>

void main_menu_level_start( Menu *self, char* level_name );
MenuDropdown* main_load_misison_drp(Menu* self);
MenuDropdown* main_load_loadout_drp(Menu* self);

MenuDropdown *main_load_custom_mission_drp( Menu* self );

Menu *menu_main_load()
{
	Menu *menu = menu_new();

    menu->tag = "main";

    menu->selector_sprite = gf2d_sprite_load_image( "images/gui/selected.png" );
    menu->selector_target_pos = vector2d( 128, 317 );
    menu->selector_position = vector2d( 128, 317 );

   // menu->background = gf2d_sprite_load_image( "images/gui/main_bg.png" );
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
    btn_drp_lvl2->action = main_menu_level_start;
    btn_drp_lvl2->data = "levels/level_2.json";
    btn_drp_lvl2->position = vector2d(332, 343);
    gfc_list_append(drp_level_select->buttons, btn_drp_lvl2);

    MenuButton* btn_drp_lvl3 = menu_button_new();
    btn_drp_lvl3->label.text = "LEVEL 3";
    btn_drp_lvl3->action = main_menu_level_start;
    btn_drp_lvl3->data = "levels/level_3.json";
    btn_drp_lvl3->position = vector2d(332, 369);
    gfc_list_append(drp_level_select->buttons, btn_drp_lvl3);

    MenuButton *btn_custom = menu_button_new();
    btn_custom->label.text = "CUSTOM";
    btn_custom->action = menu_activate_dropdown;
    btn_custom->data = main_load_custom_mission_drp( self );
    btn_custom->position = vector2d( 332, 395 );
    gfc_list_append( drp_level_select->buttons, btn_custom );

    MenuButton* btn_drp_rtrn = menu_button_new();
    btn_drp_rtrn->label.text = "BACK";
    btn_drp_rtrn->action = menu_deactivate_dropdown;
    btn_drp_rtrn->data = drp_level_select;
    btn_drp_rtrn->position = vector2d(332, 421);
    gfc_list_append(drp_level_select->buttons, btn_drp_rtrn);

    drp_level_select->label.text = "TEST";
    gfc_list_append(self->dropdowns, drp_level_select);

    return drp_level_select;
}

// CODE partially copied from https://stackoverflow.com/a/2315808
// Specifically searching the directory for filenames
MenuDropdown *main_load_custom_mission_drp( Menu *self )
{
    const wchar_t *sDir = L"levels/custom";
    WIN32_FIND_DATA fdFile;
    HANDLE hFind = NULL;
    wchar_t sPath[2048];

    MenuDropdown *drp = menu_dropdown_new();
    drp->active = false;
    drp->label.text = "Custom";
    drp->label.position = vector2d(0, 0);

    MenuButton *btn_drp_rtrn = menu_button_new();
    btn_drp_rtrn->label.text = "BACK";
    btn_drp_rtrn->action = menu_deactivate_dropdown;
    btn_drp_rtrn->data = drp;
    btn_drp_rtrn->position = vector2d( 536, 395 );
    gfc_list_append( drp->buttons, btn_drp_rtrn );
    drp->current_button = btn_drp_rtrn;

    wsprintf( sPath, L"%s\\*.*", sDir );

    float x = 536;
    float y = 395;
    float y_offset = 26;

    if ((hFind = FindFirstFile( sPath, &fdFile )) == INVALID_HANDLE_VALUE)
    {
        slog( "Path not found");
        return;
    }

    do
    {
        if (wcscmp( fdFile.cFileName, L"." ) != 0
             && wcscmp( fdFile.cFileName, L".." ) != 0)
        {
            y += y_offset;
            char filename[32];

            snprintf( filename, sizeof(filename), "%ws", fdFile.cFileName );

            slog( filename );
            char path[64];
            snprintf( path, sizeof(path), "levels/custom/%s", filename );
            slog( path );
            MenuButton *btn = menu_button_new();
            btn->label.text = "level_7.json";
            btn->action = main_menu_level_start;
            btn->data = "levels/level_7.json"; // TODO: Figure out why path does not work here
            btn->position = vector2d( x, y );
            gfc_list_append( drp->buttons, btn );
            
        }
    }
    while (FindNextFile( hFind, &fdFile ));

    FindClose( hFind );

    gfc_list_append( self->dropdowns, drp );
    return drp;
}


MenuDropdown *main_load_loadout_drp(Menu* self)
{
    //if (!self) return;

    SJson* json;

    char* primary_weapon_txt;
    char* secondary_weapon_txt;
    char* equipment_txt;


    sj_enable_debug();
    json = sj_load("config/characters/companion_1.json");

    primary_weapon_txt = sj_get_string_value(sj_object_get_value(json, "primary_weapon"));
    secondary_weapon_txt = sj_get_string_value(sj_object_get_value(json, "secondary_weapon"));
    equipment_txt = sj_get_string_value(sj_object_get_value(json, "equipment"));
    
    MenuDropdown* drop = menu_dropdown_new();
    drop->active = false;
    drop->label.position = vector2d(0, 128);

    MenuButton* primary_weapon_btn = menu_button_new();
    primary_weapon_btn->label.text = primary_weapon_txt;
    primary_weapon_btn->action = main_menu_level_start;
    primary_weapon_btn->position = vector2d(332, 343);
    gfc_list_append(drop->buttons, primary_weapon_btn);
    drop->current_button = primary_weapon_btn;

    MenuButton* secondary_weapon_btn = menu_button_new();
    secondary_weapon_btn->label.text = secondary_weapon_txt;
    secondary_weapon_btn->position = vector2d(332, 369);
    gfc_list_append(drop->buttons, secondary_weapon_btn);

    MenuButton* equipment_btn = menu_button_new();
    equipment_btn->label.text = equipment_txt;
    equipment_btn->position = vector2d(332, 395);
    gfc_list_append(drop->buttons, equipment_btn);

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

