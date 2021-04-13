#include "App.h"
#include "Fonts.h"
#include "Textures.h"
#include "Render.h"

#include "Defs.h"
#include "Log.h"
#include<string.h>

Fonts::Fonts() : Module()
{
	name.Create("fonts");
}

Fonts::~Fonts()
{

}

// Called before render is available
bool Fonts::Awake(pugi::xml_node& config)
{
	LOG("Loading Font manager");
	bool ret = (TTF_Init()) + 1;


	if (!ret) {
		printf("TTF_Init: %s\n", TTF_GetError());
	}

	return ret;
}

bool Fonts::Start()
{
	bool ret = true;

	return ret;
}

bool Fonts::CleanUp()
{

	LOG("Freeing all fonts\n");
	ListItem<TTF_Font*>* item;

	for (item = fonts.start; item != NULL; item = item->next)
	{
		if (item->data != NULL)
		{
			TTF_CloseFont(item->data);
		}
	}

	fonts.Clear();

	TTF_Quit();

	return true;
}

// Load new texture from file path
TTF_Font* Fonts::Load(const char* fontPath, int size, int style, int outline)
{

	TTF_Font* font = nullptr;

	if (fontPath == nullptr)
	{
		LOG("Could not load font");
	}
	else
	{

		font = TTF_OpenFont(fontPath, size);

		TTF_SetFontStyle(font, style);

		TTF_SetFontOutline(font, outline);


		if (fonts.Count() >= MAX_FONTS)
		{
			LOG("Cannot load font %s. List is full (max %d).", fontPath, MAX_FONTS);
		}
		else
		{
			fonts.Add(font);

			LOG("Successfully loaded BMP font from %s", fontPath);
		}
	}

	return font;
}

void Fonts::UnLoad(int font_id)
{
	LOG("Freeing a font texture\n");


	if (font_id >= 0 && font_id < MAX_FONTS && fonts.At(font_id) != NULL)
	{
		TTF_CloseFont(fonts.At(font_id)->data);
		fonts.At(font_id)->data = NULL;
		LOG("Successfully Unloaded BMP font_id %d", font_id);
	}
}

void Fonts::BlitText(int x, int y, int font_id, const char* text, SDL_Color color)
{
	if (text == nullptr || font_id < 0 || font_id >= MAX_FONTS || fonts.At(font_id)->data == NULL)
	{
		LOG("Unable to render text with bmp font id %d", font_id);
		return;
	}
	else
	{
		sur = TTF_RenderText_Blended(fonts.At(font_id)->data, text, color);
		tex = SDL_CreateTextureFromSurface(app->render->renderer, sur);
		app->render->DrawTexture(tex, x, y);
		SDL_FreeSurface(sur);
		SDL_DestroyTexture(tex); // este de aquí es un hijo de puta de mucho cuidao, id con ojo
	}

}