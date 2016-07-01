#include "PacketHandler.hpp"


PacketHandler::PacketHandler()
{
}


PacketHandler::~PacketHandler()
{
}

void PacketHandler::LoadPacketHandlerMap()
{
    m_PacketHandleMap[1] = &PacketHandler::HandleConnexion;
}

void PacketHandler::HandleConnexion(WorldPacket &p_Packet, WorldSocket* p_WorldSocket)
{
    printf("Ask Connection");
}



void PacketHandler::OperatePacket(WorldPacket &p_Packet, WorldSocket* p_WorldSocket)
{
    uint8 l_PacketID;
    p_Packet >> l_PacketID;
    printf("Operate Packet %d", l_PacketID);
    m_Func l_Fun = m_PacketHandleMap[l_PacketID];
    (this->*(l_Fun))(p_Packet, p_WorldSocket);
}

