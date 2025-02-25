// NEW - IMPORT TO ENGINE

#ifndef __IDG_HITBOX__
#define __IDG_HITBOX__

void      IDG_CreateHitbox (entity_t *e, int8_t hb_type);
void      IDG_UpdateHitbox (entity_t *e);
hitbox_t *IDG_GetHitbox    (entity_t *e);
void      IDG_DrawHitbox   (entity_t *e);

#endif // __IDG_HITBOX__