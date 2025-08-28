#include "ClientPch.h"
#include "VS_ElderAttack.h"
#include "Monster.h"

CVS_ElderAttack::CVS_ElderAttack()
{
}

HRESULT CVS_ElderAttack::Initialize()
{
	m_iStateFlag = ENUM_CLASS(STATE_FLAG::ATTACK);

    return S_OK;
}

void CVS_ElderAttack::Enter(CMonster* pMonster)
{
	_float fMinDistance = pMonster->Get_MinDistance();

	_uint iAttackFlag = pMonster->IsNear(fMinDistance) ? ENUM_CLASS(ATTACK_FLAG::MELEE) : ENUM_CLASS(ATTACK_FLAG::RANGE);
		
	ChangeActionFlag(iAttackFlag);
}

void CVS_ElderAttack::Update(CMonster* pMonster, _float fTimeDelta)
{
	if (pMonster->IsReadyAttack())
		pMonster->LookAtTarget();
//		pMonster->TurnToTarget(fTimeDelta);
}

void CVS_ElderAttack::Exit(CMonster* pMonster)
{
	m_iStateFlag = ENUM_CLASS(STATE_FLAG::ATTACK);
}

CVS_ElderAttack* CVS_ElderAttack::Create()
{
	CVS_ElderAttack* pInstance = new CVS_ElderAttack();
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed Created : CVS_ElderAttack"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CVS_ElderAttack::Free()
{
	__super::Free();
}


namespace {
	struct VS_ElderAttackRegister {
		VS_ElderAttackRegister() {
			CMonsterStateFactory::GetInstance()->Register(ENUM_CLASS(MONSTER_STATE_TYPE::VAMPIRE_ELDER), ENUM_CLASS(VAMPIRE_STATE::ATTACK), []()->CMonsterState* { return CVS_ElderAttack::Create(); });
		};

	};
	static VS_ElderAttackRegister  Register;
}