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
#include "BaseEnemy.h"
#include "UIManager.h"
#include "DialogueManager.h"
#include "CombatManager.h"

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

	Engine::GetInstance().dialogueManager->LoadDialogs("src/", "Dialogs.xml");
	Engine::GetInstance().combatManager->LoadCombatData("src/", "CombatData.xml");

	LOG("Loading Scene");
	bool ret = true;
	return ret;
}

// Called before the first frame
bool Scene::Start()
{
	
	LoadScene(currentScene); // empieza en Intro Screen

	
	
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
	case SceneID::PAUSE:
		UpdatePause(dt);
		break;
	case SceneID::EXIT:
		UpdateExit(dt);
		break;
	case SceneID::RESUME:
		UpdateResume(dt);
		break;
	case SceneID::BACK:
		UpdateBack(dt);
		break;
	case SceneID::BATTLE:
		UpdateBattle(dt);
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
	case SceneID::PAUSE:
		PostUpdatePause();
		break;
	case SceneID::EXIT:
		PostUpdateExit();
		break;
	case SceneID::RESUME:
		PostUpdateResume();
		break;
	case SceneID::BACK:
		PostUpdateBack();
		break;
	case SceneID::BATTLE:
		PostUpdateBattle();
		break;
	default:
		break;
	}

	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_Q) == KEY_DOWN && Engine::GetInstance().input->GetKey(SDL_SCANCODE_P) == KEY_DOWN) {
		ret = false;
	}

	if (closeGame) {
		ret = false;
	}

	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN && (currentScene == SceneID::LEVEL1 || currentScene == SceneID::LEVEL1)) {

		gameScene = currentScene;
		ChangeScene(SceneID::PAUSE);
	}

	return ret;
}


SceneID Scene::GetCurrentScene() {

	return currentScene;
}

SceneID Scene::GetTimeScene() {

	return timeScene;
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
		HandleMainMenuUIEvents(uiElement);
		break;
	case SceneID::MULTIPLAYER:
		break;
	case SceneID::CREDITS:
		break;
	case SceneID::SOUND:
		HandleMainMenuUIEvents(uiElement);
		break;
	case SceneID::GRAFICS:
		HandleMainMenuUIEvents(uiElement);
		break;
	case SceneID::PAUSE:
		HandleMainMenuUIEvents(uiElement);
		break;
	case SceneID::EXIT:
		break;
	case SceneID::RESUME:
		break;
	case SceneID::BACK:
		break;
	case SceneID::BATTLE:
		HandleMainMenuUIEvents(uiElement);
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
	case SceneID::PAUSE:
		LoadPause();
		break;
	case SceneID::EXIT:
		LoadExit();
		break;
	case SceneID::RESUME:
		LoadResume();
		break;
	case SceneID::BACK:
		LoadBack();
	case SceneID::LEVEL1Combat:
		LoadCombatScene(SceneID::LEVEL1Combat);
		break;
	case SceneID::BATTLE:
		LoadBattle();
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
	case SceneID::PAUSE:
		UnloadPause();
		break;
	case SceneID::EXIT:
		UnloadExit();
		break;
	case SceneID::RESUME:
		UnloadResume();
		break;
	case SceneID::BACK:
		UnloadBack();
	case SceneID::LEVEL1Combat:
		UnloadCombatScene();
		break;
	case SceneID::BATTLE:
		UnloadBattle();
		break;
	}
	
}

// *********************************************
// INTRO SCREEN functions
// *********************************************


void Scene::LoadIntroScreen()
{
	teamImg = Engine::GetInstance().textures->Load("Assets/Textures/YieldToTheAcorn.png");
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
	
	if (splashTime == 0.0f && !sfxLogoPlayed) {
		Engine::GetInstance().audio->SetSFXVolume(0.2f);
		Engine::GetInstance().audio->PlayFx(s_epic_reveal, 0);
		sfxLogoPlayed = true;
	}

	if (teamImg != nullptr && splashTime < logoGameTimer) {
		Engine::GetInstance().render->DrawTexture(teamImg, WindowSize.getX()/2 - 360, 0);
	}

	splashTime += dt / 4000.0f;

	if (splashTime >= logoGameTimer && logoImg != nullptr) {

		if (!sfxTeamPlayed) {
			Engine::GetInstance().audio->SetSFXVolume(0.6f);
			Engine::GetInstance().audio->PlayFx(s_title_name, 0);
			sfxTeamPlayed = true;
		}
		splashTime += dt / 1000.0f;
		Engine::GetInstance().render->DrawTexture(logoImg, WindowSize.getX()/2 - 530, WindowSize.getY()/2 - 360);
	}

	if (splashTime >= logoTeamTimer) {
		sfxLogoPlayed = false;
		sfxTeamPlayed = false;
		ChangeScene(SceneID::MAIN_MENU);
	}

	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && splashTime <= logoTeamTimer) {
		sfxLogoPlayed = false;
		sfxTeamPlayed = false;
		splashTime = 0;
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
		timeScene = currentScene;
		ChangeScene(SceneID::OPTIONS);
		break;
	case 4:
		LOG("Main Menu: Multiplayer clicked");
		//ChangeScene(SceneID::MULTIPLAYER);
		break;
	case 5:
		LOG("Main Menu: Credits clicked");
		ChangeScene(SceneID::CREDITS);
		break;
	case 6:
		LOG("Options/Pause: Sounds clicked");
		timeScene = currentScene;
		ChangeScene(SceneID::SOUND);
		fromSG = true;
		break;
	case 7:
		LOG("Options/Pause: Grafics clicked");
		timeScene = currentScene;
		ChangeScene(SceneID::GRAFICS);
		fromSG = true;
		break;
	case 8:
		LOG("Pause: Exit clicked");
		ChangeScene(SceneID::EXIT);
		break;
	case 9:
		LOG("Pause: Resume clicked");
		ChangeScene(SceneID::LEVEL1);
		break;
	case 10:
		LOG("Back clicked");
		ChangeScene(SceneID::BACK);
		break;
	case 11:
		LOG("Attack clicked");
		Engine::GetInstance().combatManager->ShowAttackOptions(Engine::GetInstance().combatManager->combatState->player_id_selected);
		break;
	case 12:
		LOG("Item clicked");
		//Engine::GetInstance().combatManager->ShowItemOptions(Engine::GetInstance().combatManager->combatState->player_id_selected);
		break;
	case 13:
		LOG("Crazy clicked");
		//Engine::GetInstance().combatManager->ShowCrazyOptions(Engine::GetInstance().combatManager->combatState->player_id_selected);
		break;
	case 14:
		LOG("Scape clicked");
		ChangeScene(SceneID::LEVEL1);
		break;
	case 100:
		if (!isAudioMuted)
		{
			musicVolume = 0.0f;
			sfxVolume = 0.0f;
			Engine::GetInstance().audio->SetMusicVolume(0.0f);
			Engine::GetInstance().audio->SetSFXVolume(0.0f);
			LOG("All audio muted");
		}
		else
		{
			musicVolume = 1.0f;
			sfxVolume = 1.0f;
			Engine::GetInstance().audio->SetMusicVolume(1.0f);
			Engine::GetInstance().audio->SetSFXVolume(1.0f);
			LOG("All audio restored");
		}

		isAudioMuted = !isAudioMuted;
		break;

	case 202: //less volume music
		musicVolume = std::max(0.0f, musicVolume - 0.1f);
		Engine::GetInstance().audio->SetMusicVolume(musicVolume);
		break;

	case 203: // more volume music
		musicVolume = std::min(1.0f, musicVolume + 0.1f);
		Engine::GetInstance().audio->SetMusicVolume(musicVolume);
		break;

	case 212: // less volume sfx
		sfxVolume = std::max(0.0f, sfxVolume - 0.1f);
		Engine::GetInstance().audio->SetSFXVolume(sfxVolume);
		break;

	case 213: // more volume sfx
		sfxVolume = std::min(1.0f, sfxVolume + 0.1f);
		Engine::GetInstance().audio->SetSFXVolume(sfxVolume);
		break;

	default:
		break;
	}
}

// *********************************************
// Combat functions
// *********************************************
void Scene::LoadCombatScene(SceneID sceneid) {
	switch (sceneid) {
	case SceneID::LEVEL1Combat:
		//Load the background of the combat scene (players, enemies and background png)

		break;
	default:
		break;
	}

	/*Engine::GetInstance().map->LoadEntities(player);*/ //move the players and enemies to the combat position after loading them


}
void Scene::UnloadCombatScene() {

	Engine::GetInstance().uiManager->CleanUp();

	player.reset();

	Engine::GetInstance().entityManager->CleanUp();
}
void Scene::UpdateCombatScene(float dt) {


}
void Scene::PostUpdateCombatScene() {

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
	Engine::GetInstance().map->LoadEntities(player, SceneID::LEVEL1);
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
	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_T) == KEY_DOWN) {
		Engine::GetInstance().dialogueManager->StartDialogue(0, 1);
	}


	//provisional para bajar y subir la vida del player
	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_X) == KEY_DOWN) {
		player->HP --;
		LOG("TAKE DAMAGE --> player HP: %d", player->HP);
	}
	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_Z) == KEY_DOWN) {
		player->HP++;
		LOG("HEAL --> player HP: %d", player->HP);
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
		Engine::GetInstance().map->LoadEntities(player, SceneID::LEVEL1);
	}

	//L15 TODO 4: Call the function to save entities from the map
	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN) {
		Engine::GetInstance().map->SaveEntities(player, SceneID::LEVEL1);
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
	Engine::GetInstance().map->LoadEntities(player, SceneID::LEVEL2);
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

	SDL_Rect bt1Pos = { WindowSize.getX() / 2, WindowSize.getY() / 2, 120,20 };
	std::dynamic_pointer_cast<UIButton>(Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, 6, "Sound", bt1Pos, this));

	SDL_Rect bt2Pos = { WindowSize.getX() / 2, WindowSize.getY() / 2 + 30, 120,20 };
	std::dynamic_pointer_cast<UIButton>(Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, 7, "Grafics", bt2Pos, this));

	SDL_Rect bt3Pos = { WindowSize.getX() - 200, WindowSize.getY() - 50, 120,20 };
	std::dynamic_pointer_cast<UIButton>(Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, 10, "Back", bt3Pos, this));

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

	Engine::GetInstance().uiManager->CleanUp();

}

void Scene::UpdateMultiplayer(float dt)
{
	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_B) == KEY_DOWN) {
		ChangeScene(SceneID::MAIN_MENU);
	}
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
	else if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && splashTime <= logoTeamTimer) {


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
	Engine::GetInstance().uiManager->CleanUp();
	//UI Button

	SDL_Rect bt1Pos = { WindowSize.getX() - 200, WindowSize.getY() - 50, 120,20 };
	std::dynamic_pointer_cast<UIButton>(Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, 10, "Back", bt1Pos, this));

	//MUSIC
	SDL_Rect musicTitlePos = { WindowSize.getX() / 2 - 120, WindowSize.getY() / 4 - 30, 240, 30 };
	std::dynamic_pointer_cast<UIButton>(Engine::GetInstance().uiManager->CreateUIElement(
		UIElementType::BUTTON, 200, "MUSIC", musicTitlePos, this));

	SDL_Rect musicVolPos = { WindowSize.getX() / 2 - 100, WindowSize.getY() / 4 + 5, 200, 25 };
	std::dynamic_pointer_cast<UIButton>(Engine::GetInstance().uiManager->CreateUIElement(
		UIElementType::BUTTON, 201, ("Volumen: " + std::to_string(static_cast<int>(musicVolume * 100)) + "%").c_str(),
		musicVolPos, this));

	SDL_Rect musicMinusPos = { WindowSize.getX() / 2 - 110, WindowSize.getY() / 4 + 45, 45, 25 };
	std::dynamic_pointer_cast<UIButton>(Engine::GetInstance().uiManager->CreateUIElement(
		UIElementType::BUTTON, 202, " - ", musicMinusPos, this));

	SDL_Rect musicPlusPos = { WindowSize.getX() / 2 + 65, WindowSize.getY() / 4 + 45, 45, 25 };
	std::dynamic_pointer_cast<UIButton>(Engine::GetInstance().uiManager->CreateUIElement(
		UIElementType::BUTTON, 203, "+", musicPlusPos, this));

	//SFX
	SDL_Rect sfxTitlePos = { WindowSize.getX() / 2 - 120, WindowSize.getY() / 2 - 45, 230, 30 };
	std::dynamic_pointer_cast<UIButton>(Engine::GetInstance().uiManager->CreateUIElement(
		UIElementType::BUTTON, 210, " SFX ", sfxTitlePos, this));

	SDL_Rect sfxVolPos = { WindowSize.getX() / 2 - 100, WindowSize.getY() / 2 + 0, 200, 25 };
	std::dynamic_pointer_cast<UIButton>(Engine::GetInstance().uiManager->CreateUIElement(
		UIElementType::BUTTON, 211, ("Volumen: " + std::to_string(static_cast<int>(sfxVolume * 100)) + "%").c_str(),
		sfxVolPos, this));

	SDL_Rect sfxMinusPos = { WindowSize.getX() / 2 - 110, WindowSize.getY() / 2 + 40, 45, 25 };
	std::dynamic_pointer_cast<UIButton>(Engine::GetInstance().uiManager->CreateUIElement(
		UIElementType::BUTTON, 212, " - ", sfxMinusPos, this));

	SDL_Rect sfxPlus = { WindowSize.getX()/2 + 75, WindowSize.getY()/2 + 40, 40, 24 };
    std::dynamic_pointer_cast<UIButton>(Engine::GetInstance().uiManager->CreateUIElement(
        UIElementType::BUTTON, 213, "+", sfxPlus, this));

	//MUTE ALL
	SDL_Rect mutePos = { WindowSize.getX() / 2 - 70, WindowSize.getY() * 0.78f, 140, 28 };
	const char* muteText = isAudioMuted ? "Unmute All" : "Mute All";
	std::dynamic_pointer_cast<UIButton>(Engine::GetInstance().uiManager->CreateUIElement(
		UIElementType::BUTTON, 100, muteText, mutePos, this));
}

void Scene::UnloadSounds()
{

	Engine::GetInstance().uiManager->CleanUp();

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

	//UI Button

	SDL_Rect bt1Pos = { WindowSize.getX() - 200, WindowSize.getY() - 50, 120,20 };
	std::dynamic_pointer_cast<UIButton>(Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, 10, "Back", bt1Pos, this));


}

void Scene::UnloadGrafics()
{

	Engine::GetInstance().uiManager->CleanUp();

}

void Scene::UpdateGrafics(float dt)
{
}

void Scene::PostUpdateGrafics()
{
}


// *********************************************
// PAUSE functions
// *********************************************

void Scene::LoadPause()
{

	//UI Buttons

	SDL_Rect bt1Pos = { WindowSize.getX() / 2, WindowSize.getY() / 2, 120,20 };
	std::dynamic_pointer_cast<UIButton>(Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, 9, "Resume", bt1Pos, this));

	SDL_Rect bt2Pos = { WindowSize.getX() / 2, WindowSize.getY() / 2 +30, 120,20 };
	std::dynamic_pointer_cast<UIButton>(Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, 6, "Sound", bt2Pos, this));

	SDL_Rect bt3Pos = { WindowSize.getX() / 2, WindowSize.getY() / 2 + 60, 120,20 };
	std::dynamic_pointer_cast<UIButton>(Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, 7, "Grafics", bt3Pos, this));

	SDL_Rect bt4Pos = { WindowSize.getX() / 2, WindowSize.getY() / 2 + 90, 120,20 };
	std::dynamic_pointer_cast<UIButton>(Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, 8, "Exit", bt4Pos, this));

}

void Scene::UnloadPause()
{

	Engine::GetInstance().uiManager->CleanUp();

}

void Scene::UpdatePause(float dt)
{
}

void Scene::PostUpdatePause()
{
}


// *********************************************
// EXIT functions
// *********************************************

void Scene::LoadExit()
{
}

void Scene::UnloadExit()
{

	Engine::GetInstance().uiManager->CleanUp();

}

void Scene::UpdateExit(float dt)
{

	closeGame = true;

}

void Scene::PostUpdateExit()
{
}


// *********************************************
// RESUME functions
// *********************************************

void Scene::LoadResume()
{
}

void Scene::UnloadResume()
{

	Engine::GetInstance().uiManager->CleanUp();

}

void Scene::UpdateResume(float dt)
{

	ChangeScene(gameScene);

}

void Scene::PostUpdateResume()
{
}


// *********************************************
// BACK functions
// *********************************************

void Scene::LoadBack()
{
}

void Scene::UnloadBack()
{

	Engine::GetInstance().uiManager->CleanUp();

}

void Scene::UpdateBack(float dt)
{
	
	if (fromSG == true) { 
		ChangeScene(SceneID::MAIN_MENU);
		fromSG = false;
	}
	if(Engine::GetInstance().combatManager->choosingAtk == true)
	{
		ChangeScene(SceneID::BATTLE);
		Engine::GetInstance().combatManager->choosingAtk = false;
	}
	else {
		ChangeScene(timeScene);
		timeScene = SceneID::MAIN_MENU;
	}

}

void Scene::PostUpdateBack()
{
}


// *********************************************
// BATTLE functions
// *********************************************

void Scene::LoadBattle()
{

	//UI Buttons

	SDL_Rect bt1Pos = { WindowSize.getX() / 15, WindowSize.getY() - 200, 180,30 };
	std::dynamic_pointer_cast<UIButton>(Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, 11, "Attack", bt1Pos, this));

	SDL_Rect bt2Pos = { WindowSize.getX() / 15 + 200, WindowSize.getY() - 200, 180,30 };
	std::dynamic_pointer_cast<UIButton>(Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, 12, "Items", bt2Pos, this));

	SDL_Rect bt3Pos = { WindowSize.getX() / 15 + 400, WindowSize.getY() - 200, 180,30 };
	std::dynamic_pointer_cast<UIButton>(Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, 13, "GET CRAZY", bt3Pos, this));

	SDL_Rect bt4Pos = { WindowSize.getX() / 15 + 600, WindowSize.getY() - 200, 180,30 };
	std::dynamic_pointer_cast<UIButton>(Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, 14, "Scape", bt4Pos, this));



}

void Scene::UnloadBattle()
{

	Engine::GetInstance().uiManager->CleanUp();

}

void Scene::UpdateBattle(float dt)
{
}

void Scene::PostUpdateBattle()
{
}
