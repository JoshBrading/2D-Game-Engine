#include "g_editor.h"
#include "g_static_entity.h"
#include "gf2d_sprite.h"
#include <SDL_ttf.h>
#include "simple_logger.h"
#include "simple_json.h"
#include "gfc_list.h"
#include "g_globals.h"
#include "gf2d_draw.h"

#include "g_manager.h"

#include "g_entity.h"
#include "g_static_entity.h"

void editor_populate_list( MenuDropdown *drop, SJson *list, Vector2D position, float padding );
void editor_instantiate( Menu *self, SJson *json );
Uint8 editor_check_for_selection( Vector2D p, SDL_Rect r );
Entity *editor_instantiate_entity( SJson *json, Vector2D position );
StaticEntity *editor_instantiate_static_entity( SJson *json, Vector2D position );
void editor_update_selection( int mx, int my, SDL_Event e, const Uint8 *keys );
void editor_select( int mx, int my, SDL_Event e );
void editor_deselect();
void editor_mouse_cooldown_reset();
void editor_save();


static Entity *edit_ent;
static StaticEntity *edit_sEnt;
static Menu *menu;
static SJson *entity_list, *static_entity_list, *interactable_list;
Uint32 interact_cooldown = 100;
Uint32 cooldown_timer = 0;

Menu* editor_load()
{
	entity_manager_clear();
    menu = menu_new();

    menu_g_state_change( menu, G_EDIT );

 //   menu->think = editor_menu_think;

    menu->tag = "editor";

    menu->selector_sprite = gf2d_sprite_load_image( "images/gui/selected.png" );
    menu->selector_target_pos = vector2d( 128, 317 );
    menu->selector_position = vector2d( 128, 317 );

   // menu->background = gf2d_sprite_load_image( "images/gui/main_bg.png" );
    menu->enabled = false;

    MenuText *title = menu_text_new();
    title->text = "EDITOR";
    title->font = TTF_OpenFont( "fonts/FRADMCN.ttf", 36 );
    title->position = vector2d( 32, 16 );
    gfc_list_append( menu->labels, title );


    //==== DRP_CREATE_ENTITY ===========================
    MenuDropdown *drp_create_entity = menu_dropdown_new();
    drp_create_entity->active = false;
    drp_create_entity->label.text = "drp_create_entity_back";

    MenuButton *drp_create_entity_back = menu_button_new();
    drp_create_entity_back->label.text = "BACK";
    drp_create_entity_back->action = menu_deactivate_dropdown;
    drp_create_entity_back->data = drp_create_entity;
    drp_create_entity_back->position = vector2d( 442, 64 );
    gfc_list_append( drp_create_entity->buttons, drp_create_entity_back );
    drp_create_entity->current_button = drp_create_entity_back;


    //==== DRP_CREATE_STATIC_ENTITY ===========================
    MenuDropdown *drp_create_static_entity = menu_dropdown_new();
    drp_create_static_entity->label.text = "drp_create_static_entity";
    drp_create_static_entity->active = false;

    MenuButton *drp_create_static_entity_back = menu_button_new();
    drp_create_static_entity_back->label.text = "BACK";
    drp_create_static_entity_back->action = menu_deactivate_dropdown;
    drp_create_static_entity_back->data = drp_create_static_entity;
    drp_create_static_entity_back->position = vector2d( 442, 90 );
    gfc_list_append( drp_create_static_entity->buttons, drp_create_static_entity_back );
    drp_create_static_entity->current_button = drp_create_static_entity_back;

    //==== DRP_CREATE_INTERACTABLE ===========================
    MenuDropdown *drp_create_interactable = menu_dropdown_new();
    drp_create_interactable->label.text = "drp_create_interactable";
    drp_create_interactable->active = false;

    MenuButton *drp_create_interactable_back = menu_button_new();
    drp_create_interactable_back->label.text = "BACK";
    drp_create_interactable_back->action = menu_deactivate_dropdown;
    drp_create_interactable_back->data = drp_create_interactable;
    drp_create_interactable_back->position = vector2d( 442, 116 );
    gfc_list_append( drp_create_interactable->buttons, drp_create_interactable_back );
    drp_create_interactable->current_button = drp_create_interactable_back;



    //==== DRP_CREATE ===========================
    MenuDropdown *drp_create = menu_dropdown_new();
    drp_create->active = false;
    drp_create->label.position = vector2d( 0, 128 );

    MenuButton *btn_create_ent = menu_button_new();
    btn_create_ent->label.text = "ENTITIES";
    btn_create_ent->action = menu_activate_dropdown;
    btn_create_ent->data = drp_create_entity;
    btn_create_ent->position = vector2d( 232, 64 );
    gfc_list_append( drp_create->buttons, btn_create_ent );
    drp_create->current_button = btn_create_ent;

    MenuButton *btn_create_sEnt = menu_button_new();
    btn_create_sEnt->label.text = "S_ENTITIES";
    btn_create_sEnt->action = menu_activate_dropdown;
    btn_create_sEnt->data = drp_create_static_entity;
    btn_create_sEnt->position = vector2d( 232, 90 );
    gfc_list_append( drp_create->buttons, btn_create_sEnt );

    MenuButton *btn_create_interactable = menu_button_new();
    btn_create_interactable->label.text = "INTERACTABLES";
    btn_create_interactable->action = menu_activate_dropdown;
    btn_create_interactable->data = drp_create_interactable;
    btn_create_interactable->position = vector2d( 232, 116 );
    gfc_list_append( drp_create->buttons, btn_create_interactable );

    MenuButton *btn_create_rtrn = menu_button_new();
    btn_create_rtrn->label.text = "BACK";
    btn_create_rtrn->action = menu_deactivate_dropdown;
    btn_create_rtrn->data = drp_create;
    btn_create_rtrn->position = vector2d( 232, 142 );
    gfc_list_append( drp_create->buttons, btn_create_rtrn );

    drp_create->label.text = "CREATE";
    gfc_list_append( menu->dropdowns, drp_create );



    //==== DRP_CREATE_LINK ===========================
    gfc_list_append( menu->dropdowns, drp_create_static_entity );
    gfc_list_append( menu->dropdowns, drp_create_entity );
    gfc_list_append( menu->dropdowns, drp_create_interactable );
    //==== DRP_CREATE_END ===========================


    MenuButton *btn_create = menu_button_new();
    btn_create->selected = true;
    btn_create->action = menu_activate_dropdown;
    btn_create->data = drp_create;
    btn_create->label.text = "CREATE";
    btn_create->position = vector2d( 16, 64 );
    gfc_list_append( menu->buttons, btn_create );
    menu->current_button = btn_create;

    MenuButton *btn_edit = menu_button_new();
    btn_edit->label.text = "MODIFY";
    btn_edit->position = vector2d( 16, 90 );
    gfc_list_append( menu->buttons, btn_edit );

    MenuButton *btn_save = menu_button_new();
    btn_save->label.text = "SAVE";
    btn_save->action = editor_save;
    btn_save->position = vector2d( 16, 116 );
    gfc_list_append( menu->buttons, btn_save );

    MenuButton *btn_load = menu_button_new();
    btn_load->label.text = "LOAD";
    btn_load->position = vector2d( 16, 142 );
    gfc_list_append( menu->buttons, btn_load );

    MenuButton *btn_leave = menu_button_new();
    btn_leave->label.text = "LEAVE";
    btn_leave->action = game_mission_quit;
    btn_leave->position = vector2d( 16, 168 );
    gfc_list_append( menu->buttons, btn_leave );

    MenuButton *btn_instantiate = menu_button_new();
    btn_instantiate->action = menu_quit;
    btn_instantiate->label.text = "QUIT";
    btn_instantiate->position = vector2d( 16, 194 );
    gfc_list_append( menu->buttons, btn_instantiate );



    // TODO: MOVE TO OWN FUNCTION ===========================
    SJson *json;
    char *filename = "editor/asset_list.json";
    json = sj_load( filename );

    if (!json)
    {
        slog( "Failed to load json file (%s)", filename );
        return NULL;
    }

    entity_list = sj_object_get_value( json, "entities" );
    static_entity_list = sj_object_get_value( json, "scenery" );
    interactable_list = sj_object_get_value( json, "interactables" );

    if (!entity_list)
        slog( "Failed to find entities in (%s)", filename );
    else
        editor_populate_list( drp_create_entity, entity_list, vector2d( 442, 90 ), 26 );

    if (!static_entity_list)
        slog( "Failed to find scenery in (%s)", filename );
    else
        editor_populate_list( drp_create_static_entity, static_entity_list, vector2d( 442, 116 ), 26 );

    if (!interactable_list)
        slog( "Failed to find interactables in (%s)", filename );
    else
        editor_populate_list( drp_create_interactable, interactable_list, vector2d( 442, 142 ), 26 );

}

void editor_save()
{
    SJson *json, *asset_list;

    asset_list = sj_array_new();

    StaticEntityManager *sem = static_entity_manager_get();
    for (int i = 0; i < sem->static_entity_count; i++)
    {
        if (!sem->static_entity_list[i]._inuse)
        {
            continue;
        }

       //     "name":"Wall 25x100",
       //     "type" : "static",
       //     "isStatic" : true,
       //     "sprite" : "images/scenery/wall_25x100.png",
       //     "collisionType" : "box",
       //     "height" : null,
       //     "width" : null,
       //     "position_x" : 104,
       //     "position_y" : 0

        SJson *asset, *name, *type, *isStatic, *sprite, *collisionType, *height, *width, *position_x, *position_y;

        name = sj_new_str( "" );
        type = sj_new_str( "static" );
        isStatic = sj_new_bool( true );
        sprite = sj_new_str( sem->static_entity_list[i].sprite->filepath );
        collisionType = sj_new_str( "box" );
        height = sj_null_new();
        width = sj_null_new();
        position_x = sj_new_float( sem->static_entity_list[i].position.x );
        position_y = sj_new_float( sem->static_entity_list[i].position.y );

        asset = sj_object_new();

        sj_object_insert( asset, "name", name );
        sj_object_insert( asset, "type", type );
        sj_object_insert( asset, "isStatic", isStatic );
        sj_object_insert( asset, "sprite", sprite );
        sj_object_insert( asset, "collisionType", collisionType );
        sj_object_insert( asset, "height", height );
        sj_object_insert( asset, "width", width );
        sj_object_insert( asset, "position_x", position_x );
        sj_object_insert( asset, "position_y", position_y );
    
        sj_array_append( asset_list, asset );
        
    }
    
    EntityManager *em = entity_manager_get();
    for (int i = 999; i < em->entity_count; i++)
    {
        if (!em->entity_list[i]._inuse)
        {
            continue;
        }

       //     "name":"Wall 25x100",
       //     "type" : "static",
       //     "isStatic" : true,
       //     "sprite" : "images/scenery/wall_25x100.png",
       //     "collisionType" : "box",
       //     "height" : null,
       //     "width" : null,
       //     "position_x" : 104,
       //     "position_y" : 0

        SJson *asset, *name, *type, *isStatic, *sprite, *collisionType, *health, *speed, *range, *height, *width, *position_x, *position_y, *offx, *offy, *scale_x, *scale_y, *interact_x, *interact_y, *frame_w, *frame_h, *frames_per_line;

        name = sj_new_str( "" );
        type = sj_new_str( "static" );
        isStatic = sj_new_bool( true );
        sprite = sj_new_str( em->entity_list[i].sprite->filepath );
        collisionType = sj_new_str( "box" );
        height = sj_null_new();
        width = sj_null_new();
        position_x = sj_new_float( em->entity_list[i].position.x );
        position_y = sj_new_float( em->entity_list[i].position.y );

        health = sj_new_float( em->entity_list[i].health );
        speed = sj_new_float( em->entity_list[i].speed.x );
        range = sj_new_float( em->entity_list[i].interact_radius );
        interact_x = sj_new_float( em->entity_list[i].interact_offset.x );
        interact_y = sj_new_float( em->entity_list[i].interact_offset.y );
        scale_x = sj_new_float( em->entity_list[i].scale.x );
        scale_y = sj_new_float( em->entity_list[i].scale.y );
        frame_w = sj_new_int( em->entity_list[i].sprite->frame_w );
        frame_h = sj_new_int( em->entity_list[i].sprite->frame_h );
        frames_per_line = sj_new_int( em->entity_list[i].sprite->frames_per_line );

        offx = sj_new_float( em->entity_list[i].offset.x );
        offy = sj_new_float( em->entity_list[i].offset.y );

        asset = sj_object_new();


        sj_object_insert( asset, "name", name );
        sj_object_insert( asset, "type", type );
        sj_object_insert( asset, "isStatic", isStatic );
        sj_object_insert( asset, "sprite", sprite );
        sj_object_insert( asset, "collisionType", collisionType );
        sj_object_insert( asset, "height", height );
        sj_object_insert( asset, "width", width );
        sj_object_insert( asset, "position_x", position_x );
        sj_object_insert( asset, "position_y", position_y );
        sj_object_insert( asset, "health", health );
        sj_object_insert( asset, "speed", speed );
        sj_object_insert( asset, "range", range );
        sj_object_insert( asset, "hit_off_x", offx );
        sj_object_insert( asset, "hit_off_y", offy );
        sj_object_insert( asset, "scale_x", scale_x );
        sj_object_insert( asset, "scale_y", scale_y );
        sj_object_insert( asset, "interact_x", interact_x );
        sj_object_insert( asset, "interact_y", interact_y );
        sj_object_insert( asset, "frame_w", frame_w );
        sj_object_insert( asset, "frame_h", frame_h );
        sj_object_insert( asset, "frames_per_line", frames_per_line );

    
        sj_array_append( asset_list, asset );
        
    }

    json = sj_object_new();

    sj_object_insert( json, "asset_list", asset_list );
    int n = rand() % 100;

    char lvl_name[64];

    snprintf( lvl_name, sizeof( lvl_name ), "levels/custom/level_%i.json", n );

    sj_save( json, "levels/custom/level_7.json" );

   // sj_free( json );
}

void editor_populate_create( Menu *menu, SJson asset_list, Vector2D position, float padding )
{
    // Simplify in the future to allow new object types from json
}

void editor_populate_list( MenuDropdown *drop, SJson* list, Vector2D position, float padding )
{
    if (!drop) return;
    if (!list) return;

    Uint32 asset_count = sj_array_get_count( list );

   // sj_echo( list );

    for (int i = 0; i < asset_count; i++)
    {
        SJson *data;
        char *name;

        data = sj_array_get_nth( list, i );
        name = sj_get_string_value( sj_object_get_value( data, "name" ) );

        MenuButton *btn_instantiate = menu_button_new();
        btn_instantiate->action = menu_quit;
        btn_instantiate->label.text = name;
        btn_instantiate->action = editor_instantiate;
        btn_instantiate->data = data;
        btn_instantiate->position = vector2d( position.x, position.y + ( padding * i ) );
        gfc_list_append( drop->buttons, btn_instantiate );
    }
}

void editor_instantiate( Menu* self, SJson* json )
{
    Vector2D position = self->floating_menu_offset;
    short is_static;

    sj_get_bool_value( sj_object_get_value( json, "isStatic" ), &is_static );

    if (is_static)
    {
        editor_instantiate_static_entity( json, position );
    }
    else
    {
        editor_instantiate_entity( json, position );
    }

    cooldown_timer = g_time + interact_cooldown;
}

Entity *editor_instantiate_entity( SJson *json, Vector2D position )
{
    Entity *ent;
    ent = entity_new();
    
    char *tag = sj_get_string_value( sj_object_get_value( json, "type" ) );
    char *filename = sj_get_string_value( sj_object_get_value( json, "sprite" ) );
    ent->position = position;
    ent->sprite = gf2d_sprite_load_image(filename);

    if (strcmp( tag, "spawn_point" ) == 0)
    {
        ent->tag == "spawn_point";

        ent->scale = vector2d( 0.125, 0.125 );

        menu_close( menu );
        edit_ent = ent;
        return ent;
    }




    float health, speed, range, offx, offy, height, width, scale_x, scale_y, interact_x, interact_y, interact_radius;
    int frame_h, frame_w, frames_per_line;

    sj_get_float_value( sj_object_get_value( json, "health" ), &health );
    sj_get_float_value( sj_object_get_value( json, "speed" ), &speed );
    sj_get_float_value( sj_object_get_value( json, "range" ), &range );
    sj_get_float_value( sj_object_get_value( json, "hit_off_x" ), &offx );
    sj_get_float_value( sj_object_get_value( json, "hit_off_y" ), &offy );
    sj_get_float_value( sj_object_get_value( json, "scale_y" ), &scale_y );
    sj_get_float_value( sj_object_get_value( json, "scale_x" ), &scale_x );
    sj_get_float_value( sj_object_get_value( json, "height" ), &height );
    sj_get_float_value( sj_object_get_value( json, "width" ), &width );
    sj_get_float_value( sj_object_get_value( json, "interact_x" ), &interact_x );
    sj_get_float_value( sj_object_get_value( json, "interact_y" ), &interact_y );

    sj_get_integer_value( sj_object_get_value( json, "frame_w" ), &frame_w );
    sj_get_integer_value( sj_object_get_value( json, "frame_h" ), &frame_h );
    sj_get_integer_value( sj_object_get_value( json, "frames_per_line" ), &frames_per_line );

    if (strcmp( tag, "door" ) == 0)
    {
        ent->sprite = gf2d_sprite_load_all( filename, frame_w, frame_h, frames_per_line );
    }

    ent->health = health;
    ent->position = position;
    ent->scale = vector2d( scale_x, scale_y );
    ent->view_range = range;
    ent->interact_offset = vector2d( interact_x, interact_y );
    ent->interact_radius = range;

    
    if (width == -1 || height == -1)
    {
        ent->bounds.w = ent->sprite->frame_w;
        ent->bounds.h = ent->sprite->frame_h;
        ent->bounds.x = ent->position.x;
        ent->bounds.y = ent->position.y;
    }
    else
    {
        ent->bounds.w = width;
        ent->bounds.h = height;
        ent->bounds.x = ent->position.x + offx;
        ent->bounds.y = ent->position.y + offy;
    }



    menu_close( menu );
    edit_ent = ent;

    return ent;

   //     "name": "Spawn Point",-
   //     "type" : "wall",
   //     "isStatic" : false,
   //     "sprite" : "images/player.png",
   //     "collisionType" : "box",
   //     "hit_off_x" : 0,
   //     "hit_off_y" : 0,
   //     "height" : -1,
   //     "width" : -1,
   //     "speed" : 50,
   //     "range" : 75,
   //     "health" : 100,
   //     "frame_h" : -1,
   //     "frame_w" : -1,
   //     "frames_per_line" : 0,
   //     "position_x" : 608,
   //     "position_y" : 100
}

StaticEntity *editor_instantiate_static_entity( SJson *json, Vector2D position )
{
    char *filename;
    Uint32 x = -1;
    Uint32 y = -1;

    Uint32 height = -1;
    Uint32 width = -1;

    StaticEntity *sEnt;
    sEnt = static_entity_new();
    if (!sEnt) return;

    filename = sj_get_string_value( sj_object_get_value( json, "sprite" ) );

    sj_get_integer_value( sj_object_get_value( json, "height" ), &height );
    sj_get_integer_value( sj_object_get_value( json, "width" ), &width );

    sEnt->sprite = gf2d_sprite_load_image( filename );
    sEnt->position = position;

    sEnt->collision_type = sj_get_string_value( sj_object_get_value( json, "collisionType" ) );
    if (strcmp( sEnt->collision_type, "box" ) == 0)
    {
        if (height == -1 || width == -1)
        {
            sEnt->bounds.w = sEnt->sprite->frame_w;
            sEnt->bounds.h = sEnt->sprite->frame_h;
        }
        else
        {
            sEnt->bounds.w = width;
            sEnt->bounds.h = height;
        }
    }
    else if (strcmp( sEnt->collision_type, "quad" ) == 0)
    {
        sEnt->quad_bounds.vert1 = vector2d( sEnt->position.x, sEnt->position.y + 70.5f );
        sEnt->quad_bounds.vert2 = vector2d( sEnt->position.x + 70.5f, sEnt->position.y );
        sEnt->quad_bounds.vert3 = vector2d( sEnt->position.x + 87, sEnt->position.y + 17.5f );
        sEnt->quad_bounds.vert4 = vector2d( sEnt->position.x + 17.5f, sEnt->position.y + 88 );
    }
    else
    {
        slog( "Failed to get collision type" );
    }


    if (strcmp( sj_get_string_value( sj_object_get_value( json, "type" ) ), "background" ) == 0)
    {
        sEnt->position.x = 0;
        sEnt->position.y = 0;
    }
    else
    {
        sEnt->bounds.x = sEnt->position.x;
        sEnt->bounds.y = sEnt->position.y;
    }

    menu_close( menu );
    edit_sEnt = sEnt;

    return sEnt;
}


void editor_menu_think( Menu *self)
{
//    if (!menu) return;
//
//    Uint8 moved = false;
//    const Uint8 *keys;
//    keys = SDL_GetKeyboardState( NULL );
//
//    SDL_Event e;
//    SDL_PollEvent( &e );
//
//    int mx, my;
//    SDL_GetMouseState( &mx, &my );
//
//    if (e.type == SDL_MOUSEBUTTONDOWN)
//    {
//        if (e.button.button == SDL_BUTTON_RIGHT)
//        {
//            menu_open( menu );
//            menu->floating_menu_offset = vector2d( mx, my );
//        }
//    }
//
}

void editor_think()
{
    if (!menu) return;

    Uint8 moved = false;
    const Uint8 *keys;
    keys = SDL_GetKeyboardState( NULL );

    SDL_Event e;
    SDL_PollEvent( &e );

    int mx, my;
    SDL_GetMouseState( &mx, &my );

    if (e.type == SDL_MOUSEBUTTONDOWN)
    {
        if (e.button.button == SDL_BUTTON_RIGHT)
        {
            menu_open( menu );
            menu->floating_menu_offset = vector2d( mx, my );
        }
        if (e.button.button == SDL_BUTTON_LEFT)
        {
            if( editor_check_for_selection(vector2d(mx, my), menu->current_button->bounds))
            {
                if (menu->current_button->action) menu->current_button->action( menu, menu->current_button->data );
            }
            else
            {
                menu_close( menu );
            }
           // if (cooldown_timer < g_time)
           // {
           //     editor_select( mx, my, e );
           // }

        }
    }





    editor_update_selection( mx, my, e, keys );
}

void editor_select( int mx, int my, SDL_Event e )
{
    EntityManager *em = entity_manager_get();
    StaticEntityManager *sem = static_entity_manager_get();

    Vector2D select_point = vector2d( mx, my );

    for (int i = 0; i < em->entity_count; i++)
    {
        if (!em->entity_list[i]._inuse)// not used yet
        {
            continue;// skip this iteration of the loop
        }
        SDL_Rect rect = { em->entity_list[i].bounds.x, em->entity_list[i].bounds.y, em->entity_list[i].bounds.w, em->entity_list[i].bounds.h };
        if (editor_check_for_selection( select_point, rect ))
        {
            edit_ent = &em->entity_list[i];

            editor_mouse_cooldown_reset();

            return;
        }
    }

    for (int i = 0; i < sem->static_entity_count; i++)
    {
        if (!sem->static_entity_list[i]._inuse)// not used yet
        {
            continue;// skip this iteration of the loop
        }
        SDL_Rect rect = { sem->static_entity_list[i].bounds.x, sem->static_entity_list[i].bounds.y, sem->static_entity_list[i].bounds.w, sem->static_entity_list[i].bounds.h };
        if (editor_check_for_selection( select_point, rect ))
        {
            edit_sEnt = &sem->static_entity_list[i];
            editor_mouse_cooldown_reset();

            return;

        }

    }
}

void editor_mouse_cooldown_reset()
{
    cooldown_timer = g_time + interact_cooldown;
}

void editor_deselect()
{
    edit_ent = NULL;
    edit_sEnt = NULL;

    editor_mouse_cooldown_reset();
}


void editor_update_selection( int mx, int my, SDL_Event e, const Uint8 *keys )
{
    if (edit_sEnt)
    {
        if (keys[SDL_SCANCODE_LCTRL])
        {
            edit_sEnt->position.x = (10 - mx % 10) + mx;
            edit_sEnt->position.y = (10 - my % 10) + my;
            edit_sEnt->bounds.x = (10 - mx % 10) + mx;
            edit_sEnt->bounds.y = (10 - my % 10) + my;
        }
        else
        {
            edit_sEnt->position.x = mx;
            edit_sEnt->position.y = my;
            edit_sEnt->bounds.x = mx;
            edit_sEnt->bounds.y = my;

        }


        if (cooldown_timer < g_time)
        {
            if (e.type == SDL_MOUSEBUTTONDOWN)
            {
                if (e.button.button == SDL_BUTTON_LEFT)
                {
                    editor_deselect();
                }
            }
        }
    }
    else if (edit_ent)
    {
        if (keys[SDL_SCANCODE_LCTRL])
        {
            edit_ent->position.x = (10 - mx % 10) + mx;
            edit_ent->position.y = (10 - my % 10) + my;
            edit_ent->bounds.x = (10 - mx % 10) + mx;
            edit_ent->bounds.y = (10 - my % 10) + my;
        }
        else
        {
            edit_ent->position.x = mx;
            edit_ent->position.y = my;
            edit_ent->bounds.x = mx;
            edit_ent->bounds.y = my;

        }

        if (cooldown_timer < g_time)
        {
            if (e.type == SDL_MOUSEBUTTONDOWN)
            {
                if (e.button.button == SDL_BUTTON_LEFT)
                {
                    editor_deselect();
                }
            }
        }
    }
    else if (cooldown_timer < g_time)
    {
        if (e.type == SDL_MOUSEBUTTONDOWN)
        {
            if (e.button.button == SDL_BUTTON_LEFT)
            {
                editor_select( mx, my, e );
            }
        }
        
    }
}

void editor_draw()
{
    if (edit_ent)
    {
        SDL_Rect rect = { edit_ent->bounds.x, edit_ent->bounds.y, edit_ent->bounds.w, edit_ent->bounds.h };
        gf2d_draw_rect( rect, vector4d( 255, 255, 0, 255 ) );
    }
    if (edit_sEnt)
    {
        SDL_Rect rect = { edit_sEnt->bounds.x, edit_sEnt->bounds.y, edit_sEnt->bounds.w, edit_sEnt->bounds.h };
        gf2d_draw_rect( rect, vector4d( 255, 255, 0, 255 ) );
    }
}

Uint8 editor_check_for_selection( Vector2D p, SDL_Rect r ) // Why doesnt including g_collision.h work here?
{
    if (p.x >= r.x &&
         p.x <= r.x + r.w &&
         p.y >= r.y &&
         p.y <= r.y + r.h)
    {
        return true;
    }
    return false;
}