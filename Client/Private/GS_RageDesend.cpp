#include "ClientPch.h"
#include "GS_RageDesend.h"
#include "Monster.h"

CGS_RageDesend::CGS_RageDesend()
{
}

_bool CGS_RageDesend::CanStateChange(CMonster* pMonster)
{
	if (m_iStateFlag & ENUM_CLASS(RAGE_FLAG::DESEND_END))
		return pMonster->AnimCanChange() || pMonster->AnimIsFinished();

	return false;
}

HRESULT CGS_RageDesend::Initialize()
{
	m_iStateFlag = ENUM_CLASS(STATE_FLAG::RAGE);

	m_vMoveReadyTime = _float2(55.f, 15.f);
	m_vMoveTime = _float2(182.f, 216.f);
	
	return S_OK;
}

void CGS_RageDesend::Enter(CMonster* pMonster)
{
	ChangeActionFlag(ENUM_CLASS(RAGE_FLAG::DESEND_BEGIN));

	m_IsChange = true;
}

void CGS_RageDesend::Update(CMonster* pMonster, _float fTimeDelta)
{
	if (m_IsChange)
	{
		m_IsChange = false;
		return;
	}

	if (m_iStateFlag & ENUM_CLASS(RAGE_FLAG::DESEND_BEGIN))
	{
		if (pMonster->AnimCanChange() || pMonster->AnimIsFinished())
			ChangeActionFlag(ENUM_CLASS(RAGE_FLAG::DESEND_END));
		else
			Update_Begin(pMonster, fTimeDelta);
	}
	else
		Update_End(pMonster, fTimeDelta);

}

void CGS_RageDesend::Exit(CMonster* pMonster)
{
	m_iStateFlag = ENUM_CLASS(STATE_FLAG::RAGE);

}

void CGS_RageDesend::Update_Begin(CMonster* pMonster, _float fTimeDelta)
{
	if (pMonster->IsAnimationInRangeTrackPosition(m_vMoveReadyTime))
		pMonster->TurnToTarget(fTimeDelta);
	else if (pMonster->IsAnimationInRangeTrackPosition(m_vMoveTime))
		pMonster->MoveToTarget(fTimeDelta * 2.f);
}

void CGS_RageDesend::Update_End(CMonster* pMonster, _float fTimeDelta)
{
	if (pMonster->IsReadyAttack())
		pMonster->TurnToTarget(fTimeDelta);
}

CGS_RageDesend* CGS_RageDesend::Create()
{
	CGS_RageDesend* pInstance = new CGS_RageDesend();
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed Created : CGS_RageDesend"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CGS_RageDesend::Free()
{
	__super::Free();
}

namespace {
	struct GS_RageDesendRegister {
		GS_RageDesendRegister() {
			CMonsterStateFactory::GetInstance()->Register(ENUM_CLASS(MONSTER_STATE_TYPE::GLASGAVELEN), ENUM_CLASS(GAVELEN_STATE::RAGE_DESEND), []()->CMonsterState* { return CGS_RageDesend::Create(); });
		};

	};
	static GS_RageDesendRegister   Register;
}