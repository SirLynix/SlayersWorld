#include "MovementHandler.hpp"
#include "../Map/Map.hpp"

MovementHandler::MovementHandler(uint8 p_SizeX, uint8 p_SizeY) :
    m_SizeX(p_SizeX),
    m_SizeY(p_SizeY)
{
    m_Speed = 1.0f;
    m_InMovement = false;
    m_Orientation = Orientation::Down;
    m_DiffTime = 0;
    m_Map = nullptr;
    m_AttackDamage.m_DamageDone = false;
    m_AttackDamage.m_DamageReady = false;
}

MovementHandler::~MovementHandler()
{
}

bool MovementHandler::IsInColision(int64 p_PosX, int64 p_PosY) const
{
    if (m_Map == nullptr)
        return true;

    if (p_PosX < 0 || p_PosY < 0)
        return true;

    if (p_PosX + m_SizeX >= m_Map->GetSizeX() * TILE_SIZE || p_PosY >= m_Map->GetSizeY() * TILE_SIZE)
        return true;


    std::vector<Case*> l_CaseList;
    uint8 i = 0;
    for (uint8 i = 0; i < m_SizeX; i += TILE_SIZE)
        l_CaseList.push_back(m_Map->GetCase((uint32)p_PosX + i, (uint32)p_PosY));
    l_CaseList.push_back(m_Map->GetCase((uint32)p_PosX + m_SizeX, (uint32)p_PosY));

    for (Case* l_Case : l_CaseList)
    {
        if (l_Case == nullptr)
            return true;

        if (l_Case->IsBlocking())
            return true;
    }

    return false;
}

bool MovementHandler::CheckNextMovement(uint32 p_PosX, uint32 p_PosY)
{
    if (m_MovementStack.empty())
        return false;

    bool l_NextMovement = false;
    MovementAction l_MovementAction = m_MovementStack.front();
    if (!l_MovementAction.m_PositionOptions)
        l_NextMovement = true;
    else if (!IsInAttack())
    {
        switch (GetOrientation())
        {
        case Orientation::Down:
            if (l_MovementAction.m_Pos.m_Y <= p_PosY)
                l_NextMovement = true;
            break;
        case Orientation::Left:
            if (l_MovementAction.m_Pos.m_X >= p_PosX)
                l_NextMovement = true;
            break;
        case Orientation::Right:
            if (l_MovementAction.m_Pos.m_X <= p_PosX)
                l_NextMovement = true;
            break;
        case Orientation::Up:
            if (l_MovementAction.m_Pos.m_Y >= p_PosY)
                l_NextMovement = true;
            break;
        default:
            break;
        }
    }

        if (!l_NextMovement)
            return false;

        if (l_MovementAction.m_PositionOptions)
        {
            m_Pos.m_X = l_MovementAction.m_Pos.m_X;
            m_Pos.m_Y = l_MovementAction.m_Pos.m_Y;
        }

        m_MovementStack.pop();

        if (l_MovementAction.m_ActionType == eActionType::Go)
            StartMovement((Orientation)l_MovementAction.m_Orientation);
        else if (l_MovementAction.m_ActionType == eActionType::Stop)
            StopMovement();
        else if (l_MovementAction.m_ActionType == eActionType::Attack)
            StartAttack();
        else if (l_MovementAction.m_ActionType == eActionType::StopAttack)
            StopAttack();
        return true;
}

bool MovementHandler::IsDamageReady() const
{
    return m_AttackDamage.m_DamageReady;
}

void MovementHandler::SetDamageDone(bool p_DamageDone)
{
    m_AttackDamage.m_DamageReady = !p_DamageDone;
    m_AttackDamage.m_DamageDone = p_DamageDone;
}

void MovementHandler::UpdateAttack(sf::Time p_Diff)
{
    if (!IsInAttack())
        return;

    m_DiffTimeAttack += p_Diff.asMicroseconds();

    if (!m_AttackDamage.m_DamageDone)
    {
        if (m_DiffTimeAttack > ((MAX_MOVEMENT_POSITION * UPDATE_TIME_MOVEMENT * 1000 * (2.0f - m_Speed)) / 2.0f)) ///< 1000 because microsecond
            m_AttackDamage.m_DamageReady = true;
    }

    while (m_DiffTimeAttack > (MAX_MOVEMENT_POSITION * UPDATE_TIME_MOVEMENT * 1000 * (2.0f - m_Speed))) ///< 1000 because microsecond
    {
        if (m_StopAttack)
        {
            m_InAttack = false;
            m_StopAttack = false;
        }
        m_DiffTimeAttack = 0;
        m_AttackDamage.m_DamageDone = false;
        m_AttackDamage.m_DamageReady = false;
    }
}

void MovementHandler::Update(sf::Time p_Diff)
{
    CheckNextMovement(m_Pos.m_X, m_Pos.m_Y);
    UpdateAttack(p_Diff);
    if (!IsInMovement())
        return;

    int64 l_PosX = m_Pos.m_X;
    int64 l_PosY = m_Pos.m_Y;

    m_DiffTime += p_Diff.asMicroseconds();

    while (m_DiffTime > (uint64)((UPDATE_TIME_MOVEMENT / STEP_SIZE) * 1000 * (2.0f - m_Speed))) ///< 1000 because microsecond
    {
        /// UPDATE POSITION
        switch (m_Orientation)
        {
        case Orientation::Down:
            l_PosY += (uint32)((STEP_SIZE / STEP_SIZE));
            break;
        case Orientation::Left:
            l_PosX -= (uint32)((STEP_SIZE / STEP_SIZE));
            break;
        case Orientation::Right:
            l_PosX += (uint32)((STEP_SIZE / STEP_SIZE));
            break;
        case Orientation::Up:
            l_PosY -= (uint32)((STEP_SIZE / STEP_SIZE));
            break;
        default:
            break;
        }
        m_DiffTime -= (uint64)((UPDATE_TIME_MOVEMENT / STEP_SIZE) * 1000 * (2.0f - m_Speed));

        if (!IsInColision(l_PosX, l_PosY))
        {
            m_Pos.m_X = (uint32)l_PosX;
            m_Pos.m_Y = (uint32)l_PosY;
        }
        else
            StopMovement();
    }
}

void MovementHandler::SetMap(Map* p_Map)
{
    m_Map = p_Map;
}

bool MovementHandler::IsInMovement() const
{
    return m_InMovement;
}

bool MovementHandler::IsInAttack() const
{
    return m_InAttack;
}

bool MovementHandler::IsInAction() const
{
    return m_InMovement || m_InAttack;
}


void MovementHandler::StartMovement(Orientation p_Orientation)
{
    m_InMovement = true;
    m_Orientation = p_Orientation;
}

void MovementHandler::StopMovement()
{
    m_InMovement = false;
    m_DiffTime = 0;
}

void MovementHandler::StartAttack()
{
    m_InAttack = true;
}

void MovementHandler::StopAttack()
{
    m_StopAttack = true;
}

void MovementHandler::SetOrientation(Orientation p_Orientation)
{
    m_Orientation = p_Orientation;
}

Orientation MovementHandler::GetOrientation() const
{
    return m_Orientation;
}

void MovementHandler::SetPosX(uint32 p_PosX)
{
    m_Pos.m_X = p_PosX;
}

void MovementHandler::SetSpeed(float p_Speed)
{
    m_Speed = p_Speed;
}

void MovementHandler::SetPosY(uint32 p_PosY)
{
    m_Pos.m_Y = p_PosY;
}

uint32 MovementHandler::GetPosX() const
{
    return m_Pos.m_X;
}

uint32 MovementHandler::GetPosY() const
{
    return m_Pos.m_Y;
}

Position MovementHandler::GetPos() const
{
    return m_Pos;
}

void MovementHandler::AddMovementToStack(eActionType p_Action)
{
    MovementAction l_Act;
    l_Act.m_ActionType = p_Action;
    l_Act.m_PositionOptions = false;
    m_MovementStack.push(l_Act);
}

void MovementHandler::AddMovementToStack(eActionType p_Action, Position p_Pos, Orientation p_Orientation)
{
    MovementAction l_Act;
    l_Act.m_ActionType = p_Action;
    l_Act.m_PositionOptions = true;
    l_Act.m_Pos = p_Pos;
    l_Act.m_Orientation = p_Orientation;
    m_MovementStack.push(l_Act);
}