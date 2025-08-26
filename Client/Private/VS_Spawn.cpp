#include "ClientPch.h"
#include "VS_Spawn.h"

CVS_Spawn::CVS_Spawn()
{
}

HRESULT CVS_Spawn::Initialize()
{
	m_iStateFlag = ENUM_CLASS(STATE_FLAG::SPAWN);

	return S_OK;
}

void CVS_Spawn::Enter(CMonster* pMonster)
{
	m_iStateFlag = ENUM_CLASS(STATE_FLAG::SPAWN);
}

void CVS_Spawn::Update(CMonster* pMonster, _float fTimeDelta)
{
}

void CVS_Spawn::Exit(CMonster* pMonster)
{
	m_iStateFlag = ENUM_CLASS(STATE_FLAG::SPAWN);
}

CVS_Spawn* CVS_Spawn::Create()
{
	CVS_Spawn* pInstance = new CVS_Spawn();
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed Created : CVS_Spawn"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CVS_Spawn::Free()
{
	__super::Free();
}

namespace {
	struct VS_SpawnRegister {
		VS_SpawnRegister() {
			CMonsterStateFactory::GetInstance()->Register(ENUM_CLASS(MONSTER_STATE_TYPE::VAMPIRE), ENUM_CLASS(VAMPIRE_STATE::SPAWN), []()->CMonsterState* { return CVS_Spawn::Create(); });
		};

	};
	static VS_SpawnRegister  Register;
}