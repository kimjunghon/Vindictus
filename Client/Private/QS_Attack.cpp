#include "ClientPch.h"
#include "QS_Attack.h"
#include "Monster.h"

CQS_Attack::CQS_Attack()
{
}

HRESULT CQS_Attack::Initialize()
{
	m_iStateFlag = ENUM_CLASS(STATE_FLAG::ATTACK);
	return S_OK;
}

void CQS_Attack::Enter(CMonster* pMonster)
{
	m_iStateFlag = ENUM_CLASS(STATE_FLAG::ATTACK);

	_uint iAttackIndex = pMonster->Get_AttackIndex();

	_uint iAttackFlag = ENUM_CLASS(ATTACK_FLAG::SWOOP) << iAttackIndex;

	if (iAttackFlag & ENUM_CLASS(ATTACK_FLAG::JUMP))
		pMonster->ChangeState(ENUM_CLASS(QUEEN_STATE::JUMP));
	else
		ChangeActionFlag(iAttackFlag);
}

void CQS_Attack::Update(CMonster* pMonster, _float fTimeDelta)
{
	if (pMonster->IsReadyAttack(m_iStateFlag))
		pMonster->TurnToTarget(fTimeDelta);
}

void CQS_Attack::Exit(CMonster* pMonster)
{
	m_iStateFlag = ENUM_CLASS(STATE_FLAG::ATTACK);
}

CQS_Attack* CQS_Attack::Create()
{
	CQS_Attack* pInstance = new CQS_Attack();
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed Created : CQS_Attack"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CQS_Attack::Free()
{
	__super::Free();
}


namespace {
	struct QS_AttackRegister {
		QS_AttackRegister() {
			CMonsterStateFactory::GetInstance()->Register(ENUM_CLASS(MONSTER_STATE_TYPE::QUEEN), ENUM_CLASS(QUEEN_STATE::ATTACK), []()->CMonsterState* { return CQS_Attack::Create(); });
		};

	};
	static QS_AttackRegister  Register;
}