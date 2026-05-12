#include "RewardManager.h"
#include "Log.h"

#include "QuestManager.h"
#include "DialogueManager.h"
#include "CombatManager.h"

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
		break;

	case RewardType::DIALOGUE:
		Engine::GetInstance().dialogueManager->UnlockNewDialogueTree(GetNumFromString(reward.reward_value));
		break;
	}
}

