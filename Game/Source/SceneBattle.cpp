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
    texPalyers = app->tex->Load("Assets/Textures/Characters/player_and_partners_idle_big.png");

    // Load Animations
    idleKenzie = new Animation;
    idleKeiler = new Animation;
    idleIsrra = new Animation;
    idleBrenda = new Animation;

    idleKenzie->loop = true;
    idleKeiler->loop = true;
    idleIsrra->loop = true;
    idleBrenda->loop = true;

    for (int i = 0; i < 6; i++)
    {
        idleKenzie->PushBack({ 64 * i, 0, 64, 92 });
    }
    for (int i = 0; i < 6; i++)
    {
        idleKeiler->PushBack({ 64 * i, 92, 64, 92 });
    }
    for (int i = 0; i < 6; i++)
    {
        idleIsrra->PushBack({ 64 * i, 184, 64, 92 });
    }
    for (int i = 0; i < 6; i++)
    {
        idleBrenda->PushBack({ 64 * i, 276, 64, 92 });
    }

    AddEntities();
    AddPartners();

    enemies = app->entityManager->spawnQueue;

    // Inicialize the stats
    InicializeStats();
    app->entityManager->spawnQueue = enemies;

    int num = enemies.Count();
    turnSort = new Entity[num];

    // Gui Buttons
    AddBattleMenu(btnTextureAtlas);

    // Colors
    green.r = 0; green.g = 187; green.b = 45;
    yellow.r = 229; yellow.g = 190; yellow.b = 1;
    red.r = 203; red.g = 50; red.b = 52;

    return true;
}

void SceneBattle::AddEntities()
{
    int id = app->entityManager->GetCurrentEntity()->entityData.id;
    int level = app->entityManager->GetCurrentEntity()->entityData.level;
    int randomLvl = 1;
    if (id == 1)
    {
        // Load textures
        img = app->tex->Load("Assets/Textures/Backgrounds/background_1.png");
        
        // Add Enemies
        while (randomLvl < 0) randomLvl = level + (rand() %3);
        app->entityManager->AddEntity(BANDIT, 14, 17, 0, randomLvl);
        while (randomLvl < 0) randomLvl = level + (rand() % 3);
        app->entityManager->AddEntity(BANDIT, 11, 15, 0, randomLvl);
        while (randomLvl < 0) randomLvl = level + (rand() % 3);
        app->entityManager->AddEntity(BANDIT, 11, 19, 0, randomLvl);
        while (randomLvl < 0) randomLvl = level + (rand() % 3);
        app->entityManager->AddEntity(BANDIT, 9, 17, 0, randomLvl);
    }
    else if (id == 2)
    {
        // Load textures
        img = app->tex->Load("Assets/Textures/Backgrounds/background_1.png");

        // Add Enemies
        while (randomLvl < 0) randomLvl = level + (rand() % 3);
        app->entityManager->AddEntity(BANDIT, 14, 17, 0, randomLvl);
        while (randomLvl < 0) randomLvl = level + (rand() % 3);
        app->entityManager->AddEntity(BANDIT, 11, 15, 0, randomLvl);
    }
}
void SceneBattle::AddPartners()
{
    // Partners and Player
    int num = app->player->GetNumPartners();
    app->entityManager->AddEntity(KENZIE_, 26, 13, 0, app->player->playerData.level);

    for (int i = 0; i < num; i++)
    {
        switch (app->player->GetPartners()[i].type)
        {
        case KEILER:
            app->entityManager->AddEntity(KEILER_, 29, 15, 0, app->player->GetPartners()[i].level);
            break;
        case ISRRA:
            app->entityManager->AddEntity(ISRRA_, 29, 19, 0, app->player->GetPartners()[i].level);
            break;
        case BRENDA:
            app->entityManager->AddEntity(BRENDA_, 26, 17, 0, app->player->GetPartners()[i].level);
            break; 
        default:
            break;
        }
    }
}
void SceneBattle::InicializeStats()
{
    float strong = 1;
    for (int i = 0; i < enemies.Count(); i++)
    {
        int level = enemies.At(i)->data->entityData.level;
        // Add switch for assign strong, if is a type enemy strong = x, if is other type strong = y
        TypeEntity pType = enemies.At(i)->data->entityData.type;
        switch (pType)
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
        if (pType == KENZIE_ || pType == KEILER_ || pType == ISRRA_ || pType == BRENDA_)
        {
            switch (pType)
            {
            case KENZIE_ :
                enemies.At(i)->data->stats.attack = 3.5 * level + 9.5;
                enemies.At(i)->data->stats.defense = 1.5 * level + 6.5;
                enemies.At(i)->data->stats.health = 2 * level + 6;
                enemies.At(i)->data->stats.maxHealth = enemies.At(i)->data->stats.health;
                enemies.At(i)->data->stats.mana = 3 * level +8;
                enemies.At(i)->data->stats.speed = 2.5 * level + 7.5;
                enemies.At(i)->data->entityData.currentAnimation = idleKenzie;
                break;
            case KEILER_:
                enemies.At(i)->data->stats.attack = 1.5 * level + 6.5;
                enemies.At(i)->data->stats.defense = 1.5 * level + 5.5;
                enemies.At(i)->data->stats.health = 4 * level + 10;
                enemies.At(i)->data->stats.maxHealth = enemies.At(i)->data->stats.health;
                enemies.At(i)->data->stats.mana = 2.5 * level + 7.5;
                enemies.At(i)->data->stats.speed = 2.5 * level + 8.5;
                enemies.At(i)->data->entityData.currentAnimation = idleKeiler;
                break;
            case ISRRA_:
                enemies.At(i)->data->stats.attack = 2 * level + 7;
                enemies.At(i)->data->stats.defense = 2 * level + 7;
                enemies.At(i)->data->stats.health = 2.5 * level + 7.5;
                enemies.At(i)->data->stats.maxHealth = enemies.At(i)->data->stats.health;
                enemies.At(i)->data->stats.mana = 3.5 * level + 8.5;
                enemies.At(i)->data->stats.speed = 2.5 * level + 7.5;
                enemies.At(i)->data->entityData.currentAnimation = idleIsrra;
                break;
            case BRENDA_:
                enemies.At(i)->data->stats.attack = 1.5 * level + 6.5;
                enemies.At(i)->data->stats.defense = 3.5 * level + 9.5;
                enemies.At(i)->data->stats.health = 3.5 * level + 9.5;
                enemies.At(i)->data->stats.maxHealth = enemies.At(i)->data->stats.health;
                enemies.At(i)->data->stats.mana = 1.5 * level + 6.5;
                enemies.At(i)->data->stats.speed = 1.5 * level + 6.5;
                enemies.At(i)->data->entityData.currentAnimation = idleBrenda;
                break;

            default:
                break;
            }
            enemies.At(i)->data->entityData.texture = texPalyers;
        }
        else
        {
            enemies.At(i)->data->stats.attack = ((2 * level + 7) * 0.75) * strong;
            enemies.At(i)->data->stats.defense = ((2 * level + 7) * 0.8) * strong;
            enemies.At(i)->data->stats.health = ((2.5 * level + 7.5) * 1.5) * strong;
            enemies.At(i)->data->stats.maxHealth = enemies.At(i)->data->stats.health;
            enemies.At(i)->data->stats.mana = (enemies.At(i)->data->stats.health / 2) * strong;
            enemies.At(i)->data->stats.speed = ((2.5 * level + 7.5) * 0.9) * strong;
            enemies.At(i)->data->stats.exp = (sqrt(CalculateExp(level)) / 3) * strong;
        }
    }
}
void SceneBattle::AddBattleMenu(SDL_Texture* btnTextureAtlas)
{
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
    SpeedAnimationCheck(dt);

    return true;
}
bool SceneBattle::PostUpdate()
{
    if (!assigneDone)AssignEntities();

    app->render->DrawTexture(img, 0, 0);

    // Draw Bar lives
    for (int i = 0; i < enemies.Count(); i++)
    {
        int posX = (int)enemies.At(i)->data->entityData.position.x + enemies.At(i)->data->entityData.pointsCollision[0].x + enemies.At(i)->data->entityData.centerPoint.x;
        rec = { posX - 40, (int)enemies.At(i)->data->entityData.position.y, 80, 16};
        live = rec;
        live.w = enemies.At(i)->data->stats.health * rec.w / enemies.At(i)->data->stats.maxHealth;

        if(live.w > rec.w / 2) app->render->DrawRectangle(live, green.r, green.g, green.b);
        if(live.w < rec.w / 2) app->render->DrawRectangle(live, yellow.r, yellow.g, yellow.b);
        if(live.w < rec.w / 4) app->render->DrawRectangle(live, red.r, red.g, red.b);

        app->render->DrawRectangle(rec, 71, 75, 78, 255, false);

    }
    for (int i = 0; i < partners.Count(); i++)
    {
        int posX = (int)partners.At(i)->data->entityData.position.x + partners.At(i)->data->entityData.centerPoint.x;
        
        int posY = (int)partners.At(i)->data->entityData.position.y - 30;
        rec = { posX - 40, posY, 80, 16 };
        live = rec;
        live.w = partners.At(i)->data->stats.health * rec.w / partners.At(i)->data->stats.maxHealth;

        if (live.w > rec.w / 2) app->render->DrawRectangle(live, green.r, green.g, green.b);
        if (live.w < rec.w / 2) app->render->DrawRectangle(live, yellow.r, yellow.g, yellow.b);
        if (live.w < rec.w / 4) app->render->DrawRectangle(live, red.r, red.g, red.b);

        app->render->DrawRectangle(rec, 71, 75, 78, 255, false);
    }

    return true;
}

bool SceneBattle::CleanUp()
{
    bool ret = true;
    app->tex->UnLoad(img);
    app->tex->UnLoad(texPalyers);

    RELEASE(idleKenzie);
    RELEASE(idleKeiler);
    RELEASE(idleIsrra);
    RELEASE(idleBrenda);
    delete[] turnSort;

    app->entityManager->ClearList(ret);
    enemies = app->entityManager->entities;
    partners = app->entityManager->partners;

    return ret;
}
void SceneBattle::SpeedAnimationCheck(float dt)
{
    idleKenzie->speed = dt * 6;
    idleKeiler->speed = dt * 6;
    idleIsrra->speed = dt * 6;
    idleBrenda->speed = dt * 6;
}

void SceneBattle::AssignEntities()
{
    enemies = app->entityManager->entities;
    partners = app->entityManager->partners;

    for (int i = 0; i < enemies.Count(); i++)
    {
        turnSort[i].entityData = enemies.At(i)->data->entityData;
        turnSort[i].stats = enemies.At(i)->data->stats;
    }
    int j = 0;
    tam = enemies.Count() + partners.Count();
    for (int i = enemies.Count(); i < tam; i++)
    {
        turnSort[i].entityData = partners.At(j)->data->entityData;
        turnSort[i].stats = partners.At(j)->data->stats;
        j++;
    }
    BubbleSort();

    assigneDone = true;
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
void SceneBattle::BubbleSort()
{
    int numSwaps = -1;
    while (numSwaps != 0)
    {
        numSwaps = 0;
        for (int i = 0; i < tam - 1; i++)
        {
            Entity aux;
            if (turnSort[i].stats.speed < turnSort[i + 1].stats.speed)
            {
                aux = turnSort[i];
                turnSort[i] = turnSort[i + 1];
                turnSort[i + 1] = aux;
                numSwaps++;
            }
        }
    }
}
void SceneBattle::DisplaceToLeft()
{
    Entity aux = turnSort[0];
    for (int i = 0; i < tam; i++)
    {
        turnSort[i] = turnSort[i + 1];
    }
    turnSort[tam - 1] = aux;
}
