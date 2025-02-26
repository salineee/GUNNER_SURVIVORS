// NEW - IMPORT TO ENGINE

#include "../IDG_Common.h"

#include "IDG_AnimationHandler.h"

extern stage_t stage;

void IDG_CreateAnimationHandler(entity_t *e, int16_t timer)
{
    animation_handler_t *ah;
    ah = malloc(sizeof(animation_handler_t));
    memset(ah, 0, sizeof(animation_handler_t));
    ah->timer = timer;
    e->animation_handler = ah;
}

void IDG_DoAnimationHandler(entity_t *e)
{

}

animation_handler_t *IDG_GetAnimationHandler(entity_t *e)
{
    return (animation_handler_t *)e->animation_handler;   
}