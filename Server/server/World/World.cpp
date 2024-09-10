#include "World.hpp"
#include "WorldPacket.hpp"
#include "../System/WebHook.hpp"

World::World()
    : m_Thread(&World::NetworkLoop, this),
    m_Run(true),
    m_PacketHandler(new PacketHandler()),
    m_CreatureManager(new UnitManager()),
    m_RequiredManager(new RequiredManager()),
    m_DynamicObjectManager(new DynamicObjectManager())
{
    g_MapManager    = new MapManager();
	g_Config        = new ConfigHandler();
    g_RewardManager = new RewardManager();
    g_SqlManager    = new SqlManager();
    g_LevelManager  = new LevelManager();
    g_SpellManager  = new SpellManager();
    g_QuestManager  = new QuestManager();
    g_ItemManager   = new ItemManager();
    g_GroupManager  = new GroupManager();
}


World::~World()
{
}

bool World::Initialize()
{
	printf("Load Config...\n");
	if (!g_Config->Initialize())
	{
		printf("Config error\n");
		return false;
	}
    //std::string l_Msg = "Initialisation du serveur " + g_Config->GetValue("ServerName").c_str() + "...";
    WebHook::sendMsg(g_Config->GetValue("WebhookUrl"), "Initialisation du serveur " + g_Config->GetValue("ServerName") + "...");

	printf("Load Maps...\n");
	if (!g_MapManager->InitializeMapsTemplate())
	{
		printf("Load Maps Error\n");
		return false;
	}
    printf("Connection SQL...\n");

    std::string l_ConfigSQLWorld = g_Config->GetValue("worldDB");
    if (!g_SqlManager->InitializeWorld(l_ConfigSQLWorld))
        printf("Error connection world SQL...\n");

    printf("Initialize MapsTemplate\n");
    if (!g_SqlManager->InitializeMaps())
        printf("Error Initialize Maps...\n");

    printf("Charging MapsTemplate...\n");
    if (!g_MapManager->InitializeMapsTemplate())
        printf("Error charging Map Template...\n");

    printf("Initialize XpLevel\n");
    if (!g_LevelManager->Initialize())
        printf("Error Initialize XpLevel...\n");

    printf("Initialize Required\n");
    if (!g_SqlManager->InitializeRequired(m_RequiredManager))
        printf("Error Initialize Required...\n");

    printf("Initialize Gossip\n");
    if (!g_SqlManager->InitializeGossip(m_CreatureManager, m_RequiredManager))
        printf("Error Initialize Gossip...\n");

    printf("Initialize Areatrigger\n");
    if (!g_SqlManager->InitializeAreatrigger(m_DynamicObjectManager, m_CreatureManager))
        printf("Error Initialize Areatrigger...\n");

    printf("Initialize GameObject\n");
    if (!g_SqlManager->InitializeGameObject(m_DynamicObjectManager, m_RequiredManager, m_CreatureManager))
        printf("Error Initialize GameObject...\n");

    printf("Initialize Items\n");
    if (!g_SqlManager->InitializeItems(m_RequiredManager))
        printf("Error Initialize Items...\n");

    printf("Initialize SpellEffects\n");
    if (!g_SqlManager->InitializeSpellEffects())
        printf("Error Initialize SpellEffects...\n");

    printf("Initialize Quests\n");
    if (!g_SqlManager->InitializeQuests())
        printf("Error Initialize Quests...\n");

    printf("Initialize Titles\n");
    if (!g_SqlManager->InitializeTitles())
        printf("Error Initialize Titles...\n");

    printf("Initialize Skins\n");
    if (!g_SqlManager->InitializeSkins())
        printf("Error Initialize Skins...\n");

    printf("Initialize Zones\n");
    if (!g_SqlManager->InitializeZones())
        printf("Error Initialize Zones...\n");

    printf("Initialize Spells\n");
    if (!g_SqlManager->InitializeSpells())
        printf("Error Initialize Spells...\n");

    printf("Initialize CreatureTemplate\n");
    if (!g_SqlManager->InitializeCreatureTemplate(m_CreatureManager))
        printf("Error Initialize CreatureTemplate...\n");

    printf("Initialize Creature\n");
    if (!g_SqlManager->InitializeCreature(m_CreatureManager))
        printf("Error Initialize CreatureTemplate...\n");

    printf("Initialize AnimationUnitTemplate\n");
    if (!g_SqlManager->InitializeAnimationUnitTemplate(m_CreatureManager))
        printf("Error Initialize AnimationUnitTemplate...\n");

    printf("Initialize AnimationUnit\n");
    if (!g_SqlManager->InitializeAnimationUnit(m_CreatureManager))
        printf("Error Initialize AnimationUnit...\n");

    printf("Initialize Battledrounds\n");
    if (!g_SqlManager->InitializeBattlegrounds())
        printf("Error Initialize Battledrounds...\n");

    printf("Initialize Rewards\n");
    if (!g_SqlManager->InitializeRewards(m_RequiredManager))
        printf("Error Initialize Rewards...\n");

    printf("Launch World Maps\n");
    if (!g_MapManager->LaunchWorldsMap())
        printf("Error Launching World Map...\n");

    std::string l_ConfigSQLCharacters = g_Config->GetValue("charactersDB");
    if (!g_SqlManager->InitializeCharacters(l_ConfigSQLCharacters))
        printf("Error connection character SQL...\n");

	printf("Launch Network...\n");
	if (!this->NetworkInitialize())
	{
		printf("Network error\n");
		return false;
	}

	printf("Load Packet Handler...\n");
	m_PacketHandler->LoadPacketHandlerMap();

	return true;
}

void World::Run()
{
	if (Initialize())
    {
        printf("====== WorldServer Start =====\n");
        WebHook::sendMsg(g_Config->GetValue("WebhookUrl"), "Server " + g_Config->GetValue("ServerName") + " start...");
    }
    else
        printf("WorldServer didn't start\n");

	m_Thread.launch();

	while (m_Run)
	{
		UpdatePacketQueue();
        sf::Time l_Diff = m_Clock.restart();
        g_MapManager->Update(l_Diff);
	}
}

void World::UpdatePacketQueue()
{
    m_MutexPacketQueue.lock();

	for (std::vector<std::pair<WorldSocket*, WorldPacket>>::iterator l_It = m_PaquetQueue.begin(); l_It != m_PaquetQueue.end();)
	{
        m_PacketHandler->OperatePacket((*l_It).second, (*l_It).first);
		l_It = m_PaquetQueue.erase(l_It);
	}
    for (std::vector<WorldSocket*>::iterator l_It = m_DisconnectedQueue.begin(); l_It != m_DisconnectedQueue.end();)
    {
        m_PacketHandler->HandleDisconnected(*l_It);
        if ((*l_It)->GetPlayer() != nullptr)
            printf("Disconnection : %s\n", (*l_It)->GetPlayer()->GetName().c_str());
        delete (*l_It);
        (*l_It) = nullptr;
        l_It = m_DisconnectedQueue.erase(l_It);
    }

    m_MutexPacketQueue.unlock();
}

bool World::NetworkInitialize()
{
    int l_Port = std::stoi(g_Config->GetValue("Port"));
    sf::Socket::Status l_ErrorListen;
    bool l_PortLisening = false;

    while (!l_PortLisening)
    {
        l_ErrorListen = m_Listener.listen(l_Port);
        printf("Port: %d | ", l_Port);
        switch (l_ErrorListen)
        {
        case sf::Socket::Done:
            printf("OK\n");
            l_PortLisening = true;
            break;
        case sf::Socket::NotReady:
            printf("Not Ready\n");
            break;
        case sf::Socket::Partial:
            printf("Partial\n");
            break;
        case sf::Socket::Disconnected:
            printf("Disconnected\n");
            break;
        case sf::Socket::Error:
            printf("Error\n");
            break;
        default:
            break;
        }
    }
	m_Selector.add(m_Listener);
	return true;
}

void World::NetworkLoop()
{
	bool l_Continue = true;

	while (l_Continue)
	{
		if (m_Selector.wait())
		{
			if (m_Selector.isReady(m_Listener))
			{
				WorldSocket* l_NewWorldSocket = new WorldSocket();
				if (m_Listener.accept(*l_NewWorldSocket) == sf::Socket::Done)
				{
					printf("New connection\n");
					l_NewWorldSocket->setBlocking(false);

                    m_MutexSessions.lock();
					m_Sessions.push_back(l_NewWorldSocket);
                    m_MutexSessions.unlock();
					
                    m_Selector.add(*l_NewWorldSocket);
				}
				else
				{
					delete l_NewWorldSocket;
				}
			}
			else
			{
				/// Check if receive data
                m_MutexSessions.lock();

                bool l_IncIt = true;;
				for (std::vector<WorldSocket*>::iterator l_It = m_Sessions.begin(); l_It != m_Sessions.end();)
				{
					WorldSocket* l_Session = (*l_It);

                    if (l_Session->IsDisonnected())
                    {
                        m_Selector.remove(*l_Session);

                        m_MutexPacketQueue.lock();
                        m_DisconnectedQueue.push_back(l_Session);
                        m_MutexPacketQueue.unlock();
                        l_It = m_Sessions.erase(l_It);
                        l_IncIt = false;
                    }
					else if (m_Selector.isReady(*l_Session))
					{
						WorldPacket l_Packet;
						sf::Socket::Status l_SocketStatus;
						l_SocketStatus = l_Session->receive(l_Packet);
						if (l_SocketStatus == sf::Socket::Status::Done) ///< Reception OK
						{
							std::pair<WorldSocket*, WorldPacket> l_PaquetElement;

							l_PaquetElement.first = l_Session;
							l_PaquetElement.second = l_Packet;

                            m_MutexPacketQueue.lock();
							m_PaquetQueue.push_back(l_PaquetElement);
                            m_MutexPacketQueue.unlock();
						}
                        if (l_SocketStatus == sf::Socket::Status::Disconnected) ///< Disconnecetd
                        {
                            m_Selector.remove(*l_Session);

                            m_MutexPacketQueue.lock();
                            m_DisconnectedQueue.push_back(l_Session);
                            m_MutexPacketQueue.unlock();
                            l_It = m_Sessions.erase(l_It);
                            l_IncIt = false;
                        }
					}
                    if (l_IncIt)
                        ++l_It;
                    else
                        l_IncIt = true;
				}

                m_MutexSessions.unlock();
			}
		}
	}
}