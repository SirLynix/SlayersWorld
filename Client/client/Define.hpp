#pragma once

#include <cstdio>

#define IP_SERVER "51.255.165.231"
#define CONF_FILE "config.wtf"
//#define IP_SERVER "127.0.0.1"
#define PORT_SERVER 1234

/// GRAPHICS
#define X_WINDOW 640
#define Y_WINDOW 480
#define ZOOM_FACTOR 0.5f

/// TILES
#define MAX_TILE_LEVEL 6
#define TILE_SIZE 16
#define TILE_FOLDER "img/tiles/"
#define GRID_SIZE 12

/// SKIN
#define SKIN_ZOOM_FACTOR_DEFAULT 1.0f
#define UPDATE_OPACITY_TIME 250
#define SKINS_FOLDER "img/skins/"
#define SKIN_SIZE_X 24
#define SKIN_SIZE_Y 32
#define MAX_MOVEMENT_POSITION 3
#define UPDATE_TIME_MOVEMENT 200
#define DEATH_OPACITY 100
#define MAX_OPACITY 255

/// SPELL
#define SPELLS_FOLDER "img/spells/"
#define MAX_VISUAL_IMG_X 3 

/// UNIT
#define TALK_TIME_SHOW 5000

/// CHATBAR
#define MAX_LETTER_STR 30

/// FONT
#define FONT_FOLDER "font/"
#define SIZE_NAME_FONT 20
#define SIZE_TALK_FONT 20

/// INTERFACE
#define IMG_FOLDER "img/window/"
#define FONT_FIELD_SIZE 128
#define FONT_FIELD_BORDER_SIZE 64
#define SIZE_FILED_TALK_Y 24
#define FLASK_SIZE_X 16
#define FLASK_SIZE_Y 72
#define FLASK_OFFSET_TOP 22
#define FLASK_OFFSET_BOTTOM 2
#define FLASK_SCALE 2.0f

#define XP_BAR_SIZE_X 320
#define XP_BAR_SIZE_Y 12
#define XP_BAR_SCALE 2.0f

#define MIN_HISTORY_LINE 5
#define MAX_HISTORY_LINE 19

#define MAX_HISTORY_LOG_TIME 2000000
#define MAX_WARNING_LOG_TIME 5000000
#define MAX_WARNING_LOG_TIME_BETWEEN_SAME 2000000

/// EVENT
#define MAX_KEY_SAVE 4

/// MAP
#define MAP_FOLDER "map/"
#define NAME_WINDOW "Slayers World"

/// GENERAL
#define STEP_SIZE 12 /// In pixel
#define IN_MILLISECOND 1000

typedef char int8;
typedef short int16;
typedef int int32;
typedef long long int64;
typedef unsigned long long uint64;

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned long long uint64;

enum Orientation
{
    Up      = 0,
    Right   = 1,
    Down    = 2,
    Left    = 3,
    MAX     = 4
};

enum TypeWorldObject
{
    UNIT = 0,
    CASE = 1
};

enum TypeUnit
{
    CREATURE = 0,
    PLAYER = 1
};

enum eTypeWarningMsg
{
    Red = 0,
    Yellow
};

struct Position
{
    uint32 x;
    uint32 y;
};

enum DamageResult
{
    Success = 0,
    Miss = 1
};

struct DamageInfo
{
	int8 m_Damage;
	DamageResult m_Result;

	DamageInfo() :
		m_Damage(0), m_Result(DamageResult::Success) {}

	DamageInfo(int8 p_Damage, DamageResult p_Result) :
		m_Damage(p_Damage), m_Result(p_Result) {}

	DamageInfo(const DamageInfo & p_DamageInfo) :
		m_Damage(p_DamageInfo.m_Damage), m_Result(p_DamageInfo.m_Result) {}
};

enum eVisualType
{
    VisualSkin = 0,
    VisualSpell = 1
};

enum eTimeSpellAttribute
{
    CooldownAttribute = 0,
    CastimeAttribute = 1
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

enum eActionType
{
    Go = 0,
    Attack = 1,
    Stop = 2,
    StopAttack = 4
};

enum eResourceType
{
    None = 0,
    Health = 1,
    Mana = 2,
    Alignment = 3
};

enum eWarningMsg
{
   NotEnoughMana = 0,
   InCooldown
};
