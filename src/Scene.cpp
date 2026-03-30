#include "Engine.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "Scene.h"
#include "Log.h"
#include "Entity.h"
#include "EntityManager.h"
#include "Player.h"
#include "Map.h"
#include "Item.h"
#include "Enemy.h"
#include "UIManager.h"
#include "DialogueManager.h"

Scene::Scene() : Module()
{
	name = "scene";
}

// Destructor
Scene::~Scene()
{}

// Called before render is available
bool Scene::Awake()
{
	WindowSize = { Engine::GetInstance().window->GetBaseWidth(),   Engine::GetInstance().window->GetBaseHeight() };

	LOG("Loading Scene");
	bool ret = true;
	return ret;
}

// Called before the first frame
bool Scene::Start()
{
	
	LoadScene(currentScene); // empieza en Intro Screen

	Engine::GetInstance().dialogueManager->LoadDialogs("src/", "Dialogs.xml");
	
	return true;
}

// Called each loop iteration
bool Scene::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool Scene::Update(float dt)
{
	switch (currentScene)
	{
	case SceneID::INTRO_SCREEN:
		UpdateIntroScreen(dt);
		break;
	case SceneID::MAIN_MENU:
		UpdateMainMenu(dt);
		break;
	case SceneID::LEVEL1:
		UpdateLevel1(dt);
		break;
	case SceneID::LEVEL2:
		UpdateLevel2(dt);
		break;
	case SceneID::OPTIONS:
		UpdateOptions(dt);
		break;
	case SceneID::MULTIPLAYER:
		UpdateMultiplayer(dt);
		break;
	case SceneID::CREDITS:
		UpdateCredits(dt);
		break;
	case SceneID::SOUND:
		UpdateSounds(dt);
		break;
	case SceneID::GRAFICS:
		UpdateGrafics(dt);
		break;
	}

	return true;
}

bool Scene::PostUpdate()
{
	bool ret = true;

	switch (currentScene)
	{
	case SceneID::INTRO_SCREEN:
		break;
	case SceneID::MAIN_MENU:
		break;
	case SceneID::LEVEL1:
		PostUpdateLevel1();
		break;
	case SceneID::LEVEL2:
		break;
	case SceneID::OPTIONS:
		PostUpdateOptions();
		break;
	case SceneID::MULTIPLAYER:
		PostUpdateMultiplayer();
		break;
	case SceneID::CREDITS:
		PostUpdateCredits();
		break;
	case SceneID::SOUND:
		PostUpdateSounds();
		break;
	case SceneID::GRAFICS:
		PostUpdateGrafics();
		break;
	default:
		break;
	}

	if(Engine::GetInstance().input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	return ret;
}

bool Scene::OnUIMouseClickEvent(UIElement* uiElement)
{
	switch (currentScene)
	{
	case SceneID::INTRO_SCREEN:
		break;
	case SceneID::MAIN_MENU:
		HandleMainMenuUIEvents(uiElement);
		break;
	case SceneID::LEVEL1:
		break;
	case SceneID::LEVEL2:
		break;
	case SceneID::OPTIONS:
		break;
	case SceneID::MULTIPLAYER:
		break;
	case SceneID::CREDITS:
		break;
	case SceneID::SOUND:
		break;
	case SceneID::GRAFICS:
		break;
	default:
		break;
	}

	return true;
}

bool Scene::CleanUp()
{
	LOG("Freeing scene");
	UnloadCurrentScene();
	return true;
}

Vector2D Scene::GetPlayerPosition()
{
	if (player) return player->GetPosition();
	return Vector2D(0, 0);
}

// *********************************************
// Scene change functions
// *********************************************

void Scene::LoadScene(SceneID newScene)
{
	auto& engine = Engine::GetInstance();

	switch (newScene)
	{
	case SceneID::INTRO_SCREEN:
		LoadIntroScreen();
		break;
	case SceneID::MAIN_MENU:
		LoadMainMenu();
		break;

	case SceneID::LEVEL1:
		LoadLevel1();
		break;

	case SceneID::LEVEL2:
		LoadLevel2();
		break;

	case SceneID::OPTIONS:
		LoadOptions();
		break;

	case SceneID::MULTIPLAYER:
		LoadMultiplayer();
		break;

	case SceneID::CREDITS:
		LoadCredits();
		break;

	case SceneID::SOUND:
		LoadSounds();
		break;

	case SceneID::GRAFICS:
		LoadGrafics();
		break;

	}
}

void Scene::ChangeScene(SceneID newScene)
{
	UnloadCurrentScene();
	currentScene = newScene;
	LoadScene(currentScene);
}

void Scene::UnloadCurrentScene() {

	switch (currentScene)
	{
	case SceneID::INTRO_SCREEN:
		UnloadIntroScreen();
		break;
	case SceneID::MAIN_MENU:
		UnloadMainMenu();
		break;

	case SceneID::LEVEL1:
		UnloadLevel1();
		break;

	case SceneID::LEVEL2:
		UnloadLevel2();
		break;
	case SceneID::OPTIONS:
		UnloadOptions();
		break;

	case SceneID::MULTIPLAYER:
		UnloadMultiplayer();
		break;

	case SceneID::CREDITS:
		UnloadCredits();
		break;

	case SceneID::SOUND:
		UnloadSounds();
		break;

	case SceneID::GRAFICS:
		UnloadGrafics();
		break;
	}
	
}

// *********************************************
// INTRO SCREEN functions
// *********************************************


void Scene::LoadIntroScreen()
{
	teamImg = Engine::GetInstance().textures->Load("Assets/Textures/provisional.png");
	logoImg = Engine::GetInstance().textures->Load("Assets/Textures/CARRITO_LOGO.png");


	if (logoImg == nullptr || teamImg == nullptr)
	{
		LOG("ERROR: no se pudo cargar imagen/es.png");
		LOG("SDL error: %s", SDL_GetError());
	}

	splashTime = 0.0f;
}

void Scene::UpdateIntroScreen(float dt)
{
	if (!sfxLogoPlayed) {
		Engine::GetInstance().audio->PlayFx(s_epic_reveal, 0);
		sfxLogoPlayed = true;
	}

	if (logoImg != nullptr) {
		Engine::GetInstance().render->DrawTexture(logoImg, 0, 0);
	}

	splashTime += dt / 1000.0f;
	if (splashTime >= logoGameTimer && teamImg != nullptr) {

		if (!sfxTeamPlayed) {
			Engine::GetInstance().audio->PlayFx(s_title_name, 0);
			sfxTeamPlayed = true;
		}

		Engine::GetInstance().render->DrawTexture(teamImg, 0, 0);
	}

	if (splashTime >= logoTeamTimer || Engine::GetInstance().input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN) {
		sfxLogoPlayed = false;
		sfxTeamPlayed = false;
		ChangeScene(SceneID::MAIN_MENU);
	}
}

void Scene::UnloadIntroScreen()
{
	if (logoImg != nullptr)
	{
		Engine::GetInstance().textures->UnLoad(logoImg);
		logoImg = nullptr;
	}

	if (teamImg != nullptr)
	{
		Engine::GetInstance().textures->UnLoad(teamImg);
		teamImg = nullptr;
	}
}

// *********************************************
// MAIN MENU functions
// *********************************************

void Scene::LoadMainMenu() {

	/*Engine::GetInstance().audio->PlayMusic("Assets/Audio/Music/retro-gaming-short-248416.wav");	*/

	
	Engine::GetInstance().audio->PlayMusic(m_title, 0.0);
	// Instantiate a UIButton in the Scene
	SDL_Rect bt1Pos = { WindowSize.getX()/2, (WindowSize.getY() / 2) - 40, 120,20};
	std::dynamic_pointer_cast<UIButton>(Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, 1, "Start", bt1Pos, this));

	SDL_Rect bt2Pos = { WindowSize.getX() / 2, (WindowSize.getY() / 2) - 10, 120,20 };
	std::dynamic_pointer_cast<UIButton>(Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, 3, "Options", bt2Pos, this));

	SDL_Rect bt3Pos = { WindowSize.getX() / 2, (WindowSize.getY() / 2) + 20, 120,20 };
	std::dynamic_pointer_cast<UIButton>(Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, 4, "Multiplayer", bt3Pos, this));

	SDL_Rect bt4Pos = { WindowSize.getX() / 2, (WindowSize.getY() / 2) + 50, 120,20 };
	std::dynamic_pointer_cast<UIButton>(Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, 5, "Credits", bt4Pos, this));

}

void Scene::UnloadMainMenu() {
	// Clean up UI elements related to the main menu
	Engine::GetInstance().uiManager->CleanUp();	

}

void Scene::UpdateMainMenu(float dt)
{
	
}
void Scene::HandleMainMenuUIEvents(UIElement* uiElement)
{
	switch (uiElement->id)
	{
	case 1: 
		LOG("Main Menu: Start clicked");
		ChangeScene(SceneID::LEVEL1);
		break;
	case 2: 
		LOG("LvL2");
		ChangeScene(SceneID::LEVEL2);
		break;
	case 3: 
		LOG("Main Menu: Options clicked");
		ChangeScene(SceneID::OPTIONS);
		break;
	case 4:
		LOG("Main Menu: Multiplayer clicked");
		ChangeScene(SceneID::MULTIPLAYER);
		break;
	case 5:
		LOG("Main Menu: Credits clicked");
		ChangeScene(SceneID::CREDITS);
		break;
	case 6:
		LOG("Options: Sounds clicked");
		ChangeScene(SceneID::SOUND);
		break;
	case 7:
		LOG("Options: Grafics clicked");
		ChangeScene(SceneID::GRAFICS);
		break;
	default:
		break;
	}
}

// *********************************************
// Level 1 functions
// *********************************************

void Scene::LoadLevel1() {

	/*Engine::GetInstance().audio->PlayMusic("Assets/Audio/Music/level-iv-339695.wav");*/

	//Call the function to load the map & music
	Engine::GetInstance().map->Load("Assets/Maps/", "azotea.tmx");
	//Engine::GetInstance().audio->PlayMusic(m_level1, 0);

	//Call the function to load entities from the map
	Engine::GetInstance().map->LoadEntities(player);
	const char* text = "Hello player! Move with WASD.";
	Engine::GetInstance().render->StartTextDisplay(text, 100.0f);

	////Create a new item using the entity manager and set the position to (200, 672) to test
	//std::shared_ptr<Item> item = std::dynamic_pointer_cast<Item>(Engine::GetInstance().entityManager->CreateEntity(EntityType::ITEM));
	//item->position = Vector2D(200, 672);
	//item->Start(); //L17 Important call Start

	////Create a new enemy 
	//std::shared_ptr<Enemy> enemy1 = std::dynamic_pointer_cast<Enemy>(Engine::GetInstance().entityManager->CreateEntity(EntityType::ENEMY));
	//enemy1->position = Vector2D(384, 672);
	//enemy1->Start(); //L17 Important call Start
}

void Scene::UpdateLevel1(float dt) {

	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_2) == KEY_DOWN) {
		ChangeScene(SceneID::LEVEL2);
	}
	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_6) == KEY_DOWN) {
		Engine::GetInstance().window->SetFullSize();
		Engine::GetInstance().render->UpdateScale();
	}
	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_7) == KEY_DOWN) {
		Engine::GetInstance().window->SetWindowed(2);
		Engine::GetInstance().render->UpdateScale();
	}
	//ALWAYS USE UpdateScale() when changing window sizes
	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_T) == KEY_DOWN) {
		Engine::GetInstance().dialogueManager->StartDialogue(0, 1);
	}

}

void Scene::UnloadLevel1() {

	// Clean up UI elements related to the Level1
	auto& uiManager = Engine::GetInstance().uiManager;
	uiManager->CleanUp();

	// Reset player reference (sets the shared_ptr to nullptr)
	player.reset();

	// Clean up map and entities
	Engine::GetInstance().map->CleanUp();
	Engine::GetInstance().entityManager->CleanUp();

}

void  Scene::PostUpdateLevel1() {

	//L15 TODO 3: Call the function to load entities from the map
	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN) {
		Engine::GetInstance().map->LoadEntities(player);
	}

	//L15 TODO 4: Call the function to save entities from the map
	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN) {
		Engine::GetInstance().map->SaveEntities(player);
	}
}

// *********************************************
// Level 2 functions
// *********************************************

void Scene::LoadLevel2() {

	Engine::GetInstance().audio->PlayMusic(m_title, 0);

	//Call the function to load the map. 
	Engine::GetInstance().map->Load("Assets/Maps/", "Restaurant.tmx");

	//Call the function to load entities from the map
	Engine::GetInstance().map->LoadEntities(player);
}

void Scene::UpdateLevel2(float dt) {
	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_1) == KEY_DOWN) {
		ChangeScene(SceneID::LEVEL1);
	}
}

void Scene::UnloadLevel2() {

	// Clean up UI elements related to the Level2
	auto& uiManager = Engine::GetInstance().uiManager;
	uiManager->CleanUp();

	// Reset player reference (sets the shared_ptr to nullptr)
	player.reset();

	// Clean up map and entities
	Engine::GetInstance().map->CleanUp();
	Engine::GetInstance().entityManager->CleanUp();

}


// *********************************************
// OPTIONS functions
// *********************************************

void Scene::LoadOptions()
{

	//UI Buttons

	SDL_Rect bt5Pos = { WindowSize.getX() / 2, WindowSize.getY() / 2, 120,20 };
	std::dynamic_pointer_cast<UIButton>(Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, 6, "Sound", bt5Pos, this));

	SDL_Rect bt6Pos = { WindowSize.getX() / 2, WindowSize.getY() / 2, 120,20 };
	std::dynamic_pointer_cast<UIButton>(Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, 7, "Grafics", bt6Pos, this));


}

void Scene::UnloadOptions()
{

	Engine::GetInstance().uiManager->CleanUp();
}

void Scene::UpdateOptions(float dt)
{

	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_B) == KEY_DOWN) {
		ChangeScene(SceneID::MAIN_MENU);
	}

}

void Scene::PostUpdateOptions()
{
}


// *********************************************
// MULTIPLAYER functions
// *********************************************

void Scene::LoadMultiplayer()
{
}

void Scene::UnloadMultiplayer()
{
}

void Scene::UpdateMultiplayer(float dt)
{
}

void Scene::PostUpdateMultiplayer()
{
}


// *********************************************
// CREDITS functions
// *********************************************

void Scene::LoadCredits()
{
	


	creditsText = {
	"CRAZY SUPERMARKET",
	"",
	"",
	"Developers:",
	"",
	"Erik Argemi - Adria Puerto - Biel Cadenas - Pau Gallego - Adam Molina",
	"",
	"",
	"Art Producers & Organization:",
	"",
	"Pau Gallego - Biel Cadenas",
	"",
	"",
	"Music Creator & Organization:",
	"",
	"Adam Molina",
	"",
	"",
	"Thanks for playing aour crazy game!"
	};

	creditsY = WindowSize.getY();

}

void Scene::UnloadCredits()
{
}

void Scene::UpdateCredits(float dt)
{
	
	creditsY -= scrollSpeed * dt / 1000.0f;
	if (creditsY <= creditsTimer) {

		ChangeScene(SceneID::MAIN_MENU);
		creditsY = WindowSize.getY();
	}

}

void Scene::PostUpdateCredits()
{
	
	for (int i = 0; i < creditsText.size(); ++i)
	{

		int y = creditsY + i * lineHeight;
		SDL_Color color = { 255,0,255,255 };
		Engine::GetInstance().render->DrawText(creditsText[i].c_str(), WindowSize.getX() / 2.5, y, 450, 60, color);
	}

}


// *********************************************
// SOUNDS functions
// *********************************************

void Scene::LoadSounds()
{
}

void Scene::UnloadSounds()
{
}

void Scene::UpdateSounds(float dt)
{
}

void Scene::PostUpdateSounds()
{
}


// *********************************************
// GRAFICS functions
// *********************************************

void Scene::LoadGrafics()
{
}

void Scene::UnloadGrafics()
{
}

void Scene::UpdateGrafics(float dt)
{
}

void Scene::PostUpdateGrafics()
{
}

