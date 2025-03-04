// NEW - IMPORT TO ENGINE

#ifndef __IDG_ANIMATIONHANDLER__
#define __IDG_ANIMATIONHANDLER__

animation_handler_t *IDG_CreateAnimationHandler (int16_t timer, double bob_value);
animation_handler_t *IDG_GetAnimationHandler    (entity_t *e);

#endif // __IDG_ANIMATIONHANDLER__