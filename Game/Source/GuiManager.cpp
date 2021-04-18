#include "GuiManager.h"
#include "SceneManager.h"

#include "GuiMenuPause.h"
#include "GuiSettings.h"

#include "Textures.h"

#include "Defs.h"
#include "Log.h"

GuiManager::GuiManager(Input* input, Render* render)
{
	name.Create("gui_manager");
	this->input = input;
	this->render = render;
}

GuiManager::~GuiManager()
{
}

bool GuiManager::Awake(pugi::xml_node&)
{
	LOG("Loading GUI manager");

	//gui= new GUI(info, texHead);
	bool ret = true;
	return ret;
}

bool GuiManager::Start()
{
	btnSelected = app->audio->LoadFx("Assets/Audio/Fx/button_travel.wav");
	btnPressed = app->audio->LoadFx("Assets/Audio/Fx/button_click.wav");
	btnDisabled = app->audio->LoadFx("Assets/Audio/Fx/button_disabled.wav");
	//btnTextureAtlas = app->tex->Load("Assets/Textures/GUI/button_atlas.png");
	uiAtlas = app->tex->Load("Assets/Textures/GUI/ui_atlas.png");
	moonCorner = app->tex->Load("Assets/Textures/GUI/corner.png");
	handCursor = app->tex->Load("Assets/Textures/GUI/hand_cursor.png");

	handAnim = new Animation();
	handAnim->speed = 0.1f;
	handAnim->loop = true;
	for (int i = 0; i < 9; i++)
	{
		handAnim->PushBack({ i * 32, 0, 32, 32 });
	}

	press = false;
	return true;
}


bool GuiManager::Update(float dt)
{
	bool ret = true;
	accumulatedTime += dt;
	if (accumulatedTime >= updateMsCycle) doLogic = true;

	handAnim->speed = (dt * 8);
	handAnim->Update();

	// Menu pause
	if (app->sceneManager->GetIsPause()) menu->Update(dt);

	if (doLogic == true)
	{
		accumulatedTime = 0.0f;
		doLogic = false;
	}

	SelectControl();

	for (int i = 0; i < buttons.Count(); i++)
	{
		if(buttons.At(i)->data->active) 
			ret = buttons.At(i)->data->Update(dt);
		if (ret == false) return false;
	}
	for (int i = 0; i < checkBoxs.Count(); i++)
	{
		if (checkBoxs.At(i)->data->active)
			checkBoxs.At(i)->data->Update(dt);
	}
	for (int i = 0; i < sliders.Count(); i++)
	{
		if (sliders.At(i)->data->active)
			sliders.At(i)->data->Update(dt);
	}
	
	return ret;
}

bool GuiManager::PostUpdate()
{
	if (app->sceneManager->GetIsPause())menu->PostUpdate();
	for (int i = 0; i < buttons.Count(); i++)
	{
		if (buttons.At(i)->data->active)
			buttons.At(i)->data->Draw();
	}
	for (int i = 0; i < checkBoxs.Count(); i++)
	{
		if (checkBoxs.At(i)->data->active)
			checkBoxs.At(i)->data->Draw();
	}
	for (int i = 0; i < sliders.Count(); i++)
	{
		if (sliders.At(i)->data->active)
			sliders.At(i)->data->Draw();
	}

	return true;
}

bool GuiManager::CleanUp()
{
	app->tex->UnLoad(btnTextureAtlas);
	app->tex->UnLoad(moonCorner);
	app->tex->UnLoad(handCursor);
	DeleteList();

	delete menu;
	menu = nullptr;

	return true;
}

void GuiManager::DeleteList()
{
	controls.Clear();
	buttons.Clear();
	checkBoxs.Clear();
	sliders.Clear();
}

GuiControl* GuiManager::CreateGuiControl(GuiControlType type)
{
	GuiControl* control = nullptr;

	switch (type)
	{
		// Create the corresponding GuiControl type
		default: break;
	}

	// Created entities are added to the list
	if (control != nullptr) controls.Add(control);

	return control;
}

void GuiManager::AddGuiButton(GuiButton* button)
{
	buttons.Add(button);
	controls.Add(button);
}
void GuiManager::AddGuiCheckBox(GuiCheckBox* checkBox)
{
	checkBoxs.Add(checkBox);
	controls.Add(checkBox);
}
void GuiManager::AddGuiSlider(GuiSlider* slider)
{
	sliders.Add(slider);
	controls.Add(slider);
}

void GuiManager::SelectControl()
{
	GamePad& pad = app->input->pads[0];
	if ((app->input->GetKey(SDL_SCANCODE_DOWN) == KEY_DOWN || app->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_DOWN
		|| app->input->GetKey(SDL_SCANCODE_UP) == KEY_DOWN || app->input->GetKey(SDL_SCANCODE_LEFT) == KEY_DOWN
		|| pad.right || pad.left || pad.up || pad.down || pad.l_x || pad.l_y) && !press)
	{
		press = true;
		bool isFocused = false;
		int i = 0;
		for (i; i < controls.Count(); i++)
		{
			if (controls.At(i)->data->state == GuiControlState::FOCUSED)
			{
				isFocused = true;
				if (app->input->GetKey(SDL_SCANCODE_DOWN) == KEY_DOWN || pad.down || pad.l_y > 0.2 ||
					((app->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_DOWN || pad.right || pad.l_x > 0.2) 
					&& controls.At(i)->data->type != GuiControlType::SLIDER))
				{
					controls.At(i)->data->state = GuiControlState::NORMAL;
					int j = i + 1;
					while (j != i)
					{
						if (j == controls.Count()) j = 0;
						if (controls.At(j)->data->state != GuiControlState::DISABLED && controls.At(j)->data->active)
						{
							controls.At(j)->data->state = GuiControlState::FOCUSED;
							break;
						}
						j++;
					}
				}
				if (app->input->GetKey(SDL_SCANCODE_UP) == KEY_DOWN || pad.up || pad.l_y < -0.2 ||
					((app->input->GetKey(SDL_SCANCODE_LEFT) == KEY_DOWN || pad.left || pad.l_x < -0.2)
					&& controls.At(i)->data->type != GuiControlType::SLIDER))
				{
					controls.At(i)->data->state = GuiControlState::NORMAL;
					int j = i - 1;
					while (j != i)
					{
						if (j == -1) j = controls.Count() - 1;
						if (controls.At(j)->data->state != GuiControlState::DISABLED && controls.At(j)->data->active)
						{
							controls.At(j)->data->state = GuiControlState::FOCUSED;
							break;
						}
						j--;
					}
				}
				break;
			}
		}
		if (!isFocused)
		{
			for (int i = 0; i < controls.Count(); i++)
			{
				if (controls.At(i)->data->state != GuiControlState::DISABLED && controls.At(i)->data->active)
				{
					controls.At(i)->data->state = GuiControlState::FOCUSED;
					break;
				}
			}
		}
	}
	
	if (pad.l_y >= -0.2f && pad.l_y <= 0.2f && pad.l_x >= -0.2f && pad.l_x <= 0.2f && pad.up == false && pad.down == false && pad.left == false && pad.right == false && pad.a == false) {
		press = false;
	}
	ComprobeMouseOnControl(pad);
}

void GuiManager::ComprobeMouseOnControl(GamePad& pad)
{
	// If mouse is on any button, the rest state = NORMAL
	int mouseX, mouseY;
	app->input->GetMousePosition(mouseX, mouseY);

	SDL_Rect bounds = {};
	SDL_Rect checkBoxInput = {};
	SDL_Rect sliderBarInput = {};
	SDL_Rect slider = {};

	for (int i = 0; i < controls.Count(); i++)
	{
		if (controls.At(i)->data->active)
		{
			bounds = controls.At(i)->data->bounds;
			switch (controls.At(i)->data->type)
			{
			case GuiControlType::BUTTON:
				if (((mouseX > bounds.x) && (mouseX < (bounds.x + bounds.w)) &&
					(mouseY > bounds.y) && (mouseY < (bounds.y + bounds.h))))
				{
					ReAssignState(i, pad);
					break;
				}
				break;
			case GuiControlType::CHECKBOX:

				for (int j = 0; j < checkBoxs.Count(); j++)
				{
					if (checkBoxs.At(j)->data->id == controls.At(i)->data->id)
					{
						checkBoxInput = checkBoxs.At(j)->data->GetCheckBoxInput();
						break;
					}
				}

				if ((mouseX > checkBoxInput.x) && (mouseX < (checkBoxInput.x + checkBoxInput.w)) &&
					(mouseY > checkBoxInput.y) && (mouseY < (checkBoxInput.y + checkBoxInput.h)))
				{
					ReAssignState(i, pad);
					break;
				}
				break;
			case GuiControlType::SLIDER:

				for (int j = 0; j < sliders.Count(); j++)
				{
					if (sliders.At(j)->data->id == controls.At(i)->data->id)
					{
						sliderBarInput = sliders.At(j)->data->GetsliderBarInput();
						slider = sliders.At(j)->data->GetSlider();
						break;
					}
				}

				if (((mouseX > sliderBarInput.x) && (mouseX < (sliderBarInput.x + sliderBarInput.w)) &&
					(mouseY > sliderBarInput.y) && (mouseY < (sliderBarInput.y + sliderBarInput.h))) ||
					((mouseX > slider.x) && (mouseX < (slider.x + slider.w)) &&
						(mouseY > slider.y) && (mouseY < (slider.y + slider.h))))
				{
					ReAssignState(i, pad);
					break;
				}
				break;
			default:
				break;
			}
		}
	}
}

void GuiManager::ReAssignState(int i, GamePad& pad)
{
	for (int j = 0; j < controls.Count(); j++)
	{
		if (controls.At(j)->data->state != GuiControlState::DISABLED && controls.At(j)->data->active)
			controls.At(j)->data->state = GuiControlState::NORMAL;
	}
	if (controls.At(i)->data->state != GuiControlState::DISABLED && controls.At(i)->data->active)
		controls.At(i)->data->state = GuiControlState::FOCUSED;

	if (controls.At(i)->data->state == GuiControlState::DISABLED && controls.At(i)->data->active)
		if (app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KeyState::KEY_DOWN || pad.a || app->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN)
			app->audio->PlayFx(btnDisabled);
}

void GuiManager::CreatMenuPause(SceneControl* current)
{
	// Menu pause
	delete menu;
	menu=nullptr;
	menu= (new GuiMenuPause({ -app->render->camera.x + WINDOW_W / 2 - 237 / 2, -app->render->camera.y + WINDOW_H / 2 - 237 / 2 }, current, btnTextureAtlas));
}
