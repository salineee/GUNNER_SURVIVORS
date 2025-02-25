// NEW - IMPORT TO ENGINE

#include "../IDG_Common.h"

#include "../system/IDG_Draw.h"

#include "IDG_Hitbox.h"

extern app_t   app;
extern stage_t stage;

void IDG_CreateHitbox(entity_t *e, int8_t hb_type)
{
    hitbox_t *hb;
    hb = malloc(sizeof(hitbox_t));
    memset(hb, 0, sizeof(hitbox_t));

    hb->type  = hb_type;
    e->hitbox = hb;
}

void IDG_UpdateHitbox(entity_t *e)
{
    hitbox_t *hb;
    hb = (hitbox_t *)e->hitbox;

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

hitbox_t *IDG_GetHitbox(entity_t *e)
{
    return (hitbox_t *)e->hitbox; 
}

void IDG_DrawHitbox(entity_t *e)
{
    hitbox_t *hb = IDG_GetHitbox(e);
    if(hb != NULL)
    {
        IDG_DrawOutlineRect((hb->pos.x-stage.camera.pos.x), (hb->pos.y-stage.camera.pos.y), hb->pos.w, hb->pos.h, 0xFF, 0x00, 0x00, 0xFF);
    }
}