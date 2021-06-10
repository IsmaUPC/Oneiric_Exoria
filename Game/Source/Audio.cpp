#include "App.h"
#include "Audio.h"

#include "Defs.h"
#include "Log.h"

// NOTE: Recommended using: Additional Include Directories,
// instead of 'hardcoding' library location path in code logic
#include "SDL/include/SDL.h"
#include "SDL_mixer/include/SDL_mixer.h"

// NOTE: Library linkage is configured in Linker Options
//#pragma comment(lib, "../Game/Source/External/SDL_mixer/libx86/SDL2_mixer.lib")

Audio::Audio() : Module()
{
	music = NULL;
	name.Create("audio");
}

// Destructor
Audio::~Audio()
{}

// Called before render is available
bool Audio::Awake(pugi::xml_node& config)
{
	LOG("Loading Audio Mixer");
	bool ret = true;
	SDL_Init(0);

	if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0)
	{
		LOG("SDL_INIT_AUDIO could not initialize! SDL_Error: %s\n", SDL_GetError());
		active = false;
		ret = true;
	}
	else
	{
		volumeMusic = config.child("music").attribute("volume").as_int(MIX_MAX_VOLUME);
		volumeFx = config.child("fx").attribute("volume").as_int(MIX_MAX_VOLUME);
	}

	// Load support for the JPG and PNG image formats
	int flags = MIX_INIT_OGG;
	int init = Mix_Init(flags);

	if ((init & flags) != flags)
	{
		LOG("Could not initialize Mixer lib. Mix_Init: %s", Mix_GetError());
		active = false;
		ret = true;
	}

	// Initialize SDL_mixer
	if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
	{
		LOG("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
		active = false;
		ret = true;
	}

	Mix_AllocateChannels(maxNumChannels);

	return ret;
}

// Called before quitting
bool Audio::CleanUp()
{
	if (!active)
		return true;

	LOG("Freeing sound FX, closing Mixer and Audio subsystem");

	if (music != NULL)
	{
		Mix_FreeMusic(music);
	}

	UnloadFxs();

	Mix_AllocateChannels(0);
	channels.Clear();

	Mix_CloseAudio();
	Mix_Quit();
	SDL_QuitSubSystem(SDL_INIT_AUDIO);

	return true;
}
// Unload all Fx
void Audio::UnloadFxs()
{
	for (uint i = 0; i < MAX_FX; ++i)
	{
		if (fx[i] != nullptr)
			Mix_FreeChunk(fx[i]);
	}
}
// Unload 1 Fx
void Audio::Unload1Fx(int index)
{
	if (fx[index] != nullptr)
	{
		Mix_FreeChunk(fx[index]);
		fx[index] = nullptr;
	}
}

// Play a music file
bool Audio::PlayMusic(const char* path, float fadeTime)
{
	bool ret = true;
	Mix_VolumeMusic(volumeMusic);
	if (!active)
		return false;

	if (music != NULL)
	{
		if (fadeTime > 0.0f)
		{
			Mix_FadeOutMusic(int(fadeTime * 1000.0f));
		}
		else
		{
			Mix_HaltMusic();
		}

		// This call blocks until fade out is done
		Mix_FreeMusic(music);
	}

	music = Mix_LoadMUS_RW(app->assets->Load(path), 1);

	if (music == NULL)
	{
		LOG("Cannot load music %s. Mix_GetError(): %s\n", path, Mix_GetError());
		ret = false;
	}
	else
	{
		if (fadeTime > 0.0f)
		{
			if (Mix_FadeInMusic(music, -1, (int)(fadeTime * 1000.0f)) < 0)
			{
				LOG("Cannot fade in music %s. Mix_GetError(): %s", path, Mix_GetError());
				ret = false;
			}
		}
		else
		{
			if (Mix_PlayMusic(music, -1) < 0)
			{
				LOG("Cannot play in music %s. Mix_GetError(): %s", path, Mix_GetError());
				ret = false;
			}
		}
	}

	LOG("Successfully playing %s", path);
	return ret;
}

// Load WAV
unsigned int Audio::LoadFx(const char* path)
{
	unsigned int ret = 0;

	if (!active)
		return 0;

	Mix_Chunk* chunk = Mix_LoadWAV_RW(app->assets->Load(path),1);

	if (chunk == NULL)
	{
		LOG("Cannot load wav %s. Mix_GetError(): %s", path, Mix_GetError());
	}
	else
	{
		for (ret = 0; ret < MAX_FX; ++ret)
		{
			if (fx[ret] == nullptr)
			{
				fx[ret] = chunk;
				break;
			}
		}
		LOG("Sucesfully loaded wav %s.", path);
	}

	app->assets->DeleteBuffer();
	return ret;
}

// Play WAV
bool Audio::PlayFx(unsigned int id, int channel, int repeat, int volume)
{
	bool ret = false;

	if (!active)
		return false;

	if (id <= MAX_FX)
	{
		// If Mix_Playing(-1) check all channels
		if (Mix_Playing(channel) == 0 || channel == -1)
		{
			// Check if volume is hardcoded and 
			// lower the volume if the channel volume is higher than the maximum volume  
			if (volume != -1) Mix_Volume(channel, volume);
			if (channel == -1) Mix_Volume(channel, volumeFx);
			if (Mix_Volume(channel, -1) > volumeFx)
				Mix_Volume(channel, volumeFx);
			Mix_PlayChannel(channel, fx[id], repeat);
			LOG("Sucessfully playing the Fx.");
		}
	}

	return ret;
}

// Assign a different channel to each entity and when all available channels 
// are assigned the function must create 10 new ones
int Audio::SetChannel()
{
	int ret = -1;
	//if (channels.Count() != 0)
	if (channels.start != NULL)
	{
		ret = channels.start->data;
		channels.Del(channels.start);
	}
	else
	{
		if (numChannels < maxNumChannels - 1)
		{
			numChannels++;
			return numChannels;
		}
		else
		{
			maxNumChannels += 10;
			Mix_AllocateChannels(maxNumChannels);
			numChannels++;
			return numChannels;
		}
	}

	return ret;
}

// Assign the distance and direction to which the entity of the listener is located 
// 0 = very close, 254 = far away, 255 = out of range (Volume = 0)
bool Audio::SetDistanceFx(int channel, int angle, uint distance, uint maxDistance)
{
	bool ret = false;
	if (distance < maxDistance) ret = true;
	distance = distance * 255 / maxDistance;
	if (distance > 255) distance = 255;
	Mix_SetPosition(channel, angle, distance);

	return ret;
}

// Activate the boolean variable 
void Audio::DeleteChannel(int channel)
{
	channels.Add(channel);
}

// Extra functions Music
void Audio::PauseMusic()
{
	Mix_PauseMusic();
}
void Audio::ResumeMusic()
{
	Mix_ResumeMusic();
}
void Audio::StopMusic()
{
	Mix_HaltMusic();
}
void Audio::RewindMusic()
{
	Mix_RewindMusic();
}
// Set the position of the currently playing music
void Audio::MusicPos(double second)
{
	Mix_SetMusicPosition(second);
}
// Set the volume to volume
void Audio::SetVolumeMusic(int volume)
{
	volumeMusic = volume;
	Mix_VolumeMusic(volumeMusic);
}

// Up/Down Music volume 
void Audio::ChangeMusicVolume(int volume)
{
	volumeMusic += volume;
	if (volumeMusic > MIX_MAX_VOLUME) volumeMusic = MIX_MAX_VOLUME;
	if (volumeMusic < 0) volumeMusic = 0;
	Mix_VolumeMusic(volumeMusic);
}
// Up/Down Fx volume 
void Audio::ChangeFxVolume(int volume)
{
	volumeFx += volume;
	if (volumeFx > MIX_MAX_VOLUME) volumeFx = MIX_MAX_VOLUME;
	if (volumeFx < 0) volumeFx = 0;
}

// Extra functions Fx
void Audio::PauseFx(int channel)
{
	Mix_Pause(channel);
}
void Audio::ResumeFx(int channel)
{
	Mix_Resume(channel);
}
void Audio::StopFx(int channel)
{
	Mix_HaltChannel(channel);
}

// Load/Save
bool Audio::LoadState(pugi::xml_node& node)
{
	volumeMusic = node.child("music").attribute("volume").as_int(volumeMusic);
	volumeFx = node.child("fx").attribute("volume").as_int(volumeFx);
	Mix_VolumeMusic(volumeMusic);
	return true;
}
bool Audio::SaveState(pugi::xml_node& node) const
{
	node.child("music").attribute("volume").set_value(volumeMusic);
	node.child("fx").attribute("volume").set_value(volumeFx);
	return true;
}