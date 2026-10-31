/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */

#include "bee.h"

void CBee::ResetState()
{
	m_Enabled = false;
	m_TargetClientId = -1;
	m_Camera = CCameraState{};
	m_Overlay = COverlayState{};
}

void CBee::OnInit()
{
	ResetState();
}

void CBee::OnReset()
{
	ResetState();
}
