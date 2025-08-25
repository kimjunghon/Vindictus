#include "ClientPch.h"
#include "QS_Dead.h"
#include "Monster.h"

CQS_Dead::CQS_Dead()
{
}

_bool CQS_Dead::CanStateChange(CMonster* pMonster)
{
	return false;
}

HRESULT CQS_Dead::Initialize()
{
	return S_OK;
}

void CQS_Dead::Enter(CMonster* pMonster)
{
	m_iStateFlag = ENUM_CLASS(STATE_FLAG::DEAD);
}

void CQS_Dead::Update(CMonster* pMonster, _float fTimeDelta)
{
	if (pMonster->AnimIsFinished())
		pMonster->Dead();
}

void CQS_Dead::Exit(CMonster* pMonster)
{
}

CQS_Dead* CQS_Dead::Create()
{
	CQS_Dead* pInstance = new CQS_Dead();
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed Created : CQS_Dead"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CQS_Dead::Free()
{
	__super::Free();
}

namespace {
	struct QS_DeadStrongRegister {
		QS_DeadStrongRegister() {
			CMonsterStateFactory::GetInstance()->Register(ENUM_CLASS(MONSTER_STATE_TYPE::QUEEN), ENUM_CLASS(QUEEN_STATE::DEAD), []()->CMonsterState* { return CQS_Dead::Create(); });
		};

	};
	static QS_DeadStrongRegister  Register;
}