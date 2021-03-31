#ifndef __AUDIO_H__
#define __AUDIO_H__

#include "Module.h"

#define DEFAULT_MUSIC_FADE_TIME 2.0f
#define MAX_FX 200

struct _Mix_Music;
struct Mix_Chunk;



class Audio : public Module
{
public:

	Audio();

	// Destructor
	virtual ~Audio();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called before quitting
	bool CleanUp();

	bool Update(float dt);

	// Play a music file
	bool PlayMusic(const char* path, float fadeTime = DEFAULT_MUSIC_FADE_TIME);

	// Load a WAV in memory
	unsigned int LoadFx(const char* path);

	// Play a previously loaded WAV
	bool PlayFx(unsigned int id, int volume = 0);

	bool UnloadFxs();

	bool Unload1Fx(int index);

	// Volume modify
	void ChangeVolumeMusic(int num);

	// Save and Load volume
	bool LoadState(pugi::xml_node& node);
	bool SaveState(pugi::xml_node& node)const;
	// Return volume music
	int GetVolumeMusic() { return volumeMusic; }
	// Return volume Fx
	int GetVolumeFx() { return volumeFx; }

	// Set volume Music
	void SetVolumeMusic(int volume);
	// Set volume Fx
	void SetVolumeFx(int volume) { volumeFx=volume; }
private:

	_Mix_Music* music;
	Mix_Chunk* fx[MAX_FX] = { nullptr };


	uint volumeMusic=0;
	uint volumeFx=0;
};

#endif // __AUDIO_H__