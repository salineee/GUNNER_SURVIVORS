#include "../IDG_Common.h"

#include "../system/IDG_Atlas.h"
#include "../system/IDG_Draw.h"
#include "../system/IDG_Text.h"

#include "IDG_Effect.h"

extern app_t   app;
extern stage_t stage;

void IDG_InitEffects(void)
{
    SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Initializing Effects");

    /* load effect textures below this line */
    /*****/    

    memset(&stage.effect_head, 0, sizeof(effect_t));
    stage.effect_tail = &stage.effect_head;
}

void IDG_DoEffects(void)
{
    effect_t *e, *prev;
    prev = &stage.effect_head;

    for(e=stage.effect_head.next; e!=NULL; e=e->next)
    {
        e->life  -= app.delta_time;
        e->alpha -= app.delta_time;
        e->x     += (e->dx*app.delta_time);
        e->y     += (e->dy*app.delta_time);
        // printf("LIFE: %lf\n", e->alpha);

        // if(e->life <= 50.0)
        // {
        //     printf("FADING FASTER...\n");
        //     // fade out faster
        //     e->alpha -= (app.delta_time*6);
        // }
        // if effect or dead or invisible, free it
        if(e->life <= 0 || e->alpha <= 0)
        {
            prev->next = e->next;
            if(e == stage.effect_tail)
                stage.effect_tail = prev;
            free(e);
            e = prev;
        }
        prev = e;
    }
}

void IDG_DrawEffects(void)
{
    effect_t *e;
    int x, y;

    // SDL_SetRenderDrawBlendMode(app.renderer, SDL_BLENDMODE_ADD);
    // SDL_SetTextureBlendMode(explosion_texture->texture, SDL_BLENDMODE_ADD);

    for(e=stage.effect_head.next; e!=NULL; e=e->next)
    {
        // SDL_SetTextureAlphaMod(explosion_texture->texture, MIN(e->alpha, 255));
        // SDL_SetTextureColorMod(explosion_texture->texture, e->color.r, e->color.g, e->color.b);

        x = (e->x-stage.camera.pos.x-(e->size/2));
        y = (e->y-stage.camera.pos.y-(e->size/2));
        IDG_DrawText(e->text, x, y, e->color.r, e->color.g, e->color.b, TEXT_ALIGN_CENTER, 0);
        // IDG_IDG_BlitScaled(explosion_texture, x, y, e->size, e->size, 0);
    }

    // SDL_SetTextureColorMod(explosion_texture->texture, 255, 255, 255);
    // SDL_SetTextureAlphaMod(explosion_texture->texture, 255);

    SDL_SetRenderDrawBlendMode(app.renderer, SDL_BLENDMODE_BLEND);
    // SDL_SetTextureBlendMode(explosion_texture->texture, SDL_BLENDMODE_BLEND);
}

void IDG_ClearEffects(void)
{
    effect_t *e;

    while(stage.effect_head.next != NULL)
    {
        e = stage.effect_head.next;
        stage.effect_head.next = e->next;
        free(e);
    }
}

// custom effects

void add_attack_hit_effect(int start_x, int start_y, int amt)
{
    effect_t *e;
    e = malloc(sizeof(effect_t));
    memset(e, 0, sizeof(effect_t));
    stage.effect_tail->next = e;
    stage.effect_tail       = e;

    e->x       = (start_x+rand()%8);
    e->y       = start_y;
    e->dx      = 0.05;
    e->dy      = -0.3;
    e->size    = 24;
    e->life    = FPS;
    e->alpha   = 0xFF;
    e->color.r = 0xFF;
    e->color.g = 0x00;
    e->color.b = 0x00;
    sprintf(e->text, "%d", amt);
    // char dmg[16];
    // sprintf(dmg, "%d", amt);
    // IDG_DrawText(dmg, (start_x-16), (start_y-16), 0xFF, 0x00, 0x00, TEXT_ALIGN_CENTER, 0);
} 