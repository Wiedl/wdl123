/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#include "menus.h"

#include <base/dbg.h>
#include <base/math.h>
#include <base/str.h>

#include <engine/graphics.h>
#include <engine/shared/config.h>

#include <game/client/components/menu_background.h>
#include <game/client/gameclient.h>
#include <game/client/ui.h>
#include <game/client/ui_scrollregion.h>
#include <game/localization.h>

void CMenus::SetNeedSendInfo()
{
	if(m_Dummy)
		m_NeedSendDummyinfo = true;
	else
		m_NeedSendinfo = true;
}

void CMenus::RenderSettings(CUIRect MainView)
{
	// A previously saved settings page can become invalid when tabs are added
	// or removed. Clamp it before indexing the tab animator array or dispatching.
	if(g_Config.m_UiSettingsPage < SETTINGS_LANGUAGE || g_Config.m_UiSettingsPage >= SETTINGS_LENGTH)
		g_Config.m_UiSettingsPage = SETTINGS_LANGUAGE;

	// render background
	CUIRect Button, TabBar, RestartBar;
	MainView.VSplitRight(120.0f, &MainView, &TabBar);
	MainView.Draw(ms_ColorTabbarActive, IGraphics::CORNER_B, 10.0f);
	MainView.Margin(20.0f, &MainView);

	const bool NeedRestart = m_NeedRestartGraphics || m_NeedRestartSound || m_NeedRestartUpdate;
	if(NeedRestart)
	{
		MainView.HSplitBottom(20.0f, &MainView, &RestartBar);
		MainView.HSplitBottom(10.0f, &MainView, nullptr);
	}

	TabBar.HSplitTop(50.0f, &Button, &TabBar);
	Button.Draw(ms_ColorTabbarActive, IGraphics::CORNER_BR, 10.0f);

	const char *apTabs[SETTINGS_LENGTH] = {
		Localize("Language"),
		Localize("General"),
		Localize("Player"),
		Client()->IsSixup() ? "Tee 0.7" : Localize("Tee"),
		Localize("Appearance"),
		Localize("Controls"),
		Localize("Graphics"),
		Localize("Sound"),
		Localize("DDNet"),
		Localize("Assets"),
		TCLocalize("TClient"), // TClient
		Localize("Profiles"), // TClient
		"WCL"};
	static CButtonContainer s_aTabButtons[SETTINGS_LENGTH];

	for(int i = 0; i < SETTINGS_LENGTH; i++)
	{
		TabBar.HSplitTop(10.0f, nullptr, &TabBar);
		TabBar.HSplitTop(26.0f, &Button, &TabBar);
		if(DoButton_MenuTab(&s_aTabButtons[i], apTabs[i], g_Config.m_UiSettingsPage == i, &Button, IGraphics::CORNER_R, &m_aAnimatorsSettingsTab[i]))
			g_Config.m_UiSettingsPage = i;
	}

	if(g_Config.m_UiSettingsPage == SETTINGS_LANGUAGE)
	{
		GameClient()->m_MenuBackground.ChangePosition(CMenuBackground::POS_SETTINGS_LANGUAGE);
		RenderLanguageSettings(MainView);
	}
	else if(g_Config.m_UiSettingsPage == SETTINGS_GENERAL)
	{
		GameClient()->m_MenuBackground.ChangePosition(CMenuBackground::POS_SETTINGS_GENERAL);
		RenderSettingsGeneral(MainView);
	}
	else if(g_Config.m_UiSettingsPage == SETTINGS_PLAYER)
	{
		GameClient()->m_MenuBackground.ChangePosition(CMenuBackground::POS_SETTINGS_PLAYER);
		RenderSettingsPlayer(MainView);
	}
	else if(g_Config.m_UiSettingsPage == SETTINGS_TEE)
	{
		GameClient()->m_MenuBackground.ChangePosition(CMenuBackground::POS_SETTINGS_TEE);
		if(Client()->IsSixup())
			RenderSettingsTee7(MainView);
		else
			RenderSettingsTee(MainView);
	}
	else if(g_Config.m_UiSettingsPage == SETTINGS_APPEARANCE)
	{
		GameClient()->m_MenuBackground.ChangePosition(CMenuBackground::POS_SETTINGS_APPEARANCE);
		RenderSettingsAppearance(MainView);
	}
	else if(g_Config.m_UiSettingsPage == SETTINGS_CONTROLS)
	{
		GameClient()->m_MenuBackground.ChangePosition(CMenuBackground::POS_SETTINGS_CONTROLS);
		m_MenusSettingsControls.Render(MainView);
	}
	else if(g_Config.m_UiSettingsPage == SETTINGS_GRAPHICS)
	{
		GameClient()->m_MenuBackground.ChangePosition(CMenuBackground::POS_SETTINGS_GRAPHICS);
		RenderSettingsGraphics(MainView);
	}
	else if(g_Config.m_UiSettingsPage == SETTINGS_SOUND)
	{
		GameClient()->m_MenuBackground.ChangePosition(CMenuBackground::POS_SETTINGS_SOUND);
		RenderSettingsSound(MainView);
	}
	else if(g_Config.m_UiSettingsPage == SETTINGS_DDNET)
	{
		GameClient()->m_MenuBackground.ChangePosition(CMenuBackground::POS_SETTINGS_DDNET);
		RenderSettingsDDNet(MainView);
	}
	else if(g_Config.m_UiSettingsPage == SETTINGS_ASSETS)
	{
		GameClient()->m_MenuBackground.ChangePosition(CMenuBackground::POS_SETTINGS_ASSETS);
		RenderSettingsAssets(MainView);
	}
	else if(g_Config.m_UiSettingsPage == SETTINGS_TCLIENT) // TClient
	{
		GameClient()->m_MenuBackground.ChangePosition(CMenuBackground::POS_SETTINGS_LANGUAGE);
		RenderSettingsTClient(MainView);
	}
	else if(g_Config.m_UiSettingsPage == SETTINGS_PROFILES) // TClient
	{
		GameClient()->m_MenuBackground.ChangePosition(CMenuBackground::POS_SETTINGS_GENERAL);
		RenderSettingsTClientProfiles(MainView);
	}
	else if(g_Config.m_UiSettingsPage == SETTINGS_WCL)
	{
		GameClient()->m_MenuBackground.ChangePosition(CMenuBackground::POS_SETTINGS_GENERAL);
		RenderSettingsWcl(MainView);
	}
	else
	{
		dbg_assert_failed("ui_settings_page invalid");
	}

	if(NeedRestart)
	{
		CUIRect RestartWarning, RestartButton;
		RestartBar.VSplitRight(125.0f, &RestartWarning, &RestartButton);
		RestartWarning.VSplitRight(10.0f, &RestartWarning, nullptr);
		if(m_NeedRestartUpdate)
		{
			TextRender()->TextColor(1.0f, 0.4f, 0.4f, 1.0f);
			Ui()->DoLabel(&RestartWarning, Localize("DDNet Client needs to be restarted to complete update!"), 14.0f, TEXTALIGN_ML);
			TextRender()->TextColor(1.0f, 1.0f, 1.0f, 1.0f);
		}
		else
		{
			Ui()->DoLabel(&RestartWarning, Localize("You must restart the game for all settings to take effect."), 14.0f, TEXTALIGN_ML);
		}

		static CButtonContainer s_RestartButton;
		if(DoButton_Menu(&s_RestartButton, Localize("Restart"), 0, &RestartButton))
		{
			if(Client()->State() == IClient::STATE_ONLINE || GameClient()->Editor()->HasUnsavedData())
			{
				m_Popup = POPUP_RESTART;
			}
			else
			{
				Client()->Restart();
			}
		}
	}
}

void CMenus::RenderSettingsWcl(CUIRect MainView)
{
	CUIRect Slider, TabBar, LeftTab, MiddleTab, RightTab;
	static int s_WclSettingsTab = 0;
	static CButtonContainer s_WclPiPTab;
	static CButtonContainer s_WclChatTab;
	static CButtonContainer s_WclHudEditorTab;

	// WCL settings are split into PiP, chat and HUD editor pages.
	MainView.HSplitTop(28.0f, &TabBar, &MainView);
	LeftTab = TabBar;
	MiddleTab = TabBar;
	RightTab = TabBar;
	LeftTab.w = (TabBar.w - 10.0f) / 3.0f;
	MiddleTab.x += LeftTab.w + 5.0f;
	MiddleTab.w = LeftTab.w;
	RightTab.x += 2.0f * (LeftTab.w + 5.0f);
	RightTab.w = LeftTab.w;
	if(DoButton_MenuTab(&s_WclPiPTab, Localize("Picture in Picture"), s_WclSettingsTab == 0, &LeftTab, IGraphics::CORNER_ALL))
		s_WclSettingsTab = 0;
	if(DoButton_MenuTab(&s_WclChatTab, Localize("Chat"), s_WclSettingsTab == 1, &MiddleTab, IGraphics::CORNER_ALL))
		s_WclSettingsTab = 1;
	if(DoButton_MenuTab(&s_WclHudEditorTab, Localize("HUD Editor"), s_WclSettingsTab == 2, &RightTab, IGraphics::CORNER_ALL))
		s_WclSettingsTab = 2;
	MainView.HSplitTop(8.0f, nullptr, &MainView);

	if(s_WclSettingsTab == 1)
	{
		Ui()->DoLabel(&MainView, Localize("Chat settings"), 16.0f, TEXTALIGN_TL);
		MainView.HSplitTop(8.0f, nullptr, &MainView);
		static CButtonContainer s_WclFriendMessageColor;
		const unsigned int PreviousFriendMessageColor = g_Config.m_ClMessageFriendColor;
		const int PreviousFriendMessageEnabled = g_Config.m_ClMessageFriend;
		DoLine_ColorPicker(&s_WclFriendMessageColor, 25.0f, 13.0f, 5.0f, &MainView,
			Localize("Friend message"), &g_Config.m_ClMessageFriendColor,
			ColorRGBA(1.0f, 0.137f, 0.137f), true, &g_Config.m_ClMessageFriend);
		if(PreviousFriendMessageColor != g_Config.m_ClMessageFriendColor ||
			PreviousFriendMessageEnabled != g_Config.m_ClMessageFriend)
		{
			GameClient()->m_Chat.RebuildChat();
		}
		return;
	}
	if(s_WclSettingsTab == 2)
	{
		Ui()->DoLabel(&MainView, Localize("Arrange and resize the in-game HUD groups."), 14.0f, TEXTALIGN_TL);
		MainView.HSplitTop(8.0f, nullptr, &MainView);

		CUIRect ButtonRow, OpenEditorButton, ResetEditorButton;
		MainView.HSplitTop(28.0f, &ButtonRow, &MainView);
		ButtonRow.VSplitLeft((ButtonRow.w - 8.0f) / 2.0f, &OpenEditorButton, &ResetEditorButton);
		ResetEditorButton.x += 8.0f;
		ResetEditorButton.w -= 8.0f;
		static CButtonContainer s_WclOpenHudEditor;
		static CButtonContainer s_WclResetHudEditor;
		const bool CanOpenEditor = Client()->State() == IClient::STATE_ONLINE || Client()->State() == IClient::STATE_DEMOPLAYBACK;
		if(DoButton_Menu(&s_WclOpenHudEditor, Localize("Open HUD editor"), 0, &OpenEditorButton) && CanOpenEditor)
		{
			GameClient()->m_Hud.SetHudEditorActive(true);
			GameClient()->m_Menus.SetActive(false);
		}
		if(DoButton_Menu(&s_WclResetHudEditor, Localize("Reset layout"), 0, &ResetEditorButton))
			GameClient()->m_Hud.ResetHudEditorLayout();

		if(!CanOpenEditor)
		{
			MainView.HSplitTop(20.0f, &ButtonRow, &MainView);
			Ui()->DoLabel(&ButtonRow, Localize("Connect to a server or open a demo to edit the HUD."), 12.0f, TEXTALIGN_TL);
		}
		else
		{
			MainView.HSplitTop(20.0f, &ButtonRow, &MainView);
			Ui()->DoLabel(&ButtonRow, Localize("Drag a group to move it; use the mouse wheel to resize."), 12.0f, TEXTALIGN_TL);
		}

		auto DoHudEditorGroup = [&](const char *pTitle, int *pPosX, int *pPosY, int *pScale) {
			CUIRect GroupTitle;
			MainView.HSplitTop(20.0f, &GroupTitle, &MainView);
			Ui()->DoLabel(&GroupTitle, pTitle, 14.0f, TEXTALIGN_TL);
			MainView.HSplitTop(22.0f, &Slider, &MainView);
			DoSliderWithScaledValue(pPosX, pPosX, &Slider, Localize("Horizontal offset (%)"), -100, 100, 1, &CUi::ms_LinearScrollbarScale);
			MainView.HSplitTop(22.0f, &Slider, &MainView);
			DoSliderWithScaledValue(pPosY, pPosY, &Slider, Localize("Vertical offset (%)"), -100, 100, 1, &CUi::ms_LinearScrollbarScale);
			MainView.HSplitTop(22.0f, &Slider, &MainView);
			if(DoSliderWithScaledValue(pScale, pScale, &Slider, Localize("Scale (50–200%)"), 50, 200, 5, &CUi::ms_LinearScrollbarScale))
				ConfigManager()->Save();
			MainView.HSplitTop(4.0f, nullptr, &MainView);
		};

		DoHudEditorGroup(Localize("Player status"), &g_Config.m_ClHudStatusPosX, &g_Config.m_ClHudStatusPosY, &g_Config.m_ClHudStatusScale);
		DoHudEditorGroup(Localize("Timers"), &g_Config.m_ClHudTimerPosX, &g_Config.m_ClHudTimerPosY, &g_Config.m_ClHudTimerScale);
		DoHudEditorGroup(Localize("Right-side widgets"), &g_Config.m_ClHudRightPosX, &g_Config.m_ClHudRightPosY, &g_Config.m_ClHudRightScale);
		DoHudEditorGroup(Localize("Spectator bar"), &g_Config.m_ClHudSpectatorPosX, &g_Config.m_ClHudSpectatorPosY, &g_Config.m_ClHudSpectatorScale);
		return;
	}
	CUIRect Hint;
	MainView.HSplitTop(18.0f, &Hint, &MainView);
	Ui()->DoLabel(&Hint, Localize("Full-map tracking can significantly increase network traffic."), 12.0f, TEXTALIGN_TL);
	MainView.HSplitTop(12.0f, nullptr, &MainView);
	static int s_WclEnableId, s_WclWorldId, s_WclPlayersId, s_WclExtendedRangeId;
	DoButton_CheckBoxAutoVMarginAndSet(&s_WclEnableId, Localize("Enable teammate window"), &g_Config.m_ClWclEnable, &MainView, 20.0f);
	DoButton_CheckBoxAutoVMarginAndSet(&s_WclWorldId, Localize("Show map and game world"), &g_Config.m_ClWclShowWorld, &MainView, 20.0f);
	DoButton_CheckBoxAutoVMarginAndSet(&s_WclPlayersId, Localize("Show other players"), &g_Config.m_ClWclShowPlayers, &MainView, 20.0f);
	DoButton_CheckBoxAutoVMarginAndSet(&s_WclExtendedRangeId, Localize("Full-map tracking range"), &g_Config.m_ClWclExtendedRange, &MainView, 20.0f);

	static int s_WclFrameEnabledId;
	DoButton_CheckBoxAutoVMarginAndSet(&s_WclFrameEnabledId, Localize("Show frame"), &g_Config.m_ClWclFrameEnabled, &MainView, 20.0f);
	if(g_Config.m_ClWclFrameEnabled)
	{
		Ui()->DoLabel(&MainView, Localize("Frame style"), 14.0f, TEXTALIGN_TL);
		MainView.HSplitTop(22.0f, nullptr, &MainView);
		MainView.HSplitTop(22.0f, &Slider, &MainView);
		static CUi::SDropDownState s_WclFrameStyleDropDown;
		static CScrollRegion s_WclFrameStyleScrollRegion;
		s_WclFrameStyleDropDown.m_SelectionPopupContext.m_pScrollRegion = &s_WclFrameStyleScrollRegion;
		const char *apWclFrameStyles[] = {
			Localize("Minimal"),
			Localize("Neon"),
			Localize("Terminal"),
			Localize("Tactical")};
		const int NewFrameStyle = Ui()->DoDropDown(&Slider, g_Config.m_ClWclFrame, apWclFrameStyles, 4, s_WclFrameStyleDropDown);
		if(NewFrameStyle >= 0 && NewFrameStyle < 4)
			g_Config.m_ClWclFrame = NewFrameStyle;

	}

	// Screen rounding is independent of the decorative frame toggle.
	MainView.HSplitTop(22.0f, nullptr, &MainView);
	MainView.HSplitTop(22.0f, &Slider, &MainView);
	DoSliderWithScaledValue(&g_Config.m_ClWclCornerRadius, &g_Config.m_ClWclCornerRadius,
		&Slider, Localize("Screen corner radius"), 0, 16, 4, &CUi::ms_LinearScrollbarScale);

	Ui()->DoLabel(&MainView, Localize("Target player"), 14.0f, TEXTALIGN_TL);
	MainView.HSplitTop(22.0f, nullptr, &MainView);
	MainView.HSplitTop(22.0f, &Slider, &MainView);

	// Build the dropdown from the current server snapshot. The user sees names,
	// while the selected client slot keeps players with the same IP distinct.
	static CUi::SDropDownState s_WclTargetDropDown;
	static CScrollRegion s_WclTargetScrollRegion;
	// Selection popups need a stable scroll region; its address is also used
	// as a UI item ID, so keep it static for the lifetime of the dropdown.
	s_WclTargetDropDown.m_SelectionPopupContext.m_pScrollRegion = &s_WclTargetScrollRegion;
	char aaPlayerNames[MAX_CLIENTS + 1][MAX_NAME_LENGTH + MAX_CLAN_LENGTH + 32];
	const char *apPlayerNames[MAX_CLIENTS + 1];
	int aPlayerIds[MAX_CLIENTS + 1];
	int NumPlayers = 1;
	str_copy(aaPlayerNames[0], Localize("Automatic (nearest player)"), sizeof(aaPlayerNames[0]));
	apPlayerNames[0] = aaPlayerNames[0];
	aPlayerIds[0] = -1;

	int LocalClientId = GameClient()->m_Snap.m_LocalClientId;
	if(!in_range(LocalClientId, MAX_CLIENTS - 1))
		LocalClientId = GameClient()->m_aLocalIds[0];

	for(int ClientId = 0; ClientId < MAX_CLIENTS; ++ClientId)
	{
		if(ClientId == LocalClientId || ClientId == GameClient()->m_aLocalIds[0] ||
			ClientId == GameClient()->m_aLocalIds[1] || GameClient()->m_Snap.m_apPlayerInfos[ClientId] == nullptr)
			continue;

		const auto &Player = GameClient()->m_aClients[ClientId];
		bool DuplicateName = false;
		for(int OtherId = 0; OtherId < MAX_CLIENTS; ++OtherId)
		{
			if(OtherId != ClientId && GameClient()->m_Snap.m_apPlayerInfos[OtherId] != nullptr &&
				str_comp(Player.m_aName, GameClient()->m_aClients[OtherId].m_aName) == 0)
			{
				DuplicateName = true;
				break;
			}
		}

		if(Player.m_aClan[0] != '\0')
		{
			if(DuplicateName)
				str_format(aaPlayerNames[NumPlayers], sizeof(aaPlayerNames[NumPlayers]), "%s [%s] (#%d)", Player.m_aName, Player.m_aClan, ClientId);
			else
				str_format(aaPlayerNames[NumPlayers], sizeof(aaPlayerNames[NumPlayers]), "%s [%s]", Player.m_aName, Player.m_aClan);
		}
		else if(DuplicateName)
			str_format(aaPlayerNames[NumPlayers], sizeof(aaPlayerNames[NumPlayers]), "%s (#%d)", Player.m_aName, ClientId);
		else
			str_copy(aaPlayerNames[NumPlayers], Player.m_aName, sizeof(aaPlayerNames[NumPlayers]));

		apPlayerNames[NumPlayers] = aaPlayerNames[NumPlayers];
		aPlayerIds[NumPlayers] = ClientId;
		++NumPlayers;
	}

	int CurrentSelection = 0;
	for(int i = 1; i < NumPlayers; ++i)
	{
		if(aPlayerIds[i] == g_Config.m_ClWclPlayerId)
		{
			CurrentSelection = i;
			break;
		}
	}
	if(g_Config.m_ClWclPlayerId >= 0 && CurrentSelection == 0)
		g_Config.m_ClWclPlayerId = -1;

	const int NewSelection = Ui()->DoDropDown(&Slider, CurrentSelection, apPlayerNames, NumPlayers, s_WclTargetDropDown);
	if(NewSelection >= 0 && NewSelection < NumPlayers)
		g_Config.m_ClWclPlayerId = aPlayerIds[NewSelection];
	Ui()->DoLabel(&MainView, Localize("Window size"), 14.0f, TEXTALIGN_TL);
	MainView.HSplitTop(22.0f, nullptr, &MainView);
	MainView.HSplitTop(22.0f, &Slider, &MainView);
	DoSliderWithScaledValue(&g_Config.m_ClWclWidth, &g_Config.m_ClWclWidth, &Slider, Localize("Width"), 120, 640, 1, &CUi::ms_LinearScrollbarScale);
	MainView.HSplitTop(22.0f, nullptr, &MainView);
	MainView.HSplitTop(22.0f, &Slider, &MainView);
	DoSliderWithScaledValue(&g_Config.m_ClWclHeight, &g_Config.m_ClWclHeight, &Slider, Localize("Height"), 70, 400, 1, &CUi::ms_LinearScrollbarScale);
	Ui()->DoLabel(&MainView, Localize("Window position"), 14.0f, TEXTALIGN_TL);
	MainView.HSplitTop(22.0f, nullptr, &MainView);
	MainView.HSplitTop(22.0f, &Slider, &MainView);
	DoSliderWithScaledValue(&g_Config.m_ClWclPosX, &g_Config.m_ClWclPosX, &Slider, Localize("Horizontal"), 0, 100, 1, &CUi::ms_LinearScrollbarScale);
	MainView.HSplitTop(22.0f, nullptr, &MainView);
	MainView.HSplitTop(22.0f, &Slider, &MainView);
	DoSliderWithScaledValue(&g_Config.m_ClWclPosY, &g_Config.m_ClWclPosY, &Slider, Localize("Vertical"), 0, 100, 1, &CUi::ms_LinearScrollbarScale);
	Ui()->DoLabel(&MainView, Localize("Camera zoom"), 14.0f, TEXTALIGN_TL);
	MainView.HSplitTop(22.0f, nullptr, &MainView);
	MainView.HSplitTop(22.0f, &Slider, &MainView);
	DoSliderWithScaledValue(&g_Config.m_ClWclZoom, &g_Config.m_ClWclZoom, &Slider, Localize("Zoom"), 10, 100, 1, &CUi::ms_LinearScrollbarScale);
}
bool CMenus::RenderHslaScrollbars(CUIRect *pRect, unsigned int *pColor, bool Alpha, float DarkestLight)
{
	const unsigned PrevPackedColor = *pColor;
	ColorHSLA Color(*pColor, Alpha);
	const ColorHSLA OriginalColor = Color;
	const char *apLabels[] = {Localize("Hue"), Localize("Sat."), Localize("Lht."), Localize("Alpha")};
	const float SizePerEntry = 20.0f;
	const float MarginPerEntry = 5.0f;
	const float PreviewMargin = 2.5f;
	const float PreviewHeight = 40.0f + 2 * PreviewMargin;
	const float OffY = (SizePerEntry + MarginPerEntry) * (3 + (Alpha ? 1 : 0)) - PreviewHeight;

	CUIRect Preview;
	pRect->VSplitLeft(PreviewHeight, &Preview, pRect);
	Preview.HSplitTop(OffY / 2.0f, nullptr, &Preview);
	Preview.HSplitTop(PreviewHeight, &Preview, nullptr);

	Preview.Draw(ColorRGBA(0.15f, 0.15f, 0.15f, 1.0f), IGraphics::CORNER_ALL, 4.0f + PreviewMargin);
	Preview.Margin(PreviewMargin, &Preview);
	Preview.Draw(color_cast<ColorRGBA>(Color.UnclampLighting(DarkestLight)), IGraphics::CORNER_ALL, 4.0f + PreviewMargin);

	auto &&RenderHueRect = [&](CUIRect *pColorRect) {
		float CurXOff = pColorRect->x;
		const float SizeColor = pColorRect->w / 6;

		// red to yellow
		{
			Graphics()->SetColor4(
				ColorRGBA(1, 0, 0, 1),
				ColorRGBA(1, 1, 0, 1),
				ColorRGBA(1, 1, 0, 1),
				ColorRGBA(1, 0, 0, 1));

			IGraphics::CFreeformItem Freeform(
				CurXOff, pColorRect->y,
				CurXOff + SizeColor, pColorRect->y,
				CurXOff, pColorRect->y + pColorRect->h,
				CurXOff + SizeColor, pColorRect->y + pColorRect->h);
			Graphics()->QuadsDrawFreeform(&Freeform, 1);
		}

		// yellow to green
		CurXOff += SizeColor;
		{
			Graphics()->SetColor4(
				ColorRGBA(1, 1, 0, 1),
				ColorRGBA(0, 1, 0, 1),
				ColorRGBA(0, 1, 0, 1),
				ColorRGBA(1, 1, 0, 1));

			IGraphics::CFreeformItem Freeform(
				CurXOff, pColorRect->y,
				CurXOff + SizeColor, pColorRect->y,
				CurXOff, pColorRect->y + pColorRect->h,
				CurXOff + SizeColor, pColorRect->y + pColorRect->h);
			Graphics()->QuadsDrawFreeform(&Freeform, 1);
		}

		CurXOff += SizeColor;
		// green to turquoise
		{
			Graphics()->SetColor4(
				ColorRGBA(0, 1, 0, 1),
				ColorRGBA(0, 1, 1, 1),
				ColorRGBA(0, 1, 1, 1),
				ColorRGBA(0, 1, 0, 1));

			IGraphics::CFreeformItem Freeform(
				CurXOff, pColorRect->y,
				CurXOff + SizeColor, pColorRect->y,
				CurXOff, pColorRect->y + pColorRect->h,
				CurXOff + SizeColor, pColorRect->y + pColorRect->h);
			Graphics()->QuadsDrawFreeform(&Freeform, 1);
		}

		CurXOff += SizeColor;
		// turquoise to blue
		{
			Graphics()->SetColor4(
				ColorRGBA(0, 1, 1, 1),
				ColorRGBA(0, 0, 1, 1),
				ColorRGBA(0, 0, 1, 1),
				ColorRGBA(0, 1, 1, 1));

			IGraphics::CFreeformItem Freeform(
				CurXOff, pColorRect->y,
				CurXOff + SizeColor, pColorRect->y,
				CurXOff, pColorRect->y + pColorRect->h,
				CurXOff + SizeColor, pColorRect->y + pColorRect->h);
			Graphics()->QuadsDrawFreeform(&Freeform, 1);
		}

		CurXOff += SizeColor;
		// blue to purple
		{
			Graphics()->SetColor4(
				ColorRGBA(0, 0, 1, 1),
				ColorRGBA(1, 0, 1, 1),
				ColorRGBA(1, 0, 1, 1),
				ColorRGBA(0, 0, 1, 1));

			IGraphics::CFreeformItem Freeform(
				CurXOff, pColorRect->y,
				CurXOff + SizeColor, pColorRect->y,
				CurXOff, pColorRect->y + pColorRect->h,
				CurXOff + SizeColor, pColorRect->y + pColorRect->h);
			Graphics()->QuadsDrawFreeform(&Freeform, 1);
		}

		CurXOff += SizeColor;
		// purple to red
		{
			Graphics()->SetColor4(
				ColorRGBA(1, 0, 1, 1),
				ColorRGBA(1, 0, 0, 1),
				ColorRGBA(1, 0, 0, 1),
				ColorRGBA(1, 0, 1, 1));

			IGraphics::CFreeformItem Freeform(
				CurXOff, pColorRect->y,
				CurXOff + SizeColor, pColorRect->y,
				CurXOff, pColorRect->y + pColorRect->h,
				CurXOff + SizeColor, pColorRect->y + pColorRect->h);
			Graphics()->QuadsDrawFreeform(&Freeform, 1);
		}
	};

	auto &&RenderSaturationRect = [&](CUIRect *pColorRect, const ColorRGBA &CurColor) {
		ColorHSLA LeftColor = color_cast<ColorHSLA>(CurColor);
		ColorHSLA RightColor = color_cast<ColorHSLA>(CurColor);

		LeftColor.s = 0.0f;
		RightColor.s = 1.0f;

		const ColorRGBA LeftColorRGBA = color_cast<ColorRGBA>(LeftColor);
		const ColorRGBA RightColorRGBA = color_cast<ColorRGBA>(RightColor);

		Graphics()->SetColor4(LeftColorRGBA, RightColorRGBA, RightColorRGBA, LeftColorRGBA);

		IGraphics::CFreeformItem Freeform(
			pColorRect->x, pColorRect->y,
			pColorRect->x + pColorRect->w, pColorRect->y,
			pColorRect->x, pColorRect->y + pColorRect->h,
			pColorRect->x + pColorRect->w, pColorRect->y + pColorRect->h);
		Graphics()->QuadsDrawFreeform(&Freeform, 1);
	};

	auto &&RenderLightingRect = [&](CUIRect *pColorRect, const ColorRGBA &CurColor) {
		ColorHSLA LeftColor = color_cast<ColorHSLA>(CurColor);
		ColorHSLA RightColor = color_cast<ColorHSLA>(CurColor);

		LeftColor.l = DarkestLight;
		RightColor.l = 1.0f;

		const ColorRGBA LeftColorRGBA = color_cast<ColorRGBA>(LeftColor);
		const ColorRGBA RightColorRGBA = color_cast<ColorRGBA>(RightColor);

		Graphics()->SetColor4(LeftColorRGBA, RightColorRGBA, RightColorRGBA, LeftColorRGBA);

		IGraphics::CFreeformItem Freeform(
			pColorRect->x, pColorRect->y,
			pColorRect->x + pColorRect->w, pColorRect->y,
			pColorRect->x, pColorRect->y + pColorRect->h,
			pColorRect->x + pColorRect->w, pColorRect->y + pColorRect->h);
		Graphics()->QuadsDrawFreeform(&Freeform, 1);
	};

	auto &&RenderAlphaRect = [&](CUIRect *pColorRect, const ColorRGBA &CurColorFull) {
		const ColorRGBA LeftColorRGBA = color_cast<ColorRGBA>(color_cast<ColorHSLA>(CurColorFull).WithAlpha(0.0f));
		const ColorRGBA RightColorRGBA = color_cast<ColorRGBA>(color_cast<ColorHSLA>(CurColorFull).WithAlpha(1.0f));

		Graphics()->SetColor4(LeftColorRGBA, RightColorRGBA, RightColorRGBA, LeftColorRGBA);

		IGraphics::CFreeformItem Freeform(
			pColorRect->x, pColorRect->y,
			pColorRect->x + pColorRect->w, pColorRect->y,
			pColorRect->x, pColorRect->y + pColorRect->h,
			pColorRect->x + pColorRect->w, pColorRect->y + pColorRect->h);
		Graphics()->QuadsDrawFreeform(&Freeform, 1);
	};

	for(int i = 0; i < 3 + Alpha; i++)
	{
		CUIRect Button, Label;
		pRect->HSplitTop(SizePerEntry, &Button, pRect);
		pRect->HSplitTop(MarginPerEntry, nullptr, pRect);
		Button.VSplitLeft(140.0f, &Label, &Button);
		Label.VMargin(10.0f, &Label);

		Button.Draw(ColorRGBA(0.15f, 0.15f, 0.15f, 1.0f), IGraphics::CORNER_ALL, 1.0f);

		CUIRect Rail;
		Button.Margin(2.0f, &Rail);

		char aBuf[32];

		// Hue
		if(i == 0)
			str_format(aBuf, sizeof(aBuf), "%s: %.1f° (%03d)", apLabels[i], Color[i] * 360.0f, round_to_int(Color[i] * 255.0f));
		// Lht
		else if(i == 2)
		{
			// handle internal light clamping, see `UnclampLighting`
			float Lht = DarkestLight + Color[i] * (1.0f - DarkestLight);
			str_format(aBuf, sizeof(aBuf), "%s: %.1f%% (%03d)", apLabels[i], Lht * 100.0f, round_to_int(Color[i] * 255.0f));
		}
		// Sat and Alpha
		else
			str_format(aBuf, sizeof(aBuf), "%s: %.1f%% (%03d)", apLabels[i], Color[i] * 100.0f, round_to_int(Color[i] * 255.0f));
		Ui()->DoLabel(&Label, aBuf, 12.0f, TEXTALIGN_ML);

		ColorRGBA HandleColor;
		Graphics()->TextureClear();
		Graphics()->TrianglesBegin();
		if(i == 0)
		{
			RenderHueRect(&Rail);
			HandleColor = color_cast<ColorRGBA>(ColorHSLA(Color.h, 1.0f, 0.5f, 1.0f));
		}
		else if(i == 1)
		{
			RenderSaturationRect(&Rail, color_cast<ColorRGBA>(ColorHSLA(Color.h, 1.0f, 0.5f, 1.0f)));
			HandleColor = color_cast<ColorRGBA>(ColorHSLA(Color.h, Color.s, 0.5f, 1.0f));
		}
		else if(i == 2)
		{
			RenderLightingRect(&Rail, color_cast<ColorRGBA>(ColorHSLA(Color.h, Color.s, 0.5f, 1.0f)));
			HandleColor = color_cast<ColorRGBA>(ColorHSLA(Color.h, Color.s, Color.l, 1.0f).UnclampLighting(DarkestLight));
		}
		else if(i == 3)
		{
			RenderAlphaRect(&Rail, color_cast<ColorRGBA>(ColorHSLA(Color.h, Color.s, Color.l, 1.0f).UnclampLighting(DarkestLight)));
			HandleColor = color_cast<ColorRGBA>(Color.UnclampLighting(DarkestLight));
		}
		Graphics()->TrianglesEnd();

		Color[i] = Ui()->DoScrollbarH(&((char *)pColor)[i], &Button, Color[i], &HandleColor);
	}

	if(OriginalColor != Color)
	{
		*pColor = Color.Pack(Alpha);
	}
	return PrevPackedColor != *pColor;
}
