#include "RewardManager.h"
#include "Log.h"

#include "QuestManager.h"
#include "DialogueManager.h"
#include "CombatManager.h"
#include "EventManager.h"
#include "Map.h"

bool IsANumber(char c)
{
	for (int i = 0; i < 10; ++i)
	{
		if (c == (char)std::to_string(i).c_str()) return true;
	}
	return false;
}

int GetNumFromString(std::string str) {
	int num = str.at(0) - '0';
	for (int l = 1; l < str.size(); ++l)
	{
		if (!(str[l] == ','))
		{
			num = num * 10 + (str.at(l) - '0');
		}
	}
	return num;
}

SceneID GetSceneID(const char* scene)
{
	std::string s = scene;
	int i = 0;
	while (!IsANumber(s[i]) && s.size() > i)
	{
		i++;
		if (s.size() < i) break;
	}
	s.erase(0, i-1);
	return (SceneID)(GetNumFromString(s) - 1);
}

RewardManager::RewardManager() : Module()
{
	name = "RewardManager";
}

RewardManager::~RewardManager() {}

bool RewardManager::Awake()
{
	return true;
}

bool RewardManager::Start()
{
	return true;
}

bool RewardManager::Update(float dt)
{

	return true;
}

bool RewardManager::PostUpdate() {
	
	return true;
}

bool RewardManager::CleanUp()
{
	
	return true;
}

void RewardManager::GetReward(Reward reward)
{
	switch (reward.type)
	{
	case RewardType::QUEST:
		Engine::GetInstance().questManager->ActivateQuest(reward.reward_value.c_str());
		break;

	case RewardType::COMPLETEQUEST:
		Engine::GetInstance().questManager->CompleteQuest(reward.reward_value.c_str());
		break;

	case RewardType::ITEM:
		Engine::GetInstance().itemManager->ActivateItem(reward.reward_value.c_str());
		break;

	case RewardType::ATTACK:
		Engine::GetInstance().combatManager->UnlockAttack(EntityType::PLAYER, reward.reward_value.c_str());
		break;

	case RewardType::COMPANION:
		if (std::strcmp(reward.reward_value.c_str(), "Wizard") == 0) 
		{ 
			Engine::GetInstance().scene->GetPlayer()->WizardJoined = true; Engine::GetInstance().combatManager->isWizardActive = true; LOG("Wizard joins the battle!"); 
			
			PendingChange change;
			change.entityType = EntityType::PLAYER;
			change.type = Component::WIZARDJOINED;
			change.new_value = true;

			Engine::GetInstance().map->pendingChanges.emplace_back(change);
		}

		if (std::strcmp(reward.reward_value.c_str(), "Cornelius") == 0) 
		{ 
			Engine::GetInstance().scene->GetPlayer()->CorneliusJoined = true; Engine::GetInstance().combatManager->isCorneliusActive = true; LOG("Cornelius joins the battle!");
			
			PendingChange change;
			change.entityType = EntityType::PLAYER;
			change.type = Component::CORNELIUSJOINED;
			change.new_value = true;

			Engine::GetInstance().map->pendingChanges.emplace_back(change);
		}
		Engine::GetInstance().map->SaveEntities(Engine::GetInstance().scene->GetPlayer(), Engine::GetInstance().scene->GetCurrentScene());
		break;

	case RewardType::DIALOGUE:
		Engine::GetInstance().dialogueManager->UnlockNewDialogueTree(GetNumFromString(reward.reward_value));
		break;

	case RewardType::EVENT:
		Engine::GetInstance().eventManager->PossibleActivate(reward.reward_value.c_str());
		break;

	case RewardType::TELEPORT:
		Engine::GetInstance().scene->ChangeScene(GetSceneID(reward.reward_value.c_str()));
		break;
	}
}

