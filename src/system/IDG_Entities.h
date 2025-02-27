#ifndef __IDG_ENTITIES__
#define __IDG_ENTITIES__

void IDG_InitEntities   (void);
void IDG_DoEntities     (void);
void IDG_DrawEntities   (int layer);
void IDG_ClearEntities  (void);
int  IDG_EntityIsMoving (entity_t *self);

#endif // __IDG_ENTITIES__