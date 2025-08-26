#include "ClientPch.h"
#include "GS_Desend.h"
#include "Monster.h"


CGS_Desend::CGS_Desend()
{
}

HRESULT CGS_Desend::Initialize()
{
	m_iStateFlag = ENUM_CLASS(STATE_FLAG::ATTACK);

	m_vMoveReadyTime = _float2(55.f, 145.f);
	m_vMoveTime = _float2(185.f, 211.f);

	return S_OK;
}

void CGS_Desend::Enter(CMonster* pMonster)
{
	ChangeActionFlag(ENUM_CLASS(ATTACK_FLAG::DESEND));
}

void CGS_Desend::Update(CMonster* pMonster, _float fTimeDelta)
{
	if (pMonster->IsAnimationInRangeTrackPosition(m_vMoveReadyTime))
		pMonster->TurnToTarget(fTimeDelta);
	else if (pMonster->IsAnimationInRangeTrackPosition(m_vMoveTime))
		pMonster->MoveToTarget(fTimeDelta);
}

void CGS_Desend::Exit(CMonster* pMonster)
{
	m_iStateFlag = ENUM_CLASS(STATE_FLAG::ATTACK);
}

CGS_Desend* CGS_Desend::Create()
{
	CGS_Desend* pInstance = new CGS_Desend();
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed Created : CGS_Desend"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CGS_Desend::Free()
{
	__super::Free();
}

namespace {
	struct GS_DesendRegister {
		GS_DesendRegister() {
			CMonsterStateFactory::GetInstance()->Register(ENUM_CLASS(MONSTER_STATE_TYPE::GLASGAVELEN), ENUM_CLASS(GAVELEN_STATE::DESEND), []()->CMonsterState* { return CGS_Desend::Create(); });
		};

	};
	static GS_DesendRegister   Register;
}