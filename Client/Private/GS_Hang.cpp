#include "ClientPch.h"
#include "GS_Hang.h"
#include "Monster.h"

CGS_Hang::CGS_Hang()
{
}

_bool CGS_Hang::CanStateChange(CMonster* pMonster)
{
	if (m_iHangPhase == ENUM_CLASS(HANG_PHASE::END))
		return pMonster->AnimCanChange() || pMonster->AnimIsFinished();

	return false;
}

HRESULT CGS_Hang::Initialize()
{
	m_iStateFlag = ENUM_CLASS(STATE_FLAG::HANG);
	
	m_iHangFlag = ENUM_CLASS(HANG_FLAG::BEGIN);

	m_iAttackCount = 1;

	m_vMoveReadyTime = _float2(18.f, 48.f);
	m_vMoveTime = _float2(82.f, 113.f);

	return S_OK;
}

void CGS_Hang::Enter(CMonster* pMonster)
{
	ChangeActionFlag(ENUM_CLASS(HANG_FLAG::BEGIN));
	
	m_iHangPhase = ENUM_CLASS(HANG_PHASE::BEGIN);

	m_iCurrentAttackCount = 0;

	m_IsChange = true;
}

void CGS_Hang::Update(CMonster* pMonster, _float fTimeDelta)
{
	if (m_IsChange)
	{
		m_IsChange = false;
		return;
	}

	pMonster->TurnToTarget(fTimeDelta);

	if (pMonster->AnimIsFinished() && m_iHangPhase < ENUM_CLASS(HANG_PHASE::END))
	{
		if (m_iHangPhase == ENUM_CLASS(HANG_PHASE::DURING))
		{
			if (m_iCurrentAttackCount < m_iAttackCount)
			{
				m_iCurrentAttackCount++;
				return;
			}
		}

		m_iHangPhase++;

		_uint iActionFlag = m_iHangFlag << m_iHangPhase;

		ChangeActionFlag(iActionFlag);
	}
	else if(m_iHangPhase == ENUM_CLASS(HANG_PHASE::END))
	{
		if (pMonster->IsAnimationInRangeTrackPosition(m_vMoveReadyTime))
			pMonster->TurnToTarget(fTimeDelta);
		else if (pMonster->IsAnimationInRangeTrackPosition(m_vMoveTime))
			pMonster->MoveToTarget(fTimeDelta);
	}
}

void CGS_Hang::Exit(CMonster* pMonster)
{
	m_iStateFlag = ENUM_CLASS(STATE_FLAG::HANG);

	m_iHangPhase = ENUM_CLASS(HANG_PHASE::BEGIN);

	m_iCurrentAttackCount = 0;
}

CGS_Hang* CGS_Hang::Create()
{
	CGS_Hang* pInstance = new CGS_Hang();
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed Created : CGS_Hang"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CGS_Hang::Free()
{
	__super::Free();
}

namespace {
	struct GS_HangRegister {
		GS_HangRegister() {
			CMonsterStateFactory::GetInstance()->Register(ENUM_CLASS(MONSTER_STATE_TYPE::GLASGAVELEN), ENUM_CLASS(GAVELEN_STATE::HANG), []()->CMonsterState* { return CGS_Hang::Create(); });
		};

	};
	static GS_HangRegister  Register;
}