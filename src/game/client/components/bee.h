/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
#ifndef GAME_CLIENT_COMPONENTS_BEE_H
#define GAME_CLIENT_COMPONENTS_BEE_H

#include <base/vmath.h>

#include <game/client/component.h>

class CBee : public CComponent
{
	bool m_Dragging = false;
	bool m_Resizing = false;

	vec2 MousePosition() const;

public:
	int Sizeof() const override { return sizeof(*this); }
	void OnRender() override;
	bool OnInput(const IInput::CEvent &Event) override;
	bool OnCursorMove(float x, float y, IInput::ECursorType CursorType) override;
};

#endif
