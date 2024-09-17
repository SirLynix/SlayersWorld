#include "SqlManager.hpp"
#include "../Define.hpp"
#include "../Entities/Creature.hpp"
#include "../Entities/Areatrigger.hpp"
#include "../Entities/GameObject.hpp"
#include "../Global.hpp"
#include "../System/WebHook.hpp"
#include <cstdlib>
#include <ctime>
#include <stdexcept>

SqlManager::SqlManager()
{
}


SqlManager::~SqlManager()
{
    sqlite3_close(m_MysqlCharacters);
    sqlite3_close(m_MysqlWorld);
}

bool SqlManager::InitializeCharacters(std::string p_DB)
{
    if (sqlite3_open(p_DB.c_str(), &m_MysqlCharacters) != SQLITE_OK)
    {
        printf("failed to load %s: %s", p_DB.c_str(), sqlite3_errmsg(m_MysqlCharacters));
        sqlite3_close(m_MysqlCharacters);
        return false;
    }

	return true;
}

bool SqlManager::InitializeWorld(std::string p_DB)
{
    if (sqlite3_open(p_DB.c_str(), &m_MysqlWorld) != SQLITE_OK)
    {
        printf("failed to load %s: %s", p_DB.c_str(), sqlite3_errmsg(m_MysqlWorld));
        sqlite3_close(m_MysqlWorld);
        return false;
    }

	return true;
}

void SqlManager::AddNewAccount(std::string p_Login, std::string p_Password)
{
    std::string l_Query = "INSERT INTO `login` (login, password) VALUES ('" + p_Login + "', MD5('" + p_Password + "'));";
    Exec(m_MysqlCharacters, l_Query);
}

bool SqlManager::IsExistingAccound(std::string p_Login)
{
    std::string l_Query = "SELECT `id` FROM `login` WHERE `login` = '" + p_Login + "'";

    sqlite3_stmt* l_Stmt;
    if (!Prepare(m_MysqlCharacters, l_Query, &l_Stmt))
        throw std::runtime_error("invalid database query");

    bool l_Existing = false;
    while (sqlite3_step(l_Stmt) == SQLITE_ROW)
    {
        l_Existing = true;
    }
    sqlite3_finalize(l_Stmt);

    return true;
}

int32 SqlManager::GetIDLogin(std::string p_Login, std::string p_Password)
{
    std::string l_Query = "SELECT `id` FROM `login` WHERE `login` = '" + p_Login + "' AND `password` = MD5('" + p_Password + "')";

    sqlite3_stmt* l_Stmt;
    if (!Prepare(m_MysqlCharacters, l_Query, &l_Stmt))
        throw std::runtime_error("invalid database query");

    int32 l_ID = -1;
    while (sqlite3_step(l_Stmt) == SQLITE_ROW)
    {
        l_ID = sqlite3_column_int(l_Stmt, 0);
    }
    sqlite3_finalize(l_Stmt);

    return l_ID;
}

int32 SqlManager::GetIDCharacter(uint32 p_AccountID)
{
    std::string l_Query = "SELECT `characterID` FROM `characters` WHERE `accountID` = '" + std::to_string(p_AccountID) + "'";

    sqlite3_stmt* l_Stmt;
    if (!Prepare(m_MysqlCharacters, l_Query, &l_Stmt))
        throw std::runtime_error("invalid database query");

    int32 l_ID = -1;
    while (sqlite3_step(l_Stmt) == SQLITE_ROW)
    {
        l_ID = sqlite3_column_int(l_Stmt, 0);
    }
    sqlite3_finalize(l_Stmt);

    return l_ID;
}

void SqlManager::AddConnectionLogin(uint32 p_AccountID)
{
    std::string l_Query = "insert into `connection_logs` (`accountID`) values('" + std::to_string(p_AccountID) + "');";
    Exec(m_MysqlCharacters, l_Query);
    l_Query = "UPDATE `characters` SET `lastConnection` = NOW() WHERE accountID = " + std::to_string(p_AccountID) + ";";
    Exec(m_MysqlCharacters, l_Query);
}

std::string SqlManager::GetLoginName(uint32 p_AccountID)
{
    std::string l_Query = "SELECT `characterName` FROM `login` WHERE `id` = '" + std::to_string(p_AccountID) + "';";

    sqlite3_stmt* l_Stmt;
    if (!Prepare(m_MysqlCharacters, l_Query, &l_Stmt))
        throw std::runtime_error("invalid database query");

    std::string l_Name;
    while (sqlite3_step(l_Stmt) == SQLITE_ROW)
    {
        l_Name = (const char*)sqlite3_column_text(l_Stmt, 0);
    }
    sqlite3_finalize(l_Stmt);

    return l_Name;
}

void SqlManager::AddNewPlayer(uint32 p_AccountID)
{
    std::string l_Query = "insert into `characters` (`accountID`, `name`, `skinID`, `level`, `health`, `alignment`, `mapID`, `posX`, `posY`, `orientation`, `xp`) values('" + std::to_string(p_AccountID) + "', '" + GetLoginName(p_AccountID) + "','49','1','100','0','" + std::to_string(CREATION_POINT_MAP) + "','" + std::to_string(CREATION_POINT_X) + "','" + std::to_string(CREATION_POINT_Y) + "','2','0');";
    Exec(m_MysqlCharacters, l_Query);
}

void SqlManager::AddKeyDefaultBindsForAccount(uint32 p_AccountID)
{
    std::string l_Query = "INSERT INTO `account_key_binds` VALUES (" + std::to_string(p_AccountID) + ", 1, 74), (" + std::to_string(p_AccountID) + ", 2, 73), (" + std::to_string(p_AccountID) + ", 3, 71), (" + std::to_string(p_AccountID) + ", 4, 72), (" + std::to_string(p_AccountID) + ", 5, 57), (" + std::to_string(p_AccountID) + ", 6, 58), (" + std::to_string(p_AccountID) + ", 7, 85), (" + std::to_string(p_AccountID) + ", 8, 18), (" + std::to_string(p_AccountID) + ", 9, 25), (" + std::to_string(p_AccountID) + ", 10, 4);";
    Exec(m_MysqlCharacters, l_Query);
}

void SqlManager::ReplaceKeyBindsForAccount(uint32 p_AccountID, eKeyBoardAction p_Bind, uint8 p_Key)
{
    std::string l_Query = "REPLACE `account_key_binds` SET `key` = " + std::to_string(p_Key) + " WHERE `typeID` = " + std::to_string(p_Bind) + " AND `accountID` = " + std::to_string(p_AccountID) + ";";
    Exec(m_MysqlCharacters, l_Query);
}

Player* SqlManager::GetNewPlayer(uint32 p_AccountID)
{
    std::string l_Query = "SELECT characterID, name, level, class, health, mana, alignment, activeTitleID, slotBagNb, skinID, mapID, posX, posY, orientation, xp FROM characters WHERE accountID = '" + std::to_string(p_AccountID) + "'";
    sqlite3_stmt* l_Stmt;
    if (!Prepare(m_MysqlCharacters, l_Query, &l_Stmt))
        throw std::runtime_error("invalid database query");

    uint32 l_ID = 0;
    std::string l_Name = "";
    uint8 l_Lvl = 0;
    uint8 l_Class = 0;
    uint8 l_Health = 0;
    uint8 l_Mana = 0;
    uint8 l_Alignment = 0;
    int32 l_ActiveTitleID = -1;
    int16 l_SlotBagNb = 0;
    int16 l_SkinID = 0;
    uint16 l_MapID = 0;
    uint32 l_PosX = 0;
    uint32 l_PosY = 0;
    uint8 l_Orientation = 0;
    uint32 l_Xp = 0;

    bool l_Exist = false;
    while (sqlite3_step(l_Stmt) == SQLITE_ROW)
    {
        l_Exist = true;
        l_ID = sqlite3_column_int(l_Stmt, 0);
        l_Name = (const char*) sqlite3_column_text(l_Stmt, 1);
        l_Lvl = sqlite3_column_int(l_Stmt, 2);
        l_Class = sqlite3_column_int(l_Stmt, 3);
        l_Health = sqlite3_column_int(l_Stmt, 4);
        l_Mana = sqlite3_column_int(l_Stmt, 5);
        l_Alignment = sqlite3_column_int(l_Stmt, 6);
        l_ActiveTitleID = sqlite3_column_int(l_Stmt, 7);
        l_SlotBagNb = sqlite3_column_int(l_Stmt, 8);
        l_SkinID = sqlite3_column_int(l_Stmt, 9);
        l_MapID = sqlite3_column_int(l_Stmt, 10);
        l_PosX = sqlite3_column_int(l_Stmt, 11);
        l_PosY = sqlite3_column_int(l_Stmt, 12);
        l_Orientation = sqlite3_column_int(l_Stmt, 13);
        l_Xp = sqlite3_column_int(l_Stmt, 14);
    }
    sqlite3_finalize(l_Stmt);

    eAccessType l_PlayerAccessType = GetAccessType(p_AccountID);
    eAccessType l_AccessRequired = (eAccessType)atoi(g_Config->GetValue("AccessLevel").c_str());

    if (l_AccessRequired > l_PlayerAccessType)
    {
        WebHook::sendMsg(g_Config->GetValue("WebhookUrl"), l_Name + " essai de se connecter mais n'a pas les droits");
        return nullptr;
    }
    if (!l_Exist)
    {
        AddNewPlayer(p_AccountID);
        AddKeyDefaultBindsForAccount(p_AccountID);
        Player* l_Player = GetNewPlayer(p_AccountID);
        /* FOR TEST*/
        /*AddSpellForPlayer(l_Player, 1);
        AddSpellForPlayer(l_Player, 2);
        AddSpellBind(l_Player, 1, 9);
        AddSpellBind(l_Player, 2, 10);*/
        return l_Player;
    }

    Player* l_Player = new Player(p_AccountID, l_ID, l_Name, l_Lvl, (eClass)l_Class, l_Health, l_Mana, l_Alignment, l_SkinID, l_MapID, l_PosX, l_PosY, (Orientation)l_Orientation, l_Xp, l_PlayerAccessType);
    l_Player->SetRespawnPosition(GetRespawnPositionForPlayer(l_ID));
    l_Player->SetMaxBagSlot(l_SlotBagNb);

    InitializeSpellsForPlayer(l_Player);
    InitializeKeyBindsForAccount(p_AccountID, l_Player);
    InitializeSpellsBinds(l_Player);
    InitializeQuestsProgessForPlayer(l_Player);
    InitializeListTitlesForPlayer(l_Player);
    InitializeListSkinsForPlayer(l_Player);
    InitializeListItemForPlayer(l_Player);
    InitializeListCurrenciesForPlayer(l_Player);
    InitializeListEquipmentsForPlayer(l_Player);
    l_Player->ChangeActiveTitle(l_ActiveTitleID, false);

    return l_Player;
}

void SqlManager::AddNewRespawnPositionForPlayer(uint32 p_PlayerID)
{
    std::string l_Query = "insert into `characters_respawn` (`characterID`, `posX`, `posY`, `mapID`, `orientation`) values('" + std::to_string(p_PlayerID) + "', '" + std::to_string(CREATION_POINT_X) + "', '" + std::to_string(CREATION_POINT_Y) + "', '" + std::to_string(CREATION_POINT_MAP) + "', '2');";
    Exec(m_MysqlCharacters, l_Query);
}

void SqlManager::AddNewSkinForPlayer(const uint32 & p_PlayerID, const uint16 & p_SkinID)
{
    std::string l_Query = "replace into `characters_skins` (`characterID`, `skinID`) values('" + std::to_string(p_PlayerID) + "', '" + std::to_string(p_SkinID) + "');";
    Exec(m_MysqlCharacters, l_Query);
}

void SqlManager::AddNewTitleForPlayer(const uint32 & p_PlayerID, const uint16 & p_TitleID)
{
    std::string l_Query = "replace into `characters_titles` (`characterID`, `skinID`) values('" + std::to_string(p_PlayerID) + "', '" + std::to_string(p_TitleID) + "');";
    Exec(m_MysqlCharacters, l_Query);
}

void SqlManager::AddNewItemForPlayer(const uint32 & p_PlayerID, const uint8 & p_Slot, const uint16 & p_ItemID, const uint8 & p_Stack)
{
    std::string l_Query = "replace into `characters_items` (`characterID`, `slotNb`, `itemID`, `stackNb`) values('" + std::to_string(p_PlayerID) + "', '" + std::to_string(p_Slot) + "', '" + std::to_string(p_ItemID) + "', '" + std::to_string(p_Stack) + "');";
    Exec(m_MysqlCharacters, l_Query);
}

void SqlManager::AddNewEquipmentForPlayer(const uint32 & p_PlayerID, const eTypeEquipment & p_Type, const uint16 & p_ItemID)
{
    std::string l_Query = "replace into `characters_equipments` (`characterID`, `typeID`, `itemID`) values('" + std::to_string(p_PlayerID) + "', '" + std::to_string(p_Type) + "', '" + std::to_string(p_ItemID) + "');";
    Exec(m_MysqlCharacters, l_Query);
}

void SqlManager::RemoveEquipmentForPlayer(const uint32 & p_PlayerID, const eTypeEquipment & p_Type)
{
    std::string l_Query = "DELETE FROM `characters_equipments` WHERE `characterID` = '" + std::to_string(p_PlayerID) + "' AND `typeID` =  '" + std::to_string(p_Type) + "';";
    Exec(m_MysqlCharacters, l_Query);
}


void SqlManager::UpdateItemStackForPlayer(const uint32 & p_PlayerID, const uint8 & p_Slot, const uint16 & p_ItemID, const uint8 & p_Stack)
{
    std::string l_Query = "UPDATE `characters_items` SET `stackNb` = '" + std::to_string(p_Stack) + "' WHERE `characterID` = '" + std::to_string(p_PlayerID) + "' AND `slotNb` =  '" + std::to_string(p_Slot) + "' AND `itemID` =  '" + std::to_string(p_ItemID) + "';";
    Exec(m_MysqlCharacters, l_Query);
}

void SqlManager::RemoveItemForPlayer(const uint32 & p_PlayerID, const uint8 & p_Slot)
{
    std::string l_Query = "DELETE FROM `characters_items` WHERE `characterID` = '" + std::to_string(p_PlayerID) + "' AND `SlotNb` =  '" + std::to_string(p_Slot) + "';";
    Exec(m_MysqlCharacters, l_Query);
}

void SqlManager::UpdateCurrencyForPlayer(const uint32 & p_PlayerID, const eTypeCurrency & p_Type, const uint16 & p_Value)
{
    std::string l_Query = "SELECT `value` FROM characters_currencies WHERE characterID = '" + std::to_string(p_PlayerID) + "' AND typeID = '" + std::to_string(p_Type) + "'";
    sqlite3_stmt* l_Stmt;
    if (!Prepare(m_MysqlCharacters, l_Query, &l_Stmt))
        throw std::runtime_error("invalid database query");

    bool l_Exist = false;
    while (sqlite3_step(l_Stmt) == SQLITE_ROW)
    {
        l_Exist = true;
    }
    sqlite3_finalize(l_Stmt);

    if (!l_Exist)
    {
        std::string l_Query = "INSERT INTO `characters_currencies` (`characterID`, `typeID`, `value`) values('" + std::to_string(p_PlayerID) + "', '" + std::to_string(p_Type) + "', '" + std::to_string(p_Value) + "');";
        Exec(m_MysqlCharacters, l_Query);
    }
    else
    {
        std::string l_Query = "UPDATE `characters_currencies` SET `value` = '" + std::to_string(p_Value) + "' WHERE `characterID` = '" + std::to_string(p_PlayerID) + "' AND `typeID` =  '" + std::to_string((uint8)p_Type) + "';";
        Exec(m_MysqlCharacters, l_Query);
    }
}

void SqlManager::UpdateItemSlotForPlayer(const uint32 & p_PlayerID, const uint8 & p_SlotOld, const uint8 & p_SlotNew)
{
    std::string l_Query = "UPDATE `characters_items` SET `slotNb` = -1 WHERE `characterID` = '" + std::to_string(p_PlayerID) + "' AND `slotNb` =  '" + std::to_string(p_SlotNew) + "';";
    Exec(m_MysqlCharacters, l_Query);
    l_Query = "UPDATE `characters_items` SET `slotNb` = '" + std::to_string(p_SlotNew) + "' WHERE `characterID` = '" + std::to_string(p_PlayerID) + "' AND `slotNb` =  '" + std::to_string(p_SlotOld) + "';";
    Exec(m_MysqlCharacters, l_Query);
    l_Query = "UPDATE `characters_items` SET `slotNb` = '" + std::to_string(p_SlotOld) + "' WHERE `characterID` = '" + std::to_string(p_PlayerID) + "' AND `slotNb` = -1;";
    Exec(m_MysqlCharacters, l_Query);
}

WorldPosition SqlManager::GetRespawnPositionForPlayer(uint32 p_PlayerID)
{
    std::string l_Query = "SELECT posX, posY, mapID, orientation FROM characters_respawn WHERE characterID = '" + std::to_string(p_PlayerID) + "'";
    sqlite3_stmt* l_Stmt;
    if (!Prepare(m_MysqlCharacters, l_Query, &l_Stmt))
        throw std::runtime_error("invalid database query");
 
    uint32 l_PosX = 0;
    uint32 l_PosY = 0;
    uint16 l_MapID = 0;
    uint8 l_Orientation = 0;

    bool l_Exist = false;
    while (sqlite3_step(l_Stmt) == SQLITE_ROW)
    {
        l_Exist = true;
        l_PosX = sqlite3_column_int(l_Stmt, 0);
        l_PosY = sqlite3_column_int(l_Stmt, 1);
        l_MapID = sqlite3_column_int(l_Stmt, 2);
        l_Orientation = sqlite3_column_int(l_Stmt, 3);
    }
    sqlite3_finalize(l_Stmt);

    if (!l_Exist)
    {
        AddNewRespawnPositionForPlayer(p_PlayerID);
        return GetRespawnPositionForPlayer(p_PlayerID);
    }

    WorldPosition l_Position(l_PosX, l_PosY, l_MapID, 0, (Orientation)l_Orientation);
    return l_Position;
}

void SqlManager::AddNewPointsSetForPlayer(uint32 p_PlayerID)
{
	std::string l_Query = "INSERT INTO `characters_point` (`characterID`, `free_point`, `force`, `stamina`, `dexterity`) values('" + std::to_string(p_PlayerID) + "', '0', '0', '0', '0');";
	Exec(m_MysqlCharacters, l_Query);
}

PointsSet SqlManager::GetPointsSetForPlayer(uint32 p_PlayerID)
{
	std::string l_Query = "SELECT `free_point`, `force`, `stamina`, `dexterity` FROM characters_point WHERE characterID = '" + std::to_string(p_PlayerID) + "'";
    sqlite3_stmt* l_Stmt;
    if (!Prepare(m_MysqlCharacters, l_Query, &l_Stmt))
        throw std::runtime_error("invalid database query");

	uint16 l_FreePoints = 0;
	uint16 l_Force = 0;
	uint16 l_Stamina = 0;
	uint16 l_Dexterity = 0;

	bool l_Exist = false;
	while (sqlite3_step(l_Stmt) == SQLITE_ROW)
	{
		l_Exist = true;
		l_FreePoints = sqlite3_column_int(l_Stmt, 0);
		l_Force = sqlite3_column_int(l_Stmt, 1);
		l_Stamina = sqlite3_column_int(l_Stmt, 2);
		l_Dexterity = sqlite3_column_int(l_Stmt, 3);
	}
	sqlite3_finalize(l_Stmt);

	if (!l_Exist)
	{
		AddNewPointsSetForPlayer(p_PlayerID);
		return GetPointsSetForPlayer(p_PlayerID);
	}

	PointsSet l_PointsSet;
    l_PointsSet.SetStat(eStats::Free, l_FreePoints);
    l_PointsSet.SetStat(eStats::Force, l_Force);
    l_PointsSet.SetStat(eStats::Stamina, l_Stamina);
    l_PointsSet.SetStat(eStats::Dexterity, l_Dexterity);

	return l_PointsSet;
}

void SqlManager::SavePlayer(Player* p_Player)
{
    /// Save cooldowns
    std::map< uint16, uint64 >* l_SpellList = p_Player->GetSpellList();
    for (std::map< uint16, uint64 >::iterator l_It = l_SpellList->begin(); l_It != l_SpellList->end(); ++l_It)
    {
        /// CHECK 
        std::string l_Query = "UPDATE characters_spells SET `cooldown` = '" + std::to_string((*l_It).second) + "' WHERE spellID = '" + std::to_string((*l_It).first) + "' AND characterID = '" + std::to_string(p_Player->GetID()) + "';";
        Exec(m_MysqlCharacters, l_Query);
    }

    /// Save QuestsProgess
    std::map< uint16, Quest* >* l_QuestList = p_Player->GetQuestList();
    for (std::map< uint16, Quest* >::iterator l_It = l_QuestList->begin(); l_It != l_QuestList->end(); ++l_It)
    {
        /// Save Objectf Progress
        std::map< uint8, ObjectifProgess* >* l_ObjectProgressList = (*l_It).second->GetObjectifsProgress();
        for (std::map< uint8, ObjectifProgess* >::iterator l_Itr = l_ObjectProgressList->begin(); l_Itr != l_ObjectProgressList->end(); ++l_Itr)
        {
            /// Remove it from quest in Progress
            std::string l_Query = "DELETE FROM quest_objectif_progress WHERE characterID = '" + std::to_string(p_Player->GetID()) + "' AND questID = '" + std::to_string((*l_It).first) + "';";
            Exec(m_MysqlCharacters, l_Query);
        /// CHECK 
            l_Query = "REPLACE INTO quest_objectif_progress (characterID, questID, objectifID, data0) VALUES ('" + std::to_string(p_Player->GetID()) + "', '" + std::to_string((*l_It).first) + "', '" + std::to_string((*l_Itr).first) + "', '" + std::to_string((*l_Itr).second->m_Data0) + "');";
            Exec(m_MysqlCharacters, l_Query);
        }
    }

	std::string l_Query = "UPDATE characters SET `posX` = '" + std::to_string(p_Player->GetPosX()) + "', `posY` = '" + std::to_string(p_Player->GetPosY()) + "', `mapID` = '" + std::to_string(p_Player->GetMapID()) + 
        "', `class` = '" + std::to_string(p_Player->GetClass()) + "', `orientation` = '" + std::to_string(p_Player->GetOrientation()) + "', `health` = '" + std::to_string(p_Player->GetResourceNb(eResourceType::Health)) +
        "', `mana` = '" + std::to_string(p_Player->GetResourceNb(eResourceType::Mana)) + "', `alignment` = '" + std::to_string(p_Player->GetResourceNb(eResourceType::Alignment)) +
        "', `xp` = '" + std::to_string(p_Player->GetXp()) + "', `level` = '" + std::to_string(p_Player->GetLevel()) +
        "', skinID = '" + std::to_string(p_Player->GetSkinID()) +
        "', activeTitleID = '" + std::to_string(p_Player->GetActiveTitleID()) +
        "' WHERE characterID = '" + std::to_string(p_Player->GetID()) + "';";

    Exec(m_MysqlCharacters, l_Query);
	UpdatePointsSet(p_Player);
}

void SqlManager::SaveQuestForPlayer(Player const* p_Player,  Quest const* p_Quest)
{
    /// Save Quest
    std::string l_Query = "INSERT INTO quests_done (characterID, questID) VALUES ('" + std::to_string(p_Player->GetID()) + "', '" + std::to_string(p_Quest->GetID()) + "');";
    Exec(m_MysqlCharacters, l_Query);

    /// Remove it from quest in Progress
    l_Query = "DELETE FROM quest_objectif_progress WHERE characterID = '" + std::to_string(p_Player->GetID()) + "' AND questID = '" + std::to_string(p_Quest->GetID()) + "';";
    Exec(m_MysqlCharacters, l_Query);
}

int32 SqlManager::GetDaysSinceLastQuestDone(Player const* p_Player, uint16 p_QuestID)
{
    std::string l_Query = "SELECT DATEDIFF(NOW(), MAX(`dateValidate`)) FROM quests_done WHERE characterID = '" + std::to_string(p_Player->GetID()) + "' AND questID = '" + std::to_string(p_QuestID)  + "'";
    sqlite3_stmt* l_Stmt;
    if (!Prepare(m_MysqlCharacters, l_Query, &l_Stmt))
        throw std::runtime_error("invalid database query");

    int16 l_Days = -1;

    bool l_Exist = false;
    while (sqlite3_step(l_Stmt) == SQLITE_ROW)
    {
        l_Days = sqlite3_column_int(l_Stmt, 0);
    }
    sqlite3_finalize(l_Stmt);

    return l_Days;
}

void SqlManager::InitializeListSkinsForPlayer(Player* p_Player)
{
    std::string l_Query = "SELECT skinID FROM characters_skins WHERE characterID = '" + std::to_string(p_Player->GetID()) + "'";
    sqlite3_stmt* l_Stmt;
    if (!Prepare(m_MysqlCharacters, l_Query, &l_Stmt))
        throw std::runtime_error("invalid database query");

    while (sqlite3_step(l_Stmt) == SQLITE_ROW)
    {
        uint16 l_ID = sqlite3_column_int(l_Stmt, 0);
        p_Player->LearnSkin(l_ID);
    }
    sqlite3_finalize(l_Stmt);
}
void SqlManager::InitializeListEquipmentsForPlayer(Player* p_Player)
{
    std::string l_Query = "SELECT typeID, itemID FROM characters_equipments WHERE characterID = '" + std::to_string(p_Player->GetID()) + "'";
    sqlite3_stmt* l_Stmt;
    if (!Prepare(m_MysqlCharacters, l_Query, &l_Stmt))
        throw std::runtime_error("invalid database query");

    eTypeEquipment l_Type = eTypeEquipment::EQUIP_HEAD;
    uint16 l_ItemID = 0;

    while (sqlite3_step(l_Stmt) == SQLITE_ROW)
    {
        l_Type = (eTypeEquipment)sqlite3_column_int(l_Stmt, 0);
        l_ItemID = sqlite3_column_int(l_Stmt, 1);

        ItemTemplate* l_ItemTemplate = g_ItemManager->GetItemTemplate(l_ItemID);
        if (l_ItemTemplate == nullptr)
            continue;
        Item* l_Item = new Item(p_Player, l_ItemTemplate);
        p_Player->AddEquipment(l_Type, l_Item);
    }
    sqlite3_finalize(l_Stmt);
}

void SqlManager::InitializeListItemForPlayer(Player* p_Player)
{
    std::string l_Query = "SELECT slotNb, itemID, stackNb FROM characters_items WHERE characterID = '" + std::to_string(p_Player->GetID()) + "'";
    sqlite3_stmt* l_Stmt;
    if (!Prepare(m_MysqlCharacters, l_Query, &l_Stmt))
        throw std::runtime_error("invalid database query");

    uint8 l_SlotNb = 0;
    uint16 l_ItemID = 0;
    uint8 l_StackNb = 0;

    while (sqlite3_step(l_Stmt) == SQLITE_ROW)
    {
        l_SlotNb = sqlite3_column_int(l_Stmt, 0);
        l_ItemID = sqlite3_column_int(l_Stmt, 1);
        l_StackNb = sqlite3_column_int(l_Stmt, 2);

        ItemTemplate* l_ItemTemplate = g_ItemManager->GetItemTemplate(l_ItemID);
        if (l_ItemTemplate == nullptr)
            continue;
        Item* l_Item = new Item(p_Player, l_ItemTemplate);
        l_Item->SetStackNb(l_StackNb);
        p_Player->AddItem(l_SlotNb, l_Item);
    }
    sqlite3_finalize(l_Stmt);
}

void SqlManager::InitializeListTitlesForPlayer(Player* p_Player)
{
    std::string l_Query = "SELECT titleID FROM characters_titles WHERE characterID = '" + std::to_string(p_Player->GetID()) + "'";
    sqlite3_stmt* l_Stmt;
    if (!Prepare(m_MysqlCharacters, l_Query, &l_Stmt))
        throw std::runtime_error("invalid database query");

    while (sqlite3_step(l_Stmt) == SQLITE_ROW)
    {
        uint16 l_ID = sqlite3_column_int(l_Stmt, 0);
        p_Player->LearnTitle(l_ID);
    }
    sqlite3_finalize(l_Stmt);
}

void SqlManager::InitializeListCurrenciesForPlayer(Player* p_Player)
{
    std::string l_Query = "SELECT typeID, value FROM characters_currencies WHERE characterID = '" + std::to_string(p_Player->GetID()) + "'";
    sqlite3_stmt* l_Stmt;
    if (!Prepare(m_MysqlCharacters, l_Query, &l_Stmt))
        throw std::runtime_error("invalid database query");

    uint8 l_TypeID = 0;
    uint16 l_Value = 0;
    while (sqlite3_step(l_Stmt) == SQLITE_ROW)
    {
        l_TypeID = sqlite3_column_int(l_Stmt, 0);
        l_Value = sqlite3_column_int(l_Stmt, 1);
        p_Player->UpdateCurrency((eTypeCurrency)l_TypeID, l_Value);
    }
    sqlite3_finalize(l_Stmt);
}

void SqlManager::UpdatePointsSet(Player const* p_Player)
{
	std::string l_Query = "UPDATE `characters_point` SET `free_point` = '" + std::to_string(p_Player->GetPointsSet().GetStat(eStats::Free)) + "', `force` = '" + std::to_string(p_Player->GetPointsSet().GetStat(eStats::Force)) + "', `stamina` = '" + std::to_string(p_Player->GetPointsSet().GetStat(eStats::Stamina)) + "', `dexterity` = '" + std::to_string(p_Player->GetPointsSet().GetStat(eStats::Dexterity)) + "' WHERE characterID = '" + std::to_string(p_Player->GetID()) + "';";
	Exec(m_MysqlCharacters, l_Query);
}

CreatureTemplate SqlManager::GetCreatureTemplate(uint16 p_Entry)
{
    std::string l_Query = "SELECT `skinID`, `name`, `level`, `force`, `stamina`, `dexterity`, `speed`, `xp`, `state`, `maxRay`, `maxVision`,`movingTimeMin`, `movingTimeMax`, `stopTimeMin`, `stopTimeMax`, `respawnTime`, `rank`, `aiType`, `faction`, `rewardID` FROM creature_template WHERE `entry` = '" + std::to_string(p_Entry) + "'";
    sqlite3_stmt* l_Stmt;
    if (!Prepare(m_MysqlCharacters, l_Query, &l_Stmt))
        throw std::runtime_error("invalid database query");

    int16 l_SkinID = 0;
    std::string l_Name = "";
    uint8 l_Lvl = 0;
    uint8 l_Force = 0;
    uint8 l_Stamina = 0;
    uint8 l_Dexterity = 0;
    uint8 l_Speed = 0;
    uint8 l_Xp = 0;
    uint8 l_State = 0;
    uint16 l_MaxRay = 0;
    uint16 l_MaxVision = 0;
    float l_MovingTimeMin = 0.0f;
    float l_MovingTimeMax = 0.0f;
    float l_StopTimeMin = 0.0f;
    float l_StopTimeMax = 0.0f;
    int32 l_RespawnTime = 0;
    uint8 l_Rank = 0;
    uint8 l_AiType = 0;
    uint8 l_Faction = 0;
    int32 l_RewardID = -1;

    while (sqlite3_step(l_Stmt) == SQLITE_ROW)
    {
        l_SkinID = sqlite3_column_int(l_Stmt, 0);
        l_Name = (const char*) sqlite3_column_text(l_Stmt, 1);
        l_Lvl = sqlite3_column_int(l_Stmt, 2);
        l_Force = sqlite3_column_int(l_Stmt, 3);
        l_Stamina = sqlite3_column_int(l_Stmt, 4);
        l_Dexterity = sqlite3_column_int(l_Stmt, 5);
        l_Speed = sqlite3_column_int(l_Stmt, 6);
        l_Xp = sqlite3_column_int(l_Stmt, 7);
        l_State = sqlite3_column_int(l_Stmt, 8);
        l_MaxRay = sqlite3_column_int(l_Stmt, 9);
        l_MaxVision = sqlite3_column_int(l_Stmt, 10);
        l_RespawnTime = sqlite3_column_int(l_Stmt, 11);
        l_MovingTimeMin = (float)sqlite3_column_double(l_Stmt, 12);
        l_MovingTimeMax = (float)sqlite3_column_double(l_Stmt, 13);
        l_StopTimeMin = (float)sqlite3_column_double(l_Stmt, 14);
        l_StopTimeMax = (float)sqlite3_column_double(l_Stmt, 15);
        l_RespawnTime = sqlite3_column_int(l_Stmt, 16);
        l_Rank = sqlite3_column_int(l_Stmt, 17);
        l_AiType = sqlite3_column_int(l_Stmt, 18);
        l_Faction = sqlite3_column_int(l_Stmt, 19);
        l_RewardID = sqlite3_column_int(l_Stmt, 20);
        return CreatureTemplate(p_Entry, l_SkinID, l_Name, l_Lvl, l_Force, l_Stamina, l_Dexterity, l_Speed, l_Xp, l_State, l_MaxRay, l_MaxVision, l_MovingTimeMin, l_MovingTimeMax, l_StopTimeMin, l_StopTimeMax, l_RespawnTime, l_Rank, l_AiType, (eFactionType)l_Faction, l_RewardID);
    }
    sqlite3_finalize(l_Stmt);

    return CreatureTemplate();
}

uint16 SqlManager::AddNewCreature(uint16 p_Map, uint16 p_Entry, uint32 p_PosX, uint32 p_PosY)
{
    std::string l_Query = "insert  into `creature`(`mapID`,`entry`,`posX`,`posY`) values('" + std::to_string(p_Map) + "', '" + std::to_string(p_Entry) + "', '" + std::to_string(p_PosX) + "', '" + std::to_string(p_PosY) + "')";
    Exec(m_MysqlWorld, l_Query);
    l_Query = "SELECT MAX(`id`) FROM `creature`";
    sqlite3_stmt* l_Stmt;
    if (!Prepare(m_MysqlCharacters, l_Query, &l_Stmt))
        throw std::runtime_error("invalid database query");

    uint16 l_Id = 0;
    while (sqlite3_step(l_Stmt) == SQLITE_ROW)
    {
        l_Id = sqlite3_column_int(l_Stmt, 0);
    }
    sqlite3_finalize(l_Stmt);
    return l_Id;
}

bool SqlManager::InitializeAnimationUnitTemplate(UnitManager* p_CreatureManager)
{
    std::string l_Query = "SELECT `entry`, `typeID`, `skinID`, `name`, `stopTimeMin`, `stopTimeMax` FROM animation_unit_template";
    sqlite3_stmt* l_Stmt;
    if (!Prepare(m_MysqlCharacters, l_Query, &l_Stmt))
        throw std::runtime_error("invalid database query");

    uint16 l_Entry = 0;
    uint16 l_TypeID = 0;
    int16 l_SkinID = 0;
    std::string l_Name = "";
    float l_StopTimeMin = 0.0f;
    float l_StopTimeMax = 0.0f;

    while (sqlite3_step(l_Stmt) == SQLITE_ROW)
    {
        l_Entry = sqlite3_column_int(l_Stmt, 0);
        l_TypeID = sqlite3_column_int(l_Stmt, 1);
        l_SkinID = sqlite3_column_int(l_Stmt, 2);
        l_Name = (const char*) sqlite3_column_text(l_Stmt, 3);
        l_StopTimeMin = sqlite3_column_double(l_Stmt, 4);
        l_StopTimeMax = sqlite3_column_double(l_Stmt, 5);
        p_CreatureManager->AddAnimationUnitTemplate(AnimationUnitTemplate(l_Entry, l_SkinID, l_TypeID, l_Name, l_StopTimeMin, l_StopTimeMax));
    }
    sqlite3_finalize(l_Stmt);

    return true;
}

bool SqlManager::InitializeCreatureTemplate(UnitManager* p_CreatureManager)
{
    std::string l_Query = "SELECT `entry`, `skinID`, `name`, `level`, `force`, `stamina`, `dexterity`, `speed`, `xp`, `state`, `maxRay`, `maxVision`, `movingTimeMin`, `movingTimeMax`, `stopTimeMin`, `stopTimeMax`, `respawnTime`, `rank`, `aiType`, `faction`, `rewardID` FROM creature_template";
    sqlite3_stmt* l_Stmt;
    if (!Prepare(m_MysqlWorld, l_Query, &l_Stmt))
        throw std::runtime_error("invalid database query");

    uint32 l_Entry = 0;
    uint8 l_SkinID = 0;
    std::string l_Name = "";
    uint8 l_Lvl = 0;
    uint8 l_Force = 0;
    uint8 l_Stamina = 0;
    uint8 l_Dexterity = 0;
    uint8 l_Speed = 0;
    uint8 l_Xp = 0;
    uint8 l_State = 0;
    uint16 l_MaxRay = 0;
    uint16 l_MaxVision = 0;
    float l_MovingTimeMin = 0.0f;
    float l_MovingTimeMax = 0.0f;
    float l_StopTimeMin = 0.0f;
    float l_StopTimeMax = 0.0f;
    int32 l_RespawnTime = 0;
    uint8 l_Rank = 0;
    uint8 l_AiType = 0;
    uint8 l_Faction = 0;
    int32 l_RewardID = -1;

    while (sqlite3_step(l_Stmt) == SQLITE_ROW)
    {
        l_Entry = sqlite3_column_int(l_Stmt, 0);
        l_SkinID = sqlite3_column_int(l_Stmt, 1);
        l_Name = (const char*) sqlite3_column_text(l_Stmt, 2);
        l_Lvl = sqlite3_column_int(l_Stmt, 3);
        l_Force = sqlite3_column_int(l_Stmt, 4);
        l_Stamina = sqlite3_column_int(l_Stmt, 5);
        l_Dexterity = sqlite3_column_int(l_Stmt, 6);
        l_Speed = sqlite3_column_int(l_Stmt, 7);
        l_Xp = sqlite3_column_int(l_Stmt, 8);
        l_State = sqlite3_column_int(l_Stmt, 9);
        l_MaxRay = sqlite3_column_int(l_Stmt, 10);
        l_MaxVision = sqlite3_column_int(l_Stmt, 11);
        l_MovingTimeMin = (float)sqlite3_column_double(l_Stmt, 12);
        l_MovingTimeMax = (float)sqlite3_column_double(l_Stmt, 13);
        l_StopTimeMin = (float)sqlite3_column_double(l_Stmt, 14);
        l_StopTimeMax = (float)sqlite3_column_double(l_Stmt, 15);
        l_RespawnTime = sqlite3_column_int(l_Stmt, 16);
        l_Rank = sqlite3_column_int(l_Stmt, 17);
        l_AiType = sqlite3_column_int(l_Stmt, 18);
        l_Faction = sqlite3_column_int(l_Stmt, 19);
        l_RewardID = sqlite3_column_int(l_Stmt, 20);
        p_CreatureManager->AddCreatureTemplate(CreatureTemplate(l_Entry, l_SkinID, l_Name, l_Lvl, l_Force, l_Stamina, l_Dexterity, l_Speed, l_Xp, l_State, l_MaxRay, l_MaxVision, l_MovingTimeMin, l_MovingTimeMax, l_StopTimeMin, l_StopTimeMax, l_RespawnTime, l_Rank, l_AiType, (eFactionType)l_Faction, l_RewardID));
    }
    sqlite3_finalize(l_Stmt);

    return true;
}

bool SqlManager::InitializeSpellsForPlayer(Player* p_Player)
{
    std::string l_Query = "SELECT `spellID`, `cooldown` FROM characters_spells WHERE `characterID` = '" + std::to_string(p_Player->GetID()) + "'";
    sqlite3_stmt* l_Stmt;
    if (!Prepare(m_MysqlCharacters, l_Query, &l_Stmt))
        throw std::runtime_error("invalid database query");

    uint16 l_SpellID = 0;
    uint64 l_Cooldown = 0;

    while (sqlite3_step(l_Stmt) == SQLITE_ROW)
    {
        l_SpellID = sqlite3_column_int(l_Stmt, 0);
        l_Cooldown = sqlite3_column_int(l_Stmt, 1);
        p_Player->AddSpellID(l_SpellID, l_Cooldown);
    }
    sqlite3_finalize(l_Stmt);

    return true;
}

bool SqlManager::InitializeQuestsProgessForPlayer(Player* p_Player)
{
    std::string l_Query = "SELECT `questID`, `objectifID`, `data0`, `data1`, `data2`, `data3` FROM quest_objectif_progress WHERE `characterID` = '" + std::to_string(p_Player->GetID()) + "'";
    sqlite3_stmt* l_Stmt;
    if (!Prepare(m_MysqlCharacters, l_Query, &l_Stmt))
        throw std::runtime_error("invalid database query");

    uint16 l_QuestID = 0;
    uint8 l_ObjectifID = 0;
    int16 l_Data0 = -1;
    int16 l_Data1 = -1;
    int16 l_Data2 = -1;
    int16 l_Data3 = -1;

    while (sqlite3_step(l_Stmt) == SQLITE_ROW)
    {
        l_QuestID = sqlite3_column_int(l_Stmt, 0);
        l_ObjectifID = sqlite3_column_int(l_Stmt, 1);
        l_Data0 = sqlite3_column_int(l_Stmt, 2);
        l_Data1 = sqlite3_column_int(l_Stmt, 3);
        l_Data2 = sqlite3_column_int(l_Stmt, 4);
        l_Data3 = sqlite3_column_int(l_Stmt, 5);

        Quest* l_Quest = p_Player->GetQuest(l_QuestID);
        if (l_Quest == nullptr)
        {
            l_Quest = new Quest(g_QuestManager->GetQuestTemplate(l_QuestID));
            p_Player->AddQuest(l_Quest, false);
        }
        l_Quest->SetDataOfObjectif(l_ObjectifID, 0, l_Data0); /* Actually we only use data0 */
    }
    sqlite3_finalize(l_Stmt);

    return true;
}


bool SqlManager::InitializeSpellsBinds(Player* p_Player)
{
    std::string l_Query = "SELECT `spellID`, `bindID` FROM characters_spell_binds WHERE `characterID` = '" + std::to_string(p_Player->GetID()) + "'";
    sqlite3_stmt* l_Stmt;
    if (!Prepare(m_MysqlCharacters, l_Query, &l_Stmt))
        throw std::runtime_error("invalid database query");

    uint16 l_SpellID = 0;
    uint8 l_BindID = 0;

    while (sqlite3_step(l_Stmt) == SQLITE_ROW)
    {
        l_SpellID = sqlite3_column_int(l_Stmt, 0);
        l_BindID = sqlite3_column_int(l_Stmt, 1);
        p_Player->AddSpellBindToKey(l_SpellID, l_BindID);
    }
    sqlite3_finalize(l_Stmt);

    return true;
}

void SqlManager::AddSpellForPlayer(Player* p_Player, uint16 l_SpellID)
{
    std::string l_Query = "REPLACE INTO `characters_spells` (characterID, spellID) VALUES ('" + std::to_string(p_Player->GetID()) + "', '" + std::to_string(l_SpellID) + "');";
    Exec(m_MysqlCharacters, l_Query);
    p_Player->AddSpellID(l_SpellID, 0);
}

void SqlManager::RemoveSpellForPlayer(Player* p_Player, uint16 l_SpellID)
{
    std::string l_Query = "DELETE FROM `characters_spells` WHERE characterID = '" + std::to_string(p_Player->GetID()) + "' and spellID = '" + std::to_string(l_SpellID) + "';";
    printf("----> %s\n", l_Query.c_str());
    Exec(m_MysqlCharacters, l_Query);
}

void SqlManager::AddSpellBind(Player* p_Player, uint16 l_SpellID, uint8 l_BindID)
{
    std::string l_Query = "DELETE FROM `characters_spell_binds` WHERE bindID = '" + std::to_string(l_BindID) + "' AND characterID = '" + std::to_string(p_Player->GetID()) + "';";
    Exec(m_MysqlCharacters, l_Query);
    l_Query = "INSERT INTO `characters_spell_binds` (characterID, spellID, bindID) VALUES ('" + std::to_string(p_Player->GetID()) + "', '" + std::to_string(l_SpellID) + "', '" + std::to_string(l_BindID) + "');";
    Exec(m_MysqlCharacters, l_Query);
    p_Player->AddSpellBindToKey(l_SpellID, l_BindID);
}

void SqlManager::RemoveSpellBind(Player* p_Player, uint16 l_SpellID)
{
    std::string l_Query = "DELETE FROM `characters_spell_binds` WHERE spellID = '" + std::to_string(l_SpellID) + "' AND characterID = '" + std::to_string(p_Player->GetID()) + "';";
    Exec(m_MysqlCharacters, l_Query);
}

bool SqlManager::InitializeKeyBindsForAccount(uint32 p_Account, Player* p_Player)
{
    std::string l_Query = "SELECT `typeID`, `key` FROM account_key_binds WHERE `accountID` = '" + std::to_string(p_Account) + "'";
    sqlite3_stmt* l_Stmt;
    if (!Prepare(m_MysqlCharacters, l_Query, &l_Stmt))
        throw std::runtime_error("invalid database query");

    uint8 l_TypeID = 0;
    uint8 l_Key = 0;

    while (sqlite3_step(l_Stmt) == SQLITE_ROW)
    {
        l_TypeID = sqlite3_column_int(l_Stmt, 0);
        l_Key = sqlite3_column_int(l_Stmt, 1);
        p_Player->AddKeyBoardBind((eKeyBoardAction)l_TypeID, l_Key);
    }
    sqlite3_finalize(l_Stmt);

    return true;
}

bool SqlManager::InitializeGossip(UnitManager* p_CreatureManager, RequiredManager* p_RequiredManager)
{
    std::string l_Query = "SELECT `id`, `requiredID`, `typeUnit`, `unitEntry`, `type`, `data0`, `data1`, `msg` FROM gossip";
    sqlite3_stmt* l_Stmt;
    if (!Prepare(m_MysqlWorld, l_Query, &l_Stmt))
        throw std::runtime_error("invalid database query");

    uint16 l_ID = 0;
    int32 l_RequiredID = -1;
    uint8 l_TypeUnit = 0;
    uint16 l_UnitEntry = 0;
    uint8 l_GossipType = 0;
    uint32 l_Data0 = 0;
    uint32 l_Data1 = 0;
    std::string l_Msg = "";
    bool l_Automatic = true;

    while (sqlite3_step(l_Stmt) == SQLITE_ROW)
    {
        l_ID = sqlite3_column_int(l_Stmt, 0);
        l_RequiredID = sqlite3_column_int(l_Stmt, 1);
        l_TypeUnit = sqlite3_column_int(l_Stmt, 2);
        l_UnitEntry = sqlite3_column_int(l_Stmt, 3);
        l_GossipType = sqlite3_column_int(l_Stmt, 4);
        l_Data0 = sqlite3_column_int(l_Stmt, 5);
        if (sqlite3_column_type(l_Stmt, 6) != SQLITE_NULL)
            l_Data1 = sqlite3_column_int(l_Stmt, 6);
        l_Msg = (const char*) sqlite3_column_text(l_Stmt, 7);
        Required* l_Required = nullptr;
        if (l_RequiredID >= 0) /// -1 if no required
            l_Required = p_RequiredManager->GetRequiered(l_RequiredID);
        if (l_RequiredID < -1)
            l_Automatic = false;
        else
            l_Automatic = true;

        p_CreatureManager->AddGossip(Gossip(l_ID, l_Required, (TypeUnit)l_TypeUnit, l_UnitEntry, l_Automatic, (eGossipType)l_GossipType, l_Data0, l_Data1, l_Msg));
    }
    sqlite3_finalize(l_Stmt);

    return true;
}


bool SqlManager::InitializeAnimationUnit(UnitManager* p_CreatureManager)
{
    std::string l_Query = "SELECT `id`, `entry`, `mapID`, `posX`, `posY` FROM animation_unit";
    sqlite3_stmt* l_Stmt;
    if (!Prepare(m_MysqlCharacters, l_Query, &l_Stmt))
        throw std::runtime_error("invalid database query");

    uint16 l_Id = 0;
    uint16 l_Entry = 0;
    uint16 l_MapID = 0;
    uint32 l_PosX = 0;
    uint32 l_PosY = 0;

    while (sqlite3_step(l_Stmt) == SQLITE_ROW)
    {
        l_Id = sqlite3_column_int(l_Stmt, 0);
        l_Entry = sqlite3_column_int(l_Stmt, 1);
        l_MapID = sqlite3_column_int(l_Stmt, 2);
        l_PosX = sqlite3_column_int(l_Stmt, 3);
        l_PosY = sqlite3_column_int(l_Stmt, 4);
        MapTemplate* l_MapTemplate = g_MapManager->GetMapTemplate(l_MapID);


        if (l_MapTemplate == nullptr)
        {
            printf("Erreur map %d don't exist\n", l_MapID);
            continue;
        }

        l_MapTemplate->AddAnimationUnitMapTemplate(AnimationUnitMapTemplate(l_Id, l_MapID, p_CreatureManager->GetAnimationUnitTemplate(l_Entry), l_PosX, l_PosY, p_CreatureManager->GetGossipListFor(TypeUnit::ANIMATIONUNIT, l_Entry)));
        //l_MapTemplate->AddUnit(l_Creature);
    }
    sqlite3_finalize(l_Stmt);

    return true;
}

bool SqlManager::InitializeCreature(UnitManager* p_CreatureManager)
{
    std::string l_Query = "SELECT `id`, `entry`, `mapID`, `posX`, `posY` FROM creature";
    sqlite3_stmt* l_Stmt;
    if (!Prepare(m_MysqlWorld, l_Query, &l_Stmt))
        throw std::runtime_error("invalid database query");

    uint16 l_Id = 0;
    uint16 l_Entry = 0;
    uint16 l_MapID = 0;
    uint32 l_PosX = 0;
    uint32 l_PosY = 0;

    while (sqlite3_step(l_Stmt) == SQLITE_ROW)
    {
        l_Id = sqlite3_column_int(l_Stmt, 0);
        l_Entry = sqlite3_column_int(l_Stmt, 1);
        l_MapID = sqlite3_column_int(l_Stmt, 2);
        l_PosX = sqlite3_column_int(l_Stmt, 3);
        l_PosY = sqlite3_column_int(l_Stmt, 4);

        /*Creature* l_Creature = new Creature(l_Id, l_Entry, p_CreatureManager->GetCreatureTemplate(l_Entry), l_MapID, l_PosX, l_PosY);
        if (p_CreatureManager->GetGossipListFor(TypeUnit::CREATURE, l_Entry) != nullptr)
            l_Creature->SetGossipList(p_CreatureManager->GetGossipListFor(TypeUnit::CREATURE, l_Entry));*/
        MapTemplate* l_MapTemplate = g_MapManager->GetMapTemplate(l_MapID);
        // CreatureMapTemplate(uint16 p_ID, uint16 p_MapID, CreatureTemplate* p_Template, uint32 p_PosX, uint32 p_PosY, std::vector<Gossip>* p_GossipList) :

        if (l_MapTemplate == nullptr)
        {
            printf("Erreur map %d don't exist\n", l_MapID);
            continue;
        }
        
        l_MapTemplate->AddCreatureMapTemplate(CreatureMapTemplate(l_Id, l_MapID, p_CreatureManager->GetCreatureTemplate(l_Entry), l_PosX, l_PosY, p_CreatureManager->GetGossipListFor(TypeUnit::CREATURE, l_Entry)));
        //l_MapTemplate->AddUnit(l_Creature);
    }
    sqlite3_finalize(l_Stmt);

    return true;
}

bool  SqlManager::InitializeSpells()
{
    std::string l_Query = "SELECT `id`, `level`, `visualIDUnder`, `visualID`, `visualIDTargetUnder`, `visualIDTarget`,`castTime`, `cooldown`, `duration`, `speed`,`resourceType`, `resourceNb`, `effect1`, `effect2`, `effect3`, `effect4`, `name` FROM spell_template";
    sqlite3_stmt* l_Stmt;
    if (!Prepare(m_MysqlWorld, l_Query, &l_Stmt))
        throw std::runtime_error("invalid database query");

    uint16 l_Id = 0;
    uint8 l_Level = 0;
    int32 l_VisualIDUnder = -1;
    int32 l_VisualIDTargetUnder = -1;
    int32 l_VisualID = -1;
    int32 l_VisualIDTarget = -1;
    uint16 l_CastTime = 0;
    uint32 l_Cooldown = 0;
    int32 l_Duration = 0;
    float l_Speed = 0.0f;
    int16 l_ResourceType = 0;
    int32 l_ResourceNb = 0;
    std::vector<int32> l_EffectList;
    std::string l_Name = "";

    while (sqlite3_step(l_Stmt) == SQLITE_ROW)
    {
        l_Id = sqlite3_column_int(l_Stmt, 0);
        l_Level = sqlite3_column_int(l_Stmt, 1);
        l_VisualIDUnder = sqlite3_column_int(l_Stmt, 2);
        l_VisualID = sqlite3_column_int(l_Stmt, 3);
        l_VisualIDTargetUnder = sqlite3_column_int(l_Stmt, 4);
        l_VisualIDTarget = sqlite3_column_int(l_Stmt, 5);
        l_CastTime = sqlite3_column_int(l_Stmt, 6);
        l_Cooldown = sqlite3_column_int(l_Stmt, 7);
        l_Duration = sqlite3_column_int(l_Stmt, 8);
        l_Speed = (float)sqlite3_column_double(l_Stmt, 9);
        l_ResourceType = sqlite3_column_int(l_Stmt, 10);
        l_ResourceNb = sqlite3_column_int(l_Stmt, 11);
        for (uint8 i = 0; i < MAX_EFFECTS_FOR_SPELL; ++i)
            l_EffectList.push_back(sqlite3_column_int(l_Stmt, 12 + i));
        l_Name = (const char*) sqlite3_column_text(l_Stmt, 12 + MAX_EFFECTS_FOR_SPELL);

        SpellTemplate* l_Spell = new SpellTemplate(l_Id);
        l_Spell->SetLevel(l_Level);
        l_Spell->SetVisualsID(l_VisualIDUnder, l_VisualID, l_VisualIDTargetUnder, l_VisualIDTarget);
        l_Spell->SetCastTime(l_CastTime);
        l_Spell->SetCooldown(l_Cooldown);
        l_Spell->SetSpeed(l_Speed);
        l_Spell->SetDuration(l_Duration);
        l_Spell->SetName(l_Name);
        if (l_ResourceType > 0)
        {
            l_Spell->AddResourceNeed(ResourceNeed((eResourceType)l_ResourceType, l_ResourceNb));
        }

        for (int32 l_SpellEffectID : l_EffectList)
        {
            if (g_SpellManager->GetSpellEffect(l_SpellEffectID) != nullptr)
                l_Spell->AddSpellEffect(g_SpellManager->GetSpellEffect(l_SpellEffectID));
        }

        g_SpellManager->AddSpell(l_Spell);
        l_EffectList.clear();
    }
    sqlite3_finalize(l_Stmt);
    return true;
}

bool  SqlManager::InitializeSpellEffects()
{
    std::string l_Query = "SELECT `id`, `effectType`, `target`, `basepoint1`, `basepoint2`, `basepoint3`, `basepoint4`, `radiusMin`, `radiusMax` FROM spell_effect";
    sqlite3_stmt* l_Stmt;
    if (!Prepare(m_MysqlWorld, l_Query, &l_Stmt))
        throw std::runtime_error("invalid database query");

    uint16 l_Id = 0;
    uint8 l_EffectType = 0;
    uint8 l_Target = 0;
    int32 l_BasePoint1 = 0;
    int32 l_BasePoint2 = 0;
    int32 l_BasePoint3 = 0;
    int32 l_BasePoint4 = 0;
    float l_RadiusMin = 0.0f;
    float l_RadiusMax = 0.0f;

    while (sqlite3_step(l_Stmt) == SQLITE_ROW)
    {
        l_Id = sqlite3_column_int(l_Stmt, 0);
        l_EffectType = sqlite3_column_int(l_Stmt, 1);
        l_Target = sqlite3_column_int(l_Stmt, 2);
        l_BasePoint1 = sqlite3_column_int(l_Stmt, 3);
        l_BasePoint2 = sqlite3_column_int(l_Stmt, 4);
        l_BasePoint3 = sqlite3_column_int(l_Stmt, 5);
        l_BasePoint4 = sqlite3_column_int(l_Stmt, 6);
        l_RadiusMin = (float)sqlite3_column_double(l_Stmt, 7);
        l_RadiusMax = (float)sqlite3_column_double(l_Stmt, 8);

        SpellEffect l_SpellEffect(l_Id, (SpellEffectType)l_EffectType, (SpellTarget)l_Target, l_BasePoint1, l_BasePoint2, l_BasePoint3, l_BasePoint4, l_RadiusMin, l_RadiusMax);
        g_SpellManager->AddSpellEffect(l_SpellEffect);
    }
    sqlite3_finalize(l_Stmt);
    return true;
}

bool SqlManager::InitializeRequired(RequiredManager* p_RequiredManager)
{
    std::string l_Query = "SELECT `id`, `requiredID`, `typeID`, `data0`, `data1` FROM sub_required";
    sqlite3_stmt* l_Stmt;
    if (!Prepare(m_MysqlWorld, l_Query, &l_Stmt))
        throw std::runtime_error("invalid database query");

    uint16 l_Id = 0;
    uint16 l_RequiredID = 0;
    eRequiredType l_TypeID;
    uint32 l_Data0 = 0;
    uint32 l_Data1 = 0;

    while (sqlite3_step(l_Stmt) == SQLITE_ROW)
    {
        l_Id = sqlite3_column_int(l_Stmt, 0);
        l_RequiredID = sqlite3_column_int(l_Stmt, 1);
        l_TypeID = (eRequiredType)sqlite3_column_int(l_Stmt, 2);
        l_Data0 = sqlite3_column_int(l_Stmt, 3);
        l_Data1 = sqlite3_column_int(l_Stmt, 4);

        p_RequiredManager->AddSubRequiered(l_RequiredID, (eRequiredType)l_TypeID, l_Data0, l_Data1);
    }
    sqlite3_finalize(l_Stmt);
    return true;
}

bool    SqlManager::InitializeRewards(RequiredManager* p_RequiredManager)
{
    std::string l_Query = "SELECT `rewardID`, `requiredID`, `rewardType`, `chancePct`, `data0`, `data1` FROM reward";
    sqlite3_stmt* l_Stmt;
    if (!Prepare(m_MysqlWorld, l_Query, &l_Stmt))
        throw std::runtime_error("invalid database query");

    uint16 l_Id = 0;
    int32 l_RequiredID = -1;
    eRewardType l_RewardType = eRewardType::REWARD_CURRENCY;
    float l_ChancePct = 0.0f;
    std::vector<int32> l_Data;

    while (sqlite3_step(l_Stmt) == SQLITE_ROW)
    {
        l_Id = sqlite3_column_int(l_Stmt, 0);
        l_RequiredID = sqlite3_column_int(l_Stmt, 1);
        l_RewardType = (eRewardType)sqlite3_column_int(l_Stmt, 2);
        l_ChancePct = (float)sqlite3_column_double(l_Stmt, 3);
        Required* l_Required = nullptr;

        if (l_RequiredID >= 0) /// -1 if no required
            l_Required = p_RequiredManager->GetRequiered(l_RequiredID);

        SubReward l_SubReward(l_RewardType, l_Required, l_ChancePct);
        for (uint8 i = 0; i < 2; i++)
        {
            if (sqlite3_column_int(l_Stmt, 3 + 1 + i) < 0)
                break;

            l_SubReward.AddData(sqlite3_column_int(l_Stmt, 3 + 1 + i));
        }
        Reward* l_Reward = g_RewardManager->GetReward(l_Id);
        if (l_Reward == nullptr)
        {
            l_Reward = new Reward(l_Id);
            g_RewardManager->AddReward(l_Reward);
        }
        l_Reward->AddSubReward(l_SubReward);
    }
    sqlite3_finalize(l_Stmt);
    return true;
}

bool  SqlManager::InitializeItems(RequiredManager* p_RequiredManager)
{
    /// QUEST TEMPLATE
    std::string l_Query = "SELECT `id`, `type`, `subType`, `name`, `level`, `stackMax`, `rareLevel`, `requiredID`, `price`, `data0`, `data1`, `data2`, `data3` FROM item";
    sqlite3_stmt* l_Stmt;
    if (!Prepare(m_MysqlWorld, l_Query, &l_Stmt))
        throw std::runtime_error("invalid database query");

    uint16 l_Id = 0;
    eItemType l_Type = eItemType::ITEM_USELESS;
    std::string l_Name = "";
    uint8 l_SubType;
    uint8 l_Level = 0;
    uint8 l_StackNB = 0;
    uint16 l_Price = 0;
    eItemRareLevel l_RareLevel = eItemRareLevel::ITEM_RARE1;
    int32 l_RequiredID = -1;
    std::vector<int32> l_Data;
    Required* l_Required = nullptr;

    while (sqlite3_step(l_Stmt) == SQLITE_ROW)
    {
        l_Id = sqlite3_column_int(l_Stmt, 0);
        l_Type = (eItemType)sqlite3_column_int(l_Stmt, 1);
        l_SubType = sqlite3_column_int(l_Stmt, 2);
        l_Name = (const char*) sqlite3_column_text(l_Stmt, 3);
        l_Level = sqlite3_column_int(l_Stmt, 4);
        l_StackNB = sqlite3_column_int(l_Stmt, 5);
        l_RareLevel = (eItemRareLevel)sqlite3_column_int(l_Stmt, 6);
        l_RequiredID = sqlite3_column_int(l_Stmt, 7);
        l_Price = sqlite3_column_int(l_Stmt, 8);

        if (l_RequiredID >= 0) /// -1 if no required
            l_Required = p_RequiredManager->GetRequiered(l_RequiredID);
        ItemTemplate* l_ItemTemplate = new ItemTemplate(l_Id, l_Type, l_SubType, l_Name, l_Level, l_StackNB, l_RareLevel, l_Required, l_Price);

        for (uint8 i = 0; i < 4; i++)
        {
            if (sqlite3_column_int(l_Stmt, 8 + 1 + i) < 0)
                break;
            l_ItemTemplate->AddData(sqlite3_column_int(l_Stmt, 8 + 1 + i));
        }
        g_ItemManager->AddItemTemplate(l_ItemTemplate);
    }
    sqlite3_finalize(l_Stmt);
    return true;
}

bool  SqlManager::InitializeQuests()
{
    /// QUEST TEMPLATE
    std::string l_Query = "SELECT `id`, `repetitionType`, `name`  FROM quest_template";
    sqlite3_stmt* l_Stmt;
    if (!Prepare(m_MysqlWorld, l_Query, &l_Stmt))
        throw std::runtime_error("invalid database query");

    uint16 l_Id = 0;
    uint8 l_RepetitionType = 0;
    std::string l_Name = "";

    while (sqlite3_step(l_Stmt) == SQLITE_ROW)
    {
        l_Id = sqlite3_column_int(l_Stmt, 0);
        l_RepetitionType = sqlite3_column_int(l_Stmt, 1);
        l_Name = (const char*) sqlite3_column_text(l_Stmt, 2);

        //printf("Add Quest %d %d %s\n", l_Id, l_RepetitionType, l_Name.c_str());
        QuestTemplate* l_QuestTempalte = new QuestTemplate(l_Id, (eRepetitionType)l_RepetitionType, l_Name);
        g_QuestManager->AddQuestTemplate(l_QuestTempalte);
    }
    sqlite3_finalize(l_Stmt);

    /// OBJECTIFS
    l_Query = "SELECT `questID`, `id`, `typeID`, `data0`, `data1`, `data2`, `data3`, `entitled` FROM objectif_quest_template";
    if (!Prepare(m_MysqlCharacters, l_Query, &l_Stmt))
        throw std::runtime_error("invalid database query");

    uint8 l_IdObjective = 0;
    uint16 l_QuestID = 0;
    uint8 l_TypeID = 0;
    int16 l_Data0 = 0;
    int16 l_Data1 = 0;
    int16 l_Data2 = 0;
    int16 l_Data3 = 0;
    std::string l_Entitled = "";

    while (sqlite3_step(l_Stmt) == SQLITE_ROW)
    {
        l_QuestID = sqlite3_column_int(l_Stmt, 0);
        l_IdObjective = sqlite3_column_int(l_Stmt, 1);
        l_TypeID = sqlite3_column_int(l_Stmt, 2);
        l_Data0 = sqlite3_column_int(l_Stmt, 3);
        l_Data1 = sqlite3_column_int(l_Stmt, 4);
        l_Data2 = sqlite3_column_int(l_Stmt, 5);
        l_Data3 = sqlite3_column_int(l_Stmt, 6);
        l_Entitled = (const char*) sqlite3_column_text(l_Stmt, 7);

        ObjectifQuestTemplate* l_ObjectifQuestTemplate = new ObjectifQuestTemplate(l_IdObjective, (eObjectifType)l_TypeID, l_Data0, l_Data1, l_Data2, l_Data3, l_Entitled);
        g_QuestManager->AddObjectifToTemplate(l_QuestID, l_ObjectifQuestTemplate);
    }
    sqlite3_finalize(l_Stmt);

    return true;
}

bool SqlManager::InitializeSkins()
{
    std::string l_Query = "SELECT `id`, `name` FROM skin";
    sqlite3_stmt* l_Stmt;
    if (!Prepare(m_MysqlWorld, l_Query, &l_Stmt))
        throw std::runtime_error("invalid database query");

    uint16 l_Id = 0;
    uint8 l_RepetitionType = 0;
    std::string l_Name = "";
    uint8 l_Type = 0;

    while (sqlite3_step(l_Stmt) == SQLITE_ROW)
    {
        l_Id = sqlite3_column_int(l_Stmt, 0);
        l_Name = (const char*) sqlite3_column_text(l_Stmt, 1);
        g_Skins[l_Id] = Skin(l_Id, l_Name);
    }
    sqlite3_finalize(l_Stmt);
    return true;
}

bool SqlManager::InitializeTitles()
{
    std::string l_Query = "SELECT `id`, `type`, `name` FROM title";
    sqlite3_stmt* l_Stmt;
    if (!Prepare(m_MysqlWorld, l_Query, &l_Stmt))
        throw std::runtime_error("invalid database query");

    uint16 l_Id = 0;
    std::string l_Name = "";
    uint8 l_Type = 0;

    while (sqlite3_step(l_Stmt) == SQLITE_ROW)
    {
        l_Id = sqlite3_column_int(l_Stmt, 0);
        l_Type = sqlite3_column_int(l_Stmt, 1);
        l_Name = (const char*) sqlite3_column_text(l_Stmt, 2);
        g_Titles[l_Id] = Title(l_Id, l_Name, (eTypeTitle)l_Type);
    }
    sqlite3_finalize(l_Stmt);
    return true;
}

std::map<uint8, uint16> SqlManager::GetXpLevel()
{
    std::map<uint8, uint16> l_XpLevel;

    std::string l_Query = "SELECT `level`, `xp` FROM level_xp";
    sqlite3_stmt* l_Stmt;
    if (!Prepare(m_MysqlWorld, l_Query, &l_Stmt))
        throw std::runtime_error("invalid database query");

    uint8 l_Level = 0;
    uint16 l_Xp = 0;

    while (sqlite3_step(l_Stmt) == SQLITE_ROW)
    {
        l_Level = sqlite3_column_int(l_Stmt, 0);
        l_Xp = sqlite3_column_int(l_Stmt, 1);

        l_XpLevel[l_Level] = l_Xp;
    }
    sqlite3_finalize(l_Stmt);

    return l_XpLevel;
}

bool SqlManager::InitializeGameObject(DynamicObjectManager* p_DynamicObjectManager, RequiredManager* p_RequiredManager, UnitManager* p_UnitManager)
{
    std::string l_Query = "SELECT `id`, `typeID`, `requiredID`, `blocking`, `skinID`, `duration`, `respawnTime`, `data0`, `data1`, `data2`, `data3` FROM gob_template";
    sqlite3_stmt* l_Stmt;
    if (!Prepare(m_MysqlWorld, l_Query, &l_Stmt))
        throw std::runtime_error("invalid database query");

    uint16 l_Id = 0;
    uint16 l_TypeID = 0;
    int32 l_RequiredID = -1;
    int16 l_SkinID = 0;
    bool l_Blocking = false;
    int32 l_Duration = 0;
    int32 l_RespawnTime = 0;
    uint32 l_Data0 = 0;
    uint32 l_Data1 = 0;
    uint32 l_Data2 = 0;
    uint32 l_Data3 = 0;

    while (sqlite3_step(l_Stmt) == SQLITE_ROW)
    {
        l_Id = sqlite3_column_int(l_Stmt, 0);
        l_TypeID = sqlite3_column_int(l_Stmt, 1);
        l_RequiredID = sqlite3_column_int(l_Stmt, 2);
        l_Blocking = (bool)sqlite3_column_int(l_Stmt, 3);
        l_SkinID = sqlite3_column_int(l_Stmt, 4);
        l_Duration = sqlite3_column_int(l_Stmt, 5);
        l_RespawnTime = sqlite3_column_int(l_Stmt, 6);
        l_Data0 = sqlite3_column_int(l_Stmt, 7);
        l_Data1 = sqlite3_column_int(l_Stmt, 8);
        l_Data2 = sqlite3_column_int(l_Stmt, 9);
        l_Data3 = sqlite3_column_int(l_Stmt, 10);

        Required* l_Required = nullptr;
        if (l_RequiredID >= 0) /// -1 if no required
            l_Required = p_RequiredManager->GetRequiered(l_RequiredID);
        GameObjectTemplate l_GameObjectTemplate(l_Id, l_Duration, l_RespawnTime, (eGameObjectTemplate)l_TypeID, l_SkinID, l_Blocking, l_Required);
        l_GameObjectTemplate.SetData(0, l_Data0);
        l_GameObjectTemplate.SetData(1, l_Data1);
        l_GameObjectTemplate.SetData(2, l_Data2);
        l_GameObjectTemplate.SetData(3, l_Data3);
        p_DynamicObjectManager->AddGameObjectTemplate(l_GameObjectTemplate);
    }
    sqlite3_finalize(l_Stmt);
    

    l_Query = "SELECT `id`, `gobID`, `mapID`, `caseNb` FROM gob";
    if (!Prepare(m_MysqlCharacters, l_Query, &l_Stmt))
        throw std::runtime_error("invalid database query");

    l_Id = 0;
    uint16 l_GobID = 0;
    uint16 l_MapID = 0;
    uint32 l_CaseNb = 0;

    uint32 l_PosX = 0;
    uint32 l_PosY = 0;

    while (sqlite3_step(l_Stmt) == SQLITE_ROW)
    {
        l_Id = sqlite3_column_int(l_Stmt, 0);
        l_GobID = sqlite3_column_int(l_Stmt, 1);
        l_MapID = sqlite3_column_int(l_Stmt, 2);
        l_CaseNb = sqlite3_column_int(l_Stmt, 3);

        //Map* l_Map = g_MapManager->GetMap(l_MapID);
        MapTemplate* l_MapTemplate = g_MapManager->GetMapTemplate(l_MapID);

        if (l_MapTemplate == nullptr)
        {
            printf("Erreur map %d don't exist\n", l_MapID);
            continue;
        }

        l_PosX = (l_CaseNb % l_MapTemplate->GetSizeX() * TILE_SIZE) + TILE_SIZE / 2;
        l_PosY = (l_CaseNb / l_MapTemplate->GetSizeX() * TILE_SIZE) + TILE_SIZE / 2;


        GameObjectTemplate* l_GobTemplate = p_DynamicObjectManager->GetGameObjectTemplate(l_GobID);

        if (l_GobTemplate == nullptr)
            continue;

        //GameObject* l_Gob = new GameObject(l_Id, l_MapTemplate->GetID(), l_PosX, l_PosY, l_GobTemplate);
        /*if (p_UnitManager->GetGossipListFor(TypeUnit::GAMEOBJECT, l_GobID) != nullptr)
            l_Gob->SetGossipList(p_UnitManager->GetGossipListFor(TypeUnit::GAMEOBJECT, l_GobID));*/
        l_MapTemplate->AddGobMapTemplate(GobMapTemplate(l_Id, l_MapID, l_GobTemplate, l_CaseNb, l_PosX, l_PosY, p_UnitManager->GetGossipListFor(TypeUnit::GAMEOBJECT, l_GobID)));
        //l_MapTemplate->GetCase(l_CaseNb)->AddDynamicOject(l_Gob);
    }
    sqlite3_finalize(l_Stmt);

    return true;
}

bool SqlManager::InitializeZones()
{
    std::string l_Query = "SELECT `id`, `typeID`, `name`,`mapID`, `caseBegin`, `caseEnd` FROM zone";
    sqlite3_stmt* l_Stmt;
    if (!Prepare(m_MysqlWorld, l_Query, &l_Stmt))
        throw std::runtime_error("invalid database query");

    uint16 l_ID = 0;
    uint16 l_TypeID = 0;
    uint16 l_MapID = 0;
    uint32 l_CaseNbBegin = 0;
    uint32 l_CaseNbEnd = 0;
    std::string l_Name = "";

    while (sqlite3_step(l_Stmt) == SQLITE_ROW)
    {
        l_ID = sqlite3_column_int(l_Stmt, 0);
        l_TypeID = sqlite3_column_int(l_Stmt, 1);
        l_Name = (const char*) sqlite3_column_text(l_Stmt, 2);
        l_MapID = sqlite3_column_int(l_Stmt, 3);
        l_CaseNbBegin = sqlite3_column_int(l_Stmt, 4);
        l_CaseNbEnd = sqlite3_column_int(l_Stmt, 5);

        MapTemplate* l_MapTemplate = g_MapManager->GetMapTemplate(l_MapID);
        //Map* l_Map = g_MapManager->GetMap(l_MapID);
        if (l_MapTemplate == nullptr)
        {
            printf("Erreur map %d don't exist\n", l_MapID);
            continue;
        }
        Zone l_Zone = Zone(l_ID, (eTypeZone)l_TypeID, l_Name, l_CaseNbBegin, l_CaseNbEnd);

        l_MapTemplate->AddZone(l_Zone);
    }
    sqlite3_finalize(l_Stmt);

    return true;
}

bool SqlManager::InitializeAreatrigger(DynamicObjectManager* p_DynamicObjectManager, UnitManager* p_UnitManager)
{
    std::string l_Query = "SELECT `id`, `typeID`,`skinID`, `radius`, `data0`, `data1`, `data2`, `data3` FROM areatrigger_template";
    sqlite3_stmt* l_Stmt;
    if (!Prepare(m_MysqlWorld, l_Query, &l_Stmt))
        throw std::runtime_error("invalid database query");

    uint16 l_Id = 0;
    uint16 l_TypeID = 0;
    int16 l_SkinID = 0;
    float l_Radius = 0;
    uint32 l_Data0 = 0;
    uint32 l_Data1 = 0;
    uint32 l_Data2 = 0;
    uint32 l_Data3 = 0;

    while (sqlite3_step(l_Stmt) == SQLITE_ROW)
    {
        l_Id = sqlite3_column_int(l_Stmt, 0);
        l_TypeID = sqlite3_column_int(l_Stmt, 1);
        l_SkinID = sqlite3_column_int(l_Stmt, 2);
        l_Radius = (float)sqlite3_column_double(l_Stmt, 3);
        l_Data0 = sqlite3_column_int(l_Stmt, 4);
        l_Data1 = sqlite3_column_int(l_Stmt, 5);
        l_Data2 = sqlite3_column_int(l_Stmt, 6);
        l_Data3 = sqlite3_column_int(l_Stmt, 7);

        AreatriggerTemplate l_AreatriggerTemplate(l_Id, l_Radius, (eAreatriggerType)l_TypeID, l_SkinID);
        l_AreatriggerTemplate.SetData(0, l_Data0);
        l_AreatriggerTemplate.SetData(1, l_Data1);
        l_AreatriggerTemplate.SetData(2, l_Data2);
        l_AreatriggerTemplate.SetData(3, l_Data3);
        p_DynamicObjectManager->AddAreatriggerTemplate(l_AreatriggerTemplate);
    }
    sqlite3_finalize(l_Stmt);


    l_Query = "SELECT `id`, `areatriggerID`, `mapID`, `caseNb` FROM areatrigger";
    if (!Prepare(m_MysqlWorld, l_Query, &l_Stmt))
        throw std::runtime_error("invalid database query");

    l_Id = 0;
    uint16 l_AreatriggerID = 0;
    uint16 l_MapID = 0;
    uint32 l_CaseNb = 0;

    uint32 l_PosX = 0;
    uint32 l_PosY = 0;

    while (sqlite3_step(l_Stmt) == SQLITE_ROW)
    {
        l_Id = sqlite3_column_int(l_Stmt, 0);
        l_AreatriggerID = sqlite3_column_int(l_Stmt, 1);
        l_MapID = sqlite3_column_int(l_Stmt, 2);
        l_CaseNb = sqlite3_column_int(l_Stmt, 3);

        MapTemplate* l_MapTemplate = g_MapManager->GetMapTemplate(l_MapID);
        if (l_MapTemplate == nullptr)
        {
            printf("Erreur map %d don't exist\n", l_MapID);
            continue;
        }
        l_PosX = (l_CaseNb % l_MapTemplate->GetSizeX() * TILE_SIZE) + TILE_SIZE / 2;
        l_PosY = (l_CaseNb / l_MapTemplate->GetSizeX() * TILE_SIZE) + TILE_SIZE;


        AreatriggerTemplate* l_AreatriggerTemplate = p_DynamicObjectManager->GetAreatriggerTemplate(l_AreatriggerID);

        if (l_AreatriggerTemplate == nullptr)
            continue;
        //Areatrigger* l_Areatrigger = new Areatrigger(l_Id, l_MapTemplate->GetID(), l_PosX, l_PosY, l_AreatriggerTemplate);
        l_MapTemplate->AddAreaTriggerMapTemplate(AreaTriggerMapTemplate(l_Id, l_MapID, l_AreatriggerTemplate, l_CaseNb, l_PosX, l_PosY, p_UnitManager->GetGossipListFor(TypeUnit::AREATRIGGER, l_AreatriggerID)));
        /*l_MapTemplate->AddUnit(l_Areatrigger);
        l_MapTemplate->GetCase(l_CaseNb)->AddDynamicOject(l_Areatrigger);*/
    }
    sqlite3_finalize(l_Stmt);

    return true;
}

int16 SqlManager::GetLevel(const std::string & p_PlayerName)
{
    std::string l_Query = "SELECT `level` FROM characters WHERE `name` = '" + p_PlayerName + "'";
    sqlite3_stmt* l_Stmt;
    if (!Prepare(m_MysqlCharacters, l_Query, &l_Stmt))
        throw std::runtime_error("invalid database query");

    int16 l_Level = -1;

    while (sqlite3_step(l_Stmt) == SQLITE_ROW)
    {
        l_Level = sqlite3_column_int(l_Stmt, 0);
    }
    sqlite3_finalize(l_Stmt);

    return l_Level;
}

WorldPosition SqlManager::GetPosition(const std::string & p_PlayerName)
{
    std::string l_Query = "SELECT `mapID`, `posX`, `posY` FROM characters WHERE `name` = '" + p_PlayerName + "'";
    sqlite3_stmt* l_Stmt;
    if (!Prepare(m_MysqlCharacters, l_Query, &l_Stmt))
        throw std::runtime_error("invalid database query");

    uint16 l_MapID = 0;
    uint32 l_PosX = 0;
    uint32 l_PosY = 0;

    while (sqlite3_step(l_Stmt) == SQLITE_ROW)
    {
        l_MapID = sqlite3_column_int(l_Stmt, 0);
        l_PosX = sqlite3_column_int(l_Stmt, 1);
        l_PosY = sqlite3_column_int(l_Stmt, 2);
    }
    sqlite3_finalize(l_Stmt);

    WorldPosition l_Position(l_PosX, l_PosY, l_MapID, 0, Orientation::Up);
    return l_Position;
}

bool SqlManager::InitializeMaps()
{
    std::string l_Query = "SELECT `id`, `typeID`,`name`, `fileName`, `fileChipsets`, `maxPlayers`, `instance` FROM maps";
    sqlite3_stmt* l_Stmt;
    if (!Prepare(m_MysqlWorld, l_Query, &l_Stmt))
        return true;

    uint16 l_Id = 0;
    uint16 l_TypeID = 0;
    uint16 l_MaxPlayers = 0;
    uint16 l_Instance = 0;
    std::string l_FileName = "";
    std::string l_FileChipsets = "";
    std::string l_Name = "";

    while (sqlite3_step(l_Stmt) == SQLITE_ROW)
    {
        l_Id = sqlite3_column_int(l_Stmt, 0);
        l_TypeID = sqlite3_column_int(l_Stmt, 1);
        l_Name = (const char*) sqlite3_column_text(l_Stmt, 2);
        l_FileName = (const char*) sqlite3_column_text(l_Stmt, 3);
        l_FileChipsets = (const char*) sqlite3_column_text(l_Stmt, 4);
        l_MaxPlayers = sqlite3_column_int(l_Stmt, 5);
        l_Instance = sqlite3_column_int(l_Stmt, 6);

        MapTemplate* l_MapTemplate = new MapTemplate(l_Id, (eTypeMap)l_TypeID, l_MaxPlayers, l_Name, l_FileName, l_FileChipsets, (bool)l_Instance);
        g_MapManager->AddMapTemplate(l_MapTemplate);
    }
    sqlite3_finalize(l_Stmt);
    return true;
}

bool SqlManager::InitializeBattlegrounds()
{
    std::string l_Query = "SELECT `id`, `mapID`,`minPlayers`, `maxPlayers`, `timeMax` FROM battleground";
    sqlite3_stmt* l_Stmt;
    if (!Prepare(m_MysqlWorld, l_Query, &l_Stmt))
        throw std::runtime_error("invalid database query");

    uint16 l_Id = 0;
    uint16 l_MapID = 0;
    uint16 l_MinPlayers = 0;
    uint16 l_MaxPlayers = 0;
    uint16 l_TimeMax = 0;

    while (sqlite3_step(l_Stmt) == SQLITE_ROW)
    {
        l_Id = sqlite3_column_int(l_Stmt, 0);
        l_MapID = sqlite3_column_int(l_Stmt, 1);
        l_MinPlayers = sqlite3_column_int(l_Stmt, 2);
        l_MaxPlayers = sqlite3_column_int(l_Stmt, 3);
        l_TimeMax = sqlite3_column_int(l_Stmt, 4);

        g_MapManager->AddBGTemplate(new BGTemplate(l_Id, l_MapID, l_MinPlayers, l_MaxPlayers, l_TimeMax));
    }
    sqlite3_finalize(l_Stmt);
    return true;
}

int32 SqlManager::GetPlayerID(const std::string & p_PlayerName)
{
    std::string l_Query = "SELECT `characterID` FROM characters WHERE `name` = '" + p_PlayerName + "'";
    sqlite3_stmt* l_Stmt;
    if (!Prepare(m_MysqlCharacters, l_Query, &l_Stmt))
        throw std::runtime_error("invalid database query");

    int32 l_PlayerID = -1;

    while (sqlite3_step(l_Stmt) == SQLITE_ROW)
    {
        l_PlayerID = sqlite3_column_int(l_Stmt, 0);
    }
    sqlite3_finalize(l_Stmt);

    return l_PlayerID;
}

eAccessType SqlManager::GetAccessType(uint32 p_AccountID)
{
    std::string l_Query = "SELECT `accessType` FROM `login_access` WHERE `accountID` = '" + std::to_string(p_AccountID) + "'";
    sqlite3_stmt* l_Stmt;
    if (!Prepare(m_MysqlCharacters, l_Query, &l_Stmt))
        throw std::runtime_error("invalid database query");

    int8 l_ID = 0;
    while (sqlite3_step(l_Stmt) == SQLITE_ROW)
        l_ID = sqlite3_column_int(l_Stmt, 0);

    sqlite3_finalize(l_Stmt);
    return (eAccessType)l_ID;
}

void SqlManager::BlackListIp(const std::string & p_AdressIP, const uint32 & p_AccountAdm, const uint32 & p_TotalHours, const std::string & p_Comment)
{
    std::string l_Query = "INSERT INTO `black_list` (ip, accountAdmin, totalHours, comment) VALUES ('" + p_AdressIP + "', '" + std::to_string(p_AccountAdm) + "', '" + std::to_string(p_TotalHours) + "', '" + p_Comment + "');";
    Exec(m_MysqlCharacters, l_Query);
}

void SqlManager::BlackListAccount(const uint32 & p_AccountID, const uint32 & p_AccountAdm, const uint32 & p_TotalHours, const std::string & p_Comment)
{
    std::string l_Query = "INSERT INTO `black_list` (accountID, accountAdmin, totalHours, comment) VALUES ('" + std::to_string(p_AccountID) + "', '" + std::to_string(p_AccountAdm) + "', '" + std::to_string(p_TotalHours) + "', '" + p_Comment + "');";
    Exec(m_MysqlCharacters, l_Query);
}

bool SqlManager::IsAccountBan(const uint32 & p_AccountID)
{
    std::string l_Query = "SELECT Unix_Timestamp(`date`), `totalHours` FROM `black_list` WHERE `accountID` = '" + std::to_string(p_AccountID) + "'";
    sqlite3_stmt* l_Stmt;
    if (!Prepare(m_MysqlCharacters, l_Query, &l_Stmt))
        throw std::runtime_error("invalid database query");

    bool m_IsBan = false;
    std::time_t l_Now;
    std::time_t l_Time;
    uint32 l_Hours;

    time(&l_Now);
    while (sqlite3_step(l_Stmt) == SQLITE_ROW)
    {
        l_Time = (std::time_t)sqlite3_column_int64(l_Stmt, 0);
        l_Hours = sqlite3_column_int(l_Stmt, 1);
        struct tm now_tm = *localtime(&l_Time);
        struct tm then_tm = now_tm;

        then_tm.tm_hour += l_Hours;
        const double diff = std::difftime(l_Now, mktime(&then_tm));
        if (diff <= 0)
        {
            m_IsBan = true;
            break;
        }
    }

    sqlite3_finalize(l_Stmt);
    return m_IsBan;
}

bool SqlManager::IsIPBan(const std::string & p_IP)
{
    std::string l_Query = "SELECT Unix_Timestamp(`date`), `totalHours` FROM `black_list` WHERE `ip` = '" + p_IP + "'";
    sqlite3_stmt* l_Stmt;
    if (!Prepare(m_MysqlCharacters, l_Query, &l_Stmt))
        throw std::runtime_error("invalid database query");

    bool m_IsBan = false;
    std::time_t l_Now;
    std::time_t l_Time;
    uint32 l_Hours;

    time(&l_Now);
    while (sqlite3_step(l_Stmt) == SQLITE_ROW)
    {
        l_Time = (std::time_t)sqlite3_column_int64(l_Stmt, 0);
        l_Hours = sqlite3_column_int(l_Stmt, 1);
        struct tm now_tm = *localtime(&l_Time);
        struct tm then_tm = now_tm;

        then_tm.tm_hour += l_Hours;
        const double diff = std::difftime(l_Now, mktime(&then_tm));
        if (diff <= 0)
        {
            m_IsBan = true;
            break;
        }
    }

    sqlite3_finalize(l_Stmt);
    return m_IsBan;
}

bool SqlManager::Exec(sqlite3* p_Database, const std::string& p_Query)
{
    char* errMsg;
    if (sqlite3_exec(p_Database, p_Query.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK)
    {
        printf("failed to prepare query %s: %s %s", p_Query.c_str(), sqlite3_errmsg(p_Database), errMsg);
        sqlite3_free(errMsg);
        return false;
    }

    return true;
}

bool SqlManager::Prepare(sqlite3* p_Database, const std::string& p_Query, sqlite3_stmt** p_Stmt)
{
    if (sqlite3_prepare_v2(p_Database, p_Query.c_str(), -1, p_Stmt, nullptr) != SQLITE_OK)
    {
        printf("failed to prepare query %s: %s", p_Query.c_str(), sqlite3_errmsg(p_Database));
        return false;
    }

    return true;
}
