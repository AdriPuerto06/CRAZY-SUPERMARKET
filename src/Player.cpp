#include "Player.h"
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
#include "Window.h"
#include "CombatManager.h"
#include "DialogueManager.h"
#include "UIManager.h"

Player::Player() : Entity(EntityType::PLAYER)
{
	name = "Player";
}

Player::~Player() {

}

bool Player::Awake() {
	return true;
}

bool Player::Start() {

	// load
	std::unordered_map<int, std::string> aliases = { {0,"idle"},{11,"move"},{22,"jump"} };
	anims.LoadFromTSX("Assets/Textures/PLayer2_Spritesheet.tsx", aliases);
	anims.SetCurrent("idle");

	//customMouse
	cursorSurface = IMG_Load("Assets/Textures/carrito.png");
	customCursor = SDL_CreateColorCursor(cursorSurface, 0, 0);
	SDL_SetCursor(customCursor);
	SDL_DestroySurface(cursorSurface);

	//L03: TODO 2: Initialize Player parameters
	texture = Engine::GetInstance().textures->Load("Assets/Textures/player2_spritesheet.png");

	// L08 TODO 5: Add physics to the player - initialize physics body
	//Engine::GetInstance().textures->GetSize(texture, texW, texH);
	texW = 32;
	texH = 32;
	pbody = Engine::GetInstance().physics->CreateCircle((int)position.getX(), (int)position.getY(), texW / 2, bodyType::DYNAMIC);

	// L08 TODO 6: Assign player class (using "this") to the listener of the pbody. This makes the Physics module to call the OnCollision method
	pbody->listener = this;

	// L08 TODO 7: Assign collider type
	pbody->ctype = ColliderType::PLAYER;

	//initialize audio effect
	pickCoinFxId = Engine::GetInstance().audio->LoadFx("Assets/Audio/Fx/coin-collision-sound-342335.wav");

	return true;
}

bool Player::Update(float dt)
{
	GetPhysicsValues();
	Move();
	Teleport();
	ApplyPhysics();
	GodMode();
	CheckDialogueAndCombatLogic();
	ShowMenu();
	Draw(dt);
	CenterCamera();
	return true;
}

void Player::CenterCamera() {

	int x, y;
	pbody->GetPosition(x, y);

	/*LOG("CenterCamera: pbody pos = %d, %d", x, y);*/

	Vector2D mapSize = Engine::GetInstance().map->GetMapSizeInPixels();
	int mapWidth = mapSize.getX();
	int mapHeight = mapSize.getY();

	int camX = x - Engine::GetInstance().render->camera.w / 2;
	int camY = y - Engine::GetInstance().render->camera.h / 2;

	int limitRight = mapWidth - Engine::GetInstance().render->camera.w;
	int limitDown = mapHeight - Engine::GetInstance().render->camera.h;
	// Clamp
	if (camX < 0) {
		camX = 0;
	}

	if (camX > limitRight) {
		camX = limitRight;
	}
	
	if (camY < 0) {
		camY = 0;
	}

	if (camY > limitDown) {
		camY = limitDown;
	}


	// Apply
	Engine::GetInstance().render->camera.x = -(int)camX;
	Engine::GetInstance().render->camera.y = -(int)camY;
	/*LOG("map: %d x %d", mapWidth, mapHeight);
	LOG("camera: %d x %d", Engine::GetInstance().render->camera.w, Engine::GetInstance().render->camera.h);*/
}

void Player::CheckDialogueAndCombatLogic()
{
	if (Engine::GetInstance().dialogueManager->in_conversation) can_Move = false;
	else can_Move = true;
}

void Player::Teleport() {
	// Teleport the player to a specific position for testing purposes
	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_T) == KEY_DOWN) {
		pbody->SetPosition(96, 96);
	}

	if (teleportCooldown > 0) {
		teleportCooldown--;
		/*LOG("Cooldown activo: %d", teleportCooldown);*/
		return;
	}

	
	int x, y;
	pbody->GetPosition(x, y);

	for (const auto& zone : Engine::GetInstance().map->teleportZones)
	{

		if (x >= zone.x && x <= zone.x + zone.width &&
			y >= zone.y && y <= zone.y + zone.height)
		{
			LOG("TELEPORT TRIGGERED to %s", zone.targetMap.c_str());
			pendingMapLoad = zone.targetMap;
			teleportCooldown = 120;
			return;
		}
	}
}

void Player::GetPhysicsValues() {
	// Read current velocity
	velocity = Engine::GetInstance().physics->GetLinearVelocity(pbody);
	velocity = { 0, 0 }; // Reset horizontal and vertical velocity by default
}

void Player::Move() {
	if (!can_Move) return;
	// Move left/right
	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
		velocity.x = -speed;
		anims.SetCurrent("move");
	}
	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
		velocity.x = speed;
		anims.SetCurrent("move");
	}
	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) {
		velocity.y = -speed;
		anims.SetCurrent("move");
	}
	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) {
		velocity.y = speed;
		anims.SetCurrent("move");
	}
}

void Player::ApplyPhysics() {

	// Apply velocity via helper
	Engine::GetInstance().physics->SetLinearVelocity(pbody, velocity);
}

void Player::GodMode() {

	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_G) == KEY_DOWN) {
		LOG("God mode switched: %i", godMode);
		godMode = !godMode;
		Engine::GetInstance().combatManager->godMode = godMode;
		//ideas provisionalse para el GodMode
		//desactivar colisiones

		//tecla abrir combat UI

		//all objects

	}

}

void Player::Draw(float dt) {

	anims.Update(dt);
	const SDL_Rect& animFrame = anims.GetCurrentFrame();

	// Update render position using your PhysBody helper
	int x, y;
	pbody->GetPosition(x, y);
	position.setX((float)x);
	position.setY((float)y);

	// L10: TODO 5: Draw the player using the texture and the current animation frame
	Engine::GetInstance().render->DrawTexture(texture, x - texW / 2, y - texH / 2, &animFrame);
}

void Player:: ShowMenu() {
	bool can_show_menu = Engine::GetInstance().scene->GetCurrentScene() == SceneID::LEVEL1 || Engine::GetInstance().scene->GetCurrentScene() == SceneID::LEVEL2 || Engine::GetInstance().scene->GetCurrentScene() == SceneID::LEVEL3;
	if(can_show_menu)
	if ((Engine::GetInstance().input->GetKey(SDL_SCANCODE_I) == KEY_DOWN) && !showingMenu) {
		Engine::GetInstance().itemManager->ShowInventoryOptions();
		showingMenu = true;
	}
	else if ((Engine::GetInstance().input->GetKey(SDL_SCANCODE_I) == KEY_DOWN) && showingMenu) {
		Engine::GetInstance().uiManager->CleanUp();
		showingMenu = false;
	}

}

bool Player::CleanUp()
{
	LOG("Cleanup player");
	Engine::GetInstance().textures->UnLoad(texture);
	Engine::GetInstance().physics->DeletePhysBody(pbody);
	SDL_DestroyCursor(customCursor);
	return true;
}

// L08 TODO 6: Define OnCollision function for the player. 
void Player::OnCollision(PhysBody* physA, PhysBody* physB) {
	switch (physB->ctype)
	{
	case ColliderType::PLATFORM:
		LOG("Collision PLATFORM");

		anims.SetCurrent("idle");
		break;
	case ColliderType::ITEM:
		LOG("Collision ITEM");
		Engine::GetInstance().audio->PlayFx(s_epic_reveal, 1);
		physB->listener->Destroy();
		break;
	case ColliderType::NPC:
		LOG("Collision NPC");
		break;
	case ColliderType::UNKNOWN:
		LOG("Collision UNKNOWN");
		break;
	default:
		break;
	}
}

void Player::OnCollisionEnd(PhysBody* physA, PhysBody* physB)
{
	switch (physB->ctype)
	{
	case ColliderType::PLATFORM:
		LOG("End Collision PLATFORM");
		break;
	case ColliderType::ITEM:
		LOG("End Collision ITEM");
		break;
	case ColliderType::NPC:
		LOG("End Collision NPC");
		break;
	case ColliderType::UNKNOWN:
		LOG("End Collision UNKNOWN");
		break;
	default:
		break;
	}
}

Vector2D Player::GetPosition() {
	int x, y;
	pbody->GetPosition(x, y);
	// Adjust for center
	return Vector2D((float)x - texW / 2, (float)y - texH / 2);
}

void Player::SetPosition(Vector2D pos) {
	pbody->SetPosition((int)(pos.getX() + texW / 2), (int)(pos.getY() + texH / 2));
}

bool Player::Destroy()
{
	LOG("Destroying Player");
	active = false;
	pendingToDelete = true;
	return true;
}
