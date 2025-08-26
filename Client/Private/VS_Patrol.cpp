#include "ClientPch.h"
#include "VS_Patrol.h"
#include "Monster.h"

CVS_Patrol::CVS_Patrol()
{
}

HRESULT CVS_Patrol::Initialize()
{
	m_iStateFlag = ENUM_CLASS(STATE_FLAG::MOVE);

	return S_OK;
}

void CVS_Patrol::Enter(CMonster* pMonster)
{
	if (pMonster->IsNear(pMonster->Get_MinDistance()))
		ChangeActionFlag(ENUM_CLASS(MOVE_FLAG::BACK));
	else
		rand() % 2 == 0 ? ChangeActionFlag(ENUM_CLASS(MOVE_FLAG::LEFT)) : ChangeActionFlag(ENUM_CLASS(MOVE_FLAG::RIGHT));
}

void CVS_Patrol::Update(CMonster* pMonster, _float fTimeDelta)
{
	pMonster->LookAtTarget();
}

void CVS_Patrol::Exit(CMonster* pMonster)
{
	m_iStateFlag = ENUM_CLASS(STATE_FLAG::MOVE);
}

CVS_Patrol* CVS_Patrol::Create()
{
	CVS_Patrol* pInstance = new CVS_Patrol();
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed Created : CMS_Spawn"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CVS_Patrol::Free()
{
	__super::Free();
}

namespace {
	struct VS_PatrolRegister {
		VS_PatrolRegister() {
			CMonsterStateFactory::GetInstance()->Register(ENUM_CLASS(MONSTER_STATE_TYPE::VAMPIRE), ENUM_CLASS(VAMPIRE_STATE::PATROL), []()->CMonsterState* { return CVS_Patrol::Create(); });
		};

	};
	static VS_PatrolRegister  Register;
}