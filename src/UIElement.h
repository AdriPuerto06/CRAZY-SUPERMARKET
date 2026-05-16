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

	bool Update(float dt) override
	{
		Slider();
		SDL_Renderer* renderer = Engine::GetInstance().render->renderer;

		//barra
		SDL_FRect rectF = {
		(float)bounds.x,
		(float)bounds.y,
		(float)bounds.w,
		(float)bounds.h
		};

		SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
		SDL_RenderFillRect(renderer, &rectF);
		//knob
		float t = (value - minValue) / (maxValue - minValue);
		float knobX = bounds.x + t * bounds.w;

		SDL_FRect knobF = {
			knobX - 5.0f,
			(float)bounds.y,
			10.0f,
			(float)bounds.h
		};

		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		SDL_RenderFillRect(renderer, &knobF);
		return true;
	}

	float GetValue() const { return value; }
	void UpdateValue(float newValue) { value = newValue; }
	bool DragStarted() const { return dragging && !wasDragging; }

private:
	float minValue = 0.0f;
	float maxValue = 1.0f;
	float value = 0.5f;

	bool dragging = false;
	bool wasDragging = false;



	bool Slider() {
		int mouseX, mouseY;
		mouseX = Engine::GetInstance().input->GetMousePosition().getX();
		mouseY = Engine::GetInstance().input->GetMousePosition().getY();
		bool mousePressed = Engine::GetInstance().input->GetMouseButtonDown(1);

		static int activeSliderId = -1; // solo uno activo a la vez

		if (mousePressed && (activeSliderId == -1 || activeSliderId == id))
		{
			if ((mouseX >= bounds.x) && (mouseX <= bounds.x + bounds.w) && (mouseY >= bounds.y) && (mouseY <= bounds.y + bounds.h))
			{
				dragging = true;
				activeSliderId = id;
			}
		}
		if (!mousePressed)
		{
			dragging = false;
			activeSliderId = -1;
		}

		if (dragging)
		{
			float relativeX = (float)(mouseX - bounds.x) / bounds.w;
			if (relativeX < 0.0f) relativeX = 0.0f;
			if (relativeX > 1.0f) relativeX = 1.0f;
			value = minValue + relativeX * (maxValue - minValue);

			if (observer) observer->OnUIMouseClickEvent(this);
		}

		wasDragging = dragging;
		return true;
	}
};