BEGIN TRANSACTION;
DROP TABLE IF EXISTS "creature";
CREATE TABLE "creature" (
	"id"	INTEGER NOT NULL,
	"mapID"	smallint(6) NOT NULL DEFAULT '0',
	"entry"	int(11) NOT NULL DEFAULT '0',
	"posX"	int(11) NOT NULL DEFAULT '0',
	"posY"	int(11) NOT NULL DEFAULT '0',
	PRIMARY KEY("id")
);
DROP TABLE IF EXISTS "creature_template";
CREATE TABLE "creature_template" (
	"entry"	INTEGER NOT NULL,
	"skinID"	tinyint(4) NOT NULL DEFAULT '0',
	"name"	varchar(50) DEFAULT '',
	"level"	tinyint(4) NOT NULL DEFAULT '0',
	"force"	tinyint(4) NOT NULL DEFAULT '0',
	"stamina"	tinyint(4) NOT NULL DEFAULT '0',
	"dexterity"	tinyint(4) NOT NULL DEFAULT '0',
	"xp"	tinyint(4) NOT NULL DEFAULT '0',
	"state"	tinyint(4) NOT NULL DEFAULT '0',
	"maxRay"	mediumint(9) NOT NULL DEFAULT '5',
	"maxVision"	mediumint(9) NOT NULL DEFAULT '3',
	"movingTimeMin"	float NOT NULL DEFAULT '1',
	"movingTimeMax"	float NOT NULL DEFAULT '2',
	"stopTimeMin"	float NOT NULL DEFAULT '1',
	"stopTimeMax"	float NOT NULL DEFAULT '2',
	"respawnTime"	int(11) NOT NULL DEFAULT '5000',
	"rank"	tinyint(4) NOT NULL DEFAULT '0',
	"aiType"	tinyint(4) NOT NULL DEFAULT '0',
	"faction"	tinyint(4) NOT NULL DEFAULT '0',
	PRIMARY KEY("entry")
);
DROP TABLE IF EXISTS "gossip";
CREATE TABLE "gossip" (
	"id"	INTEGER,
	"typeUnit"	tinyint(4) NOT NULL,
	"unitEntry"	int(11) NOT NULL,
	"type"	tinyint(4) NOT NULL,
	"data1"	int(11) DEFAULT NULL,
	"msg"	varchar(50) DEFAULT NULL,
	PRIMARY KEY("id")
);
DROP TABLE IF EXISTS "level_xp";
CREATE TABLE "level_xp" (
	"level"	tinyint(3) NOT NULL DEFAULT '0',
	"xp"	mediumint(8) NOT NULL DEFAULT '0'
);
DROP TABLE IF EXISTS "objectif_quest_template";
CREATE TABLE "objectif_quest_template" (
	"questID"	int(10) NOT NULL,
	"id"	tinyint(3) NOT NULL DEFAULT '0',
	"typeID"	tinyint(3) NOT NULL,
	"data0"	int(11) NOT NULL DEFAULT '-1',
	"data1"	int(11) NOT NULL DEFAULT '-1',
	"data2"	int(11) NOT NULL DEFAULT '-1',
	"data3"	int(11) NOT NULL DEFAULT '-1'
);
DROP TABLE IF EXISTS "quest_template";
CREATE TABLE "quest_template" (
	"id"	INTEGER,
	"repetitionType"	tinyint(3) NOT NULL DEFAULT '0',
	PRIMARY KEY("id")
);
DROP TABLE IF EXISTS "spell_effect";
CREATE TABLE "spell_effect" (
	"id"	INTEGER,
	"effectType"	tinyint(4) DEFAULT NULL,
	"target"	tinyint(3) NOT NULL,
	"basepoint1"	int(11) NOT NULL DEFAULT '0',
	"basepoint2"	int(11) NOT NULL DEFAULT '0',
	"basepoint3"	int(11) NOT NULL DEFAULT '0',
	"basepoint4"	int(11) NOT NULL DEFAULT '0',
	"radiusMin"	float DEFAULT '0',
	"radiusMax"	float DEFAULT '0',
	PRIMARY KEY("id")
);
DROP TABLE IF EXISTS "spell_template";
CREATE TABLE "spell_template" (
	"id"	INTEGER,
	"level"	tinyint(3) NOT NULL DEFAULT '0',
	"visualID"	int(10) NOT NULL DEFAULT '-1',
	"visualIDTarget"	int(11) NOT NULL DEFAULT '-1',
	"castTime"	int(10) NOT NULL DEFAULT '0',
	"cooldown"	int(10) NOT NULL DEFAULT '0',
	"speed"	float NOT NULL DEFAULT '0',
	"resourceType"	tinyint(4) NOT NULL DEFAULT '-1',
	"resourceNb"	int(11) NOT NULL DEFAULT '0',
	"effect1"	int(11) NOT NULL DEFAULT '-1',
	"effect2"	int(11) NOT NULL DEFAULT '-1',
	"effect3"	int(11) NOT NULL DEFAULT '-1',
	"effect4"	int(11) NOT NULL DEFAULT '-1',
	"name"	text,
	PRIMARY KEY("id")
);
COMMIT;
