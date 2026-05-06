#pragma once
#include "UIButton.h"
#include "ItemManager.h"
#include "Scene.h"
#include <vector>

//Effects
#define POISON_DAMAGE 2
#define HEAL_HITPOINTS 5
#define SHIELD_DMG_REDUCTION 3
#define BUFF_DMG_INCREASE 3

struct Attack {
    const char* name;
    int dmg;
    int magicPoints;
    std::string effect;
    bool unlocked;
};

struct Combatant {
    int id = -1;
    int hp = 0;
    bool alive = true;
    std::string status = "none";
    int status_duration = 0;
    std::pair<bool, bool> shield_and_buff;
    std::vector<Attack> attacks;
    EntityType type = EntityType::UNKNOWN;
};

struct CombatData {
    std::vector<Combatant> players;
    std::vector<Combatant> enemies;

    int fight_ID = -1;
    int possible_enemy_ID = -1;

    void Clear()
    {
        players.clear();
        enemies.clear();
    }
};

struct CombatState {
    std::string turn = "Player";

    int enemy_index_targeted = 0;        // index of CombatData::enemies
    int player_index_selected = 0;       // index of CombatData::players
    int player_attack_index_selected = 0;

    int player_attack_dmg_selected = 0;
    int enemy_attack_dmg_selected = 0;

    bool player_Wins = false;
    bool enemy_Wins = false;
    bool selecting_target = false;

    int magicPoints = 0;

    void Init(const CombatData& data)
    {
        enemy_index_targeted = 0;
        player_index_selected = 0;
        player_attack_index_selected = 0;

        turn = "Player";
        player_Wins = false;
        enemy_Wins = false;
        selecting_target = false;

        for (auto& p : const_cast<std::vector<Combatant>&>(data.players)) {
            p.alive = true;
            p.status = "none";
        }
        for (auto& e : const_cast<std::vector<Combatant>&>(data.enemies)) {
            e.alive = true;
            e.status = "none";
        }
    }
};

class CombatManager : public Module {
public:
    CombatManager();
    virtual ~CombatManager();

    bool Awake();
    bool Start();
    bool Update(float dt);
    bool PostUpdate();
    bool CleanUp();

    bool LoadCombatData(std::string path, std::string fileName);
    void UnloadCombatUI();

    bool OnUIMouseClickEvent(UIElement* uiElement);
    void ButtonAction(int ID);
    void ShowButtonStart(Vector2D position, int enemy_ID, int fight_ID);
    bool StartCombat();
    bool ShowAttackOptions(int player_ID);
    bool ShowItemOptions(int player_ID);
    bool ChangePlayer();
    bool ShowOptions(int player_ID);

    void GetTreeAttributes(int fight_ID);

    void HandleTargetSelection();
    void ApplyCombatLogic();
    void ApplyEffects();
    void CheckAlive();
    void MakeAttack(Combatant& target, Combatant& attacker, Attack attack);
    void EnemyAI();
    void MarkEnemiesAsDead();

    void UnlockAttack(EntityType type, const char* name);

    void SaveTreeAttributes();

    std::vector<bool> itemVector;

    std::string combatFileName;
    std::string combatPath;

    CombatData* combatData;
    CombatState* combatState;

    bool showing_continue = false;
    bool can_be_clicked = true;
    bool showingButtonStart = false;
    bool in_combat = false;

    bool showInventory = false;

    bool godMode = false;
    bool choosingAtk = false;

    std::vector<int> enemies_to_destroy;

private:
    pugi::xml_document combatFileXML;

    SceneID timeScene;
    SceneID currentScene;

};
