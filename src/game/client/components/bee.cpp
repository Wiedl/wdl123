/* (c) Tater contributors. */
#include "bee.h"

#include <engine/graphics.h>
#include <engine/shared/config.h>

#include <game/client/gameclient.h>

void CBee::OnInit()
{
	m_RenderTexture = Graphics()->CreateRenderTarget(TARGET_WIDTH, TARGET_HEIGHT);
}

void CBee::OnShutdown()
{
	Graphics()->DestroyRenderTarget(&m_RenderTexture);
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
