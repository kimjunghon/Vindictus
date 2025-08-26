#include "ClientPch.h"
#include "VS_Attack.h"
#include "Monster.h"

CVS_Attack::CVS_Attack()
{
}

HRESULT CVS_Attack::Initialize()
{
	m_iStateFlag = ENUM_CLASS(STATE_FLAG::ATTACK);

	return S_OK;
}

void CVS_Attack::Enter(CMonster* pMonster)
{
	m_iStateFlag = ENUM_CLASS(STATE_FLAG::ATTACK);
}

void CVS_Attack::Update(CMonster* pMonster, _float fTimeDelta)
{
	if(pMonster->IsReadyAttack(m_iStateFlag))
		pMonster->LookAtTarget();
}

void CVS_Attack::Exit(CMonster* pMonster)
{
	m_iStateFlag = ENUM_CLASS(STATE_FLAG::ATTACK);
}

CVS_Attack* CVS_Attack::Create()
{
	CVS_Attack* pInstance = new CVS_Attack();
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed Created : CVS_Attack"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CVS_Attack::Free()
{
	__super::Free();
}


namespace {
	struct VS_AttackRegister {
		VS_AttackRegister() {
			CMonsterStateFactory::GetInstance()->Register(ENUM_CLASS(MONSTER_STATE_TYPE::VAMPIRE), ENUM_CLASS(VAMPIRE_STATE::ATTACK), []()->CMonsterState* { return CVS_Attack::Create(); });
		};

	};
	static VS_AttackRegister  Register;
}