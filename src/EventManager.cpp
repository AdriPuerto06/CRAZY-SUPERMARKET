#include "EventManager.h"
#include "Engine.h"
#include "Log.h"
#include "UIManager.h"
#include "Window.h"
#include "EntityManager.h"

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
	if (event->activated) LOG("Event already made its action.");  return;
	if (!event->active)  LOG("Event not active.");  return;
	if (name == "Door1")
	{
		if (IsEventActivated("Key for Door1")) 
		{
			event->activated = true;
			LOG("Door1 is now open.");
			MakeAction(name);
		}
		else {
			LOG("Door1 is closed.");
		}

		return;
	}
	else if (name == "Door2") {}
	else if (name == "Gloves")
	{
		event->activated = true;
		MakeAction(name);
	}
	else if (name == "Key for the next dungeon")
	{
		event->activated = true;
	}
}

void EventManager::MakeAction(const char* name)
{
	std::shared_ptr<Event> event = GetEvent(name);
	if (!event->active)  LOG("Event not active.");  return;
	if (name == "Door1")
	{
		event->position = Vector2D(event->position.getX() - 2 * event->texW, event->position.getY());
	}
	else if (name == "Door2") 
	{
		event->position = Vector2D(event->position.getX() - 2 * event->texW, event->position.getY());
	}
	else if (name == "Gloves")
	{
		
	}
	else if (name == "Key for the next dungeon")
	{
		
	}
}
