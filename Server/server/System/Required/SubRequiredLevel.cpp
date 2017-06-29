#include "SubRequiredLevel.hpp"

SubRequiredLevelMin::SubRequiredLevelMin(uint8 p_Level) :
    m_Level(p_Level)
{
}

SubRequiredLevelMin::~SubRequiredLevelMin()
{
}

bool SubRequiredLevelMin::IsValid(const Player* p_Player) const
{
    if (p_Player->GetLevel() < m_Level)
        return false;
    return true;
}

SubRequiredLevelMax::SubRequiredLevelMax(uint8 p_Level) :
    m_Level(p_Level)
{
}

SubRequiredLevelMax::~SubRequiredLevelMax()
{
}

bool SubRequiredLevelMax::IsValid(const Player* p_Player) const
{
    if (p_Player->GetLevel() > m_Level)
        return false;
    return true;
}


SubRequiredAccessLevel::SubRequiredAccessLevel(uint16 p_AccessLevel, uint16) :
    m_AccessLevel(p_AccessLevel)
{
}

SubRequiredAccessLevel::~SubRequiredAccessLevel()
{
}

bool SubRequiredAccessLevel::IsValid(const Player* p_Player) const
{
    if (p_Player->GetAccessType() >= m_AccessLevel)
        return true;
    return false;
}
