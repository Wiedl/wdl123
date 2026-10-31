/* (c) Tater contributors. */
#ifndef GAME_CLIENT_COMPONENTS_BEE_H
#define GAME_CLIENT_COMPONENTS_BEE_H

#include <game/client/component.h>
#include <game/client/components/camera.h>

class CGameClient;

class CBee : public CComponent
{
	static constexpr int TARGET_WIDTH = 640;
	static constexpr int TARGET_HEIGHT = 360;

	IGraphics::CTextureHandle m_RenderTexture;
	CCamera m_Camera;

public:
	int Sizeof() const override { return sizeof(*this); }
	void OnInit() override;
	void OnShutdown() override;
	void OnRender() override;
};

#endif
