BEGIN TRANSACTION;
DROP TABLE IF EXISTS "account_key_binds";
CREATE TABLE "account_key_binds" (
	"accountID"	int(11) NOT NULL,
	"typeID"	int(11) NOT NULL,
	"key"	int(11) NOT NULL
);
DROP TABLE IF EXISTS "characters";
CREATE TABLE "characters" (
	"accountID"	int(10) DEFAULT '0',
	"characterID"	INTEGER,
	"name"	tinytext NOT NULL,
	"skinID"	tinyint(3) NOT NULL DEFAULT '0',
	"level"	tinyint(3) NOT NULL DEFAULT '0',
	"health"	tinyint(3) NOT NULL DEFAULT '100',
	"mana"	tinyint(3) NOT NULL DEFAULT '100',
	"alignment"	tinyint(3) NOT NULL DEFAULT '0',
	"mapID"	smallint(5) NOT NULL DEFAULT '0',
	"posX"	int(10) NOT NULL DEFAULT '0',
	"posY"	int(10) NOT NULL DEFAULT '0',
	"orientation"	smallint(5) NOT NULL DEFAULT '0',
	"xp"	int(10) NOT NULL DEFAULT '0',
	"creationDate"	datetime DEFAULT NULL,
	"lastConnection"	timestamp DEFAULT CURRENT_TIMESTAMP,
	PRIMARY KEY("characterID")
);
DROP TABLE IF EXISTS "characters_point";
CREATE TABLE "characters_point" (
	"characterID"	int(10) NOT NULL,
	"free_point"	int(10) DEFAULT '0',
	"force"	int(10) DEFAULT '0',
	"stamina"	int(10) DEFAULT '0',
	"dexterity"	int(10) DEFAULT '0'
);
DROP TABLE IF EXISTS "characters_respawn";
CREATE TABLE "characters_respawn" (
	"characterID"	int(11) DEFAULT '300',
	"posX"	int(11) DEFAULT '300',
	"posY"	int(11) DEFAULT '300',
	"mapID"	smallint(6) DEFAULT '0',
	"orientation"	tinyint(4) DEFAULT '0',
	UNIQUE("characterID")
);
DROP TABLE IF EXISTS "characters_spell_binds";
CREATE TABLE "characters_spell_binds" (
	"characterID"	int(10) NOT NULL,
	"bindID"	int(10) NOT NULL,
	"spellID"	int(10) NOT NULL
);
DROP TABLE IF EXISTS "characters_spells";
CREATE TABLE "characters_spells" (
	"characterID"	int(10) NOT NULL,
	"spellID"	int(10) NOT NULL,
	"cooldown"	int(10) NOT NULL DEFAULT '0',
	"key"	int(11) NOT NULL DEFAULT '-1'
);
DROP TABLE IF EXISTS "connection_logs";
CREATE TABLE "connection_logs" (
	"accountID"	int(10) DEFAULT NULL,
	"connectionDate"	timestamp DEFAULT CURRENT_TIMESTAMP
);
DROP TABLE IF EXISTS "login";
CREATE TABLE "login" (
	"id"	INTEGER,
	"login"	varchar(50) NOT NULL,
	"password"	varchar(50) NOT NULL,
	"characterName"	tinytext NOT NULL,
	PRIMARY KEY("id")
);
DROP TABLE IF EXISTS "login_access";
CREATE TABLE "login_access" (
	"accountID"	int(10) NOT NULL,
	"accessType"	tinyint(3) NOT NULL
);
COMMIT;
