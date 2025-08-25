#include "ClientPch.h"
#include "QS_Turn.h"
#include "Monster.h"

CQS_Turn::CQS_Turn()
{
}

HRESULT CQS_Turn::Initialize()
{
	m_fBackDegree = 30.f;

	return S_OK;
}

void CQS_Turn::Enter(CMonster* pMonster)
{
	m_iStateFlag = ENUM_CLASS(STATE_FLAG::MOVE);

	DIR eRightDir = pMonster->Compute_TargetDir(0.f);

	DIR eBackDir = pMonster->Compute_TargetDir(m_fBackDegree);

	m_iStateFlag |= GetTurnFlag(eRightDir, eBackDir);
}

void CQS_Turn::Update(CMonster* pMonster, _float fTimeDelta)
{
}

void CQS_Turn::Exit(CMonster* pMonster)
{
}

_uint CQS_Turn::GetTurnFlag(DIR eRightDir, DIR eBackDir)
{
	_uint iFlag = {};

	switch (eRightDir)
	{
	case DIR::RIGHT:
		iFlag = ENUM_CLASS(MOVE_FLAG::TURN_RIGHT);
		break;
	case DIR::LEFT:
		iFlag = ENUM_CLASS(MOVE_FLAG::TURN_LEFT);
		break;
	}

	if (eBackDir == DIR::BACK)
		iFlag = iFlag << 2;

	return iFlag;
}

CQS_Turn* CQS_Turn::Create()
{
	CQS_Turn* pInstance = new CQS_Turn();
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed Created : CQS_Turn"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CQS_Turn::Free()
{
	__super::Free();
}

namespace {
	struct QS_TurnRegister {
		QS_TurnRegister() {
			CMonsterStateFactory::GetInstance()->Register(ENUM_CLASS(MONSTER_STATE_TYPE::QUEEN), ENUM_CLASS(QUEEN_STATE::TURN), []()->CMonsterState* { return CQS_Turn::Create(); });
		};

	};
	static QS_TurnRegister  Register;
}