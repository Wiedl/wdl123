/* (c) Tater contributors. */
#ifndef GAME_CLIENT_COMPONENTS_BEE_H
#define GAME_CLIENT_COMPONENTS_BEE_H

#include <engine/graphics.h>

#include <game/client/component.h>
#include <game/client/components/camera.h>

class CGameClient;

class CBee : public CComponent
{
	static constexpr int TARGET_WIDTH = 640;
	static constexpr int TARGET_HEIGHT = 360;

	IGraphics::CTextureHandle m_RenderTexture;
	CCamera m_Camera;

	enum class EDragMode
	{
		NONE,
		MOVE,
		RESIZE,
	};
	EDragMode m_DragMode = EDragMode::NONE;
	vec2 m_DragStart = vec2(0.0f, 0.0f);
	int m_DragStartX = 0;
	int m_DragStartY = 0;
	int m_DragStartWidth = 0;
	int m_DragStartHeight = 0;

public:
	int Sizeof() const override { return sizeof(*this); }
	void OnInit() override;
	void OnShutdown() override;
	void OnRender() override;
	bool OnInput(const IInput::CEvent &Event) override;
	bool OnCursorMove(float x, float y, IInput::ECursorType CursorType) override;
	void OnRelease() override { m_DragMode = EDragMode::NONE; }
};

#endif
