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
    SDL_Texture* btnTextureAtlas = app->guiManager->btnTextureAtlas;
    app->render->camera.x = app->render->camera.y = 0;

    app->sceneManager->SetEnemeyDetected(false);

    int id = app->entityManager->GetCurrentEntity()->entityData.id;
    if (id == 1)
    {
        // Load texPartners
       img = app->tex->Load("Assets/Textures/Backgrounds/background_1.png");

       // Add Enemies
       app->entityManager->AddEntity(BANDIT, 14, 17, 0, 1);
       app->entityManager->AddEntity(BANDIT, 11, 15, 0, 1);
       app->entityManager->AddEntity(BANDIT, 11, 19, 0, 1);
       app->entityManager->AddEntity(BANDIT, 9, 17, 0, 1);
    }
    if (id == 2)
    {
        // Load textures
        img = app->tex->Load("Assets/Textures/Backgrounds/background_1.png");

        // Add Enemies
        app->entityManager->AddEntity(BANDIT, 14, 17, 0, 1);
        app->entityManager->AddEntity(BANDIT, 11, 15, 0, 1);
    }

    // Partners
    int num = app->player->GetNumPartners();
    for (int i = 0; i < num; i++)
    {
        switch (app->player->GetPartners()[i].type)
        {
        case KEILER:
            break;
        case ISRRA:
            break;
        case BRENDA:
            break;
        default:
            break;
        }
    }

    enemies = app->entityManager->spawnQueue;

    // Inicialize the stats
    float strong = 1;
    for (int i = 0; i < enemies.Count(); i++)
    {
        int level = enemies.At(i)->data->entityData.level;
        // Add switch for assign strong, if is a type enemy strong = x, if is other type strong = y
        switch (enemies.At(i)->data->entityData.type)
        {
        case BANDIT:
            strong = 1;
            break;
        case FIGHTER:
            strong = 1.1;
            break;
        case SAPLING:
            strong = 1.2;
            break;
        default:
            break;
        }
        enemies.At(i)->data->stats.attack = ((2 * level + 7) * 0.75)* strong;
        enemies.At(i)->data->stats.defense = ((2 * level + 7) * 0.8) * strong;
        enemies.At(i)->data->stats.health = ((2.5 * level + 7.5) * 1.5) * strong;
        enemies.At(i)->data->stats.maxHealth = enemies.At(i)->data->stats.health;
        enemies.At(i)->data->stats.mana = (enemies.At(i)->data->stats.health / 2) * strong;
        enemies.At(i)->data->stats.speed = ((2.5 * level + 7.5) * 0.9) * strong;
        enemies.At(i)->data->stats.exp = (sqrt(CalculateExp(level)) / 3) * strong;
    }
    app->entityManager->spawnQueue = enemies;

    // Gui Buttons
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

    // Colors
    green.r = 0; green.g = 187; green.b = 45;
    yellow.r = 229; yellow.g = 190; yellow.b = 1;
    red.r = 203; red.g = 50; red.b = 52;

    return true;
}

bool SceneBattle::PreUpdate()
{
    return true;
}

bool SceneBattle::Update(float dt)
{
    //for (int i = 0; i < enemies.Count(); i++)
    //{
    //    enemies.At(i)->data->stats.health -= dt*2;
    //}
    
    return true;
}

void SceneBattle::AssignEntities()
{
    enemies = app->entityManager->entities;
    assigneDone = true;
}
bool SceneBattle::PostUpdate()
{
    if (!assigneDone)AssignEntities();

    app->render->DrawTexture(img, 0, 0);

    // Draw Bar lives
    for (int i = 0; i < enemies.Count(); i++)
    {
        app->entityManager->entities;
        int posX = (int)enemies.At(i)->data->entityData.position.x + enemies.At(i)->data->entityData.pointsCollision[0].x + enemies.At(i)->data->entityData.centerPoint.x;
        rec = { posX - 40, (int)enemies.At(i)->data->entityData.position.y, 80, 16};
        live = rec;
        live.w = enemies.At(i)->data->stats.health * rec.w / enemies.At(i)->data->stats.maxHealth;

        if(live.w > rec.w / 2) app->render->DrawRectangle(live, green.r, green.g, green.b);
        if(live.w < rec.w / 2) app->render->DrawRectangle(live, yellow.r, yellow.g, yellow.b);
        if(live.w < rec.w / 4) app->render->DrawRectangle(live, red.r, red.g, red.b);

        app->render->DrawRectangle(rec, 71, 75, 78, 255, false);
    }

    return true;
}

bool SceneBattle::CleanUp()
{
    bool ret = true;
    app->tex->UnLoad(img);
    app->entityManager->ClearList(ret);

    enemies = app->entityManager->entities;

    return ret;
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
            isContinue = true;
            TransitionToScene(SceneType::LEVEL1);
        }
    }
    default: break;
    }
    return true;
}

int SceneBattle::CalculateExp(int level)
{
    return (0.04 * (level * level * level) + 0.8 * (level * level) + 2 * level) * 3.5;
}
