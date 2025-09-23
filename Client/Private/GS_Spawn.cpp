#include "ClientPch.h"
#include "GS_Spawn.h"

CGS_Spawn::CGS_Spawn()
{
}

HRESULT CGS_Spawn::Initialize()
{
	m_iStateFlag = ENUM_CLASS(STATE_FLAG::SPAWN);

	return S_OK;
}

void CGS_Spawn::Enter(CMonster* pMonster)
{
	ChangeActionFlag(ENUM_CLASS(SPAWN_FLAG::STAY));
}

void CGS_Spawn::Update(CMonster* pMonster, _float fTimeDelta)
{
}

void CGS_Spawn::Exit(CMonster* pMonster)
{
	m_iStateFlag = ENUM_CLASS(STATE_FLAG::SPAWN);
}

CGS_Spawn* CGS_Spawn::Create()
{
	CGS_Spawn* pInstance = new CGS_Spawn();
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed Created : CGS_Spawn"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CGS_Spawn::Free()
{
	__super::Free();
}

namespace {
	struct GS_SpawnRegister {
		GS_SpawnRegister() {
			CMonsterStateFactory::GetInstance()->Register(ENUM_CLASS(MONSTER_STATE_TYPE::GLASGAVELEN), ENUM_CLASS(GAVELEN_STATE::SPAWN), []()->CMonsterState* { return CGS_Spawn::Create(); });
		};

	};
	static GS_SpawnRegister  Register;
}