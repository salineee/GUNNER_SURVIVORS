#ifndef __IDG_DEFS__
#define __IDG_DEFS__

#define PI                      3.14159265358979323846

#define MIN(a,b)(a<b?a:b)
#define MAX(a,b)(a>b?a:b)

#define STRCPY(dest, src)             \
strncpy(dest, src, sizeof(dest)); \
dest[sizeof(dest)-1] = '\0'
#define STRNCPY(dest, src, n) \
strncpy(dest, src, n);    \
dest[n-1] = '\0'

#define TO_RAIDANS(angle_degrees)(angle_degrees*PI/180.0)

#define IDG_PRJ_TITLE          "GUNNER_SURVIVORS" // NEW - IMPORT TO ENGINE 

/* WINDOW */
#define SCREEN_WIDTH            1600 // These should be dynamic later.
#define SCREEN_HEIGHT           900

/* LOGIC */
#define FPS                     60   // should also be dynamic.
#define LOGIC_RATE             (FPS*1.0/1000)

/* QUADTREE */
#define MAX_QT_CANDIDATES       1024

/* SECTORS */
enum
{
    S2D_POINTS_UNSET,
    S2D_POINT_ONE_SET,
    S2D_POINT_TWO_SET
};

/* GAMESTATE */
enum {
    GS_PLAYING,
    GS_INTERMISSION,      
    GS_LOADING,
    GS_MENU,
    GS_PAUSED
};

/* IDK??? VERSIONS??? */
enum {
    GS_PROD,
    GS_DEBUG
};

/* STATIC FILE PATHS */
#define STAGE_PATH             "data/stages"
#define SYSTEM_ASSET_PATH      "data/gfx/system"
#define WIDGET_DATA_PATH       "data/widgets"

/* INPUT */
#define MAX_KEYBOARD_EVENTS     255
#define MAX_MOUSE_EVENTS        6 
#define MAX_JOYPAD_BUTTONS      16
#define DEADZONE_MAX            32000

enum
{
    JAXIS_X,
    JAXIS_Y,
    JAXIS_MAX
};

enum
{
    /* add custom controls below this line */
    CTRL_LEFT,
    CTRL_RIGHT,
    CTRL_MAX
};

/* TEXT LENGTH LIMITS */
#define MAX_LINE_LENGTH         1024
#define MAX_DESCRIPTION_LENGTH  256
#define MAX_FILENAME_LENGTH     256
#define MAX_NAME_LENGTH         32
#define MAX_TEXTURE_NAME_LENGTH 16
#define MAX_INPUT_LENGTH        16

/* TEXT ALIGNMENT */
enum
{
    TEXT_ALIGN_LEFT,
    TEXT_ALIGN_CENTER,
    TEXT_ALIGN_RIGHT
};

/* FONTS */
#define GLYPH_HEIGHT            28
#define GLYPH_WIDTH             18

/* SPRITE ATLAS */
#define NUM_ATLAS_BUCKETS       64

/* EDITOR2D, MAP, TILES */
#define MAP_WIDTH               210 // should be dynamic
#define MAP_HEIGHT              95  // ^
#define MAP_TILE_SIZE           64  // ^
#define MAX_TILES               500 // ^
// #define MAP_RENDER_WIDTH        34
// #define MAP_RENDER_HEIGHT       19
#define MAP_RENDER_WIDTH      ((SCREEN_WIDTH/MAP_TILE_SIZE)+1)
#define MAP_RENDER_HEIGHT     ((SCREEN_HEIGHT/MAP_TILE_SIZE)+1)

#define TILE_SOLID              1   // Temporary - so map editor boots.
// #define TILE_FOREGROUND         100 // ^
#define TILE_FOREGROUND         300 // ^
#define TILE_WATER              150 // ^

enum
{
    LAYER_FOREGROUND, // 0
    LAYER_BACKGROUND, // 1
    LAYER_SECTOR      // 2
};

/* ENTITY FLAGS */
#define EF_NONE     		    0
#define EF_WEIGHTLESS          (2<<0)
#define EF_SOLID               (2<<1)
#define EF_INTERACTIVE         (2<<2)
#define EF_BLOCKS_LOS          (2<<3)
#define EF_BOUNCES             (2<<4)
#define EF_UNIQUE              (2<<5)
#define EF_REQUIRED            (2<<6)

/* WEAPON FLAGS */
enum
{
    WPN_PISTOL,
    WPN_SHOTGUN,
    WPN_ROCKET,
    WPN_BFG,
    WPN_MAX
};

/* WEAPON STATS */
// TODO - refactor this all into bullets.c/h

// PISTOL
#define WPN_PISTOL_BASE_LIFE        2
#define WPN_PISTOL_BASE_DMG         1
#define WPN_PISTOL_BASE_MAX_AMMO   -1 // TODO - treat -1 as infinite. curr unused
#define WPN_PISTOL_BASE_RELOAD_SPD  20
#define WPN_PISTOL_BASE_PRJ_SPD     18

// SHOTGUN
#define WPN_SHOTGUN_BASE_LIFE       2
#define WPN_SHOTGUN_BASE_DMG        5
#define WPN_SHOTGUN_BASE_MAX_AMMO   24
#define WPN_SHOTGUN_BASE_RELOAD_SPD 32
#define WPN_SHOTGUN_BASE_PRJ_SPD    18
#define WPN_SHOTGUN_BASE_PRJ_COUNT  3
#define WPN_SHOTGUN_BASE_PRJ_SPREAD 12

// ROCKET
#define WPN_ROCKET_BASE_LIFE        5
#define WPN_ROCKET_BASE_DMG         50
#define WPN_ROCKET_BASE_MAX_AMMO    12
#define WPN_ROCKET_BASE_RELOAD_SPD  50
#define WPN_ROCKET_BASE_PRJ_SPD     6
#define WPN_ROCKET_BASE_AOE_RADIUS  100

// BFG
#define WPN_BFG_BASE_LIFE           2
#define WPN_BFG_BASE_DMG           -1 // TODO - treat -1 as instakill/gib
#define WPN_BFG_BASE_MAX_AMMO       3
#define WPN_BFG_BASE_RELOAD_SPD     50
#define WPN_BFG_BASE_PRJ_SPD        4
#define WPN_BFG_PRJ_ANIM_FRAMES     5
#define WPN_BFG_PRJ_ANIM_TIME       12
#define WPN_BFG_IMPACT_ANIM_FRAMES  10
#define WPN_BFG_IMPACT_ANIM_TIME    12
#define WPN_BFG_TRACER_RANGE        500
#define WPN_BFG_BASE_MAX_TRACERS    3

/* PLAYER STUFF */
#define P_BASE_TRGT_XP              10
#define P_XP_MODIFIER               1.5
#define P_BASE_XP                   0
#define P_BASE_LEVEL                1

// definitions for physics
// currently unused - intended to be used when
// clamping 2d vertical movement
#define MAX_VELOCITY            1
#define GRAVITY                 1
#define MAX_FALL_SPEED          12

// This is not engine specific. Get it out of here.
enum
{
    FACING_LEFT,
    FACING_RIGHT,
    FACING_UP,
    FACING_DOWN
};

// NEW - IMPORT TO ENGINE
enum
{
    HB_RECT,
    HB_SPH
};

/* WIDGETS */
enum
{
    WT_BUTTON,
    WT_SELECT,
    WT_SLIDER,
    WT_INPUT,
    WT_CONTROL
};

/* AUDIO CONFIG */
#define SND_MAX_CHANNELS        16
#define SND_FREQ                44100
#define SND_CHUNK_SIZE          1024

/* AUDIO CHANNELS */
enum
{
    CH_ANY = -1,                     // any free channel
    CH_TEMP                          // Other channels are game-specific.
};

/* SOUNDS */
enum
{
    /*** GAME SOUNDS BELOW THIS LINE ***/
    SND_MAX
};

#endif // __IDG_DEFS__