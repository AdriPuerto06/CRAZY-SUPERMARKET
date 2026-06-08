#include "BaseCompanion.h"
#include "Entity.h"
#include "Engine.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Physics.h"
#include "EntityManager.h"
#include "Map.h"
#include "CombatManager.h"
#include "DialogueManager.h"

BaseCompanion::BaseCompanion(){}

void BaseCompanion::Init(EntityType type, bool active, Vector2D position, const char* texturePath, const char* anim_tsxpath, int ID, int Dialogue_ID)
{
	this->type = type;
	this->position = position;
	this->active = active;
	this->HP = HP;
	this->ID = ID;
	this->texturePath = texturePath;
	this->anim_tsxpath = anim_tsxpath;
    this->Dialogue_ID = Dialogue_ID;
}

BaseCompanion::~BaseCompanion() {

}

bool BaseCompanion::Awake() {
	return true;
}

bool BaseCompanion::Start() {

	// texture
	texture = Engine::GetInstance().textures->Load(texturePath);
	Engine::GetInstance().textures->GetSize(texture, texW, texH);

	if (anim_tsxpath != nullptr && anim_tsxpath[0] != '\0') {
		std::unordered_map<int, std::string> emptyAliases;
		anims.LoadFromTSX(anim_tsxpath, emptyAliases);
	}

	
	anims.Update(0);
	const SDL_Rect& animFrame = anims.GetCurrentFrame();

	int frameW = texW;
	if (animFrame.w != 0)
	{
		frameW = animFrame.w;
	}
	int frameH = texH;
	if (animFrame.h != 0)
	{
		frameH = animFrame.h;
	}

	hitW = frameW;
	hitH = frameH;

	pbody = Engine::GetInstance().physics->CreateRectangle((int)(position.getX() + hitW / 2),(int)(position.getY() + hitH / 2),hitW,hitH,bodyType::KINEMATIC);
	pbody->ctype = ColliderType::COMPANION;
	pbody->listener = this;

	pathfinding = std::make_shared<Pathfinding>();
	
	return true;
}

bool BaseCompanion::Update(float dt)
{
    Draw(dt);

    return true;
}

void BaseCompanion::Move()
{
    if (pathfinding->pathTiles.empty())
    {
        Engine::GetInstance().physics->SetLinearVelocity(pbody, 0, 0);
        return;
    }

    Vector2D nextTile = pathfinding->GetPenultimateTile(pathfinding->pathTiles);


    Vector2D targetWorld = Engine::GetInstance().map->MapToWorld((int)nextTile.getX(), (int)nextTile.getY());

 
    targetWorld.setX(targetWorld.getX() + Engine::GetInstance().map->GetTileWidth() / 2);
    targetWorld.setY(targetWorld.getY() + Engine::GetInstance().map->GetTileHeight() / 2);

    Vector2D currentPos(GetPosition().getX() + texW / 2, GetPosition().getY() + texH / 2);

    float dx = targetWorld.getX() - currentPos.getX();
    float dy = targetWorld.getY() - currentPos.getY();

    float length = sqrt(dx * dx + dy * dy);

    // Already reached tile
    if (length < 4.0f)
    {
        Engine::GetInstance().physics->SetLinearVelocity(pbody, 0, 0);
        return;
    }

    // Normalize
    dx /= length;
    dy /= length;

    // Apply speed
    velocity.x = dx * speed;
    velocity.y = dy * speed;

    // Animations
    if (fabs(dx) > fabs(dy))
    {
        if (dx < 0)
            anims.SetCurrent("moveLeft");
        else
            anims.SetCurrent("moveRight");
    }
    else
    {
        if (dy < 0)
            anims.SetCurrent("moveUp");
        else
            anims.SetCurrent("moveDown");
    }

    // Apply movement
    Engine::GetInstance().physics->SetLinearVelocity(pbody, velocity.x, velocity.y);
}  

void BaseCompanion::PerformPathfinding()
{
    Vector2D pos = Vector2D(GetPosition().getX() + texW / 2, GetPosition().getY() + texH / 2);

    Vector2D tilePos = Engine::GetInstance().map->WorldToMap((int)pos.getX(), (int)pos.getY());

    Vector2D playerTilePos = Engine::GetInstance().map->WorldToMap(
        (int)Engine::GetInstance().scene->GetPlayerPosition().getX(),
        (int)Engine::GetInstance().scene->GetPlayerPosition().getY()
    );

    float distance = GetDistanceFromPlayer();

    // Already on same tile
    if (tilePos == playerTilePos)
    {
        pathfinding->ResetPath(tilePos);
        return;
    }

    // FOLLOW PLAYER WHEN FAR AWAY
    if (distance > separationRange)
    {
        pathfinding->ResetPath(tilePos);

        int maxIterations = 200;
        int iterations = 0;

        while (iterations < maxIterations)
        {
            pathfinding->PropagateAStar(ASTAR_HEURISTICS::SQUARED);

            // PATH FOUND
            if (!pathfinding->pathTiles.empty())
            {
                break;
            }

            iterations++;
        }
    }
}

void BaseCompanion::Draw(float dt) {

	anims.Update(dt);
	const SDL_Rect& animFrame = anims.GetCurrentFrame();

	// Update render position using your PhysBody helper
	int x, y;
	pbody->GetPosition(x, y);
	position.setX((float)x);
	position.setY((float)y);

	// Draw pathfinding debug
	//pathfinding->DrawPath();

	
	bool hasAnimFrame = (animFrame.w != 0 && animFrame.h != 0);
	if (texture != nullptr) {
		if (hasAnimFrame) {
			Engine::GetInstance().render->DrawTexture(texture, x - animFrame.w / 2, y - animFrame.h / 2, &animFrame);
		}
		else {
			Engine::GetInstance().render->DrawTexture(texture, x - texW / 2, y - texH / 2);
		}
	}
}

bool BaseCompanion::CleanUp()
{
	LOG("Cleanup Companion");
	Engine::GetInstance().textures->UnLoad(texture);
	Engine::GetInstance().physics->DeletePhysBody(pbody);
	return true;
}

bool BaseCompanion::Destroy()
{
	LOG("Destroying Companion");
	active = false;
	pendingToDelete = true;
	return true;
}

void BaseCompanion::SetPosition(Vector2D pos) {
	anims.Update(0);
	const SDL_Rect& animFrame = anims.GetCurrentFrame();
	int w = texW;
	if (animFrame.w != 0)
	{
		w = animFrame.w;
	}
	int h = texH;
	if (animFrame.h != 0)
	{
		h = animFrame.h;
	}
	pbody->SetPosition((int)(pos.getX() + w / 2.0f), (int)(pos.getY() + h / 2.0f));
}

void BaseCompanion::OnCollision(PhysBody* physA, PhysBody* physB) 
{
    if (Engine::GetInstance().dialogueManager->in_conversation) return;
    if (!(physB->ctype == ColliderType::PLAYER) || Engine::GetInstance().dialogueManager->showingButtonStart == true) return;
    if (Engine::GetInstance().scene->GetPlayer()->WizardJoined && this->Dialogue_ID == 101 || Engine::GetInstance().scene->GetPlayer()->CorneliusJoined && this->Dialogue_ID == 102) return;

    Vector2D buttonPos = Vector2D(500, 500);
    Engine::GetInstance().dialogueManager->ShowButtonStart(buttonPos, 0, Dialogue_ID);
    Engine::GetInstance().dialogueManager->showingButtonStart = true;
}

void BaseCompanion::OnCollisionEnd(PhysBody* physA, PhysBody* physB)
{
    if ((physB->ctype == ColliderType::PLAYER) && Engine::GetInstance().dialogueManager->showingButtonStart && !(Engine::GetInstance().dialogueManager->in_conversation))
    {
        Engine::GetInstance().dialogueManager->UnloadDialogueUI();
        Engine::GetInstance().dialogueManager->in_conversation = false;
        Engine::GetInstance().dialogueManager->showingButtonStart = false;
    }
}

Vector2D BaseCompanion::GetPosition()
{
	if (!pbody)
		return position;

	int x, y;
	pbody->GetPosition(x, y);
	anims.Update(0);
	const SDL_Rect& animFrame = anims.GetCurrentFrame();
	int w = texW;
	if (animFrame.w != 0)
	{
		w = animFrame.w;
	}
	int h = texH;
	if (animFrame.h != 0)
	{
		h = animFrame.h;
	}
	return Vector2D((float)x - w / 2.0f, (float)y - h / 2.0f);
}

float BaseCompanion::GetDistanceFromPlayer()
{
	Vector2D tilePos = Engine::GetInstance().map->WorldToMap(
		(int)GetPosition().getX(),
		(int)GetPosition().getY() + 1
		);
	Vector2D playerTilePos = Engine::GetInstance().map->WorldToMap(
		(int)Engine::GetInstance().scene->GetPlayerPosition().getX(),
		(int)Engine::GetInstance().scene->GetPlayerPosition().getY());

	return std::sqrt(std::pow(playerTilePos.getX() - tilePos.getX(), 2) +
		std::pow(playerTilePos.getY() - tilePos.getY(), 2));
}