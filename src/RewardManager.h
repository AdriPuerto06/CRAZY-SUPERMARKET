#pragma once
#include <vector>
#include <string>
#include "Module.h"

enum class RewardType
{
	NONE = -1,
	QUEST,
	ITEM,
	COMPANION,
	COMPLETEQUEST,
	ATTACK,
	DIALOGUE
};

struct Reward {
	RewardType type;
	std::string reward_value;

	Reward() {}

	Reward(RewardType type, std::string value)
	{
		this->type = (RewardType)type;
		this->reward_value = value;
	}
};

class RewardManager : public Module {
public:

	RewardManager();
	virtual ~RewardManager();

	bool Awake();
	bool Start();
	bool Update(float dt);
	bool PostUpdate();
	bool CleanUp();

	void GetReward(Reward reward);

private:

};