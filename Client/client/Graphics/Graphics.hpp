#pragma once

#include <SFML/System/Thread.hpp>
#include <SFML/System/Mutex.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include "../Define.hpp"
#include "Window.hpp"
#include <SFML/Graphics/View.hpp>
#include "../Map/MapManager.hpp"
#include "TileSet.hpp"
#include "VisualManager.hpp"
#include "Interface/InterfaceManager.hpp"
#include "../World/Events/Events.hpp"
#include "../World/ClockHandler.hpp"

class Graphics
{
public:
	Graphics(MapManager*, InterfaceManager*, Events*);
	~Graphics();
    bool LoadFont();
	bool CreateWindow(uint32, uint32, float);
	void UpdateWindow(sf::Time);
    void UpdateInterface(sf::Time);
	void Display();
	void Clear();
	void CheckEvent();
	bool WindowIsOpen() const;

    /// Draw Methods
    void DrawMap();
    void DrawWorldObjects(std::map<uint32, std::vector<WorldObject*> >*);
    void DrawUnitDetails(Unit*);
    void DrawInterface();

private:
	Window              m_Window;
	sf::View            m_View;
    sf::View            m_ViewFont;
    sf::View            m_ViewInterface;
    sf::Font            m_Font;

	MapManager*         m_MapManager;
	TileSet*            m_TileSet;
    VisualManager*       m_VisualManager;
    InterfaceManager*   m_InterfaceManager;
    Events*             m_Events;
    ClockHandler*       m_Clock;
};

