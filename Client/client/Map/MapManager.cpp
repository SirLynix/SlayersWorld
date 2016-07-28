#include "MapManager.hpp"



MapManager::MapManager(Events* p_Events) :
    m_Events(p_Events),
	m_ActualMap(nullptr)
{
    m_PosX = 0;
    m_PosY = 0;
    m_MainPlayer = nullptr;
}


MapManager::~MapManager()
{
}

void MapManager::Update(sf::Time p_Diff)
{
    if (m_ActualMap != nullptr)
        m_ActualMap->Update(p_Diff);
}

bool MapManager::LoadMap(uint16 m_MapID)
{
	m_ActualMap = new Map(m_MapID);
    if (!m_ActualMap->InitializeMap(m_MapsPath[0]))
        return false;
    return true;
}

Map* MapManager::GetActualMap() const
{
	return m_ActualMap;
}

void MapManager::InitializeMaps()
{
	m_MapsPath[0] = "map00.map";
}

bool  MapManager::HasMap()
{
	if (m_ActualMap == nullptr)
		return false;

	if (!m_ActualMap->IsValidMap())
		return false;

	return true;
}

void MapManager::SetPosX(uint32 p_PosX)
{
    m_PosX = p_PosX;
}

void MapManager::SetPosY(uint32 p_PosY)
{
    m_PosY = p_PosY;
}

uint32 MapManager::GetPosX() const
{
    return m_PosX;
}

uint32 MapManager::GetPosY() const
{
    return m_PosY;

}