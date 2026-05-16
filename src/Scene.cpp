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
#include "BaseEnemy.h"
#include "UIManager.h"
#include "DialogueManager.h"
#include "CombatManager.h"
#include "ItemManager.h"
#include "QuestManager.h"
#include "EventManager.h"

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
	Engine::GetInstance().itemManager->LoadItemsData("src/", "ItemsData.xml");
	Engine::GetInstance().questManager->LoadQuests("src/", "QuestsData.xml");

	//customMouse
	cursorSurface = IMG_Load("Assets/Textures/carrito.png");
	customCursor = SDL_CreateColorCursor(cursorSurface, 0, 0);
	SDL_SetCursor(customCursor);
	SDL_DestroySurface(cursorSurface);

	sceneStack.push(SceneID::MAIN_MENU);

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
	static_twicePressed = false;
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
	case SceneID::LEVEL3:
		UpdateLevel3(dt);
		break;
	case SceneID::LEVEL4:
		UpdateLevel4(dt);
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
	case SceneID::BATTLE:
		UpdateBattle(dt);
		break;
	case SceneID::ITEM:
		UpdateItem(dt);
		break;
	case SceneID::STATS:
		UpdateStats(dt);
		break;

	}

	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_6) == KEY_DOWN) {
		Engine::GetInstance().window->SetFullSize();
		Engine::GetInstance().render->UpdateScale();
	}
	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_7) == KEY_DOWN) {
		Engine::GetInstance().window->SetWindowed(2);
		Engine::GetInstance().render->UpdateScale();
	}

	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_M) == KEY_DOWN)
	{
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
		PostUpdateLevel2();
		break;
	case SceneID::LEVEL3:
		PostUpdateLevel3();
		break;
	case SceneID::LEVEL4:
		PostUpdateLevel4();
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

	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN && (currentScene == SceneID::LEVEL1 || currentScene == SceneID::LEVEL2 || currentScene == SceneID::LEVEL3 || currentScene == SceneID::LEVEL4)) {

		gameScene = currentScene;
		Engine::GetInstance().map->SaveEntities(player, currentScene);
		ChangeScene(SceneID::PAUSE);
		sceneStack.push(currentScene);
	}

	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_Z)) {
		LOG("LoadMainMenu");
		LoadMainMenu();
	}

	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_X)) {
		LOG("Change MainMenu");
		ChangeScene(SceneID::MAIN_MENU);
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
	case SceneID::LEVEL3:
		break;
	case SceneID::LEVEL4:
		break;
	case SceneID::OPTIONS: 
		HandleMainMenuUIEvents(uiElement);
		break;
	case SceneID::MULTIPLAYER:
		HandleMainMenuUIEvents(uiElement);
		break;
	case SceneID::CREDITS:
		HandleMainMenuUIEvents(uiElement);
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
	case SceneID::BATTLE:
		HandleMainMenuUIEvents(uiElement);
		break;
	case SceneID::ITEM:
		HandleMainMenuUIEvents(uiElement);
		break;
	case SceneID::STATS:
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
	SDL_DestroyCursor(customCursor);
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
	case SceneID::LEVEL3:
		LoadLevel3();
		break;
	case SceneID::LEVEL4:
		LoadLevel4();
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
	case SceneID::LEVEL1Combat:
		LoadCombatScene(SceneID::LEVEL1Combat);
		break;
	case SceneID::BATTLE:
		LoadBattle();
		break;
	case SceneID::ITEM:
		LoadItem();
		break;
	case SceneID::STATS:
		LoadStats();
		break;
	}
}

bool Scene::IsReloading()
{
	return Engine::GetInstance().map->isReloading;
}

std::shared_ptr<Player> Scene::GetPlayer()
{
	return player;
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

	case SceneID::LEVEL3:
		UnloadLevel3();
		break;

	case SceneID::LEVEL4:
		UnloadLevel4();
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
	case SceneID::LEVEL1Combat:
		UnloadCombatScene();
		break;
	case SceneID::BATTLE:
		UnloadBattle();
		break;
	case SceneID::ITEM:
		UnloadItem();
		break;
	case SceneID::STATS:
		UnloadStats();
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

	SDL_SetTextureBlendMode(teamImg, SDL_BLENDMODE_BLEND);
	SDL_SetTextureBlendMode(logoImg, SDL_BLENDMODE_BLEND);

	splashTime = 0.0f;
	teamFadeValue = 0.0f;
	logoFadeValue = 0.0f;
	logoFadeStarted = false;
}

void Scene::UpdateIntroScreen(float dt)
{
	if (splashTime == 0.0f && !sfxLogoPlayed) {
		Engine::GetInstance().audio->SetSFXVolume(0.2f);
		Engine::GetInstance().audio->PlayFx(s_epic_reveal, 0);
		sfxLogoPlayed = true;
	}

	if (teamImg != nullptr && splashTime < logoGameTimer) {
		teamFadeValue += (dt / 1000.0f) / 2.5f;
		if (teamFadeValue > 1.0f) teamFadeValue = 1.0f;

		float eased = teamFadeValue * teamFadeValue * (3.0f - 2.0f * teamFadeValue);
		Uint8 mod = (Uint8)(eased * 255);

		SDL_SetTextureColorMod(teamImg, mod, mod, mod);
		SDL_SetTextureAlphaMod(teamImg, mod);

		Engine::GetInstance().render->DrawTexture(teamImg, WindowSize.getX()/2 - 360, 0);
	}

	splashTime += dt / 4000.0f;

	if (splashTime >= logoGameTimer && logoImg != nullptr) {

		if (!sfxTeamPlayed) {
			Engine::GetInstance().audio->SetSFXVolume(0.6f);
			Engine::GetInstance().audio->PlayFx(s_title_name, 0);
			sfxTeamPlayed = true;
			logoFadeStarted = true;
		}

		if (logoFadeStarted) {
			logoFadeValue += (dt / 1000.0f) / 0.7f;
			if (logoFadeValue > 1.0f) logoFadeValue = 1.0f;
		}

		float eased = logoFadeValue * logoFadeValue * (3.0f - 2.0f * logoFadeValue);
		Uint8 mod = (Uint8)(eased * 255);

		SDL_SetTextureColorMod(logoImg, mod, mod, mod);
		SDL_SetTextureAlphaMod(logoImg, mod);

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
	//Load IMG Background
	SMImg = Engine::GetInstance().textures->Load("Assets/Textures/BackGrounds/normalMarket.png");

	//Load Buttos tex
	SDL_Texture* btnStartTex = Engine::GetInstance().textures->Load("Assets/Textures/UI/UI_Start_Normal.png");
	SDL_Texture* btnStartPressedTex = Engine::GetInstance().textures->Load("Assets/Textures/UI/UI_Start_Pressed.png");

	SDL_Texture* btnOptTex = Engine::GetInstance().textures->Load("Assets/Textures/UI/UI_Options_Normal.png");
	SDL_Texture* btnOptPressedTex = Engine::GetInstance().textures->Load("Assets/Textures/UI/UI_Options_Pressed.png");

	SDL_Texture* btnMltTex = Engine::GetInstance().textures->Load("Assets/Textures/UI/UI_Multi_Normal.png");
	SDL_Texture* btnMltPressedTex = Engine::GetInstance().textures->Load("Assets/Textures/UI/UI_Multi_Pressed.png");

	SDL_Texture* btnCredTex = Engine::GetInstance().textures->Load("Assets/Textures/UI/UI_Credits_Normal.png");
	SDL_Texture* btnCredPressedTex = Engine::GetInstance().textures->Load("Assets/Textures/UI/UI_Credits_Pressed.png");
		
	SDL_Texture* btnExitTex = Engine::GetInstance().textures->Load("Assets/Textures/UI/UI_Exit_Normal.png");
	SDL_Texture* btnExitPressedTex = Engine::GetInstance().textures->Load("Assets/Textures/UI/UI_Exit_Pressed.png");
	
	Engine::GetInstance().audio->PlayMusic(m_title, 0.0);

	// Instantiate a UIButton in the Scene
	SDL_Rect bt1Pos = { WindowSize.getX()/2 - 115, (WindowSize.getY() / 2) - 200, 229,90};
	CreateButton(btnStartTex, btnStartPressedTex, bt1Pos, 1);

	SDL_Rect bt2Pos = { WindowSize.getX() / 2 - 132, (WindowSize.getY() / 2) - 100, 264, 85 };
	CreateButton(btnOptTex, btnOptPressedTex, bt2Pos, 3);

	SDL_Rect bt3Pos = { WindowSize.getX() / 2 - 160, (WindowSize.getY() / 2), 320,85 };
	CreateButton(btnMltTex, btnMltPressedTex, bt3Pos, 4);

	SDL_Rect bt4Pos = { WindowSize.getX() / 2 - 130, (WindowSize.getY() / 2) + 100, 260,85 };
	CreateButton(btnCredTex, btnCredPressedTex, bt4Pos, 5);

	SDL_Rect bt5Pos = { WindowSize.getX() / 2 - 85, WindowSize.getY() / 2 + 200, 170,85 };
	CreateButton(btnExitTex, btnExitPressedTex, bt5Pos,8);
}

void Scene::UnloadMainMenu() {
	if (SMImg != nullptr)
	{
		Engine::GetInstance().textures->UnLoad(SMImg);
		SMImg = nullptr;
	}

	// Clean up UI elements related to the main menu
	Engine::GetInstance().uiManager->CleanUp();	
}

void Scene::UpdateMainMenu(float dt)
{
	if (SMImg != nullptr) {
		Engine::GetInstance().render->DrawTexture(SMImg, WindowSize.getX() / 2 - 720, WindowSize.getY() / 2 - 450);
	}
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
		sceneStack.push(currentScene);
		break;
	case 4:
		LOG("Main Menu: Multiplayer clicked");
		sceneStack.push(currentScene);
		ChangeScene(SceneID::MULTIPLAYER);
		break;
	case 5:
		LOG("Main Menu: Credits clicked");
		sceneStack.push(currentScene);
		ChangeScene(SceneID::CREDITS);
		break;
	case 6:
		LOG("Options/Pause: Sounds clicked");
		sceneStack.push(currentScene);
		ChangeScene(SceneID::SOUND);
		break;
	case 7:
		LOG("Options/Pause: Grafics clicked");
		sceneStack.push(currentScene);
		ChangeScene(SceneID::GRAFICS);
		break;
	case 8:
		LOG("Pause: Exit clicked");
		Engine::GetInstance().uiManager->CleanUp();
		closeGame = true;
		break;
	case 9:
		LOG("Pause: Resume clicked");
		Engine::GetInstance().uiManager->CleanUp();
		ChangeScene(gameScene);
		break;
	case 10:
		LOG("Back clicked");
		if (currentScene == SceneID::BATTLE) {
			ChangeScene(SceneID::BATTLE);
		}
		else {
			ChangeScene(sceneStack.pop());
		}
		break;
	case 11:
		LOG("Attack clicked");
		Engine::GetInstance().combatManager->ShowAttackOptions(Engine::GetInstance().combatManager->combatState->player_index_selected);
		break;
	case 12:
		LOG("Item clicked");
		Engine::GetInstance().combatManager->ShowItemOptions(Engine::GetInstance().combatManager->combatState->player_index_selected);
		break;
	case 13:
		Engine::GetInstance().combatManager->ChangePlayer();
		LOG("Current player ID: %i", Engine::GetInstance().combatManager->combatData->players[Engine::GetInstance().combatManager->combatState->player_index_selected].id);
		break;
	case 14:
		LOG("Scape clicked");
		ChangeScene(SceneID::LEVEL1);
		break;
	case 15:
		LOG("Main Menu clicked");
		ChangeScene(SceneID::MAIN_MENU);
		break;
	case 16:
		LOG("Grafics: Full Screen clicked");
		if (fullScreen == false) {
			Engine::GetInstance().window->SetFullSize();
			Engine::GetInstance().render->UpdateScale();
			fullScreen = true;
		}
		else if (fullScreen == true) {
			Engine::GetInstance().window->SetWindowed(2);
			Engine::GetInstance().render->UpdateScale();
			fullScreen = false;
		}
		break;
	case 17:
		Engine::GetInstance().vsync_Active = !Engine::GetInstance().vsync_Active;
		LOG("Grafics: VSync %i", Engine::GetInstance().vsync_Active);
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

	case 301: //slider music
	{
		UISlider* slider = (UISlider*)uiElement;

		musicVolume = slider->GetValue();

		Engine::GetInstance().audio->SetMusicVolume(musicVolume);
		break;
	}
	case 302: //slider vfx
	{
		UISlider* slider = (UISlider*)uiElement;
		sfxVolume = slider->GetValue();

		Engine::GetInstance().audio->SetSFXVolume(sfxVolume);
		break;
	}
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
	//Call the function to load the map & music
	Engine::GetInstance().map->Load("Assets/Maps/TiledFiles/", "azotea.tmx");
	Engine::GetInstance().audio->PlayMusic(m_roof_drums, 0.2, 0);

	//Call the function to load entities from the map
	Engine::GetInstance().map->LoadEntities(player, SceneID::LEVEL1);
}

void Scene::UpdateLevel1(float dt) {
	//Music will play drums first and then the main theme will loop
	if (!drumsFinished)
	{
		drumsTimer += dt;
		if (drumsTimer >= 4000.0f)
		{
			drumsFinished = true;
			Engine::GetInstance().audio->PlayMusic(m_roof, 0.0f, -1);
		}
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

	if (player && !player->pendingMapLoad.empty())
	{
		std::string target = player->pendingMapLoad;
		player->pendingMapLoad = "";

		if (target == "Restaurant.tmx") ChangeScene(SceneID::LEVEL2);
		else if (target == "Sala1.tmx")      ChangeScene(SceneID::LEVEL3);
		else if (target == "RestaurantDungeon.tmx") ChangeScene(SceneID::LEVEL4);
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
	Engine::GetInstance().map->Load("Assets/Maps/TiledFiles/", "Restaurant.tmx");

	//Call the function to load entities from the map
	Engine::GetInstance().map->LoadEntities(player, SceneID::LEVEL2);
}

void Scene::UpdateLevel2(float dt) {
	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_1) == KEY_DOWN) {
		ChangeScene(SceneID::LEVEL1);
	}
	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_3) == KEY_DOWN) {
		ChangeScene(SceneID::LEVEL3);
	}
	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_4) == KEY_DOWN) {
		Engine::GetInstance().entityManager->CleanUp();
	}

	if (player && !player->pendingMapLoad.empty())
	{
		std::string target = player->pendingMapLoad;
		player->pendingMapLoad = "";

		if (target == "azotea.tmx") ChangeScene(SceneID::LEVEL1);
		else if (target == "Sala1.tmx")  ChangeScene(SceneID::LEVEL3);
		else if (target == "RestaurantDungeon.tmx") ChangeScene(SceneID::LEVEL4);
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

void  Scene::PostUpdateLevel2() {
	//L15 TODO 3: Call the function to load entities from the map
	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN) {
		Engine::GetInstance().map->LoadEntities(player, SceneID::LEVEL2);
	}

	//L15 TODO 4: Call the function to save entities from the map
	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN) {
		Engine::GetInstance().map->SaveEntities(player, SceneID::LEVEL2);
	}
}

// *********************************************
// Level 3 functions
// *********************************************

void Scene::LoadLevel3() {
	Engine::GetInstance().audio->PlayMusic(m_title, 0);

	//Call the function to load the map. 
	Engine::GetInstance().map->Load("Assets/Maps/TiledFiles/", "Sala1.tmx");

	//Call the function to load entities from the map
	Engine::GetInstance().map->LoadEntities(player, SceneID::LEVEL3);
}

void Scene::UpdateLevel3(float dt) {
	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_1) == KEY_DOWN) {
		ChangeScene(SceneID::LEVEL1);
	}
	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_2) == KEY_DOWN) {
		ChangeScene(SceneID::LEVEL2);
	}
	if (player && !player->pendingMapLoad.empty())
	{
		std::string target = player->pendingMapLoad;
		player->pendingMapLoad = "";

		if (target == "azotea.tmx") ChangeScene(SceneID::LEVEL1);
		else if (target == "Restaurant.tmx") ChangeScene(SceneID::LEVEL2);
		else if (target == "RestaurantDungeon.tmx") ChangeScene(SceneID::LEVEL4);
	}
}

void Scene::UnloadLevel3() {
	// Clean up UI elements related to the Level2
	auto& uiManager = Engine::GetInstance().uiManager;
	uiManager->CleanUp();

	// Reset player reference (sets the shared_ptr to nullptr)
	player.reset();

	// Clean up map and entities
	Engine::GetInstance().map->CleanUp();
	Engine::GetInstance().entityManager->CleanUp();
}

void  Scene::PostUpdateLevel3() {
	//L15 TODO 3: Call the function to load entities from the map
	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN) {
		Engine::GetInstance().map->LoadEntities(player, SceneID::LEVEL3);
	}

	//L15 TODO 4: Call the function to save entities from the map
	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN) {
		Engine::GetInstance().map->SaveEntities(player, SceneID::LEVEL3);
	}
}

//Level 4
void Scene::LoadLevel4() {

	Engine::GetInstance().audio->PlayMusic(m_title, 0);

	//Call the function to load the map. 
	Engine::GetInstance().map->Load("Assets/Maps/TiledFiles/", "RestaurantDungeon.tmx");

	//Call the function to load entities from the map
	Engine::GetInstance().map->LoadEntities(player, SceneID::LEVEL4);
	Engine::GetInstance().eventManager->GetEvents();
}

void Scene::UpdateLevel4(float dt) {
	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_1) == KEY_DOWN) {
		ChangeScene(SceneID::LEVEL1);
	}
	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_2) == KEY_DOWN) {
		ChangeScene(SceneID::LEVEL2);
	}
	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_3) == KEY_DOWN) {
		ChangeScene(SceneID::LEVEL3);
	}

	if (player && !player->pendingMapLoad.empty())
	{
		std::string target = player->pendingMapLoad;
		player->pendingMapLoad = "";

		if (target == "azotea.tmx") ChangeScene(SceneID::LEVEL1);
		else if (target == "Restaurant.tmx")  ChangeScene(SceneID::LEVEL2);
		else if (target == "Sala1.tmx")  ChangeScene(SceneID::LEVEL3);
	}
}

void Scene::UnloadLevel4() {

	// Clean up UI elements related to the Level2
	auto& uiManager = Engine::GetInstance().uiManager;
	uiManager->CleanUp();

	// Reset player reference (sets the shared_ptr to nullptr)
	player.reset();

	// Clean up map and entities
	Engine::GetInstance().map->CleanUp();
	Engine::GetInstance().entityManager->CleanUp();

}

void  Scene::PostUpdateLevel4() {

	//L15 TODO 3: Call the function to load entities from the map
	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN) {
		Engine::GetInstance().map->LoadEntities(player, SceneID::LEVEL4);
	}

	//L15 TODO 4: Call the function to save entities from the map
	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN) {
		Engine::GetInstance().map->SaveEntities(player, SceneID::LEVEL4);
	}
}

// *********************************************
// OPTIONS functions
// *********************************************

void Scene::LoadOptions()
{
	//Load Background
	almacenIMG = Engine::GetInstance().textures->Load("Assets/Textures/BackGrounds/normal_almacen.png");

	//Load Buttos tex
	SDL_Texture* btnSndTex = Engine::GetInstance().textures->Load("Assets/Textures/UI/UI_Sound_Normal.png");
	SDL_Texture* btnSndPressedTex = Engine::GetInstance().textures->Load("Assets/Textures/UI/UI_Sound_Pressed.png");

	SDL_Texture* btnGfcTex = Engine::GetInstance().textures->Load("Assets/Textures/UI/UI_Grafics_Normal.png");
	SDL_Texture* btnGfcPressedTex = Engine::GetInstance().textures->Load("Assets/Textures/UI/UI_Grafics_Pressed.png");

	SDL_Texture* btnBckTex = Engine::GetInstance().textures->Load("Assets/Textures/UI/UI_Back_Normal.png");
	SDL_Texture* btnBckPressedTex = Engine::GetInstance().textures->Load("Assets/Textures/UI/UI_Back_Pressed.png");

	//UI Buttons
	SDL_Rect bt1Pos = { WindowSize.getX() / 2 - 315, WindowSize.getY() / 2, 215,85 };
	CreateButton(btnSndTex, btnSndPressedTex, bt1Pos, 6);
	SDL_Rect bt2Pos = { WindowSize.getX() / 2 + 80, WindowSize.getY() / 2, 280,85 };
	CreateButton(btnGfcTex, btnGfcPressedTex, bt2Pos, 7);
	SDL_Rect bt3Pos = { WindowSize.getX() - 200, WindowSize.getY() - 100, 135,68 };
	CreateButton(btnBckTex, btnBckPressedTex, bt3Pos, 10);
}

void Scene::UnloadOptions()
{
	if (almacenIMG != nullptr)
	{
		Engine::GetInstance().textures->UnLoad(almacenIMG);
		almacenIMG = nullptr;
	}
	Engine::GetInstance().uiManager->CleanUp();
}

void Scene::UpdateOptions(float dt)
{
	if (almacenIMG != nullptr) {
		Engine::GetInstance().render->DrawTexture(almacenIMG, WindowSize.getX() / 2 - 720, WindowSize.getY() / 2 - 450);
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
	SDL_Texture* btnBckTex = Engine::GetInstance().textures->Load("Assets/Textures/UI/UI_Back_Normal.png");
	SDL_Texture* btnBckPressedTex = Engine::GetInstance().textures->Load("Assets/Textures/UI/UI_Back_Pressed.png");

	SDL_Rect bt3Pos = { WindowSize.getX() - 200, WindowSize.getY() - 100, 135,68 };
	CreateButton(btnBckTex, btnBckPressedTex, bt3Pos, 10);

	teamImg = Engine::GetInstance().textures->Load("Assets/Textures/images (2).png");
	logoImg = Engine::GetInstance().textures->Load("Assets/Textures/CARRITO_LOGO.png");


	if (logoImg == nullptr || teamImg == nullptr)
	{
		LOG("ERROR: no se pudo cargar imagen/es.png");
		LOG("SDL error: %s", SDL_GetError());
	}

	splashTime = 0.0f;
}

void Scene::UnloadMultiplayer()
{
	Engine::GetInstance().uiManager->CleanUp();
}

void Scene::UpdateMultiplayer(float dt)
{
	if (splashTime == 0.0f && !sfxLogoPlayed) {
		Engine::GetInstance().audio->SetSFXVolume(1.0f);
		Engine::GetInstance().audio->PlayFx(jumpscare, 0);
		sfxLogoPlayed = true;
	}

	if (teamImg != nullptr && splashTime < logoGameTimer) {
		Engine::GetInstance().render->DrawTexture(teamImg, WindowSize.getX() / 2 - 50, 30);
	}

	splashTime += dt / 4000.0f;

	if (splashTime >= logoTeamTimer) {
		sfxLogoPlayed = false;
		sfxTeamPlayed = false;
		ChangeScene(SceneID::MAIN_MENU);
	}

	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_B) == KEY_DOWN && splashTime <= logoTeamTimer) {
		sfxLogoPlayed = false;
		sfxTeamPlayed = false;
		splashTime = 0;
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
	SDL_Texture* btnBckTex = Engine::GetInstance().textures->Load("Assets/Textures/UI/UI_Back_Normal.png");
	SDL_Texture* btnBckPressedTex = Engine::GetInstance().textures->Load("Assets/Textures/UI/UI_Back_Pressed.png");

	//UI Button
	SDL_Rect bt1Pos = { WindowSize.getX() - 200, WindowSize.getY() - 100, 135,68 };
	CreateButton(btnBckTex, btnBckPressedTex, bt1Pos, 10);

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

void Scene::UnloadCredits()
{
	Engine::GetInstance().uiManager->CleanUp();
}


// *********************************************
// SOUNDS functions
// *********************************************

void Scene::LoadSounds()
{
	Engine::GetInstance().uiManager->CleanUp();

	SDL_Texture* btnBckTex = Engine::GetInstance().textures->Load("Assets/Textures/UI/UI_Back_Normal.png");
	SDL_Texture* btnBckPressedTex = Engine::GetInstance().textures->Load("Assets/Textures/UI/UI_Back_Pressed.png");

	SDL_Texture* btnMuteTex = Engine::GetInstance().textures->Load("Assets/Textures/UI/UI_Mute_Normal.png");
	SDL_Texture* btnMutePressedTex = Engine::GetInstance().textures->Load("Assets/Textures/UI/UI_Mute_Pressed.png");

	//UI Button
	SDL_Rect bt1Pos = { WindowSize.getX() - 200, WindowSize.getY() - 100, 135,68 };
	CreateButton(btnBckTex, btnBckPressedTex, bt1Pos, 10);

	//MUSIC
	SDL_Rect musicTitlePos = { WindowSize.getX() / 2 - 120, WindowSize.getY() / 4 - 30, 240, 30 };
	std::dynamic_pointer_cast<UIButton>(Engine::GetInstance().uiManager->CreateUIElement(
		UIElementType::BUTTON, 200, "MUSIC", musicTitlePos, this));

	SDL_Rect musicVolPos = { WindowSize.getX() / 2 - 100, WindowSize.getY() / 4 + 5, 200, 25 };
	std::dynamic_pointer_cast<UIButton>(Engine::GetInstance().uiManager->CreateUIElement(
		UIElementType::BUTTON, 201, ("Volumen: " + std::to_string((int)(musicVolume * 100)) + "%").c_str(),
		musicVolPos, this));

	SDL_Rect musicMinusPos = { WindowSize.getX() / 2 - 110, WindowSize.getY() / 4 + 45, 45, 25 };
	std::dynamic_pointer_cast<UIButton>(Engine::GetInstance().uiManager->CreateUIElement(
		UIElementType::BUTTON, 202, " - ", musicMinusPos, this));

	SDL_Rect musicPlusPos = { WindowSize.getX() / 2 + 65, WindowSize.getY() / 4 + 45, 45, 25 };
	std::dynamic_pointer_cast<UIButton>(Engine::GetInstance().uiManager->CreateUIElement(
		UIElementType::BUTTON, 203, "+", musicPlusPos, this));

	//SLIDER
	SDL_Rect Slider = { WindowSize.getX() / 2 - 60, WindowSize.getY() / 4 + 75, 125, 35 };
	std::dynamic_pointer_cast<UIButton>(Engine::GetInstance().uiManager->CreateUIElement(
		UIElementType::SLIDER, 301, "", Slider, this, {}, musicVolume));

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

	SDL_Rect sfxPlus = { WindowSize.getX() / 2 + 75, WindowSize.getY() / 2 + 40, 40, 24 };
	std::dynamic_pointer_cast<UIButton>(Engine::GetInstance().uiManager->CreateUIElement(
		UIElementType::BUTTON, 213, "+", sfxPlus, this));

	//SLIDER
	SDL_Rect Slider2 = { WindowSize.getX() / 2 - 60, WindowSize.getY() / 4 + 255, 125, 35 };
	std::dynamic_pointer_cast<UIButton>(Engine::GetInstance().uiManager->CreateUIElement(
		UIElementType::SLIDER, 302, "", Slider2, this, {}, sfxVolume));

	//MUTE ALL
	SDL_Rect mutePos = { WindowSize.getX() / 2 - 70, WindowSize.getY() * 0.78f, 170, 51 };
	CreateButton(btnMuteTex, btnMutePressedTex, mutePos, 100);
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
	SDL_Texture* btnFSTex = Engine::GetInstance().textures->Load("Assets/Textures/UI/UI_FS_Normal.png");
	SDL_Texture* btnFSPressedTex = Engine::GetInstance().textures->Load("Assets/Textures/UI/UI_FS_Pressed.png");

	SDL_Texture* btnVSTex = Engine::GetInstance().textures->Load("Assets/Textures/UI/UI_VS_Normal.png");
	SDL_Texture* btnVSPressedTex = Engine::GetInstance().textures->Load("Assets/Textures/UI/UI_VS_Pressed.png");

	SDL_Texture* btnBckTex = Engine::GetInstance().textures->Load("Assets/Textures/UI/UI_Back_Normal.png");
	SDL_Texture* btnBckPressedTex = Engine::GetInstance().textures->Load("Assets/Textures/UI/UI_Back_Pressed.png");

	//UI Button
	SDL_Rect bt1Pos = { WindowSize.getX() / 2 - 144, WindowSize.getY()/ 2 - 50, 288,68 };
	CreateButton(btnFSTex, btnFSPressedTex, bt1Pos, 16);
	SDL_Rect bt2Pos = { WindowSize.getX() / 2 - 90, WindowSize.getY() / 2 + 50, 180,67 };
	CreateButton(btnVSTex, btnVSPressedTex, bt2Pos, 17);
	SDL_Rect bt3Pos = { WindowSize.getX() - 200, WindowSize.getY() - 100, 135,68 };
	CreateButton(btnBckTex, btnBckPressedTex, bt3Pos, 10);
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
	//Load Buttos tex
	SDL_Texture* btnResTex = Engine::GetInstance().textures->Load("Assets/Textures/UI/UI_Resume_Normal.png");
	SDL_Texture* btnResPressedTex = Engine::GetInstance().textures->Load("Assets/Textures/UI/UI_Resume_Pressed.png");

	SDL_Texture* btnSndTex = Engine::GetInstance().textures->Load("Assets/Textures/UI/UI_Sound_Normal.png");
	SDL_Texture* btnSndPressedTex = Engine::GetInstance().textures->Load("Assets/Textures/UI/UI_Sound_Pressed.png");

	SDL_Texture* btnGfcTex = Engine::GetInstance().textures->Load("Assets/Textures/UI/UI_Grafics_Normal.png");
	SDL_Texture* btnGfcPressedTex = Engine::GetInstance().textures->Load("Assets/Textures/UI/UI_Grafics_Pressed.png");

	SDL_Texture* btnExitTex = Engine::GetInstance().textures->Load("Assets/Textures/UI/UI_Exit_Normal.png");
	SDL_Texture* btnExitPressedTex = Engine::GetInstance().textures->Load("Assets/Textures/UI/UI_Exit_Pressed.png");

	//UI Buttons
	SDL_Rect bt1Pos = { WindowSize.getX() / 2 - 128, WindowSize.getY() / 2 - 150, 257,85 };
	CreateButton(btnResTex, btnResPressedTex, bt1Pos, 9);
	SDL_Rect bt2Pos = { WindowSize.getX() / 2 - 107, WindowSize.getY() / 2 - 50, 215,85 };
	CreateButton(btnSndTex, btnSndPressedTex, bt2Pos, 6);
	SDL_Rect bt3Pos = { WindowSize.getX() / 2 - 140, WindowSize.getY() / 2 + 50, 280,85 };
	CreateButton(btnGfcTex, btnGfcPressedTex, bt3Pos, 7);
	SDL_Rect bt4Pos = { WindowSize.getX() / 2 - 85, WindowSize.getY() / 2 + 150, 170,85 };
	CreateButton(btnExitTex, btnExitPressedTex, bt4Pos, 8);

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
// BATTLE functions
// *********************************************

void Scene::LoadBattle()
{
	//read enemy and player vector
	int actCombat = Engine::GetInstance().combatManager->combatData->fight_ID;

	Engine::GetInstance().audio->PlayMusic(m_battle, 0.2);
	//UI Buttons
	SDL_Rect bt1Pos = { WindowSize.getX() / 15, WindowSize.getY() - 200, 180,30 };
	std::dynamic_pointer_cast<UIButton>(Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, 11, "Attack", bt1Pos, this));

	SDL_Rect bt2Pos = { WindowSize.getX() / 15 + 200, WindowSize.getY() - 200, 180,30 };
	std::dynamic_pointer_cast<UIButton>(Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, 12, "Items", bt2Pos, this));

	SDL_Rect bt3Pos = { WindowSize.getX() / 15 + 400, WindowSize.getY() - 200, 180,30 };
	std::dynamic_pointer_cast<UIButton>(Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, 13, "Change player", bt3Pos, this));

	SDL_Rect bt4Pos = { WindowSize.getX() / 15 + 600, WindowSize.getY() - 200, 180,30 };
	std::dynamic_pointer_cast<UIButton>(Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, 14, "Scape", bt4Pos, this));
}

void Scene::UnloadBattle()
{
	Engine::GetInstance().combatManager->in_combat = false;
	Engine::GetInstance().uiManager->CleanUp();
}

void Scene::UpdateBattle(float dt)
{
}

void Scene::PostUpdateBattle()
{
}

// *********************************************
// Item functions
// *********************************************

void Scene::LoadItem()
{
	SDL_Texture* btnBckTex = Engine::GetInstance().textures->Load("Assets/Textures/UI/UI_Back_Normal.png");
	SDL_Texture* btnBckPressedTex = Engine::GetInstance().textures->Load("Assets/Textures/UI/UI_Back_Pressed.png");
	cajonTex = Engine::GetInstance().textures->Load("Assets/Textures/cajon_Items.png");

	SDL_Rect bt3Pos = { WindowSize.getX() - 200, WindowSize.getY() - 100, 135,68 };
	CreateButton(btnBckTex, btnBckPressedTex, bt3Pos, 10);

	Engine::GetInstance().itemManager->ShowPlayerItems();
}

void Scene::UpdateItem(float dt)
{
	Engine::GetInstance().render->DrawTexture(cajonTex, WindowSize.getX() / 2 + 450, WindowSize.getY() - 150);
}

void Scene::UnloadItem()
{
	Engine::GetInstance().uiManager->CleanUp();
}

// *********************************************
// Stats functions
// *********************************************

void Scene::LoadStats()
{
	SDL_Texture* btnBckTex = Engine::GetInstance().textures->Load("Assets/Textures/UI/UI_Back_Normal.png");
	SDL_Texture* btnBckPressedTex = Engine::GetInstance().textures->Load("Assets/Textures/UI/UI_Back_Pressed.png");
	bookTex = Engine::GetInstance().textures->Load("Assets/Textures/cajon_Items.png");

	SDL_Rect bt3Pos = { WindowSize.getX() - 200, WindowSize.getY() - 100, 135,68 };
	CreateButton(btnBckTex, btnBckPressedTex, bt3Pos, 10);
}

void Scene::UpdateStats(float dt)
{
	Engine::GetInstance().itemManager->ShowPlayerStats();
	Engine::GetInstance().render->DrawTexture(bookTex, WindowSize.getX() / 2 + 450, WindowSize.getY() - 150);
}

void Scene::UnloadStats()
{
	Engine::GetInstance().uiManager->CleanUp();
}


void Scene::CreateButton(SDL_Texture* btnTex, SDL_Texture* btnPressedTex, SDL_Rect btPos, int ID)
{
	auto btn = std::dynamic_pointer_cast<UIButton>(
		Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, ID, " ", btPos, this));
	if (btn) btn->SetTextures(btnTex, btnPressedTex, btnPressedTex);
}
