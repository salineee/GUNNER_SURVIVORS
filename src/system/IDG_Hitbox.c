// NEW - IMPORT TO ENGINE

#include "../IDG_Common.h"

#include "../system/IDG_Draw.h"

#include "IDG_Hitbox.h"

extern stage_t stage;

void IDG_CreateHitbox(entity_t *e, int8_t hb_type)
{
    hitbox_t *hb;
    hb = malloc(sizeof(hitbox_t));
    memset(hb, 0, sizeof(hitbox_t));

    if(hb_type == HB_SPH && e->texture != NULL)
        hb->radius = e->texture->rect.w/2;
    hb->type  = hb_type;
    e->hitbox = hb;
}

void IDG_CreateBulletHitbox(bullet_t *b, int8_t hb_type)
{
    hitbox_t *hb;
    hb = malloc(sizeof(hitbox_t));
    memset(hb, 0, sizeof(hitbox_t));

    if(hb_type == HB_SPH && b->texture != NULL)
        hb->radius = b->texture->rect.w/2;
    hb->type  = hb_type;
    b->hitbox = hb;
}

void IDG_UpdateHitbox(entity_t *e)
{
    hitbox_t *hb;
    hb = (hitbox_t *)e->hitbox;

    // TODO - to offset for camera here, or in draw?
    // TODO - using this function for spawning bullets may lead to some unexpected issues - 
    //        setting hb x/y and immediately resetting to correct origin in player.c may cause some
    //        issues between hb init tick and hb update tick
    hb->pos.x = e->x;
    hb->pos.y = e->y;
    hb->pos.w = e->texture->rect.w;
    hb->pos.h = e->texture->rect.h;

    if(e == stage.player)
    {
        const int nudge = 48; // frin sprite transparency offset fuckery
        hb->pos.y += nudge;
        hb->pos.h -= nudge;
    }
}

void IDG_UpdateBulletHitbox(bullet_t *b)
{
    hitbox_t *hb;
    hb = (hitbox_t *)b->hitbox;

    // TODO - to offset for camera here, or in draw?
    // TODO - using this function for spawning bullets may lead to some unexpected issues - 
    //        setting hb x/y and immediately resetting to correct origin in player.c may cause some
    //        issues between hb init tick and hb update tick
    hb->pos.x = b->x;
    hb->pos.y = b->y;
    hb->pos.w = b->texture->rect.w;
    hb->pos.h = b->texture->rect.h;
}

hitbox_t *IDG_GetHitbox(entity_t *e)
{
    return (hitbox_t *)e->hitbox; 
}

hitbox_t *IDG_GetBulletHitbox(bullet_t *b)
{
    return (hitbox_t *)b->hitbox; 
}

void IDG_DrawHitbox(entity_t *e)
{
    hitbox_t *hb = IDG_GetHitbox(e);
    if(hb != NULL)
    {
        switch(hb->type)
        {
        case HB_RECT:
            IDG_DrawOutlineRect(
                (hb->pos.x-stage.camera.pos.x), 
                (hb->pos.y-stage.camera.pos.y),
                hb->pos.w, 
                hb->pos.h, 
                0xFF, 0x00, 0x00, 0xFF
            );
            break;
        case HB_SPH:
            SDL_Point ctr_pt = { ((hb->pos.x-stage.camera.pos.x)+hb->pos.w/2), ((hb->pos.y-stage.camera.pos.y)+hb->pos.h/2) };
            IDG_BlitCircle(
                ctr_pt,
                hb->radius
            );
            break;
        default:
            break;
        }
    }
}