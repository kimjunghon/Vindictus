#include "ClientPch.h"
#include "QS_NearAttack.h"
#include "Monster.h"

CQS_NearAttack::CQS_NearAttack()
{
}

HRESULT CQS_NearAttack::Initialize()
{
    m_fCheckDegree = 45.f;

	m_iStateFlag = ENUM_CLASS(STATE_FLAG::ATTACK);

    return S_OK;
}

void CQS_NearAttack::Enter(CMonster* pMonster)
{
    _uint iDir = ENUM_CLASS(pMonster->Compute_TargetDir(m_fCheckDegree));

    _uint iAttackFlag = ENUM_CLASS(ATTACK_FLAG::MELLE) << iDir;

	ChangeActionFlag(iAttackFlag);
}

void CQS_NearAttack::Update(CMonster* pMonster, _float fTimeDelta)
{
}

void CQS_NearAttack::Exit(CMonster* pMonster)
{
	m_iStateFlag = ENUM_CLASS(STATE_FLAG::ATTACK);
}

CQS_NearAttack* CQS_NearAttack::Create()
{
	CQS_NearAttack* pInstance = new CQS_NearAttack();
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed Created : CQS_NearAttack"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CQS_NearAttack::Free()
{
	__super::Free();
}


namespace {
	struct QS_NearAttackRegister {
		QS_NearAttackRegister() {
			CMonsterStateFactory::GetInstance()->Register(ENUM_CLASS(MONSTER_STATE_TYPE::QUEEN), ENUM_CLASS(QUEEN_STATE::ATTACK_NEAR), []()->CMonsterState* { return CQS_NearAttack::Create(); });
		};

	};
	static QS_NearAttackRegister  Register;
}