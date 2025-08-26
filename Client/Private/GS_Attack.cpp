#include "ClientPch.h"
#include "GS_Attack.h"
#include "Monster.h"

CGS_Attack::CGS_Attack()
{
}

HRESULT CGS_Attack::Initialize()
{
	m_iLinkAnimAttackFlag = ENUM_CLASS(ATTACK_FLAG::GRAP) | ENUM_CLASS(ATTACK_FLAG::HANG) | ENUM_CLASS(ATTACK_FLAG::DESEND);

	m_iStateFlag = ENUM_CLASS(STATE_FLAG::ATTACK);

    return S_OK;
}

void CGS_Attack::Enter(CMonster* pMonster)
{
	_uint iAttackIndex = pMonster->Get_AttackIndex();

	_uint iAttackFlag = ENUM_CLASS(ATTACK_FLAG::DESEND) << iAttackIndex;

	if (iAttackFlag & m_iLinkAnimAttackFlag)
		ChangeAttackState(pMonster, iAttackFlag);
	else
		ChangeActionFlag(iAttackFlag);
}

void CGS_Attack::Update(CMonster* pMonster, _float fTimeDelta)
{
	if (pMonster->IsReadyAttack(m_iStateFlag))
		pMonster->TurnToTarget(fTimeDelta);
}

void CGS_Attack::Exit(CMonster* pMonster)
{
	m_iStateFlag = ENUM_CLASS(STATE_FLAG::ATTACK);
}

void CGS_Attack::ChangeAttackState(CMonster* pMonster, _uint iAttackFlag)
{
	switch (iAttackFlag)
	{
	case ENUM_CLASS(ATTACK_FLAG::GRAP):
		pMonster->ChangeState(ENUM_CLASS(GAVELEN_STATE::GRAP));
		break;
	case ENUM_CLASS(ATTACK_FLAG::HANG):
		pMonster->ChangeState(ENUM_CLASS(GAVELEN_STATE::HANG));
		break;
	}
}

CGS_Attack* CGS_Attack::Create()
{
	CGS_Attack* pInstance = new CGS_Attack();
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed Created : CGS_Attack"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CGS_Attack::Free()
{
	__super::Free();
}

namespace {
	struct GS_AttackRegister {
		GS_AttackRegister() {
			CMonsterStateFactory::GetInstance()->Register(ENUM_CLASS(MONSTER_STATE_TYPE::GLASGAVELEN), ENUM_CLASS(GAVELEN_STATE::ATTACK), []()->CMonsterState* { return CGS_Attack::Create(); });
		};

	};
	static GS_AttackRegister  Register;
}