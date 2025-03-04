// NEW - IMPORT TO ENGINE

#include "../IDG_Common.h"

#include "../system/IDG_Entities.h"

#include "IDG_AnimationHandler.h"

extern app_t   app;
extern stage_t stage;

// TODO - refactor this (AND HITBOXES) to not accept a struct.
// Return a malloced handler and assign at the entity/bullet file level
animation_handler_t *IDG_CreateAnimationHandler(int16_t timer, double bob_value)
{
    animation_handler_t *ah;
    ah = malloc(sizeof(animation_handler_t));
    memset(ah, 0, sizeof(animation_handler_t));
    ah->timer     = timer;
    ah->bob_value = bob_value;
    return ah;
}

animation_handler_t *IDG_GetAnimationHandler(entity_t *e)
{
    return (animation_handler_t *)e->animation_handler;   
}