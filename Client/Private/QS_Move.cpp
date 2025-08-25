#include "ClientPch.h"
#include "QS_Move.h"
#include "Monster.h"

CQS_Move::CQS_Move()
{
}

HRESULT CQS_Move::Initialize()
{
	return S_OK;
}

void CQS_Move::Enter(CMonster* pMonster)
{
	m_iStateFlag = ENUM_CLASS(STATE_FLAG::MOVE) | ENUM_CLASS(MOVE_FLAG::RUN);
}

void CQS_Move::Update(CMonster* pMonster, _float fTimeDelta)
{
	pMonster->LookAtTarget();
}

void CQS_Move::Exit(CMonster* pMonster)
{
}

CQS_Move* CQS_Move::Create()
{
	CQS_Move* pInstance = new CQS_Move();
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed Created : CQS_Move"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CQS_Move::Free()
{
	__super::Free();
}

namespace {
	struct QS_MoveRegister {
		QS_MoveRegister() {
			CMonsterStateFactory::GetInstance()->Register(ENUM_CLASS(MONSTER_STATE_TYPE::QUEEN), ENUM_CLASS(QUEEN_STATE::MOVE), []()->CMonsterState* { return CQS_Move::Create(); });
		};

	};
	static QS_MoveRegister  Register;
}