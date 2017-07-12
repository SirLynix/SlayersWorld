#pragma once

#include <SFML/Network/TcpSocket.hpp>
#include "WorldPacket.hpp"
#include "../Define.hpp"
#include "../Entities/Player.hpp"

class WorldSocket : public sf::TcpSocket
{
public:
	WorldSocket();
	~WorldSocket();

    void SendPlayerCreate(uint32, std::string, uint8, uint8, uint8, uint8, int16, uint16, const std::string &, const std::string &, const std::string &, uint32, uint32, uint8);
    void SendUnitCreateToSet(uint8, uint32, std::string, uint8, uint8, uint8, uint8, uint8, uint16, uint32, uint32, uint8, bool, bool, bool, bool);
    void SendUnitCreate(uint8, uint32, std::string, uint8, uint8, uint8, uint8, uint8, uint8, uint8, uint8, uint16, Position, uint8, bool, bool, bool, bool);
    void SendUnitGoDirectionToSet(uint8, uint16, const Position &, uint8);
    void SendUnitStopMovement(uint8, uint16, const Position &, uint8);
    void SendUnitStartAttack(uint8, uint16, const Position &, uint8);
    void SendUnitStopAttack(uint8, uint16);
    void SendUpdateXpPct(float);
    void SendUpdateUnitResource(uint8, uint16, uint8, uint8);
    void SendUnitTalk(uint8, uint16, const std::string &);
	void SendLogDamage(uint8, uint16, DamageInfo);
	void SendAuthResponse(uint8);
    void SendPacket(WorldPacket);
    void SendToSet(WorldPacket, bool p_ExcludePlayer = false, bool p_OnlyGroup = false);
    void SendSwitchMap(uint16, const std::string &, const std::string &, const std::string &);
    void SendKeyBoardBind(eKeyBoardAction, uint8);
    void SendUnitIsInGroup(uint8, uint32, bool);

    Player* GetPlayer();
    void SetPlayer(Player*);

private:
    Player* m_Player;
};

