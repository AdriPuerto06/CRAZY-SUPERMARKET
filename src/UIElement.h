#pragma once

#include "Input.h"
#include "Render.h"
#include "Module.h"
#include "Vector2D.h"
#include "Engine.h"

enum class UIElementType
{
	BUTTON,
	TOGGLE,
	CHECKBOX,
	SLIDER,
	SLIDERBAR,
	COMBOBOX,
	DROPDOWNBOX,
	INPUTBOX,
	VALUEBOX,
	SPINNER
};

enum class UIElementState
{
	DISABLED,
	NORMAL,
	FOCUSED,
	PRESSED,
	SELECTED
};



class UIElement : public std::enable_shared_from_this<UIElement>
{
public:

	UIElement() {}

	// Constructor
	UIElement(UIElementType type, int id) : type(type), id(id), state(UIElementState::NORMAL) {}

	// Constructor
	UIElement(UIElementType type, SDL_Rect bounds, const char* text) :
		type(type),
		state(UIElementState::NORMAL),
		bounds(bounds),
		text(text)
	{
		color.r = 255; color.g = 255; color.b = 255;
		texture = NULL;
	}

	// Called each loop iteration
	virtual bool Update(float dt)
	{
		return true;
	}

	// 
	void SetTexture(SDL_Texture* tex)
	{
		texture = tex;
		section = { 0, 0, 0, 0 };
	}

	// 
	void SetObserver(Module* module)
	{
		observer = module;
	}
	

	// 
	void NotifyObserver()
	{
		observer->OnUIMouseClickEvent(this);
	}

	virtual bool CleanUp()
	{
		return true;
	}

	virtual bool Destroy()
	{
		return true;
	}


public:

	int id;
	UIElementType type;
	UIElementState state;

	std::string text;       // UIElement text (if required)
	SDL_Rect bounds;        // Position and size
	SDL_Color color;        // Tint color

	SDL_Texture* texture;   // Texture atlas reference
	SDL_Rect section;       // Texture atlas base section

	Module* observer;        // Observer 

	bool pendingToDelete = false;
};

class UISlider : public UIElement
{
public:
	UISlider(int id, SDL_Rect bounds, float min, float max, float value)
		: UIElement(UIElementType::SLIDER, id),
		minValue(min), maxValue(max), value(value)
	{
		this->bounds = bounds;
	}

	bool Update(float dt) override;

	float GetValue() const { return value; }

private:
	float minValue = 0.0f;
	float maxValue = 1.0f;
	float value = 0.5f;

	bool dragging = false;

	//Vector2D mousepos = Engine::GetInstance().input->GetMousePosition();


	

	bool Slider() {
		int mouseX, mouseY;
		mouseX = Engine::GetInstance().input->GetMousePosition().getX();
		mouseY = Engine::GetInstance().input->GetMousePosition().getY();

		bool mousePressed = Engine::GetInstance().input->GetMouseButtonDown(1);

		if ((mousePressed && mouseX >= bounds.x) && (mouseX <= bounds.x + bounds.w) && (mouseY >= bounds.y) && (mouseY <= bounds.y + bounds.h))
		{
			dragging = true;
		}

		if (!mousePressed)
			dragging = false;

		if (dragging)
		{
			float relativeX = (float)(mouseX - bounds.x) / bounds.w;
			if (relativeX < 0.0f) relativeX = 0.0f;
			if (relativeX > 1.0f) relativeX = 1.0f;

			value = minValue + relativeX * (maxValue - minValue);

			NotifyObserver();
		}
		return true;
	};
};