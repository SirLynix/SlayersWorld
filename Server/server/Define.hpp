#pragma once
#include <ctime>
#include <string>
#include <map>
#include <vector>

#include "System/SWType.hpp"

#define MAP_PATH "map/"
#define CONF_FILE "conf/server.conf"

#define SEND_TIME_CLOCK_WEBHOOK 30 ///< In Min

#define GRID_SIZE 12
#define TILE_SIZE 16

#define IN_MILLISECOND 1000
#define IN_MICROSECOND 1000 * IN_MILLISECOND

#define PLAYER_TIME_RESPAWN 9
#define MAX_HEALTH 100
#define MAX_MANA 100
#define MAX_ALIGNMENT 100
#define MAX_EFFECTS_FOR_SPELL 4

#define REGEN_HEALTH_TIMER 5000
#define REGEN_ALIGN_TIMER 10000

#define SKIN_OFFSET_SIZE_X 14
#define UPDATE_TIME_MOVEMENT 200
#define STEP_SIZE 12 /// In pixel
#define MAX_MOVEMENT_POSITION 3

#define OUT_OF_COMBAT_TIME 7
#define MELEE_RANGE 24

#define MAX_SKIN_IMG 91

#define CREATION_POINT_X 358
#define CREATION_POINT_Y 145
#define CREATION_POINT_MAP 2

#define STR_ASSASSIN "Assassin"
#define STR_PALADIN "Paladin"
#define STR_MAGE "Mage"
#define STR_PRETRE "Prêtre"

enum Orientation
{
    Up = 0,
    Right = 1,
    Down = 2,
    Left = 3
};

enum TypeUnit
{
    CREATURE = 0,
    PLAYER,
    ANIMATIONUNIT,
    AREATRIGGER,
    GAMEOBJECT
};

enum TypeBG
{
    NONEBBG = 0,
    PointCapture = 1
};

enum eTypeMap
{
    MapWorld = 0,
    MapBattleGround = 1
};

enum eTypeTitle
{
    TypeTitleBefore = 0,
    TypeTitleAfter = 1
};

enum eGroupType
{
    SIMPLE = 0,
    BG = 1,
    GUILD = 2
};

enum eBGState
{
    STEP0 = 0,
    STEP1,
    STEP2
};

enum eTypeCurrency
{
    CURRENCY_BASISC = 0,
    CURRENCY_HONNOR = 1
};

enum eItemType
{
    ITEM_USELESS = 0,
    ITEM_LAUNCHSPELL = 1,
    ITEM_EQUIPMENT = 2,
    ITEM_QUEST = 3,
    ITEM_CONSUMABLE = 4,
    ITEM_MAX = 5
};

enum eRewardType
{
    REWARD_ITEM = 0,
    REWARD_CURRENCY = 1
};


enum eItemRareLevel
{
    ITEM_RARE1 = 0,
    ITEM_RARE2 = 1,
    ITEM_RARE3 = 2,
    ITEM_RARE4 = 3,
    ITEM_RARE5 = 4,
    ITEM_MAX_RARE = 5
};

enum eTypeEquipment
{
    EQUIP_HEAD = 0,
    EQUIP_SHOLDER = 1,
    EQUIP_BODY = 2,
    EQUIP_HAND = 3,
    EQUIP_FOOT = 4,
    EQUIP_MAIN_HAND = 5,
    EQUIP_SECOND_HAND = 6,
    EQUIP_MAX = 7
};

enum eAiType
{
    PASSIVE = 0,
    DEFENSIVE = 1,
    AGRESIVE = 2,
    PASSIVE_ATTACK_EFFECT = 3
};

enum eActionType
{
    Go = 0,
    Attack = 1,
    Stop = 2,
    StopAttack = 3
};

enum eStats
{
    Free = 0,
    Dexterity,
    Force,
    Stamina,
    Speed,
    Level,
    MaxStat
};

enum eInterMapAction
{
    SwitchMap = 0
};

enum eAccessType
{
    Dummy = 0,
    Moderator = 1,
    Administrator = 2
};

enum eFactionType
{
    Ally = 0,
    Enemy = 1,
    Neutral = 2
};

enum eGossipType
{
    Whisp = 0,
    Yell = 1,
    LaunchQuest = 2,
    ValidQuest = 3,
    Announce = 4,
    SimpleQuestion = 5,
    LaunchBG = 6,
    GossipLaunchSpell = 7
};

enum ePlayerMod
{
    NONEMOD = 0,
    GODMOD = 1
};

enum eDynamicObjectType
{
    AreaTriggerType = 0,
    GameObjectType
};

enum eAreatriggerType
{
    AreatriggerTeleport = 0,
    AreatriggerPeriodicDamage = 1
};

enum eGameObjectTemplate
{
    GameObjectSpellLauncher = 0,
    GameObjectQuest = 1,
    GameObjectTrap = 2,
    GameObjectLaunchGossip = 3,
    GameObjectDoor = 4,
    GameObjectVisu = 5
};

enum eRequiredType
{
    QuestDone = 0,
    QuestNotDone = 1,
    LevelMin = 2,
    LevelMax = 3,
    QuestAllObjectiveDone = 4,
    QuestInProgress = 5,
    AccessLevel = 6,
    ClassNeed = 7,
    HasSpell = 8,
    HasNotSpell = 9
};

enum eResourceType
{
    None = 0,
    Health = 1,
    Mana = 2,
    Alignment = 3
};

enum SpellEffectType
{
    Damage = 0,
    Heal = 1,
    Teleport = 2,
    LaunchSpell = 3,
    ApplyAura = 4,
    LearnClass = 5,
    LearnSpell = 6,
};

enum SpellTarget
{
    Caster = 0,
    CloserAlly,
    CloserEnemy,
    ZoneAlly,
    ZoneEnemy
};

enum eObjectifType
{
    KillMob = 0,
    SpeackMob = 1,
    RecoltGob = 2,
    EnterInZone = 3
};

enum eTextColor
{
    TextColorWhite = 0,
    TextColorGreen = 1,
    TextColorRed = 2,
    TextColorBlue = 3,
    TextColorYellow = 4
};

enum eTextStyle
{
    TextStyleRegular = 0,
    TextStyleBold = 1,
    TextStyleItalic = 2,
    TextStyleUnderLine = 3,
    TextStyleStrikeThrough = 4,
};

enum eRepetitionType
{
    Never = -1,
    Daily = 1,
    Weekly = 7
};

enum eTypeWarningMsg
{
    Red = 0,
    Yellow,
    Top,
    BigMsg,
    WarningReward
};

enum eTypeAuraEffect
{
    PERIODIC_DAMAGE = 0,
    PERIODIC_HEAL = 1,
    UPDATE_SPEED = 2,
    MODIFY_DAMAGE_PCT = 3,
    MOUNT = 4
};

enum eExtraInterface
{
    eBattelGroundUI = 0
};

enum eClass
{
    NONECLASS = 0,
    ASSASSIN = 1,
    PALADIN = 2,
    MAGE = 3,
    PRETRE = 4
};

struct ObjectifQuestTemplate
{
    uint8 m_Id;
    eObjectifType m_ObjectifType;
    int16 m_Data0;
    int16 m_Data1;
    int16 m_Data2;
    int16 m_Data3;
    std::string m_Entitled;

    ObjectifQuestTemplate(uint8 p_Id, eObjectifType p_ObjectifType, int16 p_Data0, int16 p_Data1, int16 p_Data2, int16 p_Data3, const std::string & p_Entitled) :
        m_Id(p_Id), m_ObjectifType(p_ObjectifType), m_Data0(p_Data0), m_Data1(p_Data1), m_Data2(p_Data2), m_Data3(m_Data3), m_Entitled(p_Entitled) {}
};

struct ObjectifProgess
{
    uint8 m_Id;
    int16 m_Data0;

    ObjectifProgess(uint8 p_Id) :
        m_Id(p_Id), m_Data0(0) {}
};

struct QuestTemplate
{
    uint16 m_Id;
    eRepetitionType m_RepetitionType;
    std::string m_Name;
    std::map< uint8, ObjectifQuestTemplate* > m_ObjectifList;

    QuestTemplate(uint16 p_Id, eRepetitionType p_RepetitionType, const std::string & p_Name) :
        m_Id(p_Id), m_RepetitionType(p_RepetitionType), m_Name(p_Name) {}

    void AddObjectif(ObjectifQuestTemplate* p_Objecif)
    {
        m_ObjectifList[p_Objecif->m_Id] = p_Objecif;
    }
};

struct ResourceNeed
{
    eResourceType m_ResourceType;
    int16 m_Nb;

    ResourceNeed(eResourceType p_ResourceType, int16 p_Nb) :
        m_ResourceType(p_ResourceType), m_Nb(p_Nb) {}
};

enum DamageResult
{
    Success = 0,
    Miss = 1
};

enum eKeyBoardAction
{
    NoneAction = 0,
    KeyBoardDown,
    KeyBoardUp,
    KeyBoardLeft,
    KeyBoardRight,
    KeyBoardAction,
    KeyBoardEnter,
    KeyBoardHistoryMsg,
    KeyBoardAutoAttack,
    KeyBoardSpell0,
    KeyBoardSpell1,
    MaxKeyBoard
};

struct DamageInfo
{
    int8 m_Damage;
    DamageResult m_Result;

    DamageInfo() :
        m_Damage(0), m_Result(DamageResult::Success) {}
};

struct Position
{
    uint32 m_X;
    uint32 m_Y;

    Position() :
    m_X(0), m_Y(0) {}

    Position(uint32 p_X, uint32 p_Y) :
    m_X(p_X), m_Y(p_Y) {}

    Position(const Position & p_Position) :
    m_X(p_Position.m_X), m_Y(p_Position.m_Y) {}

    bool operator==(Position const& a) const { return m_X == a.m_X && m_Y == a.m_Y; }
    bool operator!=(Position const& a) const { return m_X != a.m_X || m_Y != a.m_Y; }
    bool operator<(Position const& a) const { return m_X < a.m_X || (!(a.m_X < m_X) && m_Y < a.m_Y); }
};

typedef std::vector<Position> Path;

class Required; /// Double inclusion
struct Gossip
{
    uint16 m_ID;
    TypeUnit m_TypeUnit;
    uint16 m_UnitEntry;
    eGossipType m_GossipType;
    uint32 m_Data1;
    uint32 m_Data2;
    std::string m_Msg;
    uint64 m_GossipTimer;
    Required* m_Required;
    bool m_Automatic;

    Gossip() :
        m_ID(0), m_TypeUnit(TypeUnit::CREATURE), m_UnitEntry(0), m_Automatic(true), m_GossipType(eGossipType::Whisp), m_Data1(0), m_Data2(0), m_Msg(""), m_GossipTimer(0), m_Required(nullptr) {}

    Gossip(const Gossip & p_Gossip) :
        m_ID(p_Gossip.m_ID), m_Required(p_Gossip.m_Required), m_TypeUnit(p_Gossip.m_TypeUnit), m_UnitEntry(p_Gossip.m_UnitEntry), m_Automatic(p_Gossip.m_Automatic), m_GossipType(p_Gossip.m_GossipType), m_Data1(p_Gossip.m_Data1), m_Data2(p_Gossip.m_Data2), m_Msg(p_Gossip.m_Msg), m_GossipTimer(p_Gossip.m_GossipTimer){}
        
    Gossip(uint16 p_ID, Required* p_Required, TypeUnit p_TypeUnit, uint16 p_UnitEntry, bool p_Automatic, eGossipType p_GossipType, uint32 p_Data1, uint32 p_Data2, std::string p_Msg) :
        m_ID(p_ID), m_Required(p_Required), m_TypeUnit(p_TypeUnit), m_UnitEntry(p_UnitEntry), m_Automatic(p_Automatic),  m_GossipType(p_GossipType), m_Data1(p_Data1), m_Data2(p_Data2), m_Msg(p_Msg), m_GossipTimer(0) {}
};

struct SpellEffect
{
    uint16 m_ID;
    SpellEffectType m_EffectID;
    SpellTarget m_Target;
    int32 m_BasePoint1;
    int32 m_BasePoint2;
    int32 m_BasePoint3;
    int32 m_BasePoint4;
    float m_RadiusMin;
    float m_RadiusMax;

    SpellEffect() {}

    SpellEffect(uint16 p_ID, SpellEffectType p_EffectID, SpellTarget p_Target, int32 p_BP1, int32 p_BP2, int32 p_BP3, int32 p_BP4, float p_RadiusMin, float p_RadiusMax) :
        m_ID(p_ID), m_EffectID(p_EffectID), m_Target(p_Target), m_BasePoint1(p_BP1), m_BasePoint2(p_BP2), m_BasePoint3(p_BP3), m_BasePoint4(p_BP4), m_RadiusMin(p_RadiusMin), m_RadiusMax(p_RadiusMax) {}
};

enum eTypeZone
{
    NoneZone = 0,
    QuestZone = 1, ///< don't send to client
    NeutralZone = 2, ///< Attack impossible
    ArenaZone = 3
};

struct Zone
{
    uint16 m_ID;
    eTypeZone m_TypeID;
    std::string m_Name;
    uint32 m_CaseBegin;
    uint32 m_CaseEnd;
    bool m_enabled;

    Zone(const Zone & p_Zone) :
        m_ID(p_Zone.m_ID), m_TypeID(p_Zone.m_TypeID), m_Name(p_Zone.m_Name), m_CaseBegin(p_Zone.m_CaseBegin), m_CaseEnd(p_Zone.m_CaseEnd), m_enabled(p_Zone.m_enabled) {}

    Zone(uint16 p_ID, eTypeZone p_TypeID, std::string p_Name, uint32 p_CaseBegin, uint32 p_CaseEnd) :
        m_ID(p_ID), m_TypeID(p_TypeID), m_Name(p_Name), m_CaseBegin(p_CaseBegin), m_CaseEnd(p_CaseEnd), m_enabled(true) {}
};

struct AnimationUnitTemplate
{
    uint32 m_Entry;
    int16 m_SkinID;
    uint16 m_TypeID;
    std::string m_Name;

    float m_StopTimeMin;
    float m_StopTimeMax;

    AnimationUnitTemplate() :
        m_Entry(0), m_SkinID(0), m_TypeID(0), m_Name(""), m_StopTimeMin(0.0f), m_StopTimeMax(0.0f) {}

    AnimationUnitTemplate(uint32 p_Entry, int16 p_SkinID, uint16 p_TypeID, std::string p_Name, float p_StopTimeMin, float p_StopTimeMax) :
        m_Entry(p_Entry), m_SkinID(p_SkinID), m_TypeID(p_TypeID), m_Name(p_Name), m_StopTimeMin(p_StopTimeMin), m_StopTimeMax(p_StopTimeMax) {}
};

struct CreatureTemplate
{
    uint32 m_Entry;
    int16 m_SkinID;
    std::string m_Name;
    uint8 m_Level;
    uint8 m_Force;
    uint8 m_Stamina;
    uint8 m_Dexterity;
    uint8 m_Speed;
    uint8 m_Xp;
    uint8 m_State;
    uint16 m_MaxRay;
    uint16 m_MaxVision;
    float m_MovingTimeMin;
    float m_MovingTimeMax;
    float m_StopTimeMin;
    float m_StopTimeMax;
    int32 m_RespawnTime;
    uint8 m_Rank;
    uint8 m_AiType;
    eFactionType m_FactionType;
    int32 m_RewardID;

    CreatureTemplate() :
        m_Entry(0), m_SkinID(0), m_Name(""), m_Level(0), m_Force(0), m_Stamina(0), m_Dexterity(0), m_Speed(0), m_Xp(0), m_State(0), m_MaxRay(0), m_MaxVision(0), m_MovingTimeMin(0.0f), m_MovingTimeMax(0.0f), m_StopTimeMin(0.0f), m_StopTimeMax(0.0f), m_RespawnTime(0), m_Rank(0), m_AiType(0), m_FactionType(eFactionType::Ally), m_RewardID(-1) {}

    CreatureTemplate(uint32 p_Entry, int16 p_SkinID, std::string p_Name, uint8 p_Level, uint8 p_Force, uint8 p_Stamina, uint8 p_Dexterity, uint8 p_Speed, uint8 p_Xp, uint8 p_State, uint16 p_MaxRay , uint16 p_MaxVision, float p_MovingTimeMin, float p_MovingTimeMax, float p_StopTimeMin, float p_StopTimeMax, int32 p_RespawnTime, uint8 p_Rank, uint8 p_AiType, eFactionType p_FactionType, const int32 & p_RewardID) :
    m_Entry(p_Entry), m_SkinID(p_SkinID), m_Name(p_Name), m_Level(p_Level), m_Force(p_Force), m_Stamina(p_Stamina), m_Dexterity(p_Dexterity), m_Speed(p_Speed), m_Xp(p_Xp), m_State(p_State), m_MaxRay(p_MaxRay), m_MaxVision(p_MaxVision), m_MovingTimeMin(p_MovingTimeMin), m_MovingTimeMax(p_MovingTimeMax), m_StopTimeMin(p_StopTimeMin), m_StopTimeMax(p_StopTimeMax), m_RespawnTime(p_RespawnTime), m_Rank(p_Rank), m_AiType(p_AiType), m_FactionType(p_FactionType), m_RewardID(p_RewardID){}
};

struct SWTime
{
    SWTime()
    {
        m_Days = 0;
        m_Hours = 0;
        m_Minutes = 0;
        m_Seconds = 0;
    }

    long int m_TotalSeconds;
    uint16 m_Days;
    uint16 m_Hours;
    uint16 m_Minutes;
    uint16 m_Seconds;
};

struct Title
{
    std::string m_Name;
    uint16 m_ID;
    eTypeTitle m_Type;

    Title() :
        m_ID(0), m_Name(""), m_Type(eTypeTitle::TypeTitleBefore) {}

    Title(const uint16 & p_ID, const std::string & p_Name, const eTypeTitle & p_Type) :
        m_ID(p_ID), m_Name(p_Name), m_Type(p_Type){}
};
typedef std::map<uint16, Title> Titles;

struct Skin
{
    std::string m_Name;
    uint16 m_ID;

    Skin() :
        m_ID(0), m_Name("") {}

    Skin(const uint16 & p_ID, const std::string & p_Name) :
        m_ID(p_ID), m_Name(p_Name) {}
};
typedef std::map<uint16, Skin> Skins;

struct PointsSet
{
    std::map<eStats, uint16> m_Points;

	PointsSet()
    {
        for (uint8 i = 0; i < eStats::MaxStat; ++i)
            m_Points[(eStats)i] = 0;
    }

	void SetStat(eStats p_Stat, uint16 p_Points) 
    {
        if (p_Stat > eStats::MaxStat)
            return;
        m_Points[p_Stat] = p_Points;
    };

    uint16 GetStat(eStats p_Stat)
    {
        if (p_Stat > eStats::MaxStat)
            return 0;
        return m_Points[p_Stat];
    };
};

static uint32 PixelToCase(uint32 p_NbPixel) { return p_NbPixel / TILE_SIZE; }
static uint32 CaseToPixel(uint32 p_NbCase) { return p_NbCase * TILE_SIZE; }
static Position PositionToCasePosition(const Position & p_Pos) { return Position(PixelToCase(p_Pos.m_X), PixelToCase(p_Pos.m_Y)); }
static float InYard(float p_YardInPixel) { return p_YardInPixel / (float)TILE_SIZE; }
static bool RandChance(uint8 p_Rand) { return (rand() % 100) <= p_Rand ; }
static bool RandChance(float p_Rand) { return (((float)rand() / (float)(RAND_MAX)) * 100.0f) <= p_Rand; }
static void Log(const std::string & p_Str)
{
    time_t l_Time = time(NULL);
    //printf("local: %s", asctime(localtime(&l_Time)));

    struct tm * l_TimeInfo;
    char l_Format[32];

    time(&l_Time);
    l_TimeInfo = localtime(&l_Time);

    strftime(l_Format, 32, "%Y-%m-%d %H-%M", l_TimeInfo);
    printf("[%s] %s\n", l_Format, p_Str.c_str());
}
static time_t GetActualTime()
{
    time_t l_Time = time(NULL);

    return l_Time;
}
static SWTime ConvertTimeToSWTime(time_t p_Time)
{
    SWTime l_TimeInfo;
    long int l_Time = (int)p_Time;

    l_TimeInfo.m_TotalSeconds = l_Time;
    l_TimeInfo.m_Seconds = l_Time % 60;
    l_TimeInfo.m_Minutes = l_Time / 60;
    l_TimeInfo.m_Hours = l_TimeInfo.m_Minutes / 60;
    l_TimeInfo.m_Days = l_TimeInfo.m_Hours / 24;

    l_TimeInfo.m_Minutes -= (l_TimeInfo.m_Hours * 60);
    l_TimeInfo.m_Hours -= (l_TimeInfo.m_Days * 24);

    return l_TimeInfo;
}
static struct tm* ConvertTimeToTM(time_t p_Time)
{
    struct tm* l_TimeInfo;
    char l_Format[32];

    time(&p_Time);
    l_TimeInfo = localtime(&p_Time);

    return l_TimeInfo;
}
static bool replaceStr(std::string& str, const std::string& from, const std::string& to) 
{
    size_t start_pos = str.find(from);
    if (start_pos == std::string::npos)
        return false;
    str.replace(start_pos, from.length(), to);
    return true;
}