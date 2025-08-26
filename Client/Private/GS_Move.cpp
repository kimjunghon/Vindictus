#include "ClientPch.h"
#include "GS_Move.h"
#include "Monster.h"

CGS_Move::CGS_Move()
{
}

HRESULT CGS_Move::Initialize()
{
	m_iStateFlag = ENUM_CLASS(STATE_FLAG::MOVE);

	return S_OK;
}

void CGS_Move::Enter(CMonster* pMonster)
{
	ChangeActionFlag(ENUM_CLASS(MOVE_FLAG::RUN));
}

void CGS_Move::Update(CMonster* pMonster, _float fTimeDelta)
{
	pMonster->LookAtTarget();
//	pMonster->TurnToTarget(fTimeDelta);
}

void CGS_Move::Exit(CMonster* pMonster)
{
	m_iStateFlag = ENUM_CLASS(STATE_FLAG::MOVE);
}

CGS_Move* CGS_Move::Create()
{
	CGS_Move* pInstance = new CGS_Move();
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed Created : CGS_Move"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CGS_Move::Free()
{
	__super::Free();
}

namespace {
	struct GS_MoveRegister {
		GS_MoveRegister() {
			CMonsterStateFactory::GetInstance()->Register(ENUM_CLASS(MONSTER_STATE_TYPE::GLASGAVELEN), ENUM_CLASS(GAVELEN_STATE::MOVE), []()->CMonsterState* { return CGS_Move::Create(); });
		};

	};
	static GS_MoveRegister   Register;
}