#include "Player.hpp"
#include "../World/WorldSocket.hpp"
#include "../System/Spell/Spell.hpp"
#include "../System/WebHook.hpp"
#include "../World/PacketDefine.hpp"
#include "../Map/Map.hpp"
#include "../Global.hpp"

Player::Player(uint32 p_AccountID, int32 p_ID, std::string p_Name, uint8 p_Level, eClass p_Class, uint8 p_Health, uint8 p_Mana, uint8 p_Alignment, int16 p_SkinID, uint16 p_MapID, uint32 p_PosX, uint32 p_PosY, Orientation p_Orientation, uint32 p_Xp, eAccessType p_AccessType) :
    Unit(p_ID, TypeUnit::PLAYER, eFactionType::Ally),
    m_AccessType(p_AccessType),
    m_AccountID(p_AccountID)
{
    InitializeCommands();
    m_Mod = ePlayerMod::NONEMOD;
    m_Name = p_Name;
    m_Level = p_Level;
    m_Class = p_Class;
    m_SkinID = p_SkinID;
    SetMapID(p_MapID);
    SetPosX(p_PosX);
    SetPosY(p_PosY);
    SetOrientation(p_Orientation);
    m_Session = nullptr;
    m_InLoading = false;
    m_Initilize = false;
    SetResourceNb(eResourceType::Health, p_Health);
    SetResourceNb(eResourceType::Mana, p_Mana);
    SetResourceNb(eResourceType::Alignment, p_Alignment);
    m_Xp = p_Xp;
    m_RespawnTime = PLAYER_TIME_RESPAWN * IN_MICROSECOND;
    m_ActiveTitle = -1;
    m_BagSlots = 0;
    WebHook::sendMsg(g_Config->GetValue("WebhookUrl"), "Connection sur serveur " + g_Config->GetValue("ServerName") + " : " + m_Name + ":" + std::to_string(p_ID) + "   Total : " + std::to_string(g_MapManager->GetTotalPlayers() + 1));
}

WorldSocket* Player::GetSession() const
{
    return m_Session;
}

void Player::HasBeenInitialize()
{
    m_Initilize = true;
}

void Player::SetSession(WorldSocket* p_Session)
{
    m_Session = p_Session;
}

Player::~Player()
{
    printf("Erase Player %d:%s\n", m_ID, m_Name.c_str());
    WebHook::sendMsg(g_Config->GetValue("WebhookUrl"), "Deconnection sur serveur " + g_Config->GetValue("ServerName") + " : " + m_Name + ":" + std::to_string(GetID()) + "   Total : " + std::to_string(g_MapManager->GetTotalPlayers() - 1));
    for (std::map< uint16, Quest* >::iterator l_It = m_Quests.begin(); l_It != m_Quests.end(); ++l_It)
        delete (*l_It).second;
    for (std::map < uint8, Item* >::iterator l_It = m_Items.begin(); l_It != m_Items.end(); ++l_It)
        delete (*l_It).second;
    for (std::map < eTypeEquipment, Item* >::iterator l_It = m_Equipment.begin(); l_It != m_Equipment.end(); ++l_It)
        delete (*l_It).second;
}

uint32 Player::GetAccountID() const
{
    return m_AccountID;
}

void Player::Update(sf::Time p_Diff)
{
    Unit::Update(p_Diff);
    UpdateQuests();
}

void Player::UpdateNewSquares(uint16 p_OldSquareID, uint16 p_NewSquareID, bool p_UpdateAll)
{
    std::vector<uint16> l_OldSquareSet;
    if (!p_UpdateAll)
        l_OldSquareSet = m_Map->GetSquareSetID(p_OldSquareID);
    std::vector<uint16> l_NewSquareSet = m_Map->GetSquareSetID(p_NewSquareID);

    std::vector<uint16> l_DiffSquareSet;
    for (uint16 l_Id : l_NewSquareSet)
    {
        std::vector<uint16>::iterator l_It = std::find(l_OldSquareSet.begin(), l_OldSquareSet.end(), l_Id);

        if (l_It == l_OldSquareSet.end())
            l_DiffSquareSet.push_back(l_Id);
    }

    for (uint16 l_Id : l_DiffSquareSet)
    {
        Square* l_Square = m_Map->GetSquare(l_Id);

        if (l_Square == nullptr)
            continue;

        std::map<TypeUnit, std::map<uint16, Unit*>>* l_SquareList = l_Square->GetList();

        for (std::pair<TypeUnit, std::map<uint16, Unit*>> l_SquareMap : *l_SquareList)
        {
            for (std::pair<uint16, Unit*> l_SquareList : l_SquareMap.second)
            {
                Unit* l_Unit = l_SquareList.second;

                if (l_Unit == nullptr || !l_Unit->IsInWorld())
                    continue;

                if (l_Unit->IsPlayer() && l_Unit->GetID() == GetID())
                    continue;

                GetSession()->SendUnitCreate(l_Unit, l_Unit->IsInGroupWith(this));
            }
        }
    }
}

uint32 Player::GetXp() const
{
    return m_Xp;
}

void Player::SetResourceNb(eResourceType p_Resource, uint8 p_Nb)
{
    Unit::SetResourceNb(p_Resource, p_Nb);
    if (!m_Initilize)
        return;
    m_Session->SendUpdateUnitResource(GetType(), GetID(), p_Resource, p_Nb);
}

void Player::AddResourceNb(eResourceType p_Resource, uint8 p_Nb)
{
    Unit::AddResourceNb(p_Resource, p_Nb);
    if (!m_Initilize)
        return;
    m_Session->SendUpdateUnitResource(GetType(), GetID(), p_Resource, GetResourceNb(p_Resource));
}

bool Player::CheckCommand(const std::string & p_String)
{
    if (p_String.empty())
        return false;

    if (p_String[0] == '/')
    {
        std::string l_Cmd = p_String;
        std::vector<std::string> l_CmdList;

        l_Cmd.erase(0, (size_t)1);

        size_t l_Pos = 0;
        std::string l_Token;
        while ((l_Pos = l_Cmd.find(' ')) != std::string::npos) 
        {
            l_Token = l_Cmd.substr(0, l_Pos);
            l_CmdList.push_back(l_Token);
            l_Cmd.erase(0, l_Pos + 1);
        }
        l_CmdList.push_back(l_Cmd.c_str());

        if (GetAccessType() < m_CmdHandleMap[l_CmdList[0]].first)
            return false;

        m_Func l_Fun = m_CmdHandleMap[l_CmdList[0]].second;
        if (l_Fun != nullptr)
        {
            l_CmdList.erase(l_CmdList.begin());
            return (this->*(l_Fun))(l_CmdList);
        }

        return false;
    }
    return false;
}


void Player::SetXp(uint32 p_Xp)
{
    if (g_LevelManager->IsMaxLevel(GetLevel()))
        return;

    if (p_Xp > g_LevelManager->GetXpForLevel(GetLevel()))
    {
        /// Earn new level
        p_Xp -= g_LevelManager->GetXpForLevel(GetLevel());
        SetLevel(GetLevel() + 1);

		/// Earn new point
        AddPointsStat(eStats::Free, 1);

        PacketUnitPlayVisual l_Packet;
        l_Packet.BuildPacket(GetType(), GetID(), false, 4);
        GetMap()->SendToSet(l_Packet.m_Packet, this);
		SendMsg("Level UP : " + std::to_string(GetLevel()));
    }

    float l_Pct = g_LevelManager->XpPct(GetLevel(), p_Xp);
    if (m_Initilize)
        m_Session->SendUpdateXpPct(l_Pct);
    m_Xp = p_Xp;
}

void Player::SetLevel(const uint8 & p_Level)
{
    Unit::SetLevel(p_Level);

    PacketUnitUpdateStat l_Packet2;
    l_Packet2.BuildPacket(GetType(), GetID(), eStats::Level, p_Level);
    GetSession()->send(l_Packet2.m_Packet);
}


void Player::SendMsg(const std::string & p_Msg, const eTextColor & p_Color, const eTextStyle & p_Style)
{
    std::string l_Msg = p_Msg;
    ParseStringWithTag(l_Msg);
    PacketSrvPlayerMsg l_Packet;
    l_Packet.BuildPacket(l_Msg, p_Color, p_Style);
    WorldSocket* l_Session = GetSession();
    l_Session->SendPacket(l_Packet.m_Packet);
}

std::pair<Unit*, uint16> Player::GetGossipForQuestion(const uint16 & p_ID, const uint8 & p_AnswerID)
{
    std::map < uint16, std::pair< Unit*, std::vector<uint16> > >::iterator l_It = m_QuestionInProgress.find(p_ID);
    std::pair<Unit*, uint16> l_Res;
    l_Res.first = nullptr;

    if (l_It == m_QuestionInProgress.end())
        return l_Res;
    
    std::vector<uint16> l_Answers = (*l_It).second.second;
    if (l_Answers.size() < p_AnswerID)
        return l_Res;

    l_Res.first = (*l_It).second.first;
    l_Res.second = l_Answers[p_AnswerID];
    return l_Res;
}

void Player::AddSkinToCollection(const uint16 & p_ID, Skin* p_Skin)
{
    m_Skins[p_ID] = p_Skin;
}

std::map < uint16, Skin*>* Player::GetSkinsCollection()
{
    return &m_Skins;
}

bool Player::HasSkin(const uint16 & p_Skin)
{
    if (m_Skins.find(p_Skin) == m_Skins.end())
        return false;
    return true;
}

void Player::AddTitle(const uint16 & p_ID, Title* p_Title)
{
    m_Titles[p_ID] = p_Title;
}

int32 Player::GetActiveTitleID() const
{
    return m_ActiveTitle;
}

std::string Player::GetNameWithTitle()
{
    if (m_ActiveTitle < 0)
        return GetName();

    if (m_Titles.find(m_ActiveTitle) == m_Titles.end())
        return GetName();

    Title* l_Title = m_Titles[m_ActiveTitle];
    std::string l_NameWithTitle = GetName();
    if (l_Title->m_Type == eTypeTitle::TypeTitleBefore)
        l_NameWithTitle = l_Title->m_Name + " " + GetName();
    else if (l_Title->m_Type == eTypeTitle::TypeTitleAfter)
        l_NameWithTitle = GetName() + " " + l_Title->m_Name;
    return l_NameWithTitle;
}

void Player::ChangeActiveTitle(const uint16 & p_ID, bool p_Send)
{
    if (m_Titles.find(p_ID) == m_Titles.end())
        m_ActiveTitle = -1;
    else
        m_ActiveTitle = p_ID;

    if (p_Send)
    {
        PacketUpdateName l_Packet;
        l_Packet.BuildPacket(GetType(), GetID(), GetNameWithTitle());
        GetMap()->SendToSet(l_Packet.m_Packet, this);
    }
}

void Player::RemoveActiveTitle()
{
    m_ActiveTitle = -1;

    PacketUpdateName l_Packet;
    l_Packet.BuildPacket(GetType(), GetID(), GetNameWithTitle());
    GetMap()->SendToSet(l_Packet.m_Packet, this);
}

void Player::AddQuestionInProgress(uint16 p_ID, Unit* p_Unit, std::vector<uint16> p_ListAnswers)
{
    std::pair<Unit*, std::vector<uint16>> l_Pair;

    l_Pair.first = p_Unit;
    l_Pair.second = p_ListAnswers;
    m_QuestionInProgress[p_ID] = l_Pair;
}

void Player::RemoveQuestionInProgress(uint16 p_ID)
{
    std::map < uint16, std::pair<Unit*, std::vector<uint16>> >::iterator l_It = m_QuestionInProgress.find(p_ID);

    if (l_It == m_QuestionInProgress.end())
        return;

    m_QuestionInProgress.erase(l_It);
}

void Player::SendSimpleQuestion(const uint16 & p_QuestionID, const std::string & p_Msg)
{
    std::string l_Msg = p_Msg;
    ParseStringWithTag(l_Msg);
    PacketSrvPlayerQuestion l_Packet;
    l_Packet.BuildPacket(p_QuestionID, l_Msg);
    WorldSocket* l_Session = GetSession();
    l_Session->SendPacket(l_Packet.m_Packet);
}

void Player::ParseStringWithTag(std::string & p_Msg)
{
    replaceStr(p_Msg, "$name", GetName());
    replaceStr(p_Msg, "$level", std::to_string(GetLevel()));
}

void Player::Respawn()
{
    Unit::Respawn();

    SetResourceNb(eResourceType::Health, MAX_HEALTH);
    m_ResTimer = 0;
}

eAccessType Player::GetAccessType() const
{
    return m_AccessType;
}

void Player::ActionFrom(Player* p_Player)
{
    ;
}

void Player::EventAction(eKeyBoardAction p_PlayerAction)
{
    if (m_MovementHandler == nullptr)
        return;

    switch (p_PlayerAction)
    {
        case eKeyBoardAction::KeyBoardAction: /// GOSSIP
        {
            Unit* l_Unit = m_Map->GetCloserUnit(this, 1.5f, true, false);
            if (l_Unit == nullptr)
                return;

            l_Unit->ActionFrom(this);
            break;
        }
        case eKeyBoardAction::KeyBoardSpell1:
        case eKeyBoardAction::KeyBoardSpell0:
            if (m_MovementHandler->IsInAttack())
                return;
            if (GetSpellOnBind(p_PlayerAction) > 0)
                CastSpell(GetSpellOnBind(p_PlayerAction));
            break;
        default:
            break;
    }
}

void Player::CastSpell(uint16 p_SpellID)
{
    /*if (!HasSpell(p_SpellID) && GetAccessType() < eAccessType::Moderator)
    {
        PacketWarningMsg l_Packet;
        l_Packet.BuildPacket(eTypeWarningMsg::Red, "Vous ne possedez pas le sort " + std::to_string(p_SpellID));
        GetSession()->send(l_Packet.m_Packet);
        return;
    }*/
    Unit::CastSpell(p_SpellID);
}

void Player::SetPlayerMod(const ePlayerMod & p_PlayerMod)
{
    m_Mod = p_PlayerMod;
}

ePlayerMod Player::GetPlayerMod() const
{
    return m_Mod;
}

void Player::Save()
{
    g_SqlManager->SavePlayer(this);
}

void Player::LearnSpell(uint16 p_SpellID)
{
    if (HasSpell(p_SpellID))
    {
        SendMsg("Vous possedez déja le sort #" + std::to_string(p_SpellID));
    }

    SpellTemplate* l_SpellTemplate = g_SpellManager->GetSpell(p_SpellID);

    if (l_SpellTemplate == nullptr)
        return;

    SendMsg("Vous apprenez le sort " + l_SpellTemplate->GetName());
    g_SqlManager->AddSpellForPlayer(this, p_SpellID);
}

void Player::UnlearnSpell(uint16 p_SpellID)
{
    if (!HasSpell(p_SpellID))
    {
        SendMsg("Vous ne possédez pas le sort #" + std::to_string(p_SpellID));
    }

    SpellTemplate* l_SpellTemplate = g_SpellManager->GetSpell(p_SpellID);

    if (l_SpellTemplate == nullptr)
        return;

    SendMsg("Vous oubliez le sort " + l_SpellTemplate->GetName());

    RemoveSpellBindToKey(p_SpellID);
    g_SqlManager->RemoveSpellForPlayer(this, p_SpellID);

    RemoveSpellID(p_SpellID);
}

void Player::UnlearnAllSpell()
{
    std::map< uint16, uint64 >* l_SpellList = GetSpellList();
    std::vector<uint16> l_SpellIDs;
    for (auto l_Spell : (*l_SpellList))
    {
        l_SpellIDs.push_back(l_Spell.first);
    }
    for (auto l_SpellID : l_SpellIDs)
    {
        UnlearnSpell(l_SpellID);
    }
}


void Player::LearnClass(eClass p_Class)
{
    /// FOR TEST
    /*if (GetClass() != eClass::NONECLASS)
        return;*/

    SetClass(p_Class);
    UnlearnAllSpell();

    std::string l_Msg = "Vous devenez un ";
    switch (p_Class)
    {
    case eClass::ASSASSIN :
        l_Msg += STR_ASSASSIN;
        LearnSpell(6);
        LearnSpell(7);
        g_SqlManager->AddSpellBind(this, 6, 9);
        g_SqlManager->AddSpellBind(this, 7, 10);
        break;
    case eClass::MAGE:
        l_Msg += STR_MAGE;
        LearnSpell(11);
        LearnSpell(12);
        g_SqlManager->AddSpellBind(this, 11, 9);
        g_SqlManager->AddSpellBind(this, 12, 10);
        break;
    case eClass::PALADIN:
        l_Msg += STR_PALADIN;
        LearnSpell(14);
        LearnSpell(15);
        g_SqlManager->AddSpellBind(this, 14, 9);
        g_SqlManager->AddSpellBind(this, 15, 10);
        break;
    case eClass::PRETRE:
        l_Msg += STR_PRETRE;
        LearnSpell(17);
        LearnSpell(18);
        g_SqlManager->AddSpellBind(this, 17, 9);
        g_SqlManager->AddSpellBind(this, 18, 10);
        break;
    }
    SendMsg(l_Msg);
}

void Player::SetClass(eClass p_Class)
{
    m_Class = p_Class;
}

eClass Player::GetClass() const
{
    return m_Class;
}

void Player::UpdateQuests()
{
    /*for (std::map< uint16, Quest* >::iterator l_It = m_Quests.begin(); l_It != m_Quests.end(); ++l_It)
    {
        if ((*l_It).second->IsDone())
            printf("-----> Quest %d is DONE\n", (*l_It).first);
    }*/
}

void Player::ValidateQuest(Quest* p_Quest)
{
    if (m_Quests.find(p_Quest->GetID()) == m_Quests.end())
        return;

    PacketWarningMsg l_Packet;
    l_Packet.BuildPacket(eTypeWarningMsg::Yellow, "Quete : " + p_Quest->GetName() + " terminé");
    GetSession()->send(l_Packet.m_Packet);
    g_SqlManager->SaveQuestForPlayer(this, p_Quest);

    /// TEMP
    SetXp(GetXp() + (g_LevelManager->GetXpForLevel(GetLevel()) / 10));

    RemoveQuest(p_Quest->GetID());
}

void Player::RemoveQuest(uint16 p_QuestID)
{
    auto l_It = m_Quests.find(p_QuestID);
    if (l_It == m_Quests.end())
        return;
    
    delete (*l_It).second;
    m_Quests.erase(l_It);
}

void Player::AddKeyBoardBind(eKeyBoardAction p_Action, uint8 p_ID)
{
    if (p_ID >= sf::Keyboard::KeyCount)
        return;
    m_KeyBoardBinds[p_Action] = p_ID;
    g_SqlManager->ReplaceKeyBindsForAccount(GetAccountID(), p_Action, p_ID);
}

int16 Player::GetKeyBoardBind(eKeyBoardAction p_Action)
{
    if (m_KeyBoardBinds.find(p_Action) == m_KeyBoardBinds.end())
        return -1;
    return m_KeyBoardBinds[p_Action];
}

std::map< uint16, Quest* >* Player::GetQuestList()
{
    return &m_Quests;
}

Quest* Player::GetQuest(uint16 p_QuestID) const
{
    if (m_Quests.find(p_QuestID) != m_Quests.end())
        return m_Quests.at(p_QuestID);
    return nullptr;
}

void Player::AddQuest(Quest* p_Quest, bool p_New /*= true*/) /* False when load at conection*/
{
    if (m_Quests.find(p_Quest->GetID()) != m_Quests.end())
    {
        delete p_Quest;
        return;
    }
    m_Quests[p_Quest->GetID()] = p_Quest;

    if (!p_New)
        return;

    PacketWarningMsg l_Packet;
    l_Packet.BuildPacket(eTypeWarningMsg::Yellow, "Nouvelle Quete : " + p_Quest->GetName());
    GetSession()->send(l_Packet.m_Packet);
}

bool Player::HasQuestInProgress(uint16 p_QuestID)
{
    if (m_Quests.find(p_QuestID) != m_Quests.end())
        return true;
    return false;
}

bool Player::CheckQuestObjective(eObjectifType p_EventType, int32 p_Data0)
{
    bool l_HasMadeObjectif = false;
    for (std::map< uint16, Quest* >::iterator l_It = m_Quests.begin(); l_It != m_Quests.end(); ++l_It)
    {
        std::vector<std::string> l_ListMsg = (*l_It).second->CheckAtEvent(p_EventType, p_Data0);
        for (std::vector<std::string>::iterator l_It = l_ListMsg.begin(); l_It != l_ListMsg.end(); ++l_It)
        {
            PacketWarningMsg l_Packet;
            l_Packet.BuildPacket(eTypeWarningMsg::Yellow, (*l_It));
            GetSession()->send(l_Packet.m_Packet);
            l_HasMadeObjectif = true;
        }
    }
    return l_HasMadeObjectif;
}

void Player::EnterInZone(Zone* p_Zone)
{
    if (p_Zone == nullptr)
        return;

    if (p_Zone->m_TypeID != eTypeZone::QuestZone &&  p_Zone->m_Name != "")
    {
        PacketWarningMsg l_Packet;
        l_Packet.BuildPacket(eTypeWarningMsg::Yellow, p_Zone->m_Name);
        GetSession()->send(l_Packet.m_Packet);
    }
   CheckQuestObjective(eObjectifType::EnterInZone, p_Zone->m_ID);
}

void Player::OutOfZone(Zone* p_Zone)
{
    /*if (p_Zone == nullptr)
        return;

    if (p_Zone->m_Name != "")
    {
        PacketWarningMsg l_Packet;
        l_Packet.BuildPacket(eTypeWarningMsg::Yellow, "out of " + p_Zone->m_Name);
        GetSession()->send(l_Packet.m_Packet);
    }*/
}

std::map< eKeyBoardAction, uint8 >* Player::GetKeyBoardBinds()
{
    return &m_KeyBoardBinds;
}

void Player::AddSpellCooldown(uint16 p_SpellID, uint64 p_Time)
{
    Unit::AddSpellCooldown(p_SpellID, p_Time);

    int32 l_BindSpell = GetBindSpell(p_SpellID);
    if (l_BindSpell < 0)
        return;

    /// Block bind of spell
    PacketKeyBoardBlock l_Packet;
    l_Packet.BuildPacket((uint8)l_BindSpell, (uint16)(p_Time / 1000));
    WorldSocket* l_Session = GetSession();
    l_Session->SendPacket(l_Packet.m_Packet);
}

void Player::AddSpellBindToKey(uint16 p_SpellID, uint8 p_Bind)
{
    m_SpellsBindToKey[p_SpellID] = p_Bind;
}

void Player::RemoveSpellBindToKey(uint16 p_SpellID)
{
    std::map<uint16, uint8>::iterator l_It = m_SpellsBindToKey.find(p_SpellID);

    if (l_It == m_SpellsBindToKey.end())
        return;

    m_SpellsBindToKey.erase(l_It);
    g_SqlManager->RemoveSpellBind(this, p_SpellID);
}


int32 Player::GetSpellOnBind(uint8 p_Bind)
{
    int32 l_Spell = -1;
    for (std::map<uint16, uint8>::iterator l_It = m_SpellsBindToKey.begin();  l_It != m_SpellsBindToKey.end(); ++l_It)
    {
        if ((*l_It).second == p_Bind)
        {
            l_Spell = (*l_It).first;
            break;
        }
    }

    return l_Spell;
}

int32 Player::GetBindSpell(uint16 p_SpellID)
{
    if (m_SpellsBindToKey.find(p_SpellID) == m_SpellsBindToKey.end())
        return -1;
    return m_SpellsBindToKey[p_SpellID];
}

void Player::SetInLoading(bool p_InLoading)
{
    m_InLoading = p_InLoading;
    if (!p_InLoading)
        return;
    if (GetSession() == nullptr)
        return;

    PacketLoadingPing l_Packet;
    l_Packet.BuildPacket();
    WorldSocket* l_Session = GetSession();
    l_Session->SendPacket(l_Packet.m_Packet);
}

bool Player::GetInLoading() const
{
    return m_InLoading;
}

void Player::SetPointsSet(const PointsSet & p_PointSet)
{
    Unit::SetPointsSet(p_PointSet);

    for (uint8 i = 0; i < eStats::MaxStat; ++i)
    {
        if (i == eStats::Speed)
            continue;
        PacketUnitUpdateStat l_Packet;
        l_Packet.BuildPacket(GetType(), GetID(), i, GetPointsSet().GetStat((eStats)i));
        GetSession()->send(l_Packet.m_Packet);
    }
}

bool Player::AddPointsStat(eStats p_TypeStat, uint8 p_Nb)
{
    if (!Unit::AddPointsStat(p_TypeStat, p_Nb))
        return false;

    if (p_TypeStat != eStats::Free)
    {
        PacketUnitUpdateStat l_Packet;
        l_Packet.BuildPacket(GetType(), GetID(), p_TypeStat, GetPointsSet().GetStat(p_TypeStat));
        GetSession()->send(l_Packet.m_Packet);
    }
    PacketUnitUpdateStat l_Packet2;
    l_Packet2.BuildPacket(GetType(), GetID(), eStats::Free, GetPointsSet().GetStat(eStats::Free));
    GetSession()->send(l_Packet2.m_Packet);
    return true;
}

bool Player::SubPointsStat(eStats p_TypeStat, uint8 p_Nb)
{
    if (!Unit::SubPointsStat(p_TypeStat, p_Nb))
        return false;

    PacketUnitUpdateStat l_Packet;
    l_Packet.BuildPacket(GetType(), GetID(), p_TypeStat, GetPointsSet().GetStat(p_TypeStat));
    GetSession()->send(l_Packet.m_Packet);
    PacketUnitUpdateStat l_Packet2;
    l_Packet2.BuildPacket(GetType(), GetID(), eStats::Free, GetPointsSet().GetStat(eStats::Free));
    GetSession()->send(l_Packet2.m_Packet);
    return true;
}

void Player::LeaveGroup(const std::string & p_GroupName)
{
    SendMsg("Vous venez de quitter le groupe '" + p_GroupName + "'");
}

void Player::EnterInGroup(const std::string & p_GroupName)
{
    SendMsg("Vous venez de rejoindre le groupe '" + p_GroupName + "'");
}

void Player::UnitLeaveGroup(Unit* p_Unit, const std::string & p_GroupName)
{
    Unit::UnitLeaveGroup(p_Unit, p_GroupName);
    SendMsg(p_Unit->GetName() + " vient de quitter le groupe '" + p_GroupName + "'");
    if (IsInSetWith(p_Unit))
    {
        PacketUnitIsInGroup l_Packet;
        /// Send to others of group
        l_Packet.BuildPacket(p_Unit->GetType(), p_Unit->GetID(), false);
        GetSession()->SendPacket(l_Packet.m_Packet);
    }
}

void Player::UnitEnterInGroup(Unit* p_Unit, const std::string & p_GroupName)
{
    Unit::UnitEnterInGroup(p_Unit, p_GroupName);

    SendMsg(p_Unit->GetName() + " vient de rejoindre le groupe '" + p_GroupName + "'");
    if (IsInSetWith(p_Unit))
    {
        PacketUnitIsInGroup l_Packet;
        /// Send to others of group
        l_Packet.BuildPacket(p_Unit->GetType(), p_Unit->GetID(), true);
        GetSession()->SendPacket(l_Packet.m_Packet);
    }
}

std::map<uint16, Title*>* Player::GetTitles()
{
    return &m_Titles;
}

void Player::AddEquipment(const eTypeEquipment & p_Type, Item* p_Item, bool p_New)
{
    if (GetEquipment(p_Type) != nullptr)
        RemoveEquipment(p_Type);

    m_Equipment[p_Type] = p_Item;

    if (!p_New)
        return;

    g_SqlManager->AddNewEquipmentForPlayer(GetID(), p_Type, p_Item->GetTemplate()->m_Id);
    std::map<eTypeEquipment, Item*> p_Equipments;
    p_Equipments[p_Type] = p_Item;
    GetSession()->SendEquipments(&p_Equipments);
}

std::map< eTypeEquipment, Item*>* Player::GetEquipments()
{
    return &m_Equipment;
}

Item* Player::GetEquipment(const eTypeEquipment & p_Type)
{
    if (m_Equipment.find(p_Type) == m_Equipment.end())
        return nullptr;
    return m_Equipment[p_Type];
}

void Player::RemoveEquipment(const eTypeEquipment & p_Type)
{
    std::map<eTypeEquipment, Item*>::iterator l_It = m_Equipment.find(p_Type);

    if (l_It == m_Equipment.end())
        return;

    Item* l_Item = (*l_It).second;

    m_Equipment.erase(l_It);
    delete l_Item;
}

void Player::UnEquip(const eTypeEquipment & p_Type)
{
    std::map<eTypeEquipment, Item*>::iterator l_It = m_Equipment.find(p_Type);

    if (l_It == m_Equipment.end())
        return;

    Item* l_Item = (*l_It).second;
    if (!AddItemOnAvailableSlot(l_Item))
        return;

    m_Equipment.erase(l_It);

    g_SqlManager->RemoveEquipmentForPlayer(GetID(), p_Type);

    PacketPlayerRemoveEquipment l_Packet;
    l_Packet.BuildPacket(p_Type);
    GetSession()->send(l_Packet.m_Packet);
}

int16 Player::CanCanBeStackOnBagSlot(Item* p_Item)
{
    uint8 i = 0;

    if (p_Item->GetTemplate()->m_StackNb <= 1) ///< If it doesn't stack, don't need to check
        return -1;

    for (i = 0; i < GetBagSize(); i++)
    {
        std::map<uint8, Item*>::iterator l_It = m_Items.find(i);
        if (l_It == m_Items.end())
            continue;

        Item* l_Item = (*l_It).second;

        if (l_Item == nullptr)
            continue;

        if (l_Item->GetTemplate() == nullptr)
            continue;

        if (l_Item->GetTemplate()->m_Id == p_Item->GetTemplate()->m_Id && l_Item->GetStackNb() < p_Item->GetTemplate()->m_StackNb)
            return (int16)i;
    }

    return -1;
}


bool Player::AddItemOnAvailableSlot(Item* p_Item)
{
    int16 i = CanCanBeStackOnBagSlot(p_Item);
    while (i >= 0) ///< If we can fit it with existing
    {
        uint8 l_RemainingStack = StackItem((uint8)i, p_Item->GetStackNb());
        if (l_RemainingStack == 0)
        {
            delete p_Item;
            return true;
        }
        p_Item->SetStackNb(l_RemainingStack);
        i = CanCanBeStackOnBagSlot(p_Item);
    }

    /// Search a new place
    for (i = 0; (uint8)i < GetBagSize(); i++)
    {
        std::map<uint8, Item*>::iterator l_It = m_Items.find((uint8)i);
        if (l_It == m_Items.end())
            break;
    }
    if ((uint8)i == GetBagSize()) ///< No available slot
        return false;
    AddItem((uint8)i, p_Item, true);

    return true;
}

void Player::AddItem(const uint8 & p_Slot, Item* p_Item, bool p_New)
{
    m_Items[p_Slot] = p_Item;

    if (!p_New)
        return;

    g_SqlManager->AddNewItemForPlayer(GetID(), p_Slot, p_Item->GetTemplate()->m_Id, p_Item->GetStackNb());
    std::map<uint8, Item*> p_Items;
    p_Items[p_Slot] = p_Item;
    GetSession()->SendItems(&p_Items);
}

std::map< uint8, Item*>* Player::GetItems()
{
    return &m_Items;
}

Item* Player::GetItem(const uint8 & p_Slot)
{
    if (m_Items.find(p_Slot) == m_Items.end())
        return nullptr;

    return m_Items[p_Slot];
}

bool Player::ConsumeItemFromBag(const uint8 & p_Slot)
{
    std::map<uint8, Item*>::iterator l_It = m_Items.find(p_Slot);

    if (l_It == m_Items.end())
        return false;

    Item* l_Item = (*l_It).second;

    UnstackItem(p_Slot);
    return true;
}

void Player::RemoveItemFromBag(const uint8 & p_Slot, const bool & p_Delete)
{
    std::map<uint8, Item*>::iterator l_It = m_Items.find(p_Slot);

    if (l_It == m_Items.end())
        return;

    Item* l_Item = (*l_It).second;

    m_Items.erase(l_It);
    if (p_Delete)
        delete l_Item;

    g_SqlManager->RemoveItemForPlayer(GetID(), p_Slot);

    PacketPlayerRemoveItem l_Packet;
    l_Packet.BuildPacket(p_Slot);
    GetSession()->send(l_Packet.m_Packet);
}

void Player::OpenSeller()
{
    PacketSellItemInterface l_Packet;

    std::map<uint8, uint16> p_Price;

    for (std::map < uint8, Item* >::iterator l_It = m_Items.begin(); l_It != m_Items.end(); l_It++)
    {
        p_Price[(*l_It).first] = (*l_It).second->GetTemplate()->m_Price * (*l_It).second->GetStackNb();
    }

    l_Packet.BuildPacket(&p_Price);
    GetSession()->SendPacket(l_Packet.m_Packet);
}

void Player::SellItem(const uint8 & p_Slot)
{
    std::map<uint8, Item*>::iterator l_It = m_Items.find(p_Slot);

    if (l_It == m_Items.end())
        return;

    Item* l_Item = (*l_It).second;

    uint16 l_Price = l_Item->GetTemplate()->m_Price;
    l_Price *= l_Item->GetStackNb();

    uint16 l_Currency = GetCurrency(eTypeCurrency::CURRENCY_BASISC);
    UpdateCurrency(eTypeCurrency::CURRENCY_BASISC, l_Price + l_Currency, true);
    RemoveItemFromBag(p_Slot, false);
}

void Player::ActionItem(const uint8 & p_Slot)
{
    std::map<uint8, Item*>::iterator l_It = m_Items.find(p_Slot);

    if (l_It == m_Items.end())
        return;

    Item* l_Item = (*l_It).second;

    switch (l_Item->GetTemplate()->m_Type)
    {
    case eItemType::ITEM_EQUIPMENT :
        AddEquipment((eTypeEquipment)l_Item->GetTemplate()->m_SubType, l_Item, true);
        RemoveItemFromBag(p_Slot, false);
        break;
    case eItemType::ITEM_CONSUMABLE :
        CastSpell(l_Item->GetTemplate()->GetData(0));
        ConsumeItemFromBag(p_Slot);
        break;
    default :
        break;
    }
}

uint8 Player::StackItem(const uint8 & p_Slot, const uint8 & p_StackNb)
{
    Item* l_Item = GetItem(p_Slot);
    if (l_Item == nullptr)
        return p_StackNb;
    
    uint8 i = 0;
    for (i = 0; i < p_StackNb; i++)
    {
        if (!l_Item->AddStack())
            break;
    }
    g_SqlManager->UpdateItemStackForPlayer(GetID(), p_Slot, l_Item->GetTemplate()->m_Id, l_Item->GetStackNb());

    PacketPlayerStackItem l_Packet;
    l_Packet.BuildPacket(p_Slot, l_Item->GetStackNb());
    GetSession()->send(l_Packet.m_Packet);

    return p_StackNb - i;
}

void Player::UnstackItem(const uint8 & p_Slot)
{
    Item* l_Item = GetItem(p_Slot);
    if (l_Item == nullptr)
        return;

    if (!l_Item->SubStack())
        return;
    if (l_Item->GetStackNb() <= 0)
    {
        RemoveItemFromBag(p_Slot);
        return;
    }

    g_SqlManager->UpdateItemStackForPlayer(GetID(), p_Slot, l_Item->GetTemplate()->m_Id, l_Item->GetStackNb());

    PacketPlayerStackItem l_Packet;
    l_Packet.BuildPacket(p_Slot, l_Item->GetStackNb());
    GetSession()->send(l_Packet.m_Packet);
}

void Player::SetMaxBagSlot(const uint16 & p_MaxSlot)
{
    m_BagSlots = p_MaxSlot;
}

uint8 Player::GetBagSize() const
{
    return m_BagSlots;
}

void Player::UpdateCurrency(const eTypeCurrency & p_Type, const uint16 & p_Value, const bool & p_Send)
{
    m_Currencies[p_Type] = p_Value;

    if (!p_Send)
        return;

    g_SqlManager->UpdateCurrencyForPlayer(GetID(), p_Type, p_Value);

    std::map<eTypeCurrency, uint16> p_Currencies;
    p_Currencies[p_Type] = p_Value;
    GetSession()->SendCurrencies(&p_Currencies);
}

uint16 Player::GetCurrency(const eTypeCurrency & p_Type)
{
    if (m_Currencies.find(p_Type) == m_Currencies.end())
        return 0;

    return m_Currencies[p_Type];
}

std::map< eTypeCurrency, uint16>* Player::GetCurrencies()
{
    return &m_Currencies;
}

void Player::AddRewardList(std::vector<SubReward*> p_RewardList)
{
    for (uint8 i = 0; i < p_RewardList.size(); i++)
    {
        SubReward* l_SubReward = p_RewardList[i];
        if (l_SubReward->m_Required != nullptr && !l_SubReward->m_Required->IsValid(this))
            continue;

        switch (l_SubReward->m_Type)
        {
            case eRewardType::REWARD_ITEM :
            {
                RewardItem(l_SubReward->GetData(0), l_SubReward->GetData(1));
                break;
            }
            case eRewardType::REWARD_CURRENCY:
            {
                RewardCurrency((eTypeCurrency)l_SubReward->GetData(0), l_SubReward->GetData(1));
                break;
            }
        }
    }
}

void Player::RewardItem(const uint16 & p_ItemID, const uint8 & p_Stack)
{
    ItemTemplate* l_ItemTemplate = g_ItemManager->GetItemTemplate(p_ItemID);
    if (l_ItemTemplate == nullptr)
        return;
    Item* l_Item = new Item(this, l_ItemTemplate);
    l_Item->SetStackNb(p_Stack);

    if (AddItemOnAvailableSlot(l_Item))
    {
        PacketWarningMsg l_Packet;
        l_Packet.BuildPacket(eTypeWarningMsg::WarningReward, l_ItemTemplate->m_Name + "x" + std::to_string(p_Stack));
        GetSession()->send(l_Packet.m_Packet);
    }
    else
        delete l_Item;
}

void Player::RewardCurrency(const eTypeCurrency & p_Type, const uint16 & p_Nb)
{
    uint16 l_NewValue = GetCurrency(p_Type) + p_Nb;
    UpdateCurrency(p_Type, l_NewValue, true);

    PacketWarningMsg l_Packet;
    l_Packet.BuildPacket(eTypeWarningMsg::WarningReward, std::to_string(p_Nb));
    GetSession()->send(l_Packet.m_Packet);
}

void Player::LearnSkin(const uint16 & p_SkinID, const bool & p_New)
{
    if (g_Skins.find(p_SkinID) != g_Skins.end())
        AddSkinToCollection(p_SkinID, &g_Skins[p_SkinID]);
    else
        return;

    if (!p_New)
        return;

    g_SqlManager->AddNewSkinForPlayer(GetID(), p_SkinID);
    std::map<uint16, Skin*> l_SkinMap;
    l_SkinMap[p_SkinID] = &g_Skins[p_SkinID];
    GetSession()->SendSkins(&l_SkinMap);
}

void Player::LearnTitle(const uint16 & p_TitleID, const bool & p_New)
{
    if (g_Titles.find(p_TitleID) != g_Titles.end())
        AddTitle(p_TitleID, &g_Titles[p_TitleID]);
    else
        return;

    if (!p_New)
        return;

    g_SqlManager->AddNewTitleForPlayer(GetID(), p_TitleID);
    std::map<uint16, Title*> l_TitleMap;
    l_TitleMap[p_TitleID] = &g_Titles[p_TitleID];
    GetSession()->SendTitles(&l_TitleMap);
}