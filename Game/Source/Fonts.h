#ifndef __FONTS_H__
#define __FONTS_H__

#include "Module.h"
#include "SDL/include/SDL_pixels.h"
#include "SDL_ttf/include/SDL_ttf.h"

#define MAX_FONTS 10

struct SDL_Texture;

struct Font
{
	// The font texture
	SDL_Texture* texture = nullptr;

	// Font setup data
	uint totalLength = 0;
	uint rows, columns = 0;
	float char_w, char_h = 0;
};

class Fonts : public Module
{
public:

	// Constructor
	Fonts();

	// Destructor
	~Fonts();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	bool CleanUp();

	// Loads a font file from a texture
	// Returns a font index from the fonts array
	// Param fontPath	- The path to the font file
	// Param size		- Set the font size
	// Param style		- Set the font style
	// Param outline	- Set the outline font size
	TTF_Font* Load(const char* fontPath, int size, int style = TTF_STYLE_NORMAL, int outline = 0);

	// Removes a font by its index
	// Unloads the texture and removes it from the fonts array
	void UnLoad(int font);

	// Create a surface from text
	void BlitText(int x, int y, int fontIndex, const char* text, SDL_Color color) const;


private:
	// An array to keep track and store all loaded fonts
	List<TTF_Font*> fonts;

};


#endif // __ModuleFonts_H__