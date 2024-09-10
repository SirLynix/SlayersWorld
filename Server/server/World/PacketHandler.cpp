#include "PacketHandler.hpp"
#include "PacketDefine.hpp"
#include "../Global.hpp"

PacketHandler::PacketHandler()
{
}

PacketHandler::~PacketHandler()
{
}

void PacketHandler::LoadPacketHandlerMap()
{
    m_PacketHandleMap[CMSG::C_Connexion] = &PacketHandler::HandleConnexion;
    m_PacketHandleMap[CMSG::C_Save] = &PacketHandler::HandleSave;
    m_PacketHandleMap[CMSG::C_StatAction] = &PacketHandler::HandleStatAction;
    m_PacketHandleMap[CMSG::C_UnitCreate] = &PacketHandler::HandleUnitUnknow;
    m_PacketHandleMap[CMSG::C_UnitGoDirection] = &PacketHandler::HandleGoDirection;
    m_PacketHandleMap[CMSG::C_UnitStopMovement] = &PacketHandler::HandleStopMovement;
    m_PacketHandleMap[CMSG::C_UnitTalk] = &PacketHandler::HandleTalk;
    m_PacketHandleMap[CMSG::C_UnitStartAttack] = &PacketHandler::HandleStartAttack;
    m_PacketHandleMap[CMSG::C_UnitEventAction] = &PacketHandler::HandleEventAction;
    m_PacketHandleMap[CMSG::C_UnitStopAttack] = &PacketHandler::HandleStopAttack;
    m_PacketHandleMap[CMSG::C_LoadingPong] = &PacketHandler::HandleLoadingPong;
    m_PacketHandleMap[CMSG::C_UnitAnswerQuestion] = &PacketHandler::HandleAnswerQuestion;
    m_PacketHandleMap[CMSG::C_UpdateTitle] = &PacketHandler::HandleUpdateTitle;
    m_PacketHandleMap[CMSG::C_UpdateSkin] = &PacketHandler::HandleUpdateSkin;
    m_PacketHandleMap[CMSG::C_RemoveItem] = &PacketHandler::HandleRemoveItem;
    m_PacketHandleMap[CMSG::C_ActionItem] = &PacketHandler::HandleActionItem;
    m_PacketHandleMap[CMSG::C_Unequip] = &PacketHandler::HandleUnequip;
    m_PacketHandleMap[CMSG::C_SellItem] = &PacketHandler::HandleSellItem;
}

void PacketHandler::HandleUnitUnknow(WorldPacket &p_Packet, WorldSocket* p_WorldSocket)
{
    uint8 l_Type = 0;
    uint16 l_ID = 0;

    p_Packet >> l_Type;
    p_Packet >> l_ID;


    Player* l_Player = p_WorldSocket->GetPlayer();

    if (l_Player == nullptr)
        return;

    Unit* l_UniknowUnit = l_Player->GetMap()->GetUnit((TypeUnit)l_Type, l_ID);
    if (l_UniknowUnit == nullptr)
        return;

    p_WorldSocket->SendUnitCreate(l_UniknowUnit, l_UniknowUnit->IsInGroupWith(l_Player));
}

void PacketHandler::HandleAnswerQuestion(WorldPacket &p_Packet, WorldSocket* p_WorldSocket)
{
    uint16 l_QuestionID = 0;
    uint8 l_AnswerID = 0;

    p_Packet >> l_QuestionID;
    p_Packet >> l_AnswerID;


    Player* l_Player = p_WorldSocket->GetPlayer();

    if (l_Player == nullptr)
        return;

    std::pair<Unit*, uint16> l_Gossip = l_Player->GetGossipForQuestion(l_QuestionID, l_AnswerID);
    l_Player->RemoveQuestionInProgress(l_QuestionID);
    Unit* l_Unit = l_Gossip.first;
    if (l_Unit == nullptr)
        return;

    l_Unit->GossipTo(l_Player, l_Gossip.second);
    printf("[%s] answer question %d %d\n", l_Player->GetName().c_str(), l_QuestionID, l_AnswerID);
}

void PacketHandler::HandleGoDirection(WorldPacket &p_Packet, WorldSocket* p_WorldSocket)
{
    uint8 l_Orientation = 0;
    Position l_Pos(0, 0);
    uint16 l_PosX;
    uint16 l_PosY;

    p_Packet >> l_Orientation;
    p_Packet >> l_PosX;
    p_Packet >> l_PosY;

    l_Pos.m_X = (uint32)l_PosX;
    l_Pos.m_Y = (uint32)l_PosY;
    Player* l_Player = p_WorldSocket->GetPlayer();

    if (l_Player == nullptr)
        return;

    l_Player->GetMovementHandler()->AddMovementToStack(eActionType::Go, l_Pos, (Orientation)l_Orientation);
    p_WorldSocket->SendUnitGoDirectionToSet((uint8)TypeUnit::PLAYER, l_Player->GetID(), l_Pos, l_Orientation);
}

void PacketHandler::HandleStopMovement(WorldPacket &p_Packet, WorldSocket* p_WorldSocket)
{
    Position l_Pos(0, 0);
    uint16 l_PosX;
    uint16 l_PosY;

    p_Packet >> l_PosX;
    p_Packet >> l_PosY;

    l_Pos.m_X = (uint32)l_PosX;
    l_Pos.m_Y = (uint32)l_PosY;
    Player* l_Player = p_WorldSocket->GetPlayer();

    if (l_Player == nullptr)
        return;

    l_Player->GetMovementHandler()->AddMovementToStack(eActionType::Stop, l_Pos, (Orientation)l_Player->GetOrientation());
    l_Player->GetSession()->SendUnitStopMovement((uint8)TypeUnit::PLAYER, l_Player->GetID(), l_Pos, l_Player->GetOrientation());
}

void PacketHandler::HandleSave(WorldPacket &p_Packet, WorldSocket* p_WorldSocket)
{
    Player* l_Player = p_WorldSocket->GetPlayer();

    if (l_Player == nullptr)
        return;

    l_Player->Save();
    PacketWarningMsg l_Packet;
    l_Packet.BuildPacket(eTypeWarningMsg::Yellow, "Sauvegarde effectué");
    l_Player->GetSession()->send(l_Packet.m_Packet);
}

void PacketHandler::HandleTalk(WorldPacket &p_Packet, WorldSocket* p_WorldSocket)
{
    Player* l_Player = p_WorldSocket->GetPlayer();

    if (l_Player == nullptr)
        return;

    std::string l_String;
    p_Packet >> l_String;

    if (l_Player->CheckCommand(l_String))
        return;

    l_Player->GetSession()->SendUnitTalk((uint8)TypeUnit::PLAYER, l_Player->GetID(), l_String);
}

void PacketHandler::HandleLoadingPong(WorldPacket &p_Packet, WorldSocket* p_WorldSocket)
{
    Player* l_Player = p_WorldSocket->GetPlayer();

    if (l_Player == nullptr)
        return;

    l_Player->SetInLoading(false);
}

void PacketHandler::HandleStartAttack(WorldPacket &p_Packet, WorldSocket* p_WorldSocket)
{
    Position l_Pos(0, 0);

    Player* l_Player = p_WorldSocket->GetPlayer();

    if (l_Player == nullptr)
        return;

    l_Player->Dismount();
    l_Player->GetMovementHandler()->AddMovementToStack(eActionType::Attack);
    l_Player->GetSession()->SendUnitStartAttack((uint8)TypeUnit::PLAYER, l_Player->GetID());
}

void PacketHandler::HandleStopAttack(WorldPacket &p_Packet, WorldSocket* p_WorldSocket)
{
    Player* l_Player = p_WorldSocket->GetPlayer();

    if (l_Player == nullptr)
        return;

    l_Player->GetMovementHandler()->AddMovementToStack(eActionType::StopAttack);
    l_Player->GetSession()->SendUnitStopAttack((uint8)TypeUnit::PLAYER, l_Player->GetID());
}

void PacketHandler::HandleEventAction(WorldPacket &p_Packet, WorldSocket* p_WorldSocket)
{
    Player* l_Player = p_WorldSocket->GetPlayer();
    uint8 l_ActionID;

    p_Packet >> l_ActionID;

    if (l_Player == nullptr)
        return;

    l_Player->EventAction((eKeyBoardAction)l_ActionID);
}

void PacketHandler::HandleUpdateTitle(WorldPacket &p_Packet, WorldSocket* p_WorldSocket)
{
    Player* l_Player = p_WorldSocket->GetPlayer();
    bool l_Apply;
    uint16 l_TitleID;

    p_Packet >> l_Apply;
    p_Packet >> l_TitleID;

    if (l_Player == nullptr)
        return;

    if (!l_Apply)
        l_Player->ChangeActiveTitle(-1);
    else
        l_Player->ChangeActiveTitle(l_TitleID);
}

void PacketHandler::HandleUpdateSkin(WorldPacket &p_Packet, WorldSocket* p_WorldSocket)
{
    Player* l_Player = p_WorldSocket->GetPlayer();
    uint16 l_SkinID;

    p_Packet >> l_SkinID;

    if (l_Player == nullptr)
        return;

    l_Player->SetSkinID(l_SkinID);
}

void PacketHandler::HandleRemoveItem(WorldPacket &p_Packet, WorldSocket* p_WorldSocket)
{
    Player* l_Player = p_WorldSocket->GetPlayer();
    uint8 l_SlotID;

    p_Packet >> l_SlotID;

    if (l_Player == nullptr)
        return;

    l_Player->RemoveItemFromBag(l_SlotID);
}

void PacketHandler::HandleActionItem(WorldPacket &p_Packet, WorldSocket* p_WorldSocket)
{
    Player* l_Player = p_WorldSocket->GetPlayer();
    uint8 l_SlotID;

    p_Packet >> l_SlotID;

    if (l_Player == nullptr)
        return;

    l_Player->ActionItem(l_SlotID);
}

void PacketHandler::HandleSellItem(WorldPacket &p_Packet, WorldSocket* p_WorldSocket)
{
    Player* l_Player = p_WorldSocket->GetPlayer();
    uint8 l_SlotID;

    p_Packet >> l_SlotID;

    if (l_Player == nullptr)
        return;

    l_Player->SellItem(l_SlotID);
}


void PacketHandler::HandleUnequip(WorldPacket &p_Packet, WorldSocket* p_WorldSocket)
{
    Player* l_Player = p_WorldSocket->GetPlayer();
    uint8 l_SlotID;

    p_Packet >> l_SlotID;

    if (l_Player == nullptr)
        return;

    l_Player->UnEquip((eTypeEquipment)l_SlotID);
}

void PacketHandler::HandleStatAction(WorldPacket &p_Packet, WorldSocket* p_WorldSocket)
{
    Player* l_Player = p_WorldSocket->GetPlayer();
    uint8 l_TypeStat;
    bool l_Add;
    uint8 l_Nb;

    p_Packet >> l_TypeStat;
    p_Packet >> l_Add;
    p_Packet >> l_Nb;

    if (l_Player == nullptr)
        return;

    if (l_Add)
        l_Player->AddPointsStat((eStats)l_TypeStat, l_Nb);
    else
        l_Player->SubPointsStat((eStats)l_TypeStat, l_Nb);
}

void PacketHandler::HandleDisconnected(WorldSocket* p_WorldSocket)
{
    if (g_SqlManager == nullptr)
        return;

    Player* l_Player = p_WorldSocket->GetPlayer();

    if (l_Player == nullptr)
        return;

    l_Player->Save();
}

void PacketHandler::HandleConnexion(WorldPacket &p_Packet, WorldSocket* p_WorldSocket)
{
    if (g_SqlManager == nullptr)
        return;

    std::string l_Login;
    std::string l_Password;
    p_Packet >> l_Login;
    p_Packet >> l_Password;

    int32 l_Id = g_SqlManager->GetIDLogin(l_Login, l_Password);

    if (l_Id < 0)
    {
        /*if (!g_SqlManager->IsExistingAccound(l_Login) /* && g_Config->IsPositiveValue("LoginDebug"))
        {
            g_SqlManager->AddNewAccount(l_Login, l_Password);
            WorldPacket l_Packet;
            l_Packet << l_Login;
            l_Packet << l_Password;
            HandleConnexion(l_Packet, p_WorldSocket);
        }
        else*/
            p_WorldSocket->SendAuthResponse(0); ///< Auth Failed
            printf("Auth failed\n");
        return;
    }
    if (g_SqlManager->IsAccountBan(l_Id))
    {
        p_WorldSocket->SendAuthResponse(3); ///< Auth Failed
        return;
    }

    int32 l_IdCharacter = g_SqlManager->GetIDCharacter(l_Id);

    if (l_IdCharacter >= 0)
    {
        if (g_MapManager->IsOnline(TypeUnit::PLAYER, l_IdCharacter))
        {
            p_WorldSocket->SendAuthResponse(2); ///< Already connected
            return;
        }
    }

    /// Keep Log of Connection
    g_SqlManager->AddConnectionLogin(l_Id);

    /// Creation Player
    Player* l_Player = g_SqlManager->GetNewPlayer(l_Id);
    if (l_Player == nullptr)
    {
        p_WorldSocket->SendAuthResponse(3); ///< Auth Failed
        return;
    }

    /// Auth Success
    p_WorldSocket->SendAuthResponse(1); ///< Auth Success

    Map* l_Map = g_MapManager->GetMap(l_Player->GetMapID(), l_Player->GetInstanceID());

    if (l_Map == nullptr || g_MapManager->GetMapTemplate(l_Player->GetMapID())->IsInstance())
    {
        l_Player->SetOrientation(l_Player->GetRespawnPoint().GetOrientation());
        l_Player->SetMapID(l_Player->GetRespawnPoint().GetMapID());
        l_Player->SetPos(l_Player->GetRespawnPoint().GetPosX(), l_Player->GetRespawnPoint().GetPosY());
        l_Player->SetInstanceID(l_Player->GetRespawnPoint().GetInstanceID());
        l_Map = g_MapManager->GetMap(l_Player->GetMapID(), l_Player->GetInstanceID());
        if (l_Map == nullptr)
        {
            printf("Failed load map %d\n", l_Player->GetMapID());
            return;
        }
    }

    l_Player->SetSession(p_WorldSocket);
    printf("Load Player success\n");

    /// Send to Player
    p_WorldSocket->SendPlayerCreate(l_Player->GetID(), l_Player->GetNameWithTitle(), l_Player->GetLevel(), l_Player->GetResourceNb(eResourceType::Health), l_Player->GetResourceNb(eResourceType::Mana), l_Player->GetResourceNb(eResourceType::Alignment), l_Player->GetSkinID(), l_Player->GetMapID(), g_MapManager->GetMapTemplate(l_Player->GetMapID())->GetFileName(), g_MapManager->GetMapTemplate(l_Player->GetMapID())->GetFileChipset(), g_MapManager->GetMapTemplate(l_Player->GetMapID())->GetName(), l_Player->GetPosX(), l_Player->GetPosY(), l_Player->GetOrientation());
    p_WorldSocket->SendUpdateXpPct(g_LevelManager->XpPct(l_Player->GetLevel(), l_Player->GetXp()));
    p_WorldSocket->SendTitles(l_Player->GetTitles());
    p_WorldSocket->SendSkins(l_Player->GetSkinsCollection());
    p_WorldSocket->SendSpells(l_Player->GetSpellList());
    p_WorldSocket->SendBagSize(l_Player->GetBagSize());
    p_WorldSocket->SendItems(l_Player->GetItems());
    p_WorldSocket->SendCurrencies(l_Player->GetCurrencies());
    p_WorldSocket->SendEquipments(l_Player->GetEquipments());

    /// Trick to send stats to player
    l_Player->SetPointsSet(g_SqlManager->GetPointsSetForPlayer(l_Player->GetID()));
    l_Player->SetLevel(l_Player->GetLevel());

    p_WorldSocket->SetPlayer(l_Player);

    /// Send KeyBoard Binds
    p_WorldSocket->SendKeyBoardBind(l_Player->GetKeyBoardBinds());
    p_WorldSocket->SendBindingSpell();

    //printf("Player pos=%d-%d, %d, %d\n", l_Player->GetPosX(), l_Player->GetPosY(), l_Player->GetSizeX(), (l_Player->GetPosY() / TILE_SIZE) * (uint32)l_Player->GetSizeX()) + (l_Player->GetPosX() / TILE_SIZE);
    l_Map->AddUnit(l_Player);
    l_Player->HasBeenInitialize();

    //l_Player->SendMsg("- Bienvenue sur SlayersWorld V0.1 -");
}

void PacketHandler::OperatePacket(WorldPacket &p_Packet, WorldSocket* p_WorldSocket)
{
    uint8 l_PacketID;
    p_Packet >> l_PacketID;

    if (p_WorldSocket->GetPlayer() && p_WorldSocket->GetPlayer()->GetInLoading() && l_PacketID != CMSG::C_LoadingPong) ///< We should not conciderate packet while loading
        return;
    
    //Log("Receive Packet " +  std::to_string(l_PacketID));
    m_Func l_Fun = m_PacketHandleMap[l_PacketID];
    if (l_Fun != nullptr)
        (this->*(l_Fun))(p_Packet, p_WorldSocket);
    else
        Log("Packet " + std::to_string(l_PacketID) + " unknow");

}
