#include "EventManager.h"
#include "Engine.h"
#include "Log.h"
#include "UIManager.h"
#include "Window.h"
#include "EntityManager.h"
#include "Player.h"
#include "Scene.h"
#include "Physics.h"
#include "QuestManager.h"

EventManager::EventManager() : Module()
{
	name = "QuestManagerManager";
}

EventManager::~EventManager() {}

bool EventManager::Awake()
{;
	return true;
}

bool EventManager::Start()
{
	return true;
}

bool EventManager::Update(float dt)
{

	return true;
}

bool EventManager::PostUpdate() {
	
	return true;
}

bool EventManager::CleanUp()
{
	events.clear();
	return true;
}

std::shared_ptr<Event> EventManager::GetEvent(const char* name)
{
	for (auto event : events)
	{
		if (std::strcmp(event->event_Name, name) == 0) return event;
	}
	return std::shared_ptr<Event>();
}

void EventManager::GetEvents()
{
	events = Engine::GetInstance().entityManager->GetEventsEntities();
	LOG("Events vector size: %i", events.size());
}

bool EventManager::IsEventActive(const char* name)
{
	if (events.size() == 0) return false;
	for (auto event : events)
	{
		if (std::strcmp(event->event_Name, name) == 0) return event->active;
	}
	return false;
}

//void EventManager::ActivateEvent(const char* name)
//{
//	for (auto event : events)
//	{
//		if ((std::strcmp(event->event_Name, name) == 0)) {
//			event->activated = true; 
//			LOG("Event: '%s' activated.", name);
//			return;
//		}
//	}
//
//	LOG("Event: '%s' couldn't be activated. No event found with the same name.", name);
//}

bool EventManager::IsEventActivated(const char* name)
{
	if (events.size() == 0) return false;
	for (auto event : events)
	{
		if (std::strcmp(event->event_Name, name) == 0) return event->activated;
	}
	return false;
}

void EventManager::PossibleActivate(const char* name)
{
	std::shared_ptr<Event> event = GetEvent(name);
	if (event == nullptr)
	{
		//next events don't exist as an entity, but as a kind of activators for other events. These events are rewards from quests, dialogues...
		if (std::strcmp(name, "Key for Door2") == 0)
		{
			MakeAction("Door2");
		}
		else { LOG("Tried activating an event that doesn't exist or isn't active: '%s'", name); }
	}
	else {
		if (event->activated) { LOG("Event already made its action.");  return; }
		if (!event->active) { LOG("Event not active.");  return; }
		//Doors
		if (std::strcmp(name, "Door1") == 0)
		{
			/*if (IsEventActivated("Key for Door1"))
			{
				event->activated = true;
				LOG("Door1 is now open.");
				MakeAction(name);
			}
			else {
				LOG("Door1 is closed.");
			}*/

			return;
		}
		else if (std::strcmp(name, "Door2") == 0) { return; }
		else if (std::strcmp(name, "Gloves") == 0)
		{
			event->activated = true;
		}
		else if (std::strcmp(name, "Key for the next dungeon") == 0)
		{
			event->activated = true;
		}
		else if (std::strcmp(name, "Key for Door1") == 0)
		{
			event->activated = true;
			LOG("'Key for Door1' activated.");
			MakeAction("Door1");
		}
		//Rocks
		else if (std::strcmp(name, "Button1") == 0)
		{
			event->activated = true;
			event->Disable();
			LOG("'Button1' activated.");
			MakeAction(name);
		}
		else if (std::strcmp(name, "Button2") == 0)
		{
			event->activated = true;
			event->Disable();
			LOG("'Button2' activated.");
			MakeAction(name);
		}
		else if (std::strcmp(name, "Button3") == 0)
		{
			event->activated = true;
			event->Disable();
			LOG("'Button3' activated.");
			MakeAction(name);
		}
		else if (std::strcmp(name, "Button4") == 0)
		{
			event->activated = true;
			event->Disable();
			LOG("'Button4' activated.");
			MakeAction(name);
		}
	}
}

void EventManager::MakeAction(const char* name)
{
	std::shared_ptr<Event> event = GetEvent(name);
	if (!event->active) { LOG("Event not active.");  return; }
	if (std::strcmp(name,"Door1")==0)
	{
		event->Disable();
		LOG("Door1 opened.");
	}
	else if (std::strcmp(name,"Door2")==0)
	{
		event->Disable();
		LOG("Door2 opened.");
	}
	else if (std::strcmp(name,"Gloves")==0)
	{
		
	}
	else if (std::strcmp(name,"Key for the next dungeon")==0)
	{
		
	}
	else if (std::strcmp(name, "Button1") == 0 || std::strcmp(name, "Button2") == 0 || std::strcmp(name, "Button3") == 0 || std::strcmp(name, "Button4") == 0) 
	{
		if (IsEventActivated("Button1") && IsEventActivated("Button2") && IsEventActivated("Button3") && IsEventActivated("Button4"))
		{
			Engine::GetInstance().questManager->CompleteQuest("Push the buttons");
			std::shared_ptr<Event> door3 = GetEvent("Door3");
			door3->Disable();
			LOG("Door3 opened.");
		}
	}
}


