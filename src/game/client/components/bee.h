/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#ifndef GAME_CLIENT_COMPONENTS_BEE_H
#define GAME_CLIENT_COMPONENTS_BEE_H

#include <base/vmath.h>

#include <game/client/component.h>

class CBee : public CComponent
{
public:
	struct CCameraState
	{
		vec2 m_Center = vec2(0.0f, 0.0f);
		float m_Zoom = 1.0f;
	};

	struct COverlayState
	{
		vec2 m_Position = vec2(0.68f, 0.05f);
		vec2 m_Size = vec2(0.28f, 0.28f);
	};

private:
	bool m_Enabled = false;
	int m_TargetClientId = -1;
	CCameraState m_Camera;
	COverlayState m_Overlay;

	void ResetState();

public:
	int Sizeof() const override { return sizeof(*this); }
	void OnInit() override;
	void OnReset() override;
};

#endif
