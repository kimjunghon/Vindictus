#include "ClientPch.h"
#include "VS_Move.h"
#include "Monster.h"

CVS_Move::CVS_Move()
{
}

HRESULT CVS_Move::Initialize()
{
	return S_OK;
}

void CVS_Move::Enter(CMonster* pMonster)
{
	m_iStateFlag = ENUM_CLASS(STATE_FLAG::MOVE) | ENUM_CLASS(MOVE_FLAG::FRONT);
}

void CVS_Move::Update(CMonster* pMonster, _float fTimeDelta)
{
	pMonster->LookAtTarget();
}

void CVS_Move::Exit(CMonster* pMonster)
{
}

CVS_Move* CVS_Move::Create()
{
	CVS_Move* pInstance = new CVS_Move();
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed Created : CVS_Move"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CVS_Move::Free()
{
	__super::Free();
}

namespace {
	struct VS_MoveRegister {
		VS_MoveRegister() {
			CMonsterStateFactory::GetInstance()->Register(ENUM_CLASS(MONSTER_STATE_TYPE::VAMPIRE), ENUM_CLASS(VAMPIRE_STATE::MOVE), []()->CMonsterState* { return CVS_Move::Create(); });
		};

	};
	static VS_MoveRegister  Register;
}