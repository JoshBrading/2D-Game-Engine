#include <SDL.h>
#include "gf2d_graphics.h"
#include "gf2d_sprite.h"
#include "simple_logger.h"

#include "g_entity.h"

int main(int argc, char * argv[])
{
    /*variable declarations*/
    int done = 0;
    const Uint8 * keys;
    Sprite *sprite;
    
    int mx,my;
    float mf = 0;
    Sprite *mouse;
    Vector4D mouseColor = {255,100,255,200};
    
    entity_manager_init(128);

   // Entity *test;
   // test = entity_new();

   // if (!test)
   // {
   //     slog( "Failed to make entity." );
   // }
  
    /*program initializtion*/
    init_logger("gf2d.log");
    slog("---==== BEGIN ====---");
    gf2d_graphics_initialize(
        "gf2d",
        1200,
        720,
        1200,
        720,
        vector4d(0,0,0,255),
        0);
    gf2d_graphics_set_frame_delay(16);
    gf2d_sprite_init(1024);
    SDL_ShowCursor(SDL_DISABLE);
    
    /*demo setup*/
    sprite = gf2d_sprite_load_image("images/backgrounds/bg_flat.png");
    mouse = gf2d_sprite_load_all("images/pointer.png",32,32,16);

   // test->sprite = gf2d_sprite_load_all( "images/pointer.png", 32, 32, 16 );
    //if (!test->sprite)
    //{
    //    slog( "Failed to load sprite" );
    //}
    Entity *q;
    Uint8 canSpawn = 1;

    /*main game loop*/
    while(!done)
    {
        SDL_PumpEvents();   // update SDL's internal event structures
        keys = SDL_GetKeyboardState(NULL); // get the keyboard state for this frame
        /*update things here*/
        SDL_GetMouseState(&mx,&my);
        mf+=0.1;
        if (mf >= 16.0)mf = 0;
        
        
        gf2d_graphics_clear_screen();// clears drawing buffers
        // all drawing should happen betweem clear_screen and next_frame
            //backgrounds drawn first
           // gf2d_sprite_draw_image(sprite,vector2d(0,0));
            
            entity_manager_draw_all();
            entity_manager_draw_debug ();

            //UI elements last
            gf2d_sprite_draw(
                mouse,
                vector2d(mx,my),
                NULL,
                NULL,
                NULL,
                NULL,
                &mouseColor,
                (int)mf);
        gf2d_grahics_next_frame();// render current draw frame and skip to the next frame
        
        if (canSpawn && keys[SDL_SCANCODE_Q])
        {
            canSpawn = 0;
            q = entity_new();
            q->sprite = gf2d_sprite_load_all( "images/pointer.png", 32, 32, 16 );
            q->position = vector2d( mx, my );
            q->bBoxX = q->sprite->frame_w;
            q->bBoxY = q->sprite->frame_h;
        }

        if ( !canSpawn && keys[SDL_SCANCODE_E])
        {
            entity_manager_clear();
            canSpawn = 1;
        }
        if (keys[SDL_SCANCODE_ESCAPE])done = 1; // exit condition
        //slog("Rendering at %f FPS",gf2d_graphics_get_frames_per_second());
    }
    slog("---==== END ====---");
    return 0;
}
/*eol@eof*/
