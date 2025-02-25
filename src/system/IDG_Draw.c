#include "../IDG_Common.h"

#include "../system/IDG_Text.h"
#include "../system/IDG_Util.h"

#include "IDG_Draw.h"

extern app_t app;

static void IDG_ShowFPS(void);

void IDG_InitDraw(void) {}

void IDG_PrepareScene(void)
{
    SDL_SetRenderDrawColor(app.renderer, 0x00, 0xFF, 0xFF, 255);
    SDL_RenderClear(app.renderer);
}

void IDG_PresentScene(void)
{
	if(app.dev.show_fps) { IDG_ShowFPS(); }
    SDL_RenderPresent(app.renderer);
}

static void IDG_ShowFPS(void)
{
	char fps[8];
	memset(fps, 0, sizeof(fps));
	sprintf(fps, "%d", app.dev.fps);
	IDG_DrawText(fps, (SCREEN_WIDTH-10), 0, 0xFF, 0xFF, 0xFF, TEXT_ALIGN_RIGHT, 0);
}

void IDG_Blit(SDL_Texture *texture, int x, int y, int center)
{
    SDL_Rect dest;
    dest.x = x;
    dest.y = y;

    SDL_QueryTexture(texture, NULL, NULL, &dest.w, &dest.h);

    if(center)
    {
        dest.x -= (dest.w/2);
        dest.y -= (dest.h/2);
    }

    SDL_RenderCopy(
        app.renderer, // renderer
        texture,      // texture to draw
        NULL,         // src rect - tell SDL to copy the entire texture, or a partial
        &dest         // coordinates to draw at
    );
}

// NEW - IMPORT TO ENGINE
void IDG_BlitCircle(SDL_Point center, int radius)
{
	// 35/49 - slightly approximation of 1/sqrt(2)
	const int arr_size = IDG_RoundToMultipleOfEight((radius*8*35/49));
	SDL_Point points[arr_size]; // huh? this works, but fix the error.
	int       draw_count = 0;

	const int32_t diameter = (radius*2);
	int32_t x   = (radius-1);
	int32_t y   = 0;
	int32_t tx  = 1;
	int32_t ty  = 1;
	int32_t err = (tx-diameter);

	while(x>=y)
	{
		SDL_Point p1 = {(center.x+x), (center.y-y)},
				  p2 = {(center.x+x), (center.y+y)},
				  p3 = {(center.x-x), (center.y-y)},
				  p4 = {(center.x-x), (center.y+y)},
				  p5 = {(center.x+y), (center.y-x)},
				  p6 = {(center.x+y), (center.y+x)},
				  p7 = {(center.x-y), (center.y-x)},
				  p8 = {(center.x-y), (center.y+x)};

		points[draw_count+0] = p1;
		points[draw_count+1] = p2;
		points[draw_count+2] = p3;
		points[draw_count+3] = p4;
		points[draw_count+4] = p5;
		points[draw_count+5] = p6;
		points[draw_count+6] = p7;
		points[draw_count+7] = p8;

		draw_count += 8;

		if(err <= 0)
		{
			++y;
			err += ty;
			ty += 2;
		}

		if(err > 0)
		{
			--x;
			tx += 2;
			err += (tx-diameter);
		}

		SDL_RenderDrawPoints(app.renderer, points, draw_count);
	}

	// const int32_t diameter = (radius*2);
	// int32_t x   = (radius-1);
	// int32_t y   = 0;
	// int32_t tx  = 1;
	// int32_t ty  = 1;
	// int32_t err = (tx-diameter);

	// while(x>=y)
	// {
	// 	SDL_RenderDrawPoint(app.renderer, (center_x+x), (center_y-y));
	// 	SDL_RenderDrawPoint(app.renderer, (center_x+x), (center_y+y));
	// 	SDL_RenderDrawPoint(app.renderer, (center_x-x), (center_y-y));
	// 	SDL_RenderDrawPoint(app.renderer, (center_x-x), (center_y+y));
	// 	SDL_RenderDrawPoint(app.renderer, (center_x+y), (center_y-x));
	// 	SDL_RenderDrawPoint(app.renderer, (center_x+y), (center_y+x));
	// 	SDL_RenderDrawPoint(app.renderer, (center_x-y), (center_y-x));
	// 	SDL_RenderDrawPoint(app.renderer, (center_x-y), (center_y+x));

	// 	if(err <= 0)
	// 	{
	// 		++y;
	// 		err += ty;
	// 		ty += 2;
	// 	}

	// 	if(err > 0)
	// 	{
	// 		--x;
	// 		tx += 2;
	// 		err += (tx-diameter);
	// 	}
	// }
}

void IDG_BlitAtlasImage(atlas_image_t *atlas_image, int x, int y, int center, SDL_RendererFlip flip)
{
	// TODO - scaling assets would happen here by multiplying the dest rect by a scale value 

 	SDL_Rect dest;

	dest.x = (x*app.texture_scale);
	dest.y = (y*app.texture_scale);
    dest.w = (atlas_image->rect.w*app.texture_scale);
    dest.h = (atlas_image->rect.h*app.texture_scale);

	if (center)
	{
		dest.x -= dest.w / 2;
		dest.y -= dest.h / 2;
	}

	SDL_RenderCopyEx(app.renderer, atlas_image->texture, &atlas_image->rect, &dest, 0, NULL, flip);   
}

void IDG_BlitRotated(atlas_image_t *atlas_image, int x, int y, double angle)
{
    SDL_Rect dst_rect;

	dst_rect.x = x;
	dst_rect.y = y;
    dst_rect.w = atlas_image->rect.w;
    dst_rect.h = atlas_image->rect.h;

	dst_rect.x -= (dst_rect.w / 2);
	dst_rect.y -= (dst_rect.h / 2);

	SDL_RenderCopyEx(app.renderer, atlas_image->texture, &atlas_image->rect, &dst_rect, angle, NULL, SDL_FLIP_NONE);
}

void IDG_BlitScaled(atlas_image_t *atlas_image, int x, int y, int w, int h, int center)
{
	SDL_Rect dst_rect;

	dst_rect.x = x;
	dst_rect.y = y;
	dst_rect.w = w;
	dst_rect.h = h;

	if (center)
	{
		dst_rect.x -= (dst_rect.w / 2);
		dst_rect.y -= (dst_rect.h / 2);
	}

	SDL_RenderCopy(app.renderer, atlas_image->texture, &atlas_image->rect, &dst_rect);
}

void IDG_DrawRect(int x, int y, int w, int h, int r, int g, int b, int a)
{
	SDL_Rect rect;
	rect.x = x;
	rect.y = y;
	rect.w = w;
	rect.h = h;

	SDL_SetRenderDrawBlendMode(app.renderer, a < 255 ? SDL_BLENDMODE_BLEND : SDL_BLENDMODE_NONE);
	SDL_SetRenderDrawColor(app.renderer, r, g, b, a);
	SDL_RenderFillRect(app.renderer, &rect);
}

void IDG_DrawOutlineRect(int x, int y, int w, int h, int r, int g, int b, int a)
{
	SDL_Rect rect;
	rect.x = x;
	rect.y = y;
	rect.w = w;
	rect.h = h;

	SDL_SetRenderDrawBlendMode(app.renderer, a < 255 ? SDL_BLENDMODE_BLEND : SDL_BLENDMODE_NONE);
	SDL_SetRenderDrawColor(app.renderer, r, g, b, a);
	SDL_RenderDrawRect(app.renderer, &rect);
}

void IDG_DrawGridLines(int spacing)
{
    int x, y;

	SDL_SetRenderDrawColor(app.renderer, 200, 200, 200, 255);
	for (x = 0; x < SCREEN_WIDTH; x += spacing)
		SDL_RenderDrawLine(app.renderer, x, 0, x, SCREEN_HEIGHT);
	for (y = 0; y < SCREEN_WIDTH; y += spacing)
		SDL_RenderDrawLine(app.renderer, 0, y, SCREEN_WIDTH, y);
}