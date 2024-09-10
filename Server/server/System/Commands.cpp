#include "../Entities/Player.hpp"
#include "../World/PacketDefine.hpp"
#include "../World/WorldSocket.hpp"
#include "../Global.hpp"
#include "../System/SWExit.hpp"
#include  "../System/Quest/Quest.hpp"
#include <SFML/Network/IpAddress.hpp>

void Player::InitializeCommands()
{
    m_CmdHandleMap["skin"].first = eAccessType::Dummy;
    m_CmdHandleMap["skin"].second = &Player::HandleCommandSkin;
    m_CmdHandleMap["where"].first = eAccessType::Dummy;
    m_CmdHandleMap["where"].second = &Player::HandleCommandWhere;
    m_CmdHandleMap["level"].first = eAccessType::Dummy;
    m_CmdHandleMap["level"].second = &Player::HandleCommandLevel;
    m_CmdHandleMap["class"].first = eAccessType::Dummy;
    m_CmdHandleMap["class"].second = &Player::HandleCommandClass;
    m_CmdHandleMap["points"].first = eAccessType::Dummy;
	m_CmdHandleMap["points"].second = &Player::HandleCommandAddPoint;
    m_CmdHandleMap["bind"].first = eAccessType::Dummy;
    m_CmdHandleMap["bind"].second = &Player::HandleBind;
    m_CmdHandleMap["save"].first = eAccessType::Dummy;
    m_CmdHandleMap["save"].second = &Player::HandleSave;
    m_CmdHandleMap["msg"].first = eAccessType::Dummy;
    m_CmdHandleMap["msg"].second = &Player::HandleCommandWisp;
    m_CmdHandleMap["quests"].first = eAccessType::Dummy;
    m_CmdHandleMap["quests"].second = &Player::HandleCommandQuests;
    m_CmdHandleMap["join"].first = eAccessType::Dummy;
    m_CmdHandleMap["join"].second = &Player::HandleCommandJoin;
    m_CmdHandleMap["gr"].first = eAccessType::Dummy;
    m_CmdHandleMap["gr"].second = &Player::HandleCommandGroupWisp;
    m_CmdHandleMap["team"].first = eAccessType::Dummy;
    m_CmdHandleMap["team"].second = &Player::HandleCommandTeamWisp;
    m_CmdHandleMap["leave"].first = eAccessType::Dummy;
    m_CmdHandleMap["leave"].second = &Player::HandleCommandLeave;
    m_CmdHandleMap["kiss"].first = eAccessType::Dummy;
    m_CmdHandleMap["kiss"].second = &Player::HandleCommandEmote;
    m_CmdHandleMap["mount"].first = eAccessType::Dummy;
    m_CmdHandleMap["mount"].second = &Player::HandleMount;
    m_CmdHandleMap["dismount"].first = eAccessType::Dummy;
    m_CmdHandleMap["dismount"].second = &Player::HandleDismount;
    m_CmdHandleMap["npc"].first = eAccessType::Moderator;
    m_CmdHandleMap["npc"].second = &Player::HandleCommandCreature;
    m_CmdHandleMap["who"].first = eAccessType::Moderator;
    m_CmdHandleMap["who"].second = &Player::HandleCommandWho;
    m_CmdHandleMap["tele"].first = eAccessType::Moderator;
    m_CmdHandleMap["tele"].second = &Player::HandleCommandTeleport;
    m_CmdHandleMap["regen"].first = eAccessType::Moderator;
    m_CmdHandleMap["regen"].second = &Player::HandleRegen;
    m_CmdHandleMap["speed"].first = eAccessType::Moderator;
    m_CmdHandleMap["speed"].second = &Player::HandleCommandSpeed;
    m_CmdHandleMap["test"].first = eAccessType::Moderator;
    m_CmdHandleMap["test"].second = &Player::HandleTest;
    m_CmdHandleMap["announce"].first = eAccessType::Moderator;
    m_CmdHandleMap["announce"].second = &Player::HandleCommandAnnounce;
    m_CmdHandleMap["summon"].first = eAccessType::Moderator;
    m_CmdHandleMap["summon"].second = &Player::HandleCommandSummonPlayer;
    m_CmdHandleMap["server"].first = eAccessType::Moderator;
    m_CmdHandleMap["server"].second = &Player::HandleCommandServer;
    m_CmdHandleMap["kick"].first = eAccessType::Moderator;
    m_CmdHandleMap["kick"].second = &Player::HandleKick;
    m_CmdHandleMap["pinfo"].first = eAccessType::Moderator;
    m_CmdHandleMap["pinfo"].second = &Player::HandlePlayerInfo;
    m_CmdHandleMap["banIP"].first = eAccessType::Moderator;
    m_CmdHandleMap["banIP"].second = &Player::HandleBanIP;
    m_CmdHandleMap["banAccount"].first = eAccessType::Moderator;
    m_CmdHandleMap["banAccount"].second = &Player::HandleBanAccount;
    m_CmdHandleMap["godmod"].first = eAccessType::Moderator;
    m_CmdHandleMap["godmod"].second = &Player::handleGodMod;
    m_CmdHandleMap["cast"].first = eAccessType::Moderator;
    m_CmdHandleMap["cast"].second = &Player::handleCast;
    m_CmdHandleMap["bg"].first = eAccessType::Moderator;
    m_CmdHandleMap["bg"].second = &Player::HandleCommandBG;
    m_CmdHandleMap["restart"].first = eAccessType::Moderator;
    m_CmdHandleMap["restart"].second = &Player::HandleServerRestart;
}

bool Player::HandleTest(std::vector<std::string> p_ListCmd)
{
    if (!p_ListCmd.empty())
        return false;
    //RewardItem(1, 2);
    
    OpenSeller();
    /*
    PacketSellItemInterface l_Packet;

    std::map<uint8, uint16> p_Price;

    p_Price[0] = 2200;
    l_Packet.BuildPacket(&p_Price);
    GetSession()->SendPacket(l_Packet.m_Packet);*/

   /* ItemTemplate* l_ItemTemplate = g_ItemManager->GetItemTemplate(2);
    if (l_ItemTemplate == nullptr)
        return false;
    Item* l_Item = new Item(this, l_ItemTemplate);
    l_Item->SetStackNb(2);
    AddItemOnAvailableSlot(l_Item);*/
    //AddQuest(new Quest(g_QuestManager->GetQuestTemplate(1)));
    return true;
}

bool Player::HandleMount(std::vector<std::string> p_ListCmd)
{
    if (!p_ListCmd.empty())
        CastSpell(5);
    else
        CastSpell(4);
    return true;
}

bool Player::HandleDismount(std::vector<std::string> p_ListCmd)
{
    Dismount();
    return true;
}

bool Player::handleGodMod(std::vector<std::string> p_ListCmd)
{
    if (!p_ListCmd.empty())
    return true;

    if (GetPlayerMod() == ePlayerMod::GODMOD)
        SetPlayerMod(ePlayerMod::NONEMOD);
    else
        SetPlayerMod(ePlayerMod::GODMOD);
return true;
}

bool Player::handleCast(std::vector<std::string> p_ListCmd)
{
    if (p_ListCmd.empty())
        return true;

    uint16 l_SpellID = atoi(p_ListCmd[0].c_str());
    CastSpell(l_SpellID);
    return true;
}


bool Player::HandleCommandQuests(std::vector<std::string> p_ListCmd)
{
    if (!p_ListCmd.empty())
        return false;

    std::map< uint16, Quest* >* l_QuestList = GetQuestList();
    for (std::map< uint16, Quest* >::iterator l_It = l_QuestList->begin(); l_It != l_QuestList->end(); ++l_It)
    {
        SendMsg("-> " + (*l_It).second->GetName());

        std::map< uint8, ObjectifProgess* >* l_ObjectProgressList = (*l_It).second->GetObjectifsProgress();
        for (std::map< uint8, ObjectifProgess* >::iterator l_Itr = l_ObjectProgressList->begin(); l_Itr != l_ObjectProgressList->end(); ++l_Itr)
            SendMsg("---> " + (*l_It).second->GetQuestTemplate()->m_ObjectifList[(*l_Itr).first]->m_Entitled + " " + std::to_string((*l_Itr).second->m_Data0) + "/" + std::to_string((*l_It).second->GetQuestTemplate()->m_ObjectifList[(*l_Itr).first]->m_Data0));
    }
    return true;
}

bool Player::HandleRegen(std::vector<std::string> p_ListCmd)
{
    if (!p_ListCmd.empty())
        return false;

    RegenerateAll();
    return true;
}

bool Player::HandleBind(std::vector<std::string> p_ListCmd)
{
    if (p_ListCmd.empty())
        return false;

    eKeyBoardAction l_ActionKeyID = (eKeyBoardAction)atoi(p_ListCmd[0].c_str());
    if (l_ActionKeyID >= eKeyBoardAction::MaxKeyBoard)
    {
        SendMsg("Bind: error bind unknow");
        return true;
    }

    if (p_ListCmd.size() == 1)
    {
        SendMsg("Bind: " + std::to_string(GetKeyBoardBind(l_ActionKeyID)));
    }
    else if (p_ListCmd.size() == 2)
    {
        uint8 l_KeyBind = atoi(p_ListCmd[0].c_str());
        if (l_KeyBind >= sf::Keyboard::KeyCount)
        {
            SendMsg("Bind: error key unknow");
            return true;
        }
        AddKeyBoardBind(l_ActionKeyID, l_KeyBind);
        GetSession()->SendKeyBoardBind(l_ActionKeyID, l_KeyBind);
    }
    return true;
}

bool Player::HandleCommandAnnounce(std::vector<std::string> p_ListCmd)
{
    std::string l_Msg = "";
    for (uint8 i = 0; i < p_ListCmd.size(); ++i)
        l_Msg += " " + p_ListCmd[i];

    std::vector<Player*> l_AllPlayers = g_MapManager->GetAllPlayers();
    for (uint16 i = 0; i < l_AllPlayers.size(); ++i)
    {
        l_AllPlayers[i]->SendMsg("**Annonce**:" + l_Msg, eTextColor::TextColorRed);
    }
    return true;
}

bool Player::HandleCommandServer(std::vector<std::string> p_ListCmd)
{
    uint16 l_NbPlayers = 0;

    uint16 l_AllPlayers = g_MapManager->GetTotalPlayers();
    SWTime l_Time = ConvertTimeToSWTime(difftime(GetActualTime(), g_MapManager->GetTimeStart()));

    SendMsg("-> Serveur allumé depuis " + std::to_string(l_Time.m_Days) + "j " + std::to_string(l_Time.m_Hours) + "h " + std::to_string(l_Time.m_Minutes) + "m " + std::to_string(l_Time.m_Seconds) + "s ");
    SendMsg("-> " + std::to_string(l_AllPlayers) + " joueurs");

    return true;
}

bool Player::HandleCommandSkin(std::vector<std::string> p_ListCmd)
{
    if (p_ListCmd.empty())
        return false;

    int16 l_SkinID = atoi(p_ListCmd[0].c_str());
    if (!HasSkin(l_SkinID)/* && GetAccessType() == 0*/)
    {
        SendMsg("Vous ne possédez pas ce skin");
        return true;
    }
    if (/*l_SkinID >= 65 && */GetAccessType() == 0/*MAX_SKIN_IMG*/)
    {
        l_SkinID = l_SkinID % 16;
        l_SkinID += 49;
    }
    else if (l_SkinID >= MAX_SKIN_IMG)
        l_SkinID = 0;
    SetSkinID(l_SkinID);

    return true;
}

bool Player::HandleSave(std::vector<std::string> p_ListCmd)
{
    if (!p_ListCmd.empty())
        return false;

    g_SqlManager->SavePlayer(this);

    return true;
}

bool Player::HandleCommandWho(std::vector<std::string> p_ListCmd)
{
    std::string l_ListNames;
    std::map<uint16, Unit*>* l_ListPlayer = GetMap()->GetListUnitType(TypeUnit::PLAYER);
    for (std::map<uint16, Unit*>::iterator l_It = l_ListPlayer->begin(); l_It != l_ListPlayer->end();)
    {
        l_ListNames += (*l_It).second->GetName();
        if (++l_It != l_ListPlayer->end())
            l_ListNames += ":";
        else
            break;
    }
    SendMsg("ListPlayer: " + l_ListNames);

    return true;
}


bool Player::HandleCommandWhere(std::vector<std::string> p_ListCmd)
{
    Player* l_Player = this;
    std::string l_Name = GetName();
    if (!p_ListCmd.empty())
    {
        l_Name = p_ListCmd[0];

        int32 l_Id = g_SqlManager->GetPlayerID(l_Name);
        if (l_Id <= 0)
            return false;

        Player* l_Player = g_MapManager->GetPlayer(l_Id);
    }
    if (l_Player == nullptr)
        return false;
    SendMsg(l_Name + " -> Map:" + std::to_string(l_Player->GetMapID()) + " X:" + std::to_string(l_Player->GetPosX()) + " Y:" + std::to_string(l_Player->GetPosY()) + " caseNb:" + std::to_string((uint16)((l_Player->GetPosY() / TILE_SIZE) * l_Player->GetMap()->GetSizeX()) + (l_Player->GetPosX() / TILE_SIZE)));

    return true;
}

bool Player::HandleCommandLevel(std::vector<std::string> p_ListCmd)
{
    if (p_ListCmd.empty())
        return false;

    std::string l_Name = p_ListCmd[0];

    int32 l_Id = g_SqlManager->GetPlayerID(l_Name);
    if (l_Id <= 0)
    {
        SendMsg(l_Name + " est introuvable");
        return true;
    }

    Player* l_Player = g_MapManager->GetPlayer(l_Id);
    if (l_Player == nullptr)
        return true;

    SendMsg(l_Name + " est de niveau " + std::to_string(l_Player->GetLevel()));

    return true;
}

bool Player::HandleCommandClass(std::vector<std::string> p_ListCmd)
{
    if (p_ListCmd.empty())
        return false;

    std::string l_Name = p_ListCmd[0];

    int32 l_Id = g_SqlManager->GetPlayerID(l_Name);
    if (l_Id <= 0)
    {
        SendMsg(l_Name + " est introuvable");
        return true;
    }

    Player* l_Player = g_MapManager->GetPlayer(l_Id);
    if (l_Player == nullptr)
        return true;
    eClass l_Class = l_Player->GetClass();
    std::string l_Msg = l_Name;
    if (l_Class == eClass::NONECLASS)
        l_Msg += " n'a pas de classe";
    else
    {
        l_Msg += " a pour classe : ";
        switch (l_Player->GetClass())
        {
        case eClass::ASSASSIN:
            l_Msg += STR_ASSASSIN;
            break;
        case eClass::MAGE:
            l_Msg += STR_MAGE;
            break;
        case eClass::PALADIN:
            l_Msg += STR_PALADIN;
            break;
        case eClass::PRETRE:
            l_Msg += STR_PRETRE;
            break;
        }
    }
    SendMsg(l_Msg);
    return true;
}


bool Player::HandleCommandEmote(std::vector<std::string> p_ListCmd)
{
    if (p_ListCmd.empty())
        return false;

    std::string l_Name = p_ListCmd[0];

    int32 l_Id = g_SqlManager->GetPlayerID(l_Name);
    if (l_Id <= 0)
    {
        SendMsg(l_Name + " est introuvable");
        return true;
    }

    Player* l_Player = g_MapManager->GetPlayer(l_Id);
    if (l_Player == nullptr)
        return true;


    SendMsg("*Vous embrassez " + l_Name + "*");
    l_Player->SendMsg("*" + GetName() + " vous embrasse*");

    return true;
}

bool Player::HandleCommandGroupWisp(std::vector<std::string> p_ListCmd)
{
    if (p_ListCmd.empty())
        return false;

    std::string l_Msg = "";
    for (uint8 i = 0; i < p_ListCmd.size(); ++i)
        l_Msg += " " + p_ListCmd[i];

    ParseStringWithTag(l_Msg);

    std::vector<std::string>* l_Groupes = g_GroupManager->GetGroupForUnit(eGroupType::SIMPLE, this);
    if (l_Groupes == nullptr)
        return false;

    for (std::vector<std::string>::iterator l_It = l_Groupes->begin(); l_It != l_Groupes->end(); l_It++)
    {
        l_Msg = "(" + (*l_It) + ") " + GetName() + ":" + l_Msg;
        PacketSrvPlayerMsg l_Packet;
        l_Packet.BuildPacket(l_Msg);
        g_GroupManager->SendPacketForGroup(eGroupType::SIMPLE, (*l_It), l_Packet.m_Packet);
    }

    return true;
}

bool Player::HandleCommandTeamWisp(std::vector<std::string> p_ListCmd)
{
    if (p_ListCmd.empty())
        return false;

    std::string l_Msg = "";
    for (uint8 i = 0; i < p_ListCmd.size(); ++i)
        l_Msg += " " + p_ListCmd[i];

    ParseStringWithTag(l_Msg);

    if (GetMap()->GetGroupManager() == nullptr)
        return false;

    std::vector<std::string>* l_Groupes = GetMap()->GetGroupManager()->GetGroupForUnit(eGroupType::BG, this);
    if (l_Groupes == nullptr)
        return false;

    for (std::vector<std::string>::iterator l_It = l_Groupes->begin(); l_It != l_Groupes->end(); l_It++)
    {
        l_Msg = "(" + (*l_It) + ") " + GetName() + ":" + l_Msg;
        PacketSrvPlayerMsg l_Packet;
        l_Packet.BuildPacket(l_Msg);
        GetMap()->GetGroupManager()->SendPacketForGroup(eGroupType::BG, (*l_It), l_Packet.m_Packet);
    }

    return true;
}

bool Player::HandleCommandWisp(std::vector<std::string> p_ListCmd)
{
    if (p_ListCmd.empty())
        return false;

    std::string l_Name = p_ListCmd[0];

    int32 l_Id = g_SqlManager->GetPlayerID(l_Name);
    if (l_Id <= 0)
    {
        SendMsg(l_Name + " n'existe pas");
        return true;
    }

    Player* l_Player = g_MapManager->GetPlayer(l_Id);
    if (l_Player == nullptr)
    {
        SendMsg(l_Name + " n'est pas connecté");
        return true;
    }
    std::string l_Msg = "";
    for (uint8 i = 1; i < p_ListCmd.size(); ++i)
        l_Msg += " " + p_ListCmd[i];

    l_Player->SendMsg(GetName() + "(msg): " + l_Msg);
    SendMsg(l_Player->GetName() + "(msg*): " + l_Msg);

    return true;
}

bool Player::HandleCommandCreature(std::vector<std::string> p_ListCmd)
{
    if (p_ListCmd.empty())
        return false;

    if (p_ListCmd[0] == "add" && p_ListCmd.size() < 3)
    {
        /*uint8 l_CreatureEntry = atoi(p_ListCmd[1].c_str());

        uint16 l_Id = g_SqlManager->AddNewCreature(GetMapID(), l_CreatureEntry, GetPosX(), GetPosY());
        Creature* l_Creature = new Creature(l_Id, l_CreatureEntry, g_SqlManager->GetCreatureTemplate(l_CreatureEntry), GetMapID(), GetPosX(), GetPosY());
        Map* l_Map = g_MapManager->GetMap(GetMapID(), GetInstanceID());
        l_Map->AddUnit(l_Creature);*/
    }
    return true;
}

bool Player::HandleCommandSpeed(std::vector<std::string> p_ListCmd)
{
    if (p_ListCmd.empty())
        return false;

    uint8 l_Speed = atoi(p_ListCmd[0].c_str());
    SetSpeed((float)l_Speed / 10.0f);

    return true;
}

bool Player::HandleCommandAddPoint(std::vector<std::string> p_ListCmd)
{
	if (p_ListCmd.empty())
	{
		SendMsg("Vos points-> " + std::to_string(GetPointsSet().GetStat(eStats::Free)) + "|" + std::to_string(GetPointsSet().GetStat(eStats::Force)) + "|" + std::to_string(GetPointsSet().GetStat(eStats::Stamina)) + "|" + std::to_string(GetPointsSet().GetStat(eStats::Dexterity)));
		return true;
	}
	if (p_ListCmd.size() < 2)
	{
		SendMsg("/points add|sub force|stamina|dexterity");
		return true;
	}

	int8 l_IndexCmd = -1;
	if (p_ListCmd[0] == "add")
		l_IndexCmd = 0;
	else if (p_ListCmd[0] == "sub")
		l_IndexCmd = 1;

	int8 l_IndexPoint = -1;
	if (p_ListCmd[1] == "force")
		l_IndexPoint = 0;
	else if (p_ListCmd[1] == "stamina")
		l_IndexPoint = 1;
	else if (p_ListCmd[1] == "dexterity")
		l_IndexPoint = 2;

	int8 l_NbPoints = 1;
	if (p_ListCmd.size() >= 3)
		l_NbPoints = atoi(p_ListCmd[2].c_str());

	l_NbPoints = std::max((int8)1, l_NbPoints);

	if (l_IndexCmd < 0 || l_IndexPoint < 0)
	{
		SendMsg("/point add|sub force|stamina|dexterity");
		return true;
	}

	if (l_IndexCmd == 0)
	{
		if (l_NbPoints > m_PointsSet.GetStat(eStats::Free))
		{
			SendMsg("Vous n'avez pas assez de points");
			return true;
		}
		switch (l_IndexPoint)
		{
			case 0:
			{
                m_PointsSet.SetStat(eStats::Force, m_PointsSet.GetStat(eStats::Force) + l_NbPoints);
				break;
			}
			case 1:
			{
                m_PointsSet.SetStat(eStats::Stamina, m_PointsSet.GetStat(eStats::Stamina) + l_NbPoints);
				break;
			}
			case 2:
			{
                m_PointsSet.SetStat(eStats::Dexterity, m_PointsSet.GetStat(eStats::Dexterity) + l_NbPoints);
				break;
			}
		}
        m_PointsSet.SetStat(eStats::Free, m_PointsSet.GetStat(eStats::Free) - l_NbPoints);
	}

	if (l_IndexCmd == 1)
	{
		switch (l_IndexPoint)
		{
			case 0:
			{
				if (l_NbPoints > m_PointsSet.GetStat(eStats::Force))
				{
					SendMsg("Vous n'avez pas assez de points en force");
					return true;
				}
                m_PointsSet.SetStat(eStats::Force, m_PointsSet.GetStat(eStats::Force) - l_NbPoints);
				break;
			}
			case 1:
			{
				if (l_NbPoints > m_PointsSet.GetStat(eStats::Stamina))
				{
					SendMsg("Vous n'avez pas assez de points en stamina");
					return true;
				}
                m_PointsSet.SetStat(eStats::Stamina, m_PointsSet.GetStat(eStats::Stamina) - l_NbPoints);
				break;
			}
			case 2:
			{
				if (l_NbPoints > m_PointsSet.GetStat(eStats::Dexterity))
				{
					SendMsg("Vous n'avez pas assez de points en dexterity");
					return true;
				}
                m_PointsSet.SetStat(eStats::Dexterity, m_PointsSet.GetStat(eStats::Dexterity) - l_NbPoints);
                break;
			}
		}
        m_PointsSet.SetStat(eStats::Free, m_PointsSet.GetStat(eStats::Free) + l_NbPoints);
	}

	return true;
}

bool Player::HandleCommandSummonPlayer(std::vector<std::string> p_ListCmd)
{
    if (p_ListCmd.empty())
        return false;

    std::string l_Name = p_ListCmd[0];

    int32 l_Id = g_SqlManager->GetPlayerID(l_Name);
    if (l_Id <= 0)
    {
        SendMsg(l_Name + " est introuvable");
        return true;
    }
    Player* l_Player = g_MapManager->GetPlayer(l_Id);
    if (l_Player == nullptr)
        return true;

    l_Player->TeleportTo(GetMapID(), GetInstanceID(), GetPositionCentered().m_X, GetPositionCentered().m_Y, Orientation::Down);
    return true;
}

bool Player::HandleKick(std::vector<std::string> p_ListCmd)
{
    if (p_ListCmd.empty())
        return false;

    std::string l_Name = p_ListCmd[0];

    int32 l_Id = g_SqlManager->GetPlayerID(l_Name);
    if (l_Id <= 0)
    {
        SendMsg(l_Name + " est introuvable");
        return true;
    }
    Player* l_Player = g_MapManager->GetPlayer(l_Id);
    if (l_Player == nullptr)
    {
        SendMsg(l_Name + " non connecté");
        return true;
    }

    l_Player->GetSession()->Kick();

    return true;
}

bool Player::HandlePlayerInfo(std::vector<std::string> p_ListCmd)
{
    if (p_ListCmd.empty())
        return false;

    std::string l_Name = p_ListCmd[0];

    int32 l_Id = g_SqlManager->GetPlayerID(l_Name);
    if (l_Id <= 0)
    {
        SendMsg(l_Name + " est introuvable");
        return true;
    }
    Player* l_Player = g_MapManager->GetPlayer(l_Id);
    if (l_Player == nullptr)
    {
        SendMsg(l_Name + " non connecté");
        return true;
    }

    SendMsg("Player Info " + std::to_string(l_Player->GetAccountID()) + ":" + std::to_string(l_Player->GetID()) + ":" + l_Player->GetName() + ":" + l_Player->GetSession()->getRemoteAddress().toString());

    return true;
}


bool Player::HandleCommandTeleport(std::vector<std::string> p_ListCmd)
{
    if (p_ListCmd.empty())
        return false;

    if (p_ListCmd.size() == 2)
    {
        uint32 l_X = atoi(p_ListCmd[0].c_str());
        uint32 l_Y = atoi(p_ListCmd[1].c_str());

        TeleportTo(l_X, l_Y, Orientation::Down);
        return true;
    }
    else if (p_ListCmd.size() == 3)
    {
        uint16 l_Map = atoi(p_ListCmd[0].c_str());
        uint32 l_X = atoi(p_ListCmd[1].c_str());
        uint32 l_Y = atoi(p_ListCmd[2].c_str());

        TeleportTo(l_Map, 0, l_X, l_Y, Orientation::Down);
        return true;
    }
    else if (p_ListCmd.size() == 4)
    {
        uint16 l_Map = atoi(p_ListCmd[0].c_str());
        uint32 l_Instance = atoi(p_ListCmd[1].c_str());
        uint32 l_X = atoi(p_ListCmd[2].c_str());
        uint32 l_Y = atoi(p_ListCmd[2].c_str());

        TeleportTo(l_Map, l_Instance, l_X, l_Y, Orientation::Down);
        return true;
    }
    return false;
}

bool Player::HandleCommandBG(std::vector<std::string> p_ListCmd)
{
    if (p_ListCmd.empty())
        return false;

    uint32 l_BGID = atoi(p_ListCmd[0].c_str());
    g_MapManager->AddPlayerToQueue(l_BGID, this);

    return true;
}

bool Player::HandleCommandJoin(std::vector<std::string> p_ListCmd)
{
    if (p_ListCmd.empty())
        return false;


    if (p_ListCmd.size() > 1)
        return false;

    g_GroupManager->RemoveUnitFromAllGroupType(eGroupType::SIMPLE, this);
    g_GroupManager->AddUnitToGroup(eGroupType::SIMPLE, p_ListCmd[0], this);
    return true;
}

bool Player::HandleCommandLeave(std::vector<std::string> p_ListCmd)
{
    if (!p_ListCmd.empty())
        return false;

    /*std::vector< std::string >* l_Groups = GetAllGroupsForType(eGroupType::SIMPLE);
    if (l_Groups == nullptr)
    {
        SendMsg("Vous ne faite pas partie d'un groupe");
        return true;
    }*/
    g_GroupManager->RemoveUnitFromAllGroupType(eGroupType::SIMPLE, this);

    return true;
}

bool Player::HandleBanIP(std::vector<std::string> p_ListCmd)
{
    if (p_ListCmd.size() < 2)
        return true;

    std::string l_IP = p_ListCmd[0];
    uint32 l_Hours = atoi(p_ListCmd[1].c_str());

    std::string l_Msg = "";
    for (uint8 i = 2; i < p_ListCmd.size(); ++i)
        l_Msg += " " + p_ListCmd[i];

    g_SqlManager->BlackListIp(l_IP, GetAccountID(), l_Hours, l_Msg);
    return true;
}

bool Player::HandleBanAccount(std::vector<std::string> p_ListCmd)
{
    if (p_ListCmd.size() < 2)
        return true;

    uint32 l_Account = atoi(p_ListCmd[0].c_str());
    uint32 l_Hours = atoi(p_ListCmd[1].c_str());

    std::string l_Msg = "";
    for (uint8 i = 2; i < p_ListCmd.size(); ++i)
        l_Msg += " " + p_ListCmd[i];

    g_SqlManager->BlackListAccount(l_Account, GetAccountID(), l_Hours, l_Msg);
    return true;
}

bool Player::HandleServerRestart(std::vector<std::string> p_ListCmd)
{
    if (p_ListCmd.empty())
    {
        SWExit(1);
        return true;
    }
    if (p_ListCmd.size() < 2)
        return true;
    return true;
}