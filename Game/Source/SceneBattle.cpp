#include "SceneBattle.h"
#include "SceneManager.h"
#include "EntityManager.h"

#include "App.h"
#include "Audio.h"
#include "Textures.h"
#include "Render.h"
#include "Input.h"

#include "SceneManager.h"
#include "GuiManager.h"

#include "Defs.h"
#include "Log.h"

SceneBattle::SceneBattle()
{
    active = true;
    name.Create("sceneBattle");
}

SceneBattle::~SceneBattle()
{
}

bool SceneBattle::Awake()
{
    LOG("Loading SceneLose");
    bool ret = true;
    return ret;
}

bool SceneBattle::Start()
{
    //
    SDL_Texture* btnTextureAtlas = app->guiManager->btnTextureAtlas;
    app->render->camera.x = app->render->camera.y = 0;
    //
    app->sceneManager->SetEnemeyDetected(false);

    int id = app->entityManager->GetCurrentEntity()->entityData.id;
    if (id == 1)
    {
       img = app->tex->Load("Assets/Textures/Backgrounds/background_1.png");
    }
    //-----------------------
    int padding = 90;
    int yPosition = 20;
    btnAttack = new GuiButton(20, { WINDOW_W - 200 ,yPosition + (padding * 0),  183, 91 }, "ATTACK", RECTANGLE, btnTextureAtlas);
    btnAttack->SetObserver(this);
    app->guiManager->AddGuiButton(btnAttack);

    btnMagic = new GuiButton(21, { WINDOW_W - 200 , yPosition + (padding * 1),  183, 91 }, "MAGIC", RECTANGLE, btnTextureAtlas);
    btnMagic->SetObserver(this);
    app->guiManager->AddGuiButton(btnMagic);

    btnDefense = new GuiButton(22, { WINDOW_W - 200,yPosition + (padding * 2),  183, 91 }, "DEFENSE", RECTANGLE, btnTextureAtlas);
    btnDefense->SetObserver(this);
    app->guiManager->AddGuiButton(btnDefense);

    btnExit = new GuiButton(23, { WINDOW_W - 200, yPosition + (padding * 3),  183, 91 }, "EXIT", RECTANGLE, btnTextureAtlas);
    btnExit->SetObserver(this);
    app->guiManager->AddGuiButton(btnExit);
    //-----------------------

    return true;
}

bool SceneBattle::PreUpdate()
{
    return true;
}

bool SceneBattle::Update(float dt)
{
    /*if (app->input->GetKey(SDL_SCANCODE_A) == KEY_DOWN)
        TransitionToScene(SceneType::LEVEL1);*/
    return true;
}

bool SceneBattle::OnGuiMouseClickEvent(GuiControl* control)
{
	switch (control->type)
	{
	case GuiControlType::BUTTON:
	{

		if (control->id == 20)
		{
			
		}
		else if (control->id == 21)
		{
			
        }
        else if (control->id == 22)
        {

        }
        else if (control->id == 23)
        {
            TransitionToScene(SceneType::LEVEL1);
        }
	}
    default: break;
    }
    return true;
}


bool SceneBattle::PostUpdate()
{
    app->render->DrawTexture(img, -app->render->camera.x, -app->render->camera.y);
    return true;
}

bool SceneBattle::CleanUp()
{
    app->tex->UnLoad(img);
    isContinue = true;

    return true;
}

