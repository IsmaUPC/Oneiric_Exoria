#ifndef __AUDIO_H__
#define __AUDIO_H__

#include "Module.h"

#define DEFAULT_MUSIC_FADE_TIME 2.0f

struct _Mix_Music;
struct Mix_Chunk;

class Audio : public Module
{
public:
	//Constructor
	Audio();

	// Destructor
	virtual ~Audio();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called before quitting
	bool CleanUp();
	void UnloadFxs();
	void Unload1Fx(int index);

	// Play a music file
	bool PlayMusic(const char* path, float fadeTime = DEFAULT_MUSIC_FADE_TIME);
	void PauseMusic();
	void ResumeMusic();
	void StopMusic();
	void RewindMusic();

	void MusicPos(double second);

	// Load a WAV in memory
	unsigned int LoadFx(const char* path);

	// Play a previously loaded WAV
	bool PlayFx(unsigned int id, int channel = -1, int repeat = 0, int volume = -1);

	void SetDistanceFx(int channel, int angle, uint distance, uint maxDistance);
	int SetChannel();

	void DeleteChannel(int channel);

	void PauseFx(int channel);
	void ResumeFx(int channel);
	void StopFx(int channel);

	void ChangeMusicVolume(int volume);
	void ChangeFxVolume(int volume);

	// Save and Load volume
	bool LoadState(pugi::xml_node& node);
	bool SaveState(pugi::xml_node& node)const;

	// Return volume music
	int GetVolumeMusic() { return volumeMusic; }
	int GetVolumeFx() { return volumeFx; }
	bool GetPendingToDelete() { return pendingToDelete; }

	// Set Volume
	void SetVolumeMusic(int volume);
	void SetVolumeFx(int volume) { volumeFx = volume; }

private:

	_Mix_Music* music = nullptr;
	List<Mix_Chunk*> fx;
	List<int> channels;

	// TODO: Create the varaibles you need
	int maxNumChannels = 10;
	int numChannels = 0;
	int volumeMusic = 128; // 128 = MIX_MAX_VOLUME
	int volumeFx = 128;
	bool pendingToDelete = false;
};

#endif // __AUDIO_H__