#include "ClientPch.h"
#include "QS_Idle.h"

CQS_Idle::CQS_Idle()
{
}

HRESULT CQS_Idle::Initialize()
{
	m_iStateFlag = ENUM_CLASS(STATE_FLAG::IDLE);

	return S_OK;
}

void CQS_Idle::Enter(CMonster* pMonster)
{
	ChangeActionFlag(ENUM_CLASS(IDLE_FLAG::THREAT));
}

void CQS_Idle::Update(CMonster* pMonster, _float fTimeDelta)
{
}

void CQS_Idle::Exit(CMonster* pMonster)
{
	m_iStateFlag = ENUM_CLASS(STATE_FLAG::IDLE);
}

CQS_Idle* CQS_Idle::Create()
{
	CQS_Idle* pInstance = new CQS_Idle();
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed Created : CQS_Idle"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CQS_Idle::Free()
{
	__super::Free();
}

namespace {
	struct QS_IdleRegister {
		QS_IdleRegister() {
			CMonsterStateFactory::GetInstance()->Register(ENUM_CLASS(MONSTER_STATE_TYPE::QUEEN), ENUM_CLASS(QUEEN_STATE::IDLE), []()->CMonsterState* { return CQS_Idle::Create(); });
		};

	};
	static QS_IdleRegister  Register;
}