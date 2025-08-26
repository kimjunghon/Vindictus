#include "ClientPch.h"
#include "State.h"

CState::CState()
{
}

HRESULT CState::Initialize()
{
	return S_OK;
}

void CState::ChangeActionFlag(_uint iOnFlag)
{
	m_iStateFlag &= ((1 << 8) - 1);

	m_iStateFlag |= iOnFlag;
}

void CState::Free()
{
}
