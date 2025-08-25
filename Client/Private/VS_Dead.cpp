#include "ClientPch.h"
#include "VS_Dead.h"
#include "Monster.h"

CVS_Dead::CVS_Dead()
{
}

_bool CVS_Dead::CanStateChange(CMonster* pMonster)
{
	return false;
}

HRESULT CVS_Dead::Initialize()
{
	return S_OK;
}

void CVS_Dead::Enter(CMonster* pMonster)
{
	m_iStateFlag = ENUM_CLASS(STATE_FLAG::DEAD) | ENUM_CLASS(DEAD_FLAG::DEFAULT);
}

void CVS_Dead::Update(CMonster* pMonster, _float fTimeDelta)
{
	if (pMonster->AnimIsFinished())
		pMonster->Dead();
}

void CVS_Dead::Exit(CMonster* pMonster)
{
}

CVS_Dead* CVS_Dead::Create()
{
	CVS_Dead* pInstance = new CVS_Dead();
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed Created : CVS_Dead"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CVS_Dead::Free()
{
	__super::Free();
}

namespace {
	struct VS_DeadStrongRegister {
		VS_DeadStrongRegister() {
			CMonsterStateFactory::GetInstance()->Register(ENUM_CLASS(MONSTER_STATE_TYPE::VAMPIRE), ENUM_CLASS(VAMPIRE_STATE::DEAD), []()->CMonsterState* { return CVS_Dead::Create(); });
		};

	};
	static VS_DeadStrongRegister  Register;
}