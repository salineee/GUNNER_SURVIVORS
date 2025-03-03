// NEW - IMPORT TO ENGINE

#include "../IDG_Common.h"

#include "../system/IDG_Entities.h"

#include "IDG_AnimationHandler.h"

extern app_t   app;
extern stage_t stage;

// TODO - refactor this (AND HITBOXES) to not accept a struct.
// Return a malloced handler and assign at the entity/bullet file level
void IDG_CreateAnimationHandler(entity_t *e, int16_t timer)
{
    animation_handler_t *ah;
    ah = malloc(sizeof(animation_handler_t));
    memset(ah, 0, sizeof(animation_handler_t));
    ah->timer = timer;
    e->animation_handler = ah;
}

animation_handler_t *IDG_GetAnimationHandler(entity_t *e)
{
    return (animation_handler_t *)e->animation_handler;   
}