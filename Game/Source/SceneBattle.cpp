#include "SceneBattle.h"
#include "SceneManager.h"

#include "App.h"
#include "Audio.h"
#include "Textures.h"
#include "Render.h"

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
    app->sceneManager->SetEnemeyDetected(false);

    int id = app->sceneManager->GetEnemyId();
    if (id == 0)
    {
       img = app->tex->Load("Assets/Textures/Backgrounds/background_1.png");
    }
    return true;
}

bool SceneBattle::PreUpdate()
{
    return true;
}

bool SceneBattle::Update(float dt)
{
    return true;
}

bool SceneBattle::PostUpdate()
{
    app->render->DrawTexture(img, app->render->camera.x, app->render->camera.y);
    return true;
}

bool SceneBattle::CleanUp()
{
    app->tex->UnLoad(img);
    return true;
}
