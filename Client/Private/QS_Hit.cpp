#include "ClientPch.h"
#include "QS_Hit.h"
#include "Monster.h"

CQS_Hit::CQS_Hit()
{
}

HRESULT CQS_Hit::Initialize()
{
	m_fCheckDegree = 45.f;
	m_iStateFlag = ENUM_CLASS(STATE_FLAG::HIT);

	return S_OK;
}

void CQS_Hit::Enter(CMonster* pMonster)
{
	_uint iDir = ENUM_CLASS(pMonster->Compute_TargetDir(m_fCheckDegree));

	_uint iHitFlag = ENUM_CLASS(HIT_FLAG::FRONT) << iDir;

	ChangeActionFlag(iHitFlag);
}

void CQS_Hit::Update(CMonster* pMonster, _float fTimeDelta)
{
}

void CQS_Hit::Exit(CMonster* pMonster)
{
	m_iStateFlag = ENUM_CLASS(STATE_FLAG::HIT);
}

CQS_Hit* CQS_Hit::Create()
{
	CQS_Hit* pInstance = new CQS_Hit();
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed Created : CQS_Hit"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CQS_Hit::Free()
{
	__super::Free();
}

namespace {
	struct QS_HitRegister {
		QS_HitRegister() {
			CMonsterStateFactory::GetInstance()->Register(ENUM_CLASS(MONSTER_STATE_TYPE::QUEEN), ENUM_CLASS(QUEEN_STATE::HIT), []()->CMonsterState* { return CQS_Hit::Create(); });
		};

	};
	static QS_HitRegister  Register;
}