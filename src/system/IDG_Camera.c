#include "../IDG_Common.h"

#include "IDG_Camera.h"

extern app_t   app;
extern stage_t stage;

// TODO - would love to refactor this as a preprocessor directive
//        to handle initialization in the background.
void IDG_InitCamera(void)
{
	stage.camera.initialized = (stage.player == NULL) ? 0 : 1;
	stage.camera.shake       = 0;
	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "%s", (stage.camera.initialized ? "Initialized Camera ..." : "Failed to initialize Camera ..."));
}

void IDG_DoCamera(void)
{
	if(!stage.camera.initialized)
    {
		SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_CRITICAL, "Can't update camera: Player NULL \nAborting ...");
		exit(1);
	}
	
	if(!stage.camera.shake)
	{
		stage.camera.pos.x = (int)stage.player->x + (stage.player->w/2);
		stage.camera.pos.y = (int)stage.player->y + (stage.player->h/2);
	
		stage.camera.pos.x -= (SCREEN_WIDTH/2);
		stage.camera.pos.y -= (SCREEN_HEIGHT/2);
	
		stage.camera.pos.x = MIN(MAX(stage.camera.pos.x, 0), (MAP_WIDTH*MAP_TILE_SIZE)-SCREEN_WIDTH);
		stage.camera.pos.y = MIN(MAX(stage.camera.pos.y, 0), (MAP_HEIGHT*MAP_TILE_SIZE)-SCREEN_HEIGHT);
	}
}

void IDG_DoCameraShake(double life, int shake_amt)
{
	if(!stage.camera.initialized) { return; } // TODO - return w errors

	stage.camera.shake = 1;
	while((life-=app.delta_time) > 0)
	{
		stage.camera.pos.x += rand()%shake_amt;
		stage.camera.pos.y += rand()%shake_amt;
	}
	stage.camera.shake = 0;
}