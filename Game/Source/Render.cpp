#include "App.h"
#include "Window.h"
#include "Render.h"
#include "Player.h"

#include "Defs.h"
#include "Log.h"

#define VSYNC false

Render::Render() : Module()
{
	name.Create("renderer");
	background.r = 0;
	background.g = 0;
	background.b = 0;
	background.a = 0;
}
 
// Destructor
Render::~Render()
{}

// Called before render is available
bool Render::Awake(pugi::xml_node& config)
{
	LOG("Create SDL rendering context");
	bool ret = true;



	Uint32 flags = SDL_RENDERER_ACCELERATED;

	if(config.child("vsync").attribute("value").as_bool(true) == true)
	{
		flags |= SDL_RENDERER_PRESENTVSYNC;
		LOG("Using vsync");
	}

	renderer = SDL_CreateRenderer(app->win->window, -1, flags);


	if (app->fullScreen == 1) 
	{
		SDL_RenderSetLogicalSize(renderer, WINDOW_W, WINDOW_H);
	}
	if(renderer == NULL)
	{
		LOG("Could not create the renderer! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	else
	{
		camera.w = app->win->screenSurface->w;
		camera.h = app->win->screenSurface->h;
		camera.x = (WINDOW_W / 2) + (app->player->playerData.position.x * -1);
		camera.y = (WINDOW_H / 2) + (app->player->playerData.position.y * -1);
	}

	return ret;
}

// Called before the first frame
bool Render::Start()
{
	LOG("render start");
	// back background
	SDL_RenderGetViewport(renderer, &viewport);
	return true;
}

// Called each loop iteration
bool Render::PreUpdate()
{
	SDL_RenderClear(renderer);
	return true;
}

bool Render::Update(float dt)
{
	return true;
}

bool Render::PostUpdate()
{
	SDL_SetRenderDrawColor(renderer, background.r, background.g, background.g, background.a);
	SDL_RenderPresent(renderer);
	return true;
}

// Called before quitting
bool Render::CleanUp()
{
	LOG("Destroying SDL render");
	SDL_DestroyRenderer(renderer);
	return true;
}


// Load Game State
bool Render::LoadState(pugi::xml_node& data)
{
	camera.x = data.child("camera").attribute("x").as_int(0);
	camera.y = data.child("camera").attribute("y").as_int(0);

	return true;
}


// Save Game State
bool Render::SaveState(pugi::xml_node& data) const
{
	pugi::xml_node cam = data.child("camera");

	cam.attribute("x").set_value(camera.x);
	cam.attribute("y").set_value(camera.y);

	return true;
}

void Render::SetBackgroundColor(SDL_Color color)
{
	background = color;
}

void Render::FullScreen()
{
	SDL_RenderSetLogicalSize(renderer, WINDOW_W, WINDOW_H);
}

void Render::SetViewPort(const SDL_Rect& rect)
{
	SDL_RenderSetViewport(renderer, &rect);
}

void Render::ResetViewPort()
{
	SDL_RenderSetViewport(renderer, &viewport);
}

// Blit to screen
bool Render::DrawTexture(SDL_Texture* texture, int x, int y, const SDL_Rect* section, float _scale, float speed, double angle, int pivotX, int pivotY) const
{
	bool ret = true;
	uint scale = app->win->GetScale();

	SDL_Rect rect;
	rect.x = (int)(camera.x * speed) + x * scale;
	rect.y = (int)(camera.y * speed) + y * scale;

	if(section != NULL)
	{
		rect.w = section->w;
		rect.h = section->h;
	}
	else
	{
		SDL_QueryTexture(texture, NULL, NULL, &rect.w, &rect.h);
	}

	rect.w *= _scale;
	rect.h *= _scale;

	SDL_Point* p = NULL;
	SDL_Point pivot;

	if(pivotX != INT_MAX && pivotY != INT_MAX)
	{
		pivot.x = pivotX;
		pivot.y = pivotY;
		p = &pivot;
	}

	if(SDL_RenderCopyEx(renderer, texture, section, &rect, angle, p, SDL_FLIP_NONE) != 0)
	{
		LOG("Cannot blit to screen. SDL_RenderCopy error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

bool Render::DrawTextureFlip(SDL_Texture* texture, int x, int y, const SDL_Rect* section, int _scale, float speed, double angle, int pivotX, int pivotY) const
{
	bool ret = true;
	uint scale = app->win->GetScale();

	SDL_Rect rect;
	rect.x = (int)(camera.x * speed) + x * scale;
	rect.y = (int)(camera.y * speed) + y * scale;

	if(section != NULL)
	{
		rect.w = section->w;
		rect.h = section->h;
	}
	else
	{
		SDL_QueryTexture(texture, NULL, NULL, &rect.w, &rect.h);
	}

	rect.w *= _scale;
	rect.h *= _scale;

	SDL_Point* p = NULL;
	SDL_Point pivot;

	if(pivotX != INT_MAX && pivotY != INT_MAX)
	{
		pivot.x = pivotX;
		pivot.y = pivotY;
		p = &pivot;
	}

	if(SDL_RenderCopyEx(renderer, texture, section, &rect, angle, p, SDL_FLIP_HORIZONTAL) != 0)
	{
		LOG("Cannot blit to screen. SDL_RenderCopy error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

bool Render::DrawRectangle(const SDL_Rect& rect, Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool filled, bool use_camera) const
{
	bool ret = true;
	uint scale = app->win->GetScale();

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, r, g, b, a);

	SDL_Rect rec(rect);
	if(use_camera)
	{
		rec.x = (int)(camera.x + rect.x * scale);
		rec.y = (int)(camera.y + rect.y * scale);
		rec.w *= scale;
		rec.h *= scale;
	}

	int result = (filled) ? SDL_RenderFillRect(renderer, &rec) : SDL_RenderDrawRect(renderer, &rec);

	if(result != 0)
	{
		LOG("Cannot draw quad to screen. SDL_RenderFillRect error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

bool Render::DrawLine(int x1, int y1, int x2, int y2, Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool use_camera) const
{
	bool ret = true;
	uint scale = app->win->GetScale();

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, r, g, b, a);

	int result = -1;

	if(use_camera)
		result = SDL_RenderDrawLine(renderer, camera.x + x1 * scale, camera.y + y1 * scale, camera.x + x2 * scale, camera.y + y2 * scale);
	else
		result = SDL_RenderDrawLine(renderer, x1 * scale, y1 * scale, x2 * scale, y2 * scale);

	if(result != 0)
	{
		LOG("Cannot draw quad to screen. SDL_RenderFillRect error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

bool Render::DrawCircle(int x, int y, int radius, Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool use_camera) const
{
	bool ret = true;
	uint scale = app->win->GetScale();

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, r, g, b, a);

	int result = -1;
	SDL_Point points[360];

	float factor = (float)M_PI / 180.0f;

	for(uint i = 0; i < 360; ++i)
	{
		points[i].x = (int)(x + radius * cos(i * factor));
		points[i].y = (int)(y + radius * sin(i * factor));
	}

	result = SDL_RenderDrawPoints(renderer, points, 360);

	if(result != 0)
	{
		LOG("Cannot draw quad to screen. SDL_RenderFillRect error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

bool Render::DrawTextBox(int x, int y, int w, int h, SDL_Color base, SDL_Color interiorBorder, SDL_Color exteriorBorder, SDL_Texture* cornerTex, Uint8 a) const
{
	bool ret = true;
	
	//Base rectangle
	DrawRectangle({ x + 10, y + 10, w - 20, h - 20}, base.r, base.g, base.b, a);

	//Interior border
	DrawRectangle({ x + 5, y + 5, 5, h - 10 }, interiorBorder.r, interiorBorder.g, interiorBorder.b, a);
	DrawRectangle({ x + w - 10, y + 5, 5, h - 10 }, interiorBorder.r, interiorBorder.g, interiorBorder.b, a);
	DrawRectangle({ x + 10, y + 5, w - 20, 5 }, interiorBorder.r, interiorBorder.g, interiorBorder.b, a);
	DrawRectangle({ x + 10, y + h - 10, w - 20, 5 }, interiorBorder.r, interiorBorder.g, interiorBorder.b, a);

	//Exterior border
	DrawRectangle({ x, y, 5, h }, exteriorBorder.r, exteriorBorder.g, exteriorBorder.b, a);
	DrawRectangle({ x + w - 5, y, 5, h}, exteriorBorder.r, exteriorBorder.g, exteriorBorder.b, a);
	DrawRectangle({ x + 5, y, w - 10, 5 }, exteriorBorder.r, exteriorBorder.g, exteriorBorder.b, a);
	DrawRectangle({ x + 5, y + h - 5, w - 10, 5 }, exteriorBorder.r, exteriorBorder.g, exteriorBorder.b, a);

	//Corner draw
	uint texW, texH;
	app->tex->GetSize(cornerTex, texW, texH);
	DrawTexture(cornerTex, x + 15, y + 15);
	DrawTexture(cornerTex, x + w - 15 - texW, y + 15, 0, 1, 1.0f, 90);
	DrawTexture(cornerTex, x + w - 15 - texW, y + h - 15 - texH, 0, 1, 1.0f, 180);
	DrawTexture(cornerTex, x + 15, y + h - 15 - texH, 0, 1, 1.0f, 270);

	return ret;
}