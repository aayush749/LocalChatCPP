#pragma once

#include <imgui.h>

#include <Events/Event.h>

#include <functional>

#define MAKE_UI_ELEMENT(ClassName) : public UIElement<ClassName>\
									 {\
										using BaseClassTp = UIElement<ClassName>;

/// <summary>
/// An abstract base class for all UI elements, utilizes the CRTP pattern
/// </summary>
/// <typeparam name="Derived"></typeparam>
template <class Derived>
class UIElement
{
public:
	UIElement(ImGuiIO& imguiIO)
		:io(imguiIO)
	{
		
	}

	void OnCreate()
	{
		static_cast<Derived*>(this)->OnCreate();
	}

	bool OnImGuiRender()
	{
		static_cast<Derived*>(this)->OnImGuiRender();
		return false;	// Because we want all the UIElements to handle this event
	}

protected:
	ImGuiIO& io;
	bool m_Created = false;
};