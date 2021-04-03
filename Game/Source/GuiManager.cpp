#include "GuiManager.h"

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
	bool ret = true;
	return ret;
}

bool GuiManager::Start()
{
	btnSelected = app->audio->LoadFx("Assets/Audio/Fx/button_selected.wav");
	btnPressed = app->audio->LoadFx("Assets/Audio/Fx/button_pressed.wav");
	btnDisabled = app->audio->LoadFx("Assets/Audio/Fx/button_disable.wav");
	btnSlider = app->audio->LoadFx("Assets/Audio/Fx/coin.wav");
	btnTextureAtlas = app->tex->Load("Assets/Textures/GUI/button_atlas.png");

	return true;
}


bool GuiManager::Update(float dt)
{
	accumulatedTime += dt;
	if (accumulatedTime >= updateMsCycle) doLogic = true;

	if (doLogic == true)
	{
		accumulatedTime = 0.0f;
		doLogic = false;
	}

	SelectButton();

	for (int i = 0; i < buttons.Count(); i++)
	{
		buttons.At(i)->data->Update(dt);
	}
	for (int i = 0; i < checkBoxs.Count(); i++)
	{
		checkBoxs.At(i)->data->Update(dt);
	}
	for (int i = 0; i < sliders.Count(); i++)
	{
		sliders.At(i)->data->Update(dt);
	}
	
	return true;
}

bool GuiManager::PostUpdate()
{
	for (int i = 0; i < buttons.Count(); i++)
	{
		buttons.At(i)->data->Draw();
	}
	for (int i = 0; i < checkBoxs.Count(); i++)
	{
		checkBoxs.At(i)->data->Draw();
	}
	for (int i = 0; i < sliders.Count(); i++)
	{
		sliders.At(i)->data->Draw();
	}

	return true;
}

bool GuiManager::CleanUp()
{
	app->tex->UnLoad(btnTextureAtlas);
	DeleteList();

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

void GuiManager::SelectButton()
{
	GamePad& pad = app->input->pads[0];
	if (app->input->GetKey(SDL_SCANCODE_DOWN) == KEY_DOWN || app->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_DOWN
		|| app->input->GetKey(SDL_SCANCODE_UP) == KEY_DOWN || app->input->GetKey(SDL_SCANCODE_LEFT) == KEY_DOWN
		|| pad.right || pad.left || pad.up || pad.down || pad.l_x || pad.l_y)
	{
		bool isFocused = false;
		int i = 0;
		for (i; i < controls.Count(); i++)
		{
			if (controls.At(i)->data->state == GuiControlState::FOCUSED)
			{
				isFocused = true;
				controls.At(i)->data->state = GuiControlState::NORMAL;
				if (app->input->GetKey(SDL_SCANCODE_DOWN) == KEY_DOWN || app->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_DOWN
					|| pad.right || pad.down || pad.l_x > 0.2 || pad.l_y < -0.2)
				{
					int j = i + 1;
					while (j != i)
					{
						if (j == controls.Count()) j = 0;
						if (controls.At(j)->data->state != GuiControlState::DISABLED)
						{
							controls.At(j)->data->state = GuiControlState::FOCUSED;
							break;
						}
						j++;
					}
				}
				if (app->input->GetKey(SDL_SCANCODE_UP) == KEY_DOWN || app->input->GetKey(SDL_SCANCODE_LEFT) == KEY_DOWN
					|| pad.left || pad.up || pad.l_x < -0.2 || pad.l_y > 0.2)
				{
					int j = i - 1;
					while (j != i)
					{
						if (j == -1) j = controls.Count() - 1;
						if (controls.At(j)->data->state != GuiControlState::DISABLED)
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
		//if (!isFocused && !menuSettings->GetActiveSettings()) controls.At(0)->data->state = GuiControlState::FOCUSED;
	}

	// If mouse is on any button, the rest state = NORMAL
	int mouseX, mouseY;
	app->input->GetMousePosition(mouseX, mouseY);

	for (int i = 0; i < controls.Count(); i++)
	{
		SDL_Rect bounds = controls.At(i)->data->bounds;
		if (((mouseX > bounds.x) && (mouseX < (bounds.x + bounds.w)) &&
			(mouseY > bounds.y) && (mouseY < (bounds.y + bounds.h))))
		{
			for (int j = 0; j < controls.Count(); j++)
			{
				if (controls.At(j)->data->state != GuiControlState::DISABLED)
					controls.At(j)->data->state = GuiControlState::NORMAL;
			}
			if (controls.At(i)->data->state != GuiControlState::DISABLED)
				controls.At(i)->data->state = GuiControlState::FOCUSED;

			if (controls.At(i)->data->state == GuiControlState::DISABLED)
				if (app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KeyState::KEY_DOWN || pad.a || app->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN)
					app->audio->PlayFx(btnDisabled);
			break;
		}
	}

}

