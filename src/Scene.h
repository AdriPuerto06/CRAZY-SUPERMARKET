#pragma once

#include "Module.h"
#include "Player.h"
#include "UIButton.h"
#include "Window.h"

struct SDL_Texture;

//L17 TODO 1: Define SceneID enum class with INTRO_SCREEN, MAIN_MENU, LEVEL1, LEVEL2
enum class SceneID
{
	INTRO_SCREEN,
	MAIN_MENU,
	LEVEL1,
	LEVEL1Combat,
	LEVEL2,
	LEVEL2Combat,
	LEVEL3,
	LEVEL4,
	OPTIONS,
	CREDITS,
	MULTIPLAYER,
	SOUND,
	GRAFICS,
	PAUSE,
	EXIT,
	RESUME,
	BATTLE,
	ITEM,
	STATS,
	QUESTS,
	WIN,
	LOSE,
	NULLSCENE
};

struct SceneStack {
	std::vector<SceneID> scenes;
	bool twiceClicked = false;

	SceneID pop()
	{
		SceneID ret;
		scenes.pop_back();
		ret = scenes[scenes.size()-1];
		return ret;
	}

	void push(SceneID scene)
	{
		scenes.push_back(scene);
	}

};

class Scene : public Module
{
public:

	Scene();

	// Destructor
	virtual ~Scene();

	// Called before render is available
	bool Awake();

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	//Win
	void LoadWin();
	void UnloadWin();
	void UpdateWin(float dt);

	//Loose
	void LoadLose();
	void UnloadLose();
	void UpdateLose(float dt);

	// Return the player position
	Vector2D GetPlayerPosition();

	// Get tilePosDebug value
	std::string GetTilePosDebug() {
		return tilePosDebug;
	}

	// Handles multiple Gui Event methods
	bool OnUIMouseClickEvent(UIElement* uiElement);

	// L17 TODO 2: Define functions to handle scene changes
	void ChangeScene(SceneID newScene);
	void UnloadCurrentScene();
	void LoadScene(SceneID newScene);
	bool IsReloading();

	std::shared_ptr<Player> GetPlayer();

	//Getter
	SceneID GetCurrentScene();
	SceneID GetTimeScene();

	SceneStack sceneStack;
private:

	// Intro / Splash
	void LoadIntroScreen();
	void UpdateIntroScreen(float dt);
	void UnloadIntroScreen();

	// L17 TODO 3: Define specific function for main menu scene: Load, Unload, Handle UI events
	void LoadMainMenu();
	void CreateButton(SDL_Texture* btnOptTex, SDL_Texture* btnOptPressedTex, SDL_Rect btPos, int n);
	void UnloadMainMenu();
	void UpdateMainMenu(float dt);
	void HandleMainMenuUIEvents(UIElement* uiElement);

	// L17 TODO 4: Define specific functions for level1 scene: Load, Unload, Update, PostUpdate
	void LoadLevel1();
	void UnloadLevel1();
	void UpdateLevel1(float dt);
	void PostUpdateLevel1();

	// L17 TODO 5: Define specific functions for level2 scene: Load, Unload, Update
	void LoadLevel2();
	void UpdateLevel2(float dt);
	void UnloadLevel2();
	void PostUpdateLevel2();

	//Level3 functions
	void LoadLevel3();
	void UpdateLevel3(float dt);
	void UnloadLevel3();
	void PostUpdateLevel3();

	void LoadLevel4();
	void UpdateLevel4(float dt);
	void UnloadLevel4();
	void PostUpdateLevel4();

	//OPTIONS
	void LoadOptions();
	void UnloadOptions();
	void UpdateOptions(float dt);
	void PostUpdateOptions();

	//MULTIPLAYER
	void LoadMultiplayer();
	void UnloadMultiplayer();
	void UpdateMultiplayer(float dt);
	void PostUpdateMultiplayer();

	//CREDITS
	void LoadCredits();
	void UnloadCredits();
	void UpdateCredits(float dt);
	void PostUpdateCredits();

	//SOUNDS
	void LoadSounds();
	void UnloadSounds();
	void UpdateSounds(float dt);
	void PostUpdateSounds();
	float musicVolume = 1.0f;
	float sfxVolume = 1.0f;
	bool  isAudioMuted = false;
	bool drumsFinished = false;
	float drumsTimer = 0.0f;

	//GRAFICS
	void LoadGrafics();
	void UnloadGrafics();
	void UpdateGrafics(float dt);
	void PostUpdateGrafics();

	//PAUSE
	void LoadPause();
	void UnloadPause();
	void UpdatePause(float dt);
	void PostUpdatePause();

	//BATTLE
	void LoadBattle();
	void UnloadBattle();
	void UpdateBattle(float dt);
	void PostUpdateBattle();

	//Combat scenes
	void LoadCombatScene(SceneID sceneid);
	void UnloadCombatScene();
	void UpdateCombatScene(float dt);
	void PostUpdateCombatScene();

	//Item
	void LoadItem();
	void UnloadItem();
	void UpdateItem(float dt);

	//Stats
	void LoadStats();
	void UnloadStats();
	void UpdateStats(float dt);


	//Quests
	void LoadQuests();
	void UnloadQuests();
	void UpdateQuests(float dt);
	void PostUpdateQuests();

private:

	//L03: TODO 3b: Declare a Player attribute
	std::shared_ptr<Player> player;
	SDL_Texture* mouseTileTex = nullptr;
	std::string tilePosDebug = "[0,0]";
	bool once = false;

	// L16: TODO 2: Declare a UIButton 
	std::shared_ptr<UIButton> uiBt;
	float volume = 1.0;

	// L17 TODO 1: Current scene attribute with initial value
	//SceneID currentScene = SceneID::MAIN_MENU;
	SceneID currentScene = SceneID::INTRO_SCREEN;
	Vector2D WindowSize;
	SceneID gameScene;
	SceneID timeScene;

	//Imagen
	SDL_Texture* logoImg = nullptr;
	SDL_Texture* teamImg = nullptr;
	SDL_Texture* SMImg = nullptr;
	SDL_Texture* almacenIMG = nullptr;
	SDL_Texture* cajonTex = nullptr;
	SDL_Texture* bookTex = nullptr;
	SDL_Texture* winImg = nullptr;
	SDL_Texture* loseImg = nullptr;
	float splashTime = 0.0f;
	float logoGameTimer = 3.0f;
	float logoTeamTimer = 6.0f;
	float teamFadeValue = 0.0f;
	float logoFadeValue = 0.0f;
	bool  logoFadeStarted = false;
	bool sfxLogoPlayed = false;
	bool sfxTeamPlayed = false;
	float screenFadeValue = 0.0f;
	bool  screenFadeStarted = false;
	float winLoseTimer = 5.0f;

	//Creditos
	std::vector<std::string> creditsText;
	float creditsY = 0.0f;
	float scrollSpeed = 100.0f;
	int lineHeight = 30;
	bool isCredits = false;
	float creditsTimer = 5.f;

	//Cursor
	SDL_Surface* cursorSurface = nullptr;
	SDL_Cursor* customCursor = nullptr;

	bool closeGame = false;
	bool fromSG = false;
	bool fullScreen = true;
	
};