#include "SceneBattle.h"
#include "SceneManager.h"
#include "EntityManager.h"

#include "App.h"
#include "Audio.h"
#include "Textures.h"
#include "Render.h"
#include "Input.h"
#include "Fonts.h"

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
    texPalyers = app->tex->Load("Assets/Textures/Characters/atlas_players_battle.png");
    texEnemies = app->tex->Load("Assets/Textures/Enemies/enemies_battle.png");
    app->audio->PlayMusic("Assets/Audio/Music/battle_music.ogg");

    // Load Animations
    LoadAnimations();

    // Add Enemies and Players
    AddEntities();
    AddPartners();

    enemies = app->entityManager->spawnQueue;
    enemiSelected = 0;

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
    blue.r = 37; blue.g = 40; blue.b = 80;
    cyan.r = 0; cyan.g = 183; cyan.b = 255;
    orange.r = 255; orange.g = 136; orange.b = 18;
    white.r = 255; white.g = 255; white.b = 255;

    god = false;
    return true;
}

void SceneBattle::LoadAnimations()
{
    for (int i = 0; i < 4; i++)
    {
        Animation* b = new Animation;
        b->loop = true;
        for (int j = 0; j < 6; j++)
        {
            b->PushBack({ 64 * j,92 * i, 64, 92 });
        }
        animationsPlayer.Add(b);
    }
    // Load Animation Bar Turn
    for (int i = 0; i < 4; i++)
    {
        Animation* b = new Animation;
        b->loop = true;
        for (int j = 0; j < 3; j++)
        {
            b->PushBack({ 384,32 * j + (96 * i), 32, 32 });
        }
        spritesBarTurn.Add(b);
    }
    // Load Animations Enemies
    int numSprites = 1;
    for (int i = 0; i < 12; i++)
    {
        Animation* b = new Animation;
        b->loop = true;
        if (i % 2) numSprites = 2;
        else numSprites = 4;
        if (i == 6) numSprites = 5;
        else if (i == 8) numSprites = 5;
        else if (i == 10) numSprites = 6;
        if ((i+1) % 4 == 0 && i != 0)
        {
            for (int j = 0; j < 4; j++)
            {
                b->PushBack({ 32 * j,64 * i, 32, 32 });
            }
            spritesBarTurn.Add(b);
        }
        else
        {
            for (int j = 0; j < numSprites; j++)
            {
                b->PushBack({ 128 * j,64 * i, 128, 64 });
            }
            animationsEnemies.Add(b);
        }
    }
}

void SceneBattle::AddEntities()
{
    int id = app->entityManager->GetCurrentEntity()->entityData.id;
    int level = app->entityManager->GetCurrentEntity()->entityData.level;
    int randomLvl = 1;
    switch (id)
    {
    case 1:
        // Load textures
        img = app->tex->Load("Assets/Textures/Backgrounds/background_1.png");

        // Add Enemies
        while (randomLvl < 0) randomLvl = level + (rand() % 3);
        app->entityManager->AddEntity(BANDIT, 13, 17, 0, randomLvl);
        while (randomLvl < 0) randomLvl = level + (rand() % 3);
        app->entityManager->AddEntity(BANDIT, 10, 15, 0, randomLvl);
        while (randomLvl < 0) randomLvl = level + (rand() % 3);
        app->entityManager->AddEntity(BANDIT, 10, 19, 0, randomLvl);
        while (randomLvl < 0) randomLvl = level + (rand() % 3);
        app->entityManager->AddEntity(BANDIT, 7, 17, 0, randomLvl);
        break;

    case 2:
        // Load textures
        img = app->tex->Load("Assets/Textures/Backgrounds/background_1.png");

        // Add Enemies
        while (randomLvl < 0) randomLvl = level + (rand() % 3);
        app->entityManager->AddEntity(FIGHTER, 13, 17, 0, randomLvl);
        while (randomLvl < 0) randomLvl = level + (rand() % 3);
        app->entityManager->AddEntity(BANDIT, 10, 15, 0, randomLvl);
        while (randomLvl < 0) randomLvl = level + (rand() % 3);
        app->entityManager->AddEntity(SAPLING, 10, 19, 0, randomLvl);
        break;

    case 3:
        // Load textures
        img = app->tex->Load("Assets/Textures/Backgrounds/background_1.png");

        // Add Enemies
        while (randomLvl < 0) randomLvl = level + (rand() % 3);
        app->entityManager->AddEntity(SAPLING, 13, 17, 0, randomLvl);
        while (randomLvl < 0) randomLvl = level + (rand() % 3);
        app->entityManager->AddEntity(FIGHTER, 10, 15, 0, randomLvl);
        break;
    default:
        break;
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
                enemies.At(i)->data->stats.exp = app->player->playerData.exp;
                enemies.At(i)->data->entityData.currentAnimation = animationsPlayer.At(0)->data;
                break;
            case KEILER_:
                enemies.At(i)->data->stats.attack = 1.5 * level + 6.5;
                enemies.At(i)->data->stats.defense = 1.5 * level + 5.5;
                enemies.At(i)->data->stats.health = 4 * level + 10;
                enemies.At(i)->data->stats.maxHealth = enemies.At(i)->data->stats.health;
                enemies.At(i)->data->stats.mana = 2.5 * level + 7.5;
                enemies.At(i)->data->stats.speed = 2.5 * level + 8.5;
                enemies.At(i)->data->stats.exp = app->player->GetPartners()[0].exp;
                enemies.At(i)->data->entityData.currentAnimation = animationsPlayer.At(1)->data;
                break;
            case ISRRA_:
                enemies.At(i)->data->stats.attack = 2 * level + 7;
                enemies.At(i)->data->stats.defense = 2 * level + 7;
                enemies.At(i)->data->stats.health = 2.5 * level + 7.5;
                enemies.At(i)->data->stats.maxHealth = enemies.At(i)->data->stats.health;
                enemies.At(i)->data->stats.mana = 3.5 * level + 8.5;
                enemies.At(i)->data->stats.speed = 2.5 * level + 7.5;
                enemies.At(i)->data->stats.exp = app->player->GetPartners()[1].exp;
                enemies.At(i)->data->entityData.currentAnimation = animationsPlayer.At(2)->data;
                break;
            case BRENDA_:
                enemies.At(i)->data->stats.attack = 1.5 * level + 6.5;
                enemies.At(i)->data->stats.defense = 3.5 * level + 9.5;
                enemies.At(i)->data->stats.health = 3.5 * level + 9.5;
                enemies.At(i)->data->stats.maxHealth = enemies.At(i)->data->stats.health;
                enemies.At(i)->data->stats.mana = 1.5 * level + 6.5;
                enemies.At(i)->data->stats.speed = 1.5 * level + 6.5;
                enemies.At(i)->data->stats.exp = app->player->GetPartners()[2].exp;
                enemies.At(i)->data->entityData.currentAnimation = animationsPlayer.At(3)->data;
                break;

            default:
                break;
            }
            enemies.At(i)->data->entityData.texture = texPalyers;
        }
        else
        {
            switch (pType)
            {
            case BANDIT:
                strong = 1;
                enemies.At(i)->data->entityData.currentAnimation = animationsEnemies.At(0)->data;
                break;
            case FIGHTER:
                strong = 1.1;
                enemies.At(i)->data->entityData.currentAnimation = animationsEnemies.At(3)->data;
                break;
            case SAPLING:
                strong = 1.2;
                enemies.At(i)->data->entityData.currentAnimation = animationsEnemies.At(6)->data;
                break;
            default:
                break;
            }
            enemies.At(i)->data->stats.attack = ((2 * level + 7) * 0.75) * strong;
            enemies.At(i)->data->stats.defense = ((2 * level + 7) * 0.8) * strong;
            enemies.At(i)->data->stats.health = ((2.5 * level + 7.5) * 1.5) * strong;
            enemies.At(i)->data->stats.maxHealth = enemies.At(i)->data->stats.health;
            enemies.At(i)->data->stats.mana = (enemies.At(i)->data->stats.health / 2) * strong;
            enemies.At(i)->data->stats.speed = ((2.5 * level + 7.5) * 0.9) * strong;
            enemies.At(i)->data->stats.exp = (sqrt(CalculateExp(level)) / 3) * strong;

            enemies.At(i)->data->entityData.texture = texEnemies;
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

    btnContinue = new GuiButton(24, { WINDOW_W/2 - 45, WINDOW_H/2 + 150,  180, 90 }, "CONTINUE", RECTANGLE, btnTextureAtlas);
    btnContinue->SetObserver(this);
    btnContinue->active = false;
    app->guiManager->AddGuiButton(btnContinue);

    btnGod = new GuiButton(25, { WINDOW_W -40, WINDOW_H-30,  10, 20 }, "GOD", RECTANGLE, btnTextureAtlas);
    btnGod->SetObserver(this);
    btnGod->active = true;
    app->guiManager->AddGuiButton(btnGod);

    //MenuMagic
    menuMagic = new GuiMenuMagic({ WINDOW_W / 2 -200, yPosition}, this);
    activeMenuMagic = false;
}

bool SceneBattle::PreUpdate()
{ 
    return true;
}

bool SceneBattle::Update(float dt_)
{
    /*for (int i = 0; i < enemies.Count(); i++)
    {
        enemies.At(i)->data->stats.health -= dt*2;
    }*/
    dt = dt_;
    SpeedAnimationCheck(dt_);

    //*******************
    switch (faseAction)
    {
    case SELECT_ACTION:

        if (turnSort[turn].entityData.type < 15)
        {
            faseAction = ENEMY_ATTACK;
        }
        else
        {
            int theHealth = 1;
            for (int i = 0; i < partners.Count(); i++) {
                if (turnSort[turn].entityData.positionInitial == partners.At(i)->data->entityData.positionInitial)
                    theHealth = partners.At(i)->data->stats.health;
            }
            if (theHealth <= 0)
            {
                faseAction = END_ACTION;
            }
        }
        break;

    case SELECT_ENEMI:
        btnAttack->state = GuiControlState::DISABLED;
        btnMagic->state = GuiControlState::DISABLED;
        btnDefense->state = GuiControlState::DISABLED;
        btnExit->state = GuiControlState::DISABLED;

        if (app->input->GetKey(SDL_SCANCODE_Q) == KEY_DOWN) {
            btnAttack->state = GuiControlState::NORMAL;
            btnMagic->state = GuiControlState::NORMAL;
            btnDefense->state = GuiControlState::NORMAL;
            btnExit->state = GuiControlState::NORMAL;
            faseAction = SELECT_ACTION;

        }

        if (app->input->GetKey(SDL_SCANCODE_E) == KEY_DOWN) {
            faseAction = DO_ACITON;

        }

        break;

    case DO_ACITON:

        if (magicInUse == nullptr)
        {
            float attack = turnSort[turn].stats.attack;
            enemies.At(enemiSelected)->data->stats.health -= attack;
        }
        else
        {
            enemies.At(enemiSelected)->data->stats.health -= magicInUse->damage;
            magicInUse = nullptr;

            activeMenuMagic = false;
            btnAttack->state = GuiControlState::DISABLED;
            btnMagic->state = GuiControlState::DISABLED;
            btnDefense->state = GuiControlState::DISABLED;
            btnExit->state = GuiControlState::DISABLED;
        }
        if (enemies.At(enemiSelected)->data->stats.health <= 0)
        {
            enemies.At(enemiSelected)->data->stats.health = 0;
        }
        faseAction = END_ACTION;
        break;

    case END_ACTION:
        moveBarTurn = true;
        faseAction = SELECT_ACTION;
        break;

    case ENEMY_ATTACK:
        btnAttack->state = GuiControlState::DISABLED;
        btnMagic->state = GuiControlState::DISABLED;
        btnDefense->state = GuiControlState::DISABLED;
        btnExit->state = GuiControlState::DISABLED;
        if (!moveBarTurn)
        {
            // Find heald of actual enemy
            int theHealth = 1;
            for (int i = 0; i < enemies.Count(); i++){
                if (turnSort[turn].entityData.positionInitial == enemies.At(i)->data->entityData.positionInitial)
                    theHealth = enemies.At(i)->data->stats.health;
            }

            if (theHealth > 0 && !god){
                int ally;
                if (magicInUse == nullptr)
                {
                    ally = (rand() % partners.Count());
                    if (partners.At(ally)->data->stats.health <= 0) {
                        for (int i = ally; i >= 0; i--) {
                            if (partners.At(i)->data->stats.health > 0) {
                                ally = i;
                                break;
                            }
                        }
                        if (partners.At(ally)->data->stats.health <= 0) {
                            for (int i = partners.Count() - 1; i >= 0; i--) {
                                if (partners.At(i)->data->stats.health > 0) {
                                    ally = i;
                                    break;
                                }
                            }
                        }
                    }
                    partners.At(ally)->data->stats.health -= turnSort[turn].stats.attack;
                }
                else
                {
                    ally = (rand() % partners.Count());
                    if (partners.At(ally)->data->stats.health <= 0) {
                        for (int i = ally; i > 0; i--) {
                            if (partners.At(i)->data->stats.health > 0) {
                                ally = i;
                                break;
                            }
                        }
                        if (partners.At(ally)->data->stats.health <= 0) {
                            for (int i = partners.Count() - 1; i > 0; i--) {
                                if (partners.At(i)->data->stats.health > 0) {
                                    ally = i;
                                    break;
                                }
                            }
                        }
                    }
                    partners.At((rand() % partners.Count()))->data->stats.health -= magicInUse->damage;
                }
                if (partners.At(ally)->data->stats.health <= 0){
                    partners.At(ally)->data->stats.health = 0;
                }
            }
            moveBarTurn = true;
        }

        faseAction = SELECT_ACTION;
        break;
    default:
        break;
    }

    // Win Condition
    if (!win) {
        int kills = 0;
        for (int i = 0; i < enemies.Count(); i++){
            if (enemies.At(i)->data->stats.health <= 0) {
                kills++;
            }
        }
        if (kills == enemies.Count()) {
            //TODO. WHEN YOU WIN??
            win = true;
            AbleDisableButtons();
        }
    }
    // Lose Condition
    int deads = 0;
    for (int i = 0; i < partners.Count(); i++){

        if (partners.At(i)->data->stats.health <= 0) deads++;
        if (deads == partners.Count()) {
            //TODO. WHEN YOU LOSE??
            TransitionToScene(SceneType::LOSE);
        }
    }

    return true;
}

bool SceneBattle::PostUpdate()
{
    if (!assigneDone)AssignEntities();

    app->render->DrawTexture(img, 0, 0);

    //TODO: Borrar recuadro Azul de parners una vez que se reconozca al que se lleva en el turno
    if (faseAction == SELECT_ACTION){
        if (turnSort[turn].entityData.type >= 15) {
        int posX = (int)turnSort[turn].entityData.position.x + 20;
        int posY = (int)turnSort[turn].entityData.position.y - 40;
        app->render->DrawRectangle({ posX, posY, 20, 20 }, blue.r, blue.g, blue.b, 255);
        }
    }
    if (god)
    {
        app->render->DrawRectangle({ WINDOW_W - 40, WINDOW_H - 30,  50, 50 }, orange.r, orange.g, orange.b, 255);
    }
    //Icon Enemy selected
    if (faseAction == SELECT_ENEMI) {

        if (app->input->GetKey(SDL_SCANCODE_A) == KEY_DOWN) { 
            enemiSelected--;
            if (enemiSelected < 0)
            {
                enemiSelected = enemies.Count() - 1;
            }
            if (enemies.At(enemiSelected)->data->stats.health <= 0){
                for (int i = enemiSelected; i >= 0; i--){
                    if (enemies.At(i)->data->stats.health > 0) {
                        enemiSelected = i;
                        break;
                    }
                }
                if (enemies.At(enemiSelected)->data->stats.health <= 0){
                    for (int i = enemies.Count() - 1; i >= 0; i--) {
                        if (enemies.At(i)->data->stats.health > 0) {
                            enemiSelected = i;
                            break;
                        }
                    }
                }
            }
        }
        if (app->input->GetKey(SDL_SCANCODE_D) == KEY_DOWN) { 
            enemiSelected++;
            if (enemiSelected >= enemies.Count())
            {
                enemiSelected = 0;
            }
            if (enemies.At(enemiSelected)->data->stats.health <= 0) {
                for (int i = enemiSelected; i < enemies.Count(); i++) {
                    if (enemies.At(i)->data->stats.health > 0) {
                        enemiSelected = i;
                        break;
                    }
                }
                if (enemies.At(enemiSelected)->data->stats.health <= 0) {
                    for (int i = 0; i < enemies.Count(); i++) {
                        if (enemies.At(i)->data->stats.health > 0) {
                            enemiSelected = i;
                            break;
                        }
                    }
                }
            }
        }

        int posX = (int)enemies.At(enemiSelected)->data->entityData.position.x + 60;
        int posY = (int)enemies.At(enemiSelected)->data->entityData.position.y - 60;
        app->render->DrawRectangle({ posX, posY, 20, 20 }, red.r, red.g, red.b, 255);
    }

    // Draw Bar lives
    if (!win)
    {
        for (int i = 0; i < enemies.Count(); i++)
        {
            posX = (int)enemies.At(i)->data->entityData.position.x + 2 * enemies.At(i)->data->entityData.pointsCollision[0].x + 2 * enemies.At(i)->data->entityData.centerPoint.x;
            posY = (int)enemies.At(i)->data->entityData.position.y - 30;
            rec = { posX - 45, posY, 90, 20 };
            live = rec;
            live.w = enemies.At(i)->data->stats.health * rec.w / enemies.At(i)->data->stats.maxHealth;

            sprintf_s(textLive, 8, "%d/%d", (int)enemies.At(i)->data->stats.health, enemies.At(i)->data->stats.maxHealth);
            DrawBarLives();
        }

        for (int i = 0; i < partners.Count(); i++)
        {
            posX = (int)partners.At(i)->data->entityData.position.x + partners.At(i)->data->entityData.centerPoint.x;
            posY = (int)partners.At(i)->data->entityData.position.y - 30;
            rec = { posX - 45, posY, 90, 20 };
            live = rec;
            live.w = partners.At(i)->data->stats.health * rec.w / partners.At(i)->data->stats.maxHealth;

            sprintf_s(textLive, 8, "%d/%d", (int)partners.At(i)->data->stats.health, partners.At(i)->data->stats.maxHealth);
            DrawBarLives();
        }
    }
   
    // Draw turn bar
    app->render->DrawRectangle({20, 30, 48, 64*tam-16}, blue.r, blue.g, blue.b, 100);
    DrawTurnBar();
    app->render->DrawRectangle({ 20, 30, 48, 64 * tam - 16 }, orange.r, orange.g, orange.b, 255, false);

    if (win)
    {
        app->render->DrawTextBox(WINDOW_W / 2 - 400, WINDOW_H / 2 - 200, 800, 400, { 24, 61, 172 }, { 97, 159, 207 }, { 0, 33, 78 }, app->guiManager->moonCorner, 200);
        for (int i = 0; i < app->player->GetNumPartners() + 1; i++)
        {
            posX = WINDOW_W / 2 - 400 + 40 + (i * 190);
            posY = WINDOW_H / 2 - 200 + 40;
            app->render->DrawRectangle({ posX, posY , 150, 150},0,33,78);
            // Draw Head Players
            // 

            // Draw Bar Lives
            rec = { posX, posY + 200, 150, 20 };
            live = rec;
            live.w = partners.At(i)->data->stats.health * rec.w / partners.At(i)->data->stats.maxHealth;
            sprintf_s(textLive, 8, "%d/%d", (int)partners.At(i)->data->stats.health, partners.At(i)->data->stats.maxHealth);
            DrawBarLives();

            // Draw Bar Experience
            rec.y += 30;
            live.y += 30;
            int exp = CalculateExp(partners.At(i)->data->entityData.level);
            live.w = partners.At(i)->data->stats.exp * rec.w / exp;
            sprintf_s(textExperience, 14, "%d/%d", (int)partners.At(i)->data->stats.exp, exp);
            DrawBarExperience();

            // Draw Level
            rec.y -= 60;
            live.y -= 60;
            sprintf_s(textLevel, 8, "Lvl.%d", partners.At(i)->data->entityData.level);
            int w = 0, h = 0;
            TTF_SizeText(app->sceneManager->guiFont, textLevel, &w, &h);
            app->fonts->BlitText(rec.x + rec.w / 2 - w / 2, rec.y + rec.h / 2 - h / 2, 0, textLevel, white);
           
        }
    }

    return true;
}

void SceneBattle::AbleDisableButtons()
{
    btnContinue->active = true;
    btnAttack->active = false;
    btnDefense->active = false;
    btnMagic->active = false;
    btnExit->active = false;
}

void SceneBattle::DrawBarLives()
{
    app->render->DrawRectangle(rec, 71, 75, 78, 150);

    if (live.w > rec.w) live.w = rec.w;
    if (live.w < 0)live.w = 0;
    if (live.w > rec.w / 2) app->render->DrawRectangle(live, green.r, green.g, green.b);
    else if (live.w < rec.w / 4) app->render->DrawRectangle(live, red.r, red.g, red.b);
    else if (live.w < rec.w / 2) app->render->DrawRectangle(live, yellow.r, yellow.g, yellow.b);

    int w = 0,  h = 0;
    TTF_SizeText(app->sceneManager->guiFont, textLive, &w, &h);
    app->fonts->BlitText(rec.x + rec.w / 2 - w / 2, rec.y + rec.h / 2 - h / 2, 0, textLive, white);

    app->render->DrawRectangle(rec, 71, 75, 78, 255, false);
}

void SceneBattle::DrawBarExperience()
{
    app->render->DrawRectangle(rec, 71, 75, 78, 150);

    if (live.w > rec.w) live.w = rec.w;
    if (live.w < 0)live.w = 0;
    app->render->DrawRectangle(live, cyan.r, cyan.g, cyan.b);

    int w = 0, h = 0;
    TTF_SizeText(app->sceneManager->guiFont, textExperience, &w, &h);
    app->fonts->BlitText(rec.x + rec.w / 2 - w / 2, rec.y + rec.h / 2 - h / 2, 0, textExperience, white);

    app->render->DrawRectangle(rec, 71, 75, 78, 255, false);
}

void SceneBattle::DrawTurnBar()
{
    if (moveBarTurn) offset -= dt * 60, numArrows = 0;
    else offset = 38, numArrows = 1;
    for (int i = 0; i < tam; i++)
    {
        switch (turnSort[i].entityData.type)
        {
        case KENZIE_:
            if (i == 0) {
                spritesBarTurn.At(0)->data->Update();
                face = spritesBarTurn.At(0)->data->GetCurrentFrame();
            }
            else face = spritesBarTurn.At(0)->data->frames[0];
            app->render->DrawTexture(texPalyers, 28, 64 * i + offset, &face);
            if (moveBarTurn && i == 0)
            {
                face = spritesBarTurn.At(0)->data->frames[0];
                app->render->DrawTexture(texPalyers, 28, 64 * tam + offset, &face);
            }
            break;
        case KEILER_:
            if (i == 0) {
                spritesBarTurn.At(1)->data->Update();
                face = spritesBarTurn.At(1)->data->GetCurrentFrame();
            }
            else face = spritesBarTurn.At(1)->data->frames[0];
            app->render->DrawTexture(texPalyers, 28, 64 * i + offset, &face);
            if (moveBarTurn && i == 0)
            {
                face = spritesBarTurn.At(1)->data->frames[0];
                app->render->DrawTexture(texPalyers, 28, 64 * tam + offset, &face);
            }
            break;
        case ISRRA_:
            if (i == 0) {
                spritesBarTurn.At(2)->data->Update();
                face = spritesBarTurn.At(2)->data->GetCurrentFrame();
            }
            else face = spritesBarTurn.At(2)->data->frames[0];
            app->render->DrawTexture(texPalyers, 28, 64 * i + offset, &face);
            if (moveBarTurn && i == 0)
            {
                face = spritesBarTurn.At(2)->data->frames[0];
                app->render->DrawTexture(texPalyers, 28, 64 * tam + offset, &face);
            }
            break;
        case BRENDA_:
            if (i == 0) {
                spritesBarTurn.At(3)->data->Update();
                face = spritesBarTurn.At(3)->data->GetCurrentFrame();
            }
            else face = spritesBarTurn.At(3)->data->frames[0];
            app->render->DrawTexture(texPalyers, 28, 64 * i + offset, &face);
            if (moveBarTurn && i == 0)
            {
                face = spritesBarTurn.At(3)->data->frames[0];
                app->render->DrawTexture(texPalyers, 28, 64 * tam + offset, &face);
            }
            break;

        case BANDIT:
            if (i == 0) {
                spritesBarTurn.At(4)->data->Update();
                face = spritesBarTurn.At(4)->data->GetCurrentFrame();
            }
            else face = spritesBarTurn.At(4)->data->frames[1];
            app->render->DrawTexture(texEnemies, 28, 64 * i + offset, &face);
            if (moveBarTurn && i == 0)
            {
                face = spritesBarTurn.At(4)->data->frames[0];
                app->render->DrawTexture(texEnemies, 28, 64 * tam + offset, &face);
            }
            break;
        case FIGHTER:
            if (i == 0) {
                spritesBarTurn.At(5)->data->Update();
                face = spritesBarTurn.At(5)->data->GetCurrentFrame();
            }
            else face = spritesBarTurn.At(5)->data->frames[1];
            app->render->DrawTexture(texEnemies, 28, 64 * i + offset, &face);
            if (moveBarTurn && i == 0)
            {
                face = spritesBarTurn.At(5)->data->frames[0];
                app->render->DrawTexture(texEnemies, 28, 64 * tam + offset, &face);
            }
            break;
        case SAPLING:
            if (i == 0) {
                spritesBarTurn.At(6)->data->Update();
                face = spritesBarTurn.At(6)->data->GetCurrentFrame();
            }
            else face = spritesBarTurn.At(6)->data->frames[1];
            app->render->DrawTexture(texEnemies, 28, 64 * i + offset, &face);
            if (moveBarTurn && i == 0)
            {
                face = spritesBarTurn.At(6)->data->frames[0];
                app->render->DrawTexture(texEnemies, 28, 64 * tam + offset, &face);
            }
            break;
        default:
            break;
        }
        face = { 416,0, 32,14 };
        if (i < tam - numArrows)app->render->DrawTexture(texPalyers, 28, 64 * i + 40 + offset, &face);
    }
    if (moveBarTurn)
    {
        face = { 20,0,48,30 };
        app->render->DrawTexture(img, 20, 0, &face);
        face = { 20,64 * tam - 16 + 30,48,64 };
        app->render->DrawTexture(img, 20, 64 * tam - 16 + 30, &face);
        btnAttack->state = GuiControlState::DISABLED;
        btnMagic->state = GuiControlState::DISABLED;
        btnDefense->state = GuiControlState::DISABLED;
        btnExit->state = GuiControlState::DISABLED;
    }

    if (offset < -64 + 38) {
        moveBarTurn = false, DisplaceToLeft(), faseAction = SELECT_ACTION;
        btnAttack->state = GuiControlState::NORMAL;
        btnMagic->state = GuiControlState::NORMAL;
        btnDefense->state = GuiControlState::NORMAL;
        btnExit->state = GuiControlState::NORMAL;
    }
}

bool SceneBattle::CleanUp()
{
    bool ret = true;
    app->tex->UnLoad(img);
    app->tex->UnLoad(texPalyers);
    app->tex->UnLoad(texEnemies);

    spritesBarTurn.Clear();
    animationsPlayer.Clear();
    animationsEnemies.Clear();

    delete[] turnSort;
    turnSort = nullptr;

    app->entityManager->ClearList(ret);
    enemies = app->entityManager->entities;
    partners = app->entityManager->partners;

    return ret;
}

void SceneBattle::SpeedAnimationCheck(float dt)
{
    for (int i = 0; i < spritesBarTurn.Count(); i++)
    {
        spritesBarTurn.At(i)->data->speed = dt * 6;
    }
    for (int i = 0; i < animationsPlayer.Count(); i++)
    {
        animationsPlayer.At(i)->data->speed = dt * 6;
    }
    for (int i = 0; i < animationsEnemies.Count(); i++)
    {
        animationsEnemies.At(i)->data->speed = dt * 2;
    }
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
        //ATTACK
        if (control->id == 20)
        {
            magicInUse = nullptr;
            faseAction = SELECT_ENEMI;
            for (int i = 0; i < enemies.Count(); i++)
            {
                if (enemies.At(i)->data->stats.health > 0) {
                    enemiSelected = i;
                    break;
                }
            }
        }
        //MAGIC
        else if (control->id == 21)
        {
            activeMenuMagic = true;

            menuMagic->SetIdTurn(turnSort[turn].entityData.type);

            menuMagic->AbleDisableMagic();
            menuMagic->MovePosition();

            btnAttack->state = GuiControlState::DISABLED;
            btnMagic->state = GuiControlState::DISABLED;
            btnDefense->state = GuiControlState::DISABLED;
            btnExit->state = GuiControlState::DISABLED;

        }
        //DEFENSE
        else if (control->id == 22)
        {
            moveBarTurn = true;
            switch (turnSort[turn].entityData.type)
            {
            case 15:
                LOG("Kenzie");
                break;
            case 16:
                LOG("Keiler");
                break;
            case 17:
                LOG("Isrra");
                break;
            case 18:
                LOG("Brenda");
                break;
            default:
                break;
            }
        }
        //EXIT
        else if (control->id == 23)
        {
            isContinue = true;
            AbleDisableButtons();
        }
        //Continue
        else if (control->id == 24)
        {
            TransitionToScene(SceneType::WIN);
        }
        else if (control->id == 25)
        {
            if (!god)
            {
                god = true;
            }else{
                god = false;
            }
        }
        //--MAGIC MENU--
        else if (control->id == 30)
        {
            activeMenuMagic = false;
            btnAttack->state = GuiControlState::NORMAL;
            btnMagic->state = GuiControlState::NORMAL;
            btnDefense->state = GuiControlState::NORMAL;
            btnExit->state = GuiControlState::NORMAL;
            menuMagic->AbleDisableMagic();

        }
        //Magic1
        else if (control->id == 31)
        {
            menuMagic->AbleDisableMagic();
            magicInUse = menuMagic->GetMagic(1);
            UseAMagic();
        }
        //Magic2
        else if (control->id == 32)
        {
            menuMagic->AbleDisableMagic();
            magicInUse = menuMagic->GetMagic(2);
            UseAMagic();
        }
        //Magic3
        else if (control->id == 33)
        {
            menuMagic->AbleDisableMagic();
            magicInUse = menuMagic->GetMagic(3);
            UseAMagic();
        }
        //Magic4
        else if (control->id == 34)
        {
            menuMagic->AbleDisableMagic();
            magicInUse = menuMagic->GetMagic(4);
            UseAMagic();
        }
    }
    default: break;
    }
    return true;
}

void SceneBattle::UseAMagic()
{
    if (magicInUse == nullptr) {
        // not exists magic
    }
    else {
        // Use magicInUse
        for (int i = 0; i < enemies.Count(); i++)
        {
            if (enemies.At(i)->data->stats.health > 0) {
                enemiSelected = i;
                break;
            }
        }
        faseAction = SELECT_ENEMI;
    }
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
    for (int i = 0; i < tam-1; i++)
    {
        turnSort[i] = turnSort[i + 1];
    }
    turnSort[tam - 1] = aux;
}
