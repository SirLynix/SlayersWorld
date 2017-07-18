#include "InterfaceManager.hpp"
#include "../../Global.hpp"

InterfaceManager::InterfaceManager(Events* p_Events) :
    m_Events(p_Events),
    m_WritingField(new WritingField()),
    m_HistoryField(new HistoryField())
{
    m_Events->SetWritingField(m_WritingField);
    m_Events->SetHistoryField(m_HistoryField);

    m_SystemMsg.setCharacterSize(30);
    m_SystemMsg.setColor(sf::Color::White);

    m_ClockTxt.setCharacterSize(20);
    m_ClockTxt.setColor(sf::Color::Red);

    m_Align.setCharacterSize(20);
    m_Align.setColor(sf::Color::Black);

    InitializeWarningMsgs();
    m_IsLoading = false;
}

InterfaceManager::~InterfaceManager()
{
}

void InterfaceManager::InitializeWarningMsgs()
{
    m_WarningMsgsEnum[eWarningMsg::NotEnoughMana] = "Pas assez de mana";
    m_WarningMsgsEnum[eWarningMsg::InCooldown] = "Temps de recharge en cours";
}

void InterfaceManager::Initialize()
{
    std::string l_FileSystemName = "system.png";
    if (!m_SystemTexture.loadFromFile(IMG_FOLDER + l_FileSystemName))
        printf("Load SystemImg Failed\n");

    l_FileSystemName = "lifebars.png";
    if (!m_LifeBarTexture.loadFromFile(IMG_FOLDER + l_FileSystemName))
        printf("Load SystemImg LifeBar Failed\n");

    l_FileSystemName = "sepbar.png";
    if (!m_SepBarTexture.loadFromFile(IMG_FOLDER + l_FileSystemName))
        printf("Load SystemImg SepBar Failed\n");

    l_FileSystemName = "xp.png";
    if (!m_XpTexture.loadFromFile(IMG_FOLDER + l_FileSystemName))
        printf("Load SystemImg Xp Failed\n");

    l_FileSystemName = "castBar.png";
    if (!m_CastBarTexture.loadFromFile(IMG_FOLDER + l_FileSystemName))
        printf("Load SystemImg Cast Bar Failed\n");

    l_FileSystemName = "background.png";
    if (!m_Background.loadFromFile(IMG_FOLDER + l_FileSystemName))
        printf("Load Background Failed\n");

    l_FileSystemName = "bg0.png";
    if (!m_BGInterface.loadFromFile(IMG_FOLDER + l_FileSystemName))
        printf("Load Bg interface Failed\n");
}

void  InterfaceManager::ManageEvent(sf::Event p_Event)
{
    eKeyBoardAction l_KeyBoardAction = eKeyBoardAction::NoneAction;
    if (m_KeyBoardBind.find(p_Event.key.code) != m_KeyBoardBind.end())
        l_KeyBoardAction = (eKeyBoardAction)m_KeyBoardBind[p_Event.key.code];

    switch (p_Event.type)
    {
        case sf::Event::KeyPressed: ///< Key Press
            if (!m_ListSimpleQuestion.empty())
            {
                (*--m_ListSimpleQuestion.end()).second.KeyPress(p_Event.key.code);
                break;
            }
            else if (p_Event.key.code == sf::Keyboard::Escape && !m_MenuManager.IsOpen()) ///< Bypass for Menu
                    m_MenuManager.Open();
            else if (m_MenuManager.IsOpen())
            {
                m_MenuManager.KeyPress(p_Event.key.code);
                break;
            }

            if (IsBlockingBind(l_KeyBoardAction))
                AddWarningMsg(eTypeWarningMsg::Red, "Temps de recharge restant : " + std::to_string(uint8(m_BlockingBinds[l_KeyBoardAction] / 1000000)) + "s");
            else
                m_Events->NewKeyPressed(l_KeyBoardAction);
            break;
        case sf::Event::KeyReleased: ///< Key Release
            m_Events->KeyRelease(l_KeyBoardAction);
            break;
        case sf::Event::TextEntered: ///< Text Entered
            m_Events->TextEntered(p_Event.text.unicode);
            break;
        case sf::Event::LostFocus:
            m_Events->LostFocus();
        default:
            break;
    }
}

void InterfaceManager::DrawMenu(Window & p_Window, Menu * p_Menu)
{
    sf::Vector2i l_Size(0, 0);
    sf::Text    l_Title;
    if (p_Menu->GetTitle() != "")
    {
        l_Title.setCharacterSize(18);
        l_Title.setColor(sf::Color::White);
        l_Title.setString(p_Menu->GetTitle());
        l_Title.setFont(*g_Font);
        l_Size = TextSplitToFit((p_Menu->GetColumn() * MENU_COLUMN_SIZE) - 20, l_Title);
        l_Size.y += 10;
        l_Title.setPosition(p_Menu->GetPosition().x + 10, p_Menu->GetPosition().y + 10);
    }
    DrawField(p_Window, p_Menu->GetPosition().x, p_Menu->GetPosition().y, (p_Menu->GetColumn() * MENU_COLUMN_SIZE), p_Menu->GetRow() * MENU_ROW_SIZE + 20 + l_Size.y);
    std::map<uint8, std::map<uint8, MenuElement> >* m_Elements = p_Menu->GetElements();

    if (l_Title.getString() != "")
    {
        p_Window.draw(l_Title);
    }
    for (std::map<uint8, std::map<uint8, MenuElement> >::iterator l_It = m_Elements->begin(); l_It != m_Elements->end(); ++l_It)
    {
        for (std::map<uint8, MenuElement>::iterator l_Itr = (*l_It).second.begin(); l_Itr != (*l_It).second.end(); l_Itr++)
        {
            sf::Text    l_Label;
            l_Label.setCharacterSize(18);
            l_Label.setColor(sf::Color::White);
            l_Label.setString((*l_Itr).second.GetLabel());
            l_Label.setFont(*g_Font);

            l_Label.setPosition(p_Menu->GetPosition().x + (*l_It).first * MENU_COLUMN_SIZE + 10, p_Menu->GetPosition().y + l_Size.y + (*l_Itr).first * MENU_ROW_SIZE + 5);
            p_Window.draw(l_Label);
        }
    }
    std::pair<uint8, uint8> l_SelectedElement = p_Menu->GetSelectedElement();
    DrawBorderField(p_Window, p_Menu->GetPosition().x + l_SelectedElement.first * MENU_COLUMN_SIZE, p_Menu->GetPosition().y + l_Size.y + l_SelectedElement.second * MENU_ROW_SIZE + 5, MENU_COLUMN_SIZE, MENU_ROW_SIZE);
}

TileSprite InterfaceManager::GetField(uint16 p_SizeX, uint16 p_SizeY)
{
    TileSprite l_TileSprite;
    l_TileSprite.setTexture(m_SystemTexture);
    l_TileSprite.setTextureRect(sf::IntRect(0, 0, FONT_FIELD_SIZE, FONT_FIELD_SIZE));
    l_TileSprite.setScale((float)p_SizeX / FONT_FIELD_SIZE, (float)p_SizeY / FONT_FIELD_SIZE);
    return l_TileSprite;
}

void InterfaceManager::DrawField(Window & p_Window, const float & p_PosX, const float & p_PosY, uint16 p_SizeX, uint16 p_SizeY, bool p_WithBorder /*= true*/)
{
    TileSprite l_TileSprite;

    uint16 l_SizeX = p_SizeX;
    uint16 l_SizeY = p_SizeY;

    if (p_WithBorder)
    {
        l_SizeX -= 6;
        l_SizeY -= 6;
    }

    l_TileSprite.setTexture(m_SystemTexture);
    l_TileSprite.setTextureRect(sf::IntRect(0, 0, FONT_FIELD_SIZE, FONT_FIELD_SIZE));
    l_TileSprite.setScale((float)l_SizeX / FONT_FIELD_SIZE, (float)l_SizeY / FONT_FIELD_SIZE);

    if (p_WithBorder)
        l_TileSprite.setPosition(p_PosX + 3, p_PosY + 3);
    else
        l_TileSprite.setPosition(p_PosX, p_PosY);

    p_Window.draw(l_TileSprite);

    if (p_WithBorder)
        DrawBorderField(p_Window, p_PosX, p_PosY, p_SizeX, p_SizeY);
}

void InterfaceManager::DrawBorderField(Window & p_Window, const float & p_PosX, const float & p_PosY, uint16 p_SizeX, uint16 p_SizeY)
{
    /* CORNER TOP LEFT */
    TileSprite l_TileSprite;
    l_TileSprite.setTexture(m_SystemTexture);
    l_TileSprite.setTextureRect(sf::IntRect(FONT_FIELD_SIZE, 0, 10, 10));
    l_TileSprite.setPosition(p_PosX, p_PosY);
    p_Window.draw(l_TileSprite);

    /* TOP */
    l_TileSprite.setTextureRect(sf::IntRect(FONT_FIELD_SIZE + 10, 0, 10, 3));
    l_TileSprite.setPosition(p_PosX + 10, p_PosY);
    l_TileSprite.setScale((float)(p_SizeX - 20) / 10, 1.0f);
    p_Window.draw(l_TileSprite);

    /* CORNER TOP RIGHT */
    l_TileSprite.setTextureRect(sf::IntRect(FONT_FIELD_SIZE + 64 - 10, 0, 10, 10));
    l_TileSprite.setPosition(p_PosX + p_SizeX - 10, p_PosY);
    l_TileSprite.setScale(1.0f, 1.0f);
    p_Window.draw(l_TileSprite);

    /* RIGHT */
    l_TileSprite.setTextureRect(sf::IntRect(FONT_FIELD_SIZE + 64 - 3, 10, 3, 10));
    l_TileSprite.setPosition(p_PosX + p_SizeX - 3, p_PosY + 10);
    l_TileSprite.setScale(1.0f, (float)(p_SizeY - 20) / 10);
    p_Window.draw(l_TileSprite);

    /* CORNER BOTTOM RIGHT */
    l_TileSprite.setTextureRect(sf::IntRect(FONT_FIELD_SIZE + 64 - 10, 64 - 10, 10, 10));
    l_TileSprite.setPosition(p_PosX + p_SizeX - 10, p_PosY + p_SizeY - 10);
    l_TileSprite.setScale(1.0f, 1.0f);
    p_Window.draw(l_TileSprite);

    /* BOTTOM */
    l_TileSprite.setTextureRect(sf::IntRect(FONT_FIELD_SIZE + 10, 64 - 3, 10, 3));
    l_TileSprite.setPosition(p_PosX + 10, p_PosY + p_SizeY - 3);
    l_TileSprite.setScale((float)(p_SizeX - 20) / 10, 1.0f);
    p_Window.draw(l_TileSprite);

    /* CORNER BOTTOM LEFT */
    l_TileSprite.setTextureRect(sf::IntRect(FONT_FIELD_SIZE, 64 - 10, 10, 10));
    l_TileSprite.setPosition(p_PosX, p_PosY + p_SizeY - 10);
    l_TileSprite.setScale(1.0f, 1.0f);
    p_Window.draw(l_TileSprite);

    /* LEFT */
    l_TileSprite.setTextureRect(sf::IntRect(FONT_FIELD_SIZE, 10, 3, 10));
    l_TileSprite.setPosition(p_PosX, p_PosY + 10);
    l_TileSprite.setScale(1.0f, (float)(p_SizeY - 20) / 10);
    p_Window.draw(l_TileSprite);
}

TileSprite InterfaceManager::GetLifeBar(uint8 p_ID, uint8 p_Pct)
{
    TileSprite l_TileSprite;
    l_TileSprite.setTexture(m_LifeBarTexture);
    l_TileSprite.setTextureRect(sf::IntRect(0, p_ID * LIFE_BAR_Y, ((float)LIFE_BAR_X / 100.0f) * p_Pct, LIFE_BAR_Y));
    return l_TileSprite;

    /*uint8 l_Full = 0;
    if (!p_Full)
        l_Full = 1;

    TileSprite l_TileSprite;
    l_TileSprite.setTexture(m_FlaskTexture);
    l_TileSprite.setTextureRect(sf::IntRect(FLASK_SIZE_X * ((p_ID * 2) + l_Full), 0, FLASK_SIZE_X, FLASK_SIZE_Y));
    return l_TileSprite;*/
}

TileSprite InterfaceManager::GetXpBar(bool p_Full, uint8 p_Pct)
{
    uint8 l_Full = 0;
    if (p_Full)
        l_Full = 1;

    TileSprite l_TileSprite;
    l_TileSprite.setTexture(m_XpTexture);
    l_TileSprite.setTextureRect(sf::IntRect(0, (m_XpTexture.getSize().y / 2) * l_Full, m_XpTexture.getSize().x, m_XpTexture.getSize().y / 2));
    return l_TileSprite;
}

TileSprite InterfaceManager::GetCastBar(bool p_Full, uint8 p_Pct)
{
    uint8 l_Full = 0;
    if (p_Full)
        l_Full = 1;

    TileSprite l_TileSprite;
    l_TileSprite.setTexture(m_CastBarTexture);
    l_TileSprite.setTextureRect(sf::IntRect(0, (m_CastBarTexture.getSize().y / 2) * l_Full, m_CastBarTexture.getSize().x, m_CastBarTexture.getSize().y / 2));
    return l_TileSprite;
}

HistoryField* InterfaceManager::GetHistoryField() const
{
    return m_HistoryField;
}

void InterfaceManager::Update(sf::Time p_Diff)
{
    m_HistoryField->Update(p_Diff);

    for (std::map< eTypeWarningMsg, std::vector< std::pair<std::string, uint32> > >::iterator l_It = m_WarningMsgs.begin(); l_It != m_WarningMsgs.end(); ++l_It)
    {
        for (std::vector<std::pair<std::string, uint32>>::iterator l_Itr = (*l_It).second.begin(); l_Itr != (*l_It).second.end();)
        {
            if ((*l_Itr).second <= (p_Diff.asMicroseconds()))
                l_Itr = (*l_It).second.erase(l_Itr);
            else
            {
                (*l_Itr).second -= (p_Diff.asMicroseconds());
                ++l_Itr;
            }
        }
    }

    for (std::map< uint8, uint64 >::iterator l_It = m_BlockingBinds.begin(); l_It != m_BlockingBinds.end();)
    {
        if ((*l_It).second == 0)
        {
            l_It = m_BlockingBinds.erase(l_It);
            continue;
        }

        if ((*l_It).second < p_Diff.asMicroseconds())
        {
            l_It = m_BlockingBinds.erase(l_It);
            continue;
        }

        //printf("-> %d:%d\n", (*l_It).first, (*l_It).second);
        (*l_It).second -= p_Diff.asMicroseconds();
        ++l_It;
    }
    /*m_ClockTxt.setString(std::to_string(1000 / p_Diff.asMilliseconds()));*/
}

void InterfaceManager::DrawStartingPage(Window & p_Window)
{
    TileSprite l_Background;
    l_Background.setTexture(m_Background);
    l_Background.setPosition(0, 0);
    p_Window.draw(l_Background);

    if (m_SystemMsg.getString() != "")
    {
        DrawField(p_Window, (X_WINDOW / 2) - ((m_SystemMsg.getGlobalBounds().width + 8) / 2), (Y_WINDOW / 2) - ((g_Font->getLineSpacing(m_SystemMsg.getCharacterSize()) + 8) / 2), m_SystemMsg.getGlobalBounds().width + 8, (float)g_Font->getLineSpacing(m_SystemMsg.getCharacterSize()) + 8);

        m_SystemMsg.setPosition((X_WINDOW / 2) - ((m_SystemMsg.getGlobalBounds().width) / 2), (Y_WINDOW / 2) - ((g_Font->getLineSpacing(m_SystemMsg.getCharacterSize())) / 2));
        m_SystemMsg.setFont(*g_Font);
        p_Window.draw(m_SystemMsg);
    }
}

void InterfaceManager::DrawExtraUI(Window & p_Window)
{
    for (uint8 i = 0; i < m_ExtraUI.size(); i++)
    {
        switch (m_ExtraUI[i])
        {
            case eExtraInterface::eBattelGroundUI :
            {
                TileSprite l_BGUI;
                l_BGUI.setTexture(m_BGInterface);
                l_BGUI.setScale(1.5f, 1.5f);
                l_BGUI.setPosition((X_WINDOW / 2) - (l_BGUI.getGlobalBounds().width / 2), 0);
                p_Window.draw(l_BGUI);
                break;
            }
            default :
                break;
        }
    }
}

void InterfaceManager::DrawClock(Window & p_Window)
{
    m_ClockTxt.setFont(*g_Font);
    m_ClockTxt.setPosition(0.0f, 0.0f);
    p_Window.draw(m_ClockTxt);
}

void InterfaceManager::DrawAlign(Window & p_Window)
{
    m_Align.setString("Align: " + std::to_string(g_Player->GetResourceNb(eResourceType::Alignment)));
    m_Align.setFont(*g_Font);
    m_Align.setPosition(X_WINDOW - 140.0f, 20.0f);
    p_Window.draw(m_Align);
}

void InterfaceManager::DrawWarnings(Window & p_Window)
{
    for (std::map< eTypeWarningMsg, std::vector< std::pair<std::string, uint32> > >::iterator l_It = m_WarningMsgs.begin(); l_It != m_WarningMsgs.end(); ++l_It)
    {
        for (uint8 i = 0; i < (*l_It).second.size(); ++i)
        {
            sf::Text    l_WarningMsg;
            l_WarningMsg.setCharacterSize(22);
            if ((*l_It).first == eTypeWarningMsg::Red)
                l_WarningMsg.setColor(sf::Color(255, 66, 66, 255));
            else
                l_WarningMsg.setColor(sf::Color(44, 45, 86, 255));
            l_WarningMsg.setString((*l_It).second[i].first);
            l_WarningMsg.setFont(*g_Font);

            l_WarningMsg.setPosition((X_WINDOW / 2) - ((l_WarningMsg.getGlobalBounds().width) / 2), (Y_WINDOW / 2) - 50 - ((g_Font->getLineSpacing(l_WarningMsg.getCharacterSize())) / 2) + (g_Font->getLineSpacing(l_WarningMsg.getCharacterSize()) * i));
            p_Window.draw(l_WarningMsg);
        }
    }
}

void InterfaceManager::Draw(Window & p_Window)
{
    if (m_Events == nullptr)
        return;

    /// Draw Starting
    if (g_Player == nullptr/* || IsLoading()*/)
    {
        DrawStartingPage(p_Window);
        return;
    }

    /// Health Bars
    TileSprite l_SepBarLife;
    l_SepBarLife.setTexture(m_SepBarTexture);
    l_SepBarLife.setPosition(0, 0);
    l_SepBarLife.setScale(FLASK_SCALE, FLASK_SCALE);
    p_Window.draw(l_SepBarLife);

    TileSprite l_ManaBar = GetLifeBar(0, g_Player->GetResourceNb(eResourceType::Mana));
    l_ManaBar.setPosition(16, 14);
    l_ManaBar.setScale(FLASK_SCALE, FLASK_SCALE);
    p_Window.draw(l_ManaBar);

    TileSprite l_LifeBar = GetLifeBar(1, g_Player->GetResourceNb(eResourceType::Health));
    l_LifeBar.setPosition(16, 30);
    l_LifeBar.setScale(FLASK_SCALE, FLASK_SCALE);
    p_Window.draw(l_LifeBar);
    /// Draw Flask Life
   /* TileSprite l_FlaskEmpty = GetFlask(0, false);
    l_FlaskEmpty.setPosition(0, Y_WINDOW - (FLASK_SIZE_Y * FLASK_SCALE) - (m_XpTexture.getSize().y / 2 * XP_BAR_SCALE));
    l_FlaskEmpty.setScale(FLASK_SCALE, FLASK_SCALE);
    p_Window.draw(l_FlaskEmpty);
    TileSprite l_Flask = GetFlask(0, true);
    l_Flask.setTextureRect(sf::IntRect(0, ((FLASK_SIZE_Y - FLASK_OFFSET_TOP - FLASK_OFFSET_BOTTOM) * (1.0f - ((float)g_Player->GetResourceNb(eResourceType::Health) / 100.0f))) + FLASK_OFFSET_TOP, FLASK_SIZE_X, ((FLASK_SIZE_Y - FLASK_OFFSET_TOP - FLASK_OFFSET_BOTTOM) * ((float)g_Player->GetResourceNb(eResourceType::Health) / 100.0f))));
    l_Flask.setPosition(0, Y_WINDOW - (m_XpTexture.getSize().y / 2 * XP_BAR_SCALE) - (FLASK_OFFSET_BOTTOM * FLASK_SCALE) - (((FLASK_SIZE_Y - FLASK_OFFSET_TOP - FLASK_OFFSET_BOTTOM) * ((float)g_Player->GetResourceNb(eResourceType::Health) / 100.0f)) * FLASK_SCALE));
    l_Flask.setScale(FLASK_SCALE, FLASK_SCALE);
    p_Window.draw(l_Flask);

    /// Draw Flask Mana
    l_FlaskEmpty = GetFlask(1, false);
    l_FlaskEmpty.setPosition(X_WINDOW - (FLASK_SIZE_X * FLASK_SCALE), Y_WINDOW - (FLASK_SIZE_Y * FLASK_SCALE) - (m_XpTexture.getSize().y / 2 * XP_BAR_SCALE));
    l_FlaskEmpty.setScale(FLASK_SCALE, FLASK_SCALE);
    p_Window.draw(l_FlaskEmpty);
    l_Flask = GetFlask(1, true);
    l_Flask.setTextureRect(sf::IntRect(FLASK_SIZE_X * 2, ((FLASK_SIZE_Y - FLASK_OFFSET_TOP - FLASK_OFFSET_BOTTOM) * (1.0f - ((float)g_Player->GetResourceNb(eResourceType::Mana) / 100.0f))) + FLASK_OFFSET_TOP, FLASK_SIZE_X, ((FLASK_SIZE_Y - FLASK_OFFSET_TOP - FLASK_OFFSET_BOTTOM) * ((float)g_Player->GetResourceNb(eResourceType::Mana) / 100.0f))));
    l_Flask.setPosition(X_WINDOW - (FLASK_SIZE_X * FLASK_SCALE), Y_WINDOW - (m_XpTexture.getSize().y / 2 * XP_BAR_SCALE) - (FLASK_OFFSET_BOTTOM * FLASK_SCALE) - (((FLASK_SIZE_Y - FLASK_OFFSET_TOP - FLASK_OFFSET_BOTTOM) * ((float)g_Player->GetResourceNb(eResourceType::Mana) / 100.0f)) * FLASK_SCALE));
    l_Flask.setScale(FLASK_SCALE, FLASK_SCALE);
    p_Window.draw(l_Flask);*/

    /// Draw XP bar
    TileSprite l_XpBarEmpty = GetXpBar(false);
    l_XpBarEmpty.setPosition(0, Y_WINDOW - (m_XpTexture.getSize().y / 2 * XP_BAR_SCALE));
    l_XpBarEmpty.setScale(XP_BAR_SCALE, XP_BAR_SCALE);
    p_Window.draw(l_XpBarEmpty);
    TileSprite l_XpBar = GetXpBar(true);
    l_XpBar.setTextureRect(sf::IntRect(0, m_XpTexture.getSize().y / 2, (m_XpTexture.getSize().x / 100.0f) * g_Player->GetXpPct(), m_XpTexture.getSize().y / 2));
    l_XpBar.setPosition(0, Y_WINDOW - (m_XpTexture.getSize().y / 2 * XP_BAR_SCALE));
    l_XpBar.setScale(XP_BAR_SCALE, XP_BAR_SCALE);
    p_Window.draw(l_XpBar);
    
    /// Draw CAST Bar
    uint8 l_CastPct = g_Player->GetCastPct();
    if (l_CastPct)
    {
        TileSprite l_CastBarEmpty = GetCastBar(false);
        l_CastBarEmpty.setPosition((X_WINDOW / 2) - (m_CastBarTexture.getSize().x / 2 * XP_BAR_SCALE), Y_WINDOW - 180);
        l_CastBarEmpty.setScale(XP_BAR_SCALE, XP_BAR_SCALE);
        p_Window.draw(l_CastBarEmpty);
        TileSprite l_CastBar = GetCastBar(true);
        l_CastBar.setTextureRect(sf::IntRect(0, m_CastBarTexture.getSize().y / 2, (m_CastBarTexture.getSize().x / 100.0f) * l_CastPct, m_CastBarTexture.getSize().y / 2));
        l_CastBar.setPosition((X_WINDOW / 2) - (m_CastBarTexture.getSize().x / 2 * XP_BAR_SCALE), Y_WINDOW - 180);
        l_CastBar.setScale(XP_BAR_SCALE, XP_BAR_SCALE);
        p_Window.draw(l_CastBar);
    }

    /// Draw chat bar
    if (m_WritingField->IsFieldOpen())
    {
        DrawField(p_Window, 0, Y_WINDOW - SIZE_FILED_TALK_Y, X_WINDOW, SIZE_FILED_TALK_Y);
        p_Window.draw(m_WritingField->GetText());
    }
    DrawAlign(p_Window);

    /// Draw Top Text
    if (!m_TopMessages.empty())
    {
        if (!m_Events->TopIsOpen())
        {
            RemoveFirstTopMsg();
            if (!m_TopMessages.empty())
                m_Events->SetTopIsOpen(true);
        }
        if (!m_TopMessages.empty())
        {
            sf::Text l_Text(m_TopMessages[0], *g_Font, SIZE_TALK_FONT);
            sf::Vector2i l_FieldSize = TextSplitToFit(X_WINDOW, l_Text);

            DrawField(p_Window, 0, 0, X_WINDOW, l_FieldSize.y + 14);
            p_Window.draw(m_WritingField->GetText());

            l_Text.setColor(sf::Color::White);
            l_Text.setPosition(0.0f, 0.0f);
            p_Window.draw(l_Text);
        }
    }
    /// Draw history
    if (m_HistoryField->IsFieldOpen())
    {
        DrawField(p_Window, FLASK_SIZE_X * FLASK_SCALE, Y_WINDOW - SIZE_FILED_TALK_Y - (SIZE_FILED_TALK_Y * m_HistoryField->GetLineHistory()), X_WINDOW - (FLASK_SIZE_X * 2 * FLASK_SCALE), SIZE_FILED_TALK_Y * m_HistoryField->GetLineHistory());
        
        uint8 l_HistorySize = m_HistoryField->GetHistory().size();
        uint8 m_LineMax = std::min(m_HistoryField->GetLineHistory(), l_HistorySize);
        for (uint8 i = 0; i < m_LineMax; ++i)
        {
            sf::Text l_Text = m_HistoryField->GetText();
            l_Text.setFont(*g_Font);
            l_Text.setPosition(FLASK_SIZE_X * FLASK_SCALE + 10, Y_WINDOW - SIZE_FILED_TALK_Y - (SIZE_FILED_TALK_Y * (i + 1)));
            l_Text.setString(m_HistoryField->GetHistory()[l_HistorySize - 1- i]);
            p_Window.draw(l_Text);
        }
        //p_Window.draw(m_WritingField->GetText());
    }
    DrawWarnings(p_Window);
    DrawExtraUI(p_Window);
    //DrawClock(p_Window);

    std::vector<Menu*> l_ListOpenMenu = m_MenuManager.GetOpenMenus();
    for (std::vector<Menu*>::iterator l_It = l_ListOpenMenu.begin(); l_It != l_ListOpenMenu.end(); ++l_It)
        DrawMenu(p_Window, (*l_It));
    for (std::map<uint16, QuestionBox>::iterator l_It = m_ListSimpleQuestion.begin(); l_It != m_ListSimpleQuestion.end();)
    {
        if (!(*l_It).second.IsOpen())
            l_It = m_ListSimpleQuestion.erase(l_It);
        else
        {
            DrawMenu(p_Window, &(*l_It).second);
            ++l_It;
        }
    }
}

void InterfaceManager::AddWarningMsg(eTypeWarningMsg p_Type, const std::string & p_Msg)
{
    m_WarningMsgs[p_Type].push_back(std::pair<std::string, uint32>(p_Msg, MAX_WARNING_LOG_TIME));
}

void InterfaceManager::AddTopMsg(const std::string & p_Msg)
{
    if (!m_Events->TopIsOpen())
        m_Events->SetTopIsOpen(true);
    m_TopMessages.push_back(p_Msg);
}

void InterfaceManager::RemoveFirstTopMsg()
{
    if (m_TopMessages.begin() != m_TopMessages.end())
        m_TopMessages.erase(m_TopMessages.begin());
}

void InterfaceManager::AddWarningMsg(eTypeWarningMsg p_Type, eWarningMsg p_WarningMsg)
{
    if (m_WarningMsgsEnum.find(p_WarningMsg) == m_WarningMsgsEnum.end())
        return;

    if (m_WarningMsgs.size() && (m_WarningMsgs[p_Type][m_WarningMsgs.size() - 1].first == m_WarningMsgsEnum[p_WarningMsg]) && m_WarningMsgs[p_Type][m_WarningMsgs.size() - 1].second >= MAX_WARNING_LOG_TIME_BETWEEN_SAME)
        return;

    m_WarningMsgs[p_Type].push_back(std::pair<std::string, uint32>(m_WarningMsgsEnum[p_WarningMsg], MAX_WARNING_LOG_TIME));
}

void InterfaceManager::SetSystemMsg(const std::string & p_Msg)
{
    m_SystemMsg.setString(p_Msg);
}

void InterfaceManager::AddKeyBind(uint8 p_Key, uint8 p_Bind)
{
    m_KeyBoardBind[p_Key] = p_Bind;
}

int16 InterfaceManager::GetBindForKey(uint8 p_Key)
{
    if (m_KeyBoardBind.find(p_Key) == m_KeyBoardBind.end())
        return -1;
    return m_KeyBoardBind[p_Key];
}

void InterfaceManager::AddBlockingBind(uint8 p_BindType, uint16 p_Time)
{
    m_BlockingBinds[p_BindType] = (uint64)p_Time * 1000;
}

bool InterfaceManager::IsLoading() const
{
    return m_IsLoading;
}

void InterfaceManager::SetIsLoading(bool p_IsLoading)
{
    m_IsLoading = p_IsLoading;
    SetSystemMsg("Loading...");
}

bool InterfaceManager::IsBlockingBind(uint8 p_BindType)
{
    if (m_BlockingBinds.find(p_BindType) == m_BlockingBinds.end())
        return false;
    return true;
}

sf::Vector2i InterfaceManager::TextSplitToFit(uint16 p_MaxSizeX, sf::Text & p_Txt)
{
    sf::Vector2i l_FinalSquare;
    l_FinalSquare.x = 0;
    l_FinalSquare.y = 0;

    sf::Text l_TxtSpliter = p_Txt;
    std::string l_ActualString = p_Txt.getString();
    std::string l_FinalString = "";
    std::string l_StringSplitter = "";
    l_TxtSpliter.setString(l_StringSplitter);
    for (uint16 i = 0; i < l_ActualString.size(); ++i)
    {
        if (l_TxtSpliter.getGlobalBounds().width > (float)p_MaxSizeX)
        {
            l_FinalString += "\n";
            l_StringSplitter = "";
        }
        l_FinalString += l_ActualString[i];
        l_StringSplitter += l_ActualString[i];
        l_TxtSpliter.setString(l_StringSplitter);
    }
    p_Txt.setString(l_FinalString);
    l_FinalSquare.x = (uint16)p_Txt.getGlobalBounds().width;
    l_FinalSquare.y = (uint16)p_Txt.getGlobalBounds().height;

    return l_FinalSquare;
}

MenuManager* InterfaceManager::GetMenuManager()
{
    return &m_MenuManager;
}


void InterfaceManager::AddExtraInterface(eExtraInterface p_ExtraUI)
{
    std::vector<eExtraInterface>::iterator l_It = std::find(m_ExtraUI.begin(), m_ExtraUI.end(), p_ExtraUI);
    if (l_It != m_ExtraUI.end())
        return;

    m_ExtraUI.push_back(p_ExtraUI);
}

void InterfaceManager::RemoveExtraInterface(eExtraInterface p_ExtraUI)
{
    std::vector<eExtraInterface>::iterator l_It = std::find(m_ExtraUI.begin(), m_ExtraUI.end(), p_ExtraUI);
    if (l_It == m_ExtraUI.end())
        return;

    m_ExtraUI.erase(l_It);
}

void InterfaceManager::AddSimpleQuestion(const uint16 & p_GossipID, const std::string & p_Msg)
{
    std::map<uint16, QuestionBox>::iterator l_It = m_ListSimpleQuestion.find(p_GossipID);
    if (l_It != m_ListSimpleQuestion.end())
        return;

    m_ListSimpleQuestion[p_GossipID] = QuestionBox(p_GossipID, p_Msg);
    m_ListSimpleQuestion[p_GossipID].Open();
}
