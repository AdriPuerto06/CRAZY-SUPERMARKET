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

void BaseCompanion::Init(EntityType type, bool active, Vector2D position, const char* texturePath, int ID, int Dialogue_ID)
{
	this->type = type;
	this->position = position;
	this->active = active;
	this->HP = HP;
	this->ID = ID;
	this->texturePath = texturePath;
    this->Dialogue_ID = Dialogue_ID;
}

BaseCompanion::~BaseCompanion() {

}

bool BaseCompanion::Awake() {
	return true;
}

bool BaseCompanion::Start() {

	//texture
	texture = Engine::GetInstance().textures->Load(texturePath);
	texH = texture->h;
	texW = texture->w;
	//sensor
	pbody = Engine::GetInstance().physics->CreateRectangle(position.getX() + texW / 2, position.getY() + texH / 2, texH * 1.25, texW * 1.25, bodyType::KINEMATIC);
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
        /*LOG("pathTiles is empty");*/

        Engine::GetInstance().physics->SetLinearVelocity(
            pbody,
            0,
            0
        );
        /*LOG("PathTiles empty...");*/
        return;
    }

    // Get next tile in path
    Vector2D nextTile =
        pathfinding->GetPenultimateTile(pathfinding->pathTiles);

    // Convert TILE position to WORLD position
    Vector2D targetWorld =
        Engine::GetInstance().map->MapToWorld(
            (int)nextTile.getX(),
            (int)nextTile.getY()
        );

    // Center target tile
    targetWorld.setX(
        targetWorld.getX() +
        Engine::GetInstance().map->GetTileWidth() / 2
    );

    targetWorld.setY(
        targetWorld.getY() +
        Engine::GetInstance().map->GetTileHeight() / 2
    );

    // Current world position
    Vector2D currentPos(
        GetPosition().getX() + texW / 2,
        GetPosition().getY() + texH / 2
    );

    // Direction in WORLD coordinates
    float dx = targetWorld.getX() - currentPos.getX();
    float dy = targetWorld.getY() - currentPos.getY();

    // Distance to target
    float length = sqrt(dx * dx + dy * dy);

    // Already reached tile
    if (length < 4.0f)
    {
        Engine::GetInstance().physics->SetLinearVelocity(
            pbody,
            0,
            0
        );

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
    Engine::GetInstance().physics->SetLinearVelocity(
        pbody,
        velocity.x,
        velocity.y
    );

    /*LOG("Moving to tile (%f, %f) | velocity (%f, %f)",
        nextTile.getX(),
        nextTile.getY(),
        velocity.x,
        velocity.y);*/
}  

void BaseCompanion::PerformPathfinding()
{
    Vector2D pos = Vector2D(
        GetPosition().getX() + texW / 2,
        GetPosition().getY() + texH / 2
    );

    Vector2D tilePos =
        Engine::GetInstance().map->WorldToMap(
            (int)pos.getX(),
            (int)pos.getY()
        );

    Vector2D playerTilePos =
        Engine::GetInstance().map->WorldToMap(
            (int)Engine::GetInstance().scene->GetPlayerPosition().getX(),
            (int)Engine::GetInstance().scene->GetPlayerPosition().getY()
        );

    float distance = GetDistanceFromPlayer();

    /*LOG("Distance from player: %f", distance);*/

    // Already on same tile
    if (tilePos == playerTilePos)
    {
        /*LOG("Same tile as player");*/

        pathfinding->ResetPath(tilePos);

        return;
    }

    // FOLLOW PLAYER WHEN FAR AWAY
    if (distance > separationRange)
    {
        /*LOG("Generating path...");*/

        pathfinding->ResetPath(tilePos);

        int maxIterations = 200;
        int iterations = 0;

        while (iterations < maxIterations)
        {
            pathfinding->PropagateAStar(
                ASTAR_HEURISTICS::SQUARED
            );

            // PATH FOUND
            if (!pathfinding->pathTiles.empty())
            {
                /*LOG("PATH FOUND");*/

                break;
            }

            iterations++;
        }

        /*LOG("Generated path size: %d",
            (int)pathfinding->pathTiles.size());*/
    }
    else
    {
       /* LOG("Companion close enough -> no pathfinding");*/
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

	//Draw the player using the texture and the current animation frame
	Engine::GetInstance().render->DrawTexture(texture, x - texW / 2, y - texH / 2);
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
	pbody->SetPosition((int)(pos.getX()), (int)(pos.getY()));
}

void BaseCompanion::OnCollision(PhysBody* physA, PhysBody* physB) 
{
    if (Engine::GetInstance().dialogueManager->in_conversation) return;
    if (!(physB->ctype == ColliderType::PLAYER) || Engine::GetInstance().dialogueManager->showingButtonStart == true) return;
    if (Engine::GetInstance().scene->GetPlayer()->WizardJoined && this->Dialogue_ID == 101 || Engine::GetInstance().scene->GetPlayer()->CorneliusJoined && this->Dialogue_ID == 102) return;

    /*Vector2D buttonPos = Vector2D((position.getX() + texW / 2), (position.getY() + texH * 1.5));*/
    Vector2D buttonPos = Vector2D(500, 500);
    /*LOG("Vector 'cDT' size: %i", Engine::GetInstance().dialogueManager->currentDialogueTreesNPC.size());
    int dialogue_Tree = Engine::GetInstance().dialogueManager->currentDialogueTreesNPC[ID - 1];*/
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
	return Vector2D((float)x - texW / 2, (float)y - texH / 2);
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