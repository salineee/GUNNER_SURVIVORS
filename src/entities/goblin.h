#ifndef __GOBLIN__
#define __GOBLIN__

#define G_BASE_LIFE   1000
#define G_THINKTIME  (FPS*(1+rand()%3))

#define G_ANIM_TIME   12
#define G_ANIM_FRAMES 4

#define G_RUN_SPEED   4
#define G_FRICTION    1

void init_goblin(entity_t *e);

#endif // __GOBLIN__