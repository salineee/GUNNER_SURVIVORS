#ifndef __PLAYER__
#define __PLAYER__

#define P_BASE_LIFE            10
#define P_BASE_REST            0

#define P_MAGNET_RADIUS        128

#define P_ANIM_TIME            12
#define P_WALK_ANIM_FRAMES     4
// #define P_ANIM_TIME          6 // frin test
// #define P_WALK_ANIM_FRAMES   7 // frin test
#define P_DIE_ANIM_FRAMES      2
#define P_WALK_SPEED           5

#define P_FRICTION             0.970

void init_player(entity_t *e);

#endif // __PLAYER__