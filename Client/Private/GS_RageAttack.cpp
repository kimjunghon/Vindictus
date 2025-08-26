#include "ClientPch.h"
#include "GS_RageAttack.h"
#include "Monster.h"

CGS_RageAttack::CGS_RageAttack()
{
}

HRESULT CGS_RageAttack::Initialize()
{
	m_iStateFlag = ENUM_CLASS(STATE_FLAG::RAGE);

	m_iLinkAnimAttackFlag = ENUM_CLASS(RAGE_FLAG::GRAP) | ENUM_CLASS(RAGE_FLAG::HANG) | ENUM_CLASS(RAGE_FLAG::DESEND_BEGIN);
	return S_OK;
}

void CGS_RageAttack::Enter(CMonster* pMonster)
{

	_uint iAttackIndex = pMonster->Get_AttackIndex();

	_uint iAttackFlag = ENUM_CLASS(RAGE_FLAG::DESEND_BEGIN) << iAttackIndex;

	if (iAttackFlag & m_iLinkAnimAttackFlag)
		ChangeAttackState(pMonster, iAttackFlag);
	else
		ChangeActionFlag(iAttackFlag);
}

void CGS_RageAttack::Update(CMonster* pMonster, _float fTimeDelta)
{
	if (pMonster->IsReadyAttack(m_iStateFlag))
		pMonster->TurnToTarget(fTimeDelta);
}

void CGS_RageAttack::Exit(CMonster* pMonster)
{
	m_iStateFlag = ENUM_CLASS(STATE_FLAG::RAGE);
}

void CGS_RageAttack::ChangeAttackState(CMonster* pMonster, _uint iAttackFlag)
{
	switch (iAttackFlag)
	{
	case ENUM_CLASS(ATTACK_FLAG::GRAP):
		pMonster->ChangeState(ENUM_CLASS(GAVELEN_STATE::GRAP));
		break;
	case ENUM_CLASS(ATTACK_FLAG::HANG):
		pMonster->ChangeState(ENUM_CLASS(GAVELEN_STATE::HANG));
		break;
	case ENUM_CLASS(ATTACK_FLAG::DESEND):
		pMonster->ChangeState(ENUM_CLASS(GAVELEN_STATE::RAGE_DESEND));
		break;
	}
}

CGS_RageAttack* CGS_RageAttack::Create()
{
	CGS_RageAttack* pInstance = new CGS_RageAttack();
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed Created : CGS_RageAttack"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CGS_RageAttack::Free()
{
	__super::Free();
}

namespace {
	struct GS_RageAttackRegister {
		GS_RageAttackRegister() {
			CMonsterStateFactory::GetInstance()->Register(ENUM_CLASS(MONSTER_STATE_TYPE::GLASGAVELEN), ENUM_CLASS(GAVELEN_STATE::RAGE), []()->CMonsterState* { return CGS_RageAttack::Create(); });
		};

	};
	static GS_RageAttackRegister   Register;
}