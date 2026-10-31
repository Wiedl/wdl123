/* (c) Tater contributors. */
#include "bee.h"

#include <engine/graphics.h>
#include <engine/keys.h>
#include <engine/shared/config.h>

#include <game/client/gameclient.h>

#include <algorithm>

void CBee::OnInit()
{
	m_RenderTexture = Graphics()->CreateRenderTarget(TARGET_WIDTH, TARGET_HEIGHT);
}

void CBee::OnShutdown()
{
	Graphics()->DestroyRenderTarget(&m_RenderTexture);
}


bool CBee::OnInput(const IInput::CEvent &Event)
{
	if(Event.m_Key != KEY_MOUSE_1)
		return false;

	if(Event.m_Flags & IInput::FLAG_RELEASE)
	{
		const bool WasDragging = m_DragMode != EDragMode::NONE;
		m_DragMode = EDragMode::NONE;
		return WasDragging;
	}

	if(!(Event.m_Flags & IInput::FLAG_PRESS) || !g_Config.m_ClBeeEnabled ||
		(Client()->State() != IClient::STATE_ONLINE && Client()->State() != IClient::STATE_DEMOPLAYBACK))
		return false;

	const float ScreenWidth = Graphics()->ScreenWidth();
	const float Left = ScreenWidth - g_Config.m_ClBeeX - g_Config.m_ClBeeWidth;
	const float Top = (float)g_Config.m_ClBeeY;
	const float Right = Left + g_Config.m_ClBeeWidth;
	const float Bottom = Top + g_Config.m_ClBeeHeight;
	const vec2 Mouse = Ui()->MousePos();

	if(Mouse.x < Left || Mouse.x > Right || Mouse.y < Top || Mouse.y > Bottom)
		return false;

	m_DragStart = Mouse;
	m_DragStartX = g_Config.m_ClBeeX;
	m_DragStartY = g_Config.m_ClBeeY;
	m_DragStartWidth = g_Config.m_ClBeeWidth;
	m_DragStartHeight = g_Config.m_ClBeeHeight;
	const float ResizeHandle = 18.0f;
	m_DragMode = Mouse.x >= Right - ResizeHandle && Mouse.y >= Bottom - ResizeHandle ? EDragMode::RESIZE : EDragMode::MOVE;
	return true;
}

bool CBee::OnCursorMove(float x, float y, IInput::ECursorType CursorType)
{
	if(m_DragMode == EDragMode::NONE || CursorType != IInput::CURSOR_MOUSE)
		return false;

	const float DeltaX = Ui()->MousePos().x - m_DragStart.x;
	const float DeltaY = Ui()->MousePos().y - m_DragStart.y;
	const int ScreenWidth = (int)Graphics()->ScreenWidth();
	const int ScreenHeight = (int)Graphics()->ScreenHeight();

	if(m_DragMode == EDragMode::MOVE)
	{
		const int Width = g_Config.m_ClBeeWidth;
		const int Height = g_Config.m_ClBeeHeight;
		const int Left = std::clamp(ScreenWidth - m_DragStartX - Width + round_to_int(DeltaX), 0, std::max(0, ScreenWidth - Width));
		const int Top = std::clamp(m_DragStartY + round_to_int(DeltaY), 0, std::max(0, ScreenHeight - Height));
		g_Config.m_ClBeeX = ScreenWidth - Left - Width;
		g_Config.m_ClBeeY = Top;
	}
	else
	{
		g_Config.m_ClBeeWidth = std::clamp(m_DragStartWidth + round_to_int(DeltaX), 160, std::min(1280, ScreenWidth));
		g_Config.m_ClBeeHeight = std::clamp(m_DragStartHeight + round_to_int(DeltaY), 90, std::min(720, ScreenHeight));
		g_Config.m_ClBeeX = std::clamp(m_DragStartX, 0, std::max(0, ScreenWidth - g_Config.m_ClBeeWidth));
		g_Config.m_ClBeeY = std::clamp(m_DragStartY, 0, std::max(0, ScreenHeight - g_Config.m_ClBeeHeight));
	}
	return true;
}

void CBee::OnRender()
{
	if(!g_Config.m_ClBeeEnabled || !m_RenderTexture.IsValid())
		return;
	if(Client()->State() != IClient::STATE_ONLINE && Client()->State() != IClient::STATE_DEMOPLAYBACK)
		return;

	int TargetId = g_Config.m_ClBeeTarget;
	if(TargetId < 0)
		TargetId = GameClient()->m_Snap.m_LocalClientId;
	if(TargetId < 0 || TargetId >= MAX_CLIENTS || !GameClient()->m_Snap.m_aCharacters[TargetId].m_Active)
		return;

	m_Camera.m_Center = GameClient()->m_aClients[TargetId].m_RenderPos;
	m_Camera.m_Zoom = g_Config.m_ClBeeZoom / 100.0f;

	Graphics()->BeginRenderTarget(m_RenderTexture, TARGET_WIDTH, TARGET_HEIGHT);
	Graphics()->Clear(0.0f, 0.0f, 0.0f);
	const CScreenRect WorldScreen = Graphics()->MapScreenToWorld(
		m_Camera.m_Center.x, m_Camera.m_Center.y,
		1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
		(float)TARGET_WIDTH / TARGET_HEIGHT, m_Camera.m_Zoom);
	Graphics()->MapScreen(WorldScreen);

	GameClient()->m_MapLayersBackground.SetCameraOverride(&m_Camera);
	GameClient()->m_MapLayersForeground.SetCameraOverride(&m_Camera);
	GameClient()->m_MapLayersBackground.OnRender();
	GameClient()->m_Items.OnRender();
	GameClient()->m_Players.RenderForCamera(&m_Camera);
	GameClient()->m_MapLayersForeground.OnRender();
	GameClient()->m_MapLayersBackground.SetCameraOverride(nullptr);
	GameClient()->m_MapLayersForeground.SetCameraOverride(nullptr);
	Graphics()->EndRenderTarget();

	Graphics()->MapScreen(CScreenRect(0.0f, 0.0f, Graphics()->ScreenWidth(), Graphics()->ScreenHeight()));
	Graphics()->BlendNormal();
	Graphics()->TextureSet(m_RenderTexture);
	Graphics()->SetColor(1.0f, 1.0f, 1.0f, 1.0f);
	Graphics()->QuadsBegin();
	const float Width = (float)g_Config.m_ClBeeWidth;
	const float Height = (float)g_Config.m_ClBeeHeight;
	const float X = (float)g_Config.m_ClBeeX;
	const float Y = (float)g_Config.m_ClBeeY;
	IGraphics::CQuadItem Quad(Graphics()->ScreenWidth() - X - Width, Y, Width, Height);
	Graphics()->QuadsDrawTL(&Quad, 1);
	Graphics()->QuadsEnd();
	Graphics()->TextureClear();
}
