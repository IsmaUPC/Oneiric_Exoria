#ifndef __INPUT_H__
#define __INPUT_H__

#include "Module.h"

#define NUM_MOUSE_BUTTONS 5
#define MAX_PADS 4

struct SDL_Rect;

struct _SDL_GameController;
struct _SDL_Haptic;

enum EventWindow
{
	WE_QUIT = 0,
	WE_HIDE = 1,
	WE_SHOW = 2,
	WE_COUNT
};

enum KeyState
{
	KEY_IDLE = 0,
	KEY_DOWN,
	KEY_REPEAT,
	KEY_UP
};

struct GamePad
{
	//Input data
	bool start, back, guide = false;
	bool x, y, a, b, l1, r1, l3, r3 = false;
	bool up, down, left, right = false;
	float l2, r2 = 0;
	float l_x, l_y, r_x, r_y, l_dz, r_dz = 0;

	//Controller data
	bool enabled = false;
	int index = 0;
	_SDL_GameController* controller = nullptr;
	_SDL_Haptic* haptic = nullptr;

	//Rumble controller
	int rumble_countdown = 0;
	float rumble_strength = 0;
};

class Input : public Module
{

public:

	Input();

	// Destructor
	virtual ~Input();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool PreUpdate();

	// Called before quitting
	bool CleanUp();

	// Check key states (includes mouse and joy buttons)
	KeyState GetKey(int id) const
	{
		return keyboard[id];
	}

	KeyState GetMouseButtonDown(int id) const
	{
		return mouseButtons[id - 1];
	}

	// Check if a certain window event happened
	bool GetWindowEvent(EventWindow ev);

	// Get mouse / axis position
	void GetMousePosition(int &x, int &y);
	void GetMouseMotion(int& x, int& y);

	//GamePad
	
	// Activates SDL device funcionallity when a gamepad has been connected
	void HandleDeviceConnection(int index);

	// Deactivates SDL device funcionallity when a gamepad has been disconnected
	void HandleDeviceRemoval(int index);

	// Called at PreUpdate
	// Iterates through all active gamepads and update all input data
	void UpdateGamepadsInput();

	bool ShakeController(int id, float duration, float strength = 0.5f);
	const char* GetControllerName(int id) const;

private:
	bool windowEvents[WE_COUNT];
	KeyState* keyboard = nullptr;
	KeyState mouseButtons[NUM_MOUSE_BUTTONS];
	int	mouseMotionX = 0;
	int mouseMotionY = 0;
	int mouseX = 0;
	int mouseY = 0;

public:
	//GamePad
	// An array to fill in all detected gamepads
	GamePad pads[MAX_PADS];
};

#endif // __INPUT_H__