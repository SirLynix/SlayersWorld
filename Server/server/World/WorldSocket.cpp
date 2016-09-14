#include "../stdafx.h"
#include "WorldSocket.hpp"
#include "../Map/Map.hpp"
#include "PacketDefine.hpp"

WorldSocket::WorldSocket()
{
    m_Player = nullptr;
}


WorldSocket::~WorldSocket()
{
    if (m_Player == nullptr)
        return;

    WorldPacket l_Packet;
    uint8 l_ID = SMSG::S_UnitRemove;
    uint8 l_Type = (uint8)TypeUnit::PLAYER;

    l_Packet << l_ID << l_Type << m_Player->GetID();
    SendToSet(l_Packet, true);
    /// Save Player
    delete m_Player;
}

void WorldSocket::SendAuthResponse(uint8 p_Status)
{
	WorldPacket l_Packet;
	uint8 l_ID = SMSG::S_Connexion;

	l_Packet << l_ID << p_Status;
	send(l_Packet);
	printf("Send Status Auth %d\n", p_Status);
}

void WorldSocket::SendUpdatePosition(uint8 p_Type, uint16 p_ID, uint32 p_PosX, uint32 p_PosY)
{
    WorldPacket l_Packet;
    uint8 l_ID = SMSG::S_UnitUpdatePosition;

    l_Packet << l_ID << p_Type << p_ID << p_PosX << p_PosY;
    send(l_Packet);
}

void WorldSocket::SendPlayerCreate(uint32 p_ID, std::string p_Name, uint8 p_Level, uint8 p_Health, uint8 p_Alignment, uint8 p_SkinID, uint16 p_MapID, uint32 p_PosX, uint32 p_PosY, uint8 p_Orientation)
{
    WorldPacket l_Packet;
    uint8 l_ID = SMSG::S_PlayerCreate;

    l_Packet << l_ID << p_ID << p_Name << p_Level << p_Health << p_Alignment << p_SkinID << p_MapID << p_PosX << p_PosY << p_Orientation;
    send(l_Packet);
    printf("Send create\n");
}

void WorldSocket::SendUnitCreateToSet(uint8 p_Type, uint32 p_ID, std::string p_Name, uint8 p_Level, uint8 p_SkinID, uint16 p_MapID, uint32 p_PosX, uint32 p_PosY, uint8 p_Orientation, bool p_InMovement)
{
    WorldPacket l_Packet;
    uint8 l_ID = SMSG::S_UnitCreate;

    l_Packet << l_ID << p_Type << p_ID << p_Name << p_Level << p_SkinID << p_MapID << p_PosX << p_PosY << p_Orientation << p_InMovement;
    SendToSet(l_Packet, true);
    printf("Send create to square\n");
}

void WorldSocket::SendUnitCreate(uint8 p_Type, uint32 p_ID, std::string p_Name, uint8 p_Level, uint8 p_SkinID, uint16 p_MapID, uint32 p_PosX, uint32 p_PosY, uint8 p_Orientation, bool p_InMovement)
{
    WorldPacket l_Packet;
    uint8 l_ID = SMSG::S_UnitCreate;

    if (p_Type == TypeUnit::PLAYER && p_ID == GetPlayer()->GetID())
        return;

    l_Packet << l_ID << p_Type << p_ID << p_Name << p_Level << p_SkinID << p_MapID << p_PosX << p_PosY << p_Orientation << p_InMovement;
    send(l_Packet);
    printf("Send create to unit\n");
}

void WorldSocket::SendUnitGoDirectionToSet(uint8 p_Type, uint16 p_UnitID, uint32 p_PosX, uint32 p_PosY, uint8 p_Direction)
{
    WorldPacket l_Packet;
    uint8 l_ID = SMSG::S_UnitGoDirection;

    l_Packet << l_ID << p_Type << p_UnitID << p_PosX << p_PosY << p_Direction;
    SendToSet(l_Packet, true);
}

void WorldSocket::SendUnitTalk(uint8 p_Type, uint16 p_UnitID, const std::string & p_TalkString)
{
    WorldPacket l_Packet;
    uint8 l_ID = SMSG::S_UnitTalk;

    l_Packet << l_ID << p_Type << p_UnitID << p_TalkString;
    SendToSet(l_Packet);
}

void WorldSocket::SendUnitStopMovement(uint8 p_TypeID, uint16 p_ID, uint32 p_PosX, uint32 p_PosY, uint8 p_Orientation)
{
    WorldPacket l_Packet;
    uint8 l_ID = SMSG::S_UnitStopMovement;

    l_Packet << l_ID << p_TypeID << p_ID << p_PosX << p_PosY << p_Orientation;
    SendToSet(l_Packet, true);
}

void WorldSocket::SendUnitStartAttack(uint8 p_TypeID, uint16 p_ID, uint32 p_PosX, uint32 p_PosY, uint8 p_Orientation)
{
    WorldPacket l_Packet;
    uint8 l_ID = SMSG::S_UnitStartAttack;

    l_Packet << l_ID << p_TypeID << p_ID << p_PosX << p_PosY << p_Orientation;
    SendToSet(l_Packet, true);
    printf("Send Attack\n");
}

void WorldSocket::SendUnitStopAttack(uint8 p_TypeID, uint16 p_ID)
{
    WorldPacket l_Packet;
    uint8 l_ID = SMSG::S_UnitStopAttack;

    l_Packet << l_ID << p_TypeID << p_ID;
    SendToSet(l_Packet, true);
    printf("Send Attack\n");
}

Player* WorldSocket::GetPlayer()
{
    return m_Player;
}

void WorldSocket::SetPlayer(Player* p_Player)
{
    m_Player = p_Player;
}

void WorldSocket::SendMsg(WorldPacket p_Packet)
{
    send(p_Packet);
}

void WorldSocket::SendToSet(WorldPacket p_Packet, bool p_ExcludePlayer /*= false*/)
{
    Map* l_Map = GetPlayer()->GetMap();

    if (l_Map == nullptr)
        return;

    std::vector<Square*> l_SquareSet = l_Map->GetSquareSet(GetPlayer()->GetSquareID());
    for (Square* l_Square : l_SquareSet)
    {
        std::map<uint16, Unit*>* l_SquareList = l_Square->GetList(TypeUnit::PLAYER);
        if (l_SquareList == nullptr)
            continue;

        for (std::pair<uint16, Unit*> l_UnitPair : *l_SquareList)
        {
            Unit* l_Unit = l_UnitPair.second;

            if (l_Unit == nullptr)
                continue;

            if (Player* l_Player = l_Unit->ToPlayer())
            {
                if (p_ExcludePlayer && l_Player->GetID() == GetPlayer()->GetID())
                    continue;

                printf("Send to %s\n", l_Player->GetName().c_str());
                l_Player->GetSession()->send(p_Packet);
            }
        }
    }
}
