#include "ClientPch.h"
#include "QS_Jump.h"
#include "Monster.h"

CQS_Jump::CQS_Jump()
{
}

HRESULT CQS_Jump::Initialize()
{
	m_vJumpReadyTime = _float2(0.f, 84.f);
	m_vJumpMoveTime = _float2(85.f, 135.f);

	return S_OK;
}

void CQS_Jump::Enter(CMonster* pMonster)
{
	m_iStateFlag = ENUM_CLASS(STATE_FLAG::ATTACK) | ENUM_CLASS(ATTACK_FLAG::JUMP);
}

void CQS_Jump::Update(CMonster* pMonster, _float fTimeDelta)
{
	if (pMonster->IsAnimationInRangeTrackPosition(m_vJumpReadyTime))
		pMonster->LookAtTarget();
	else if (pMonster->IsAnimationInRangeTrackPosition(m_vJumpMoveTime))
		pMonster->MoveToTarget(fTimeDelta);
}

void CQS_Jump::Exit(CMonster* pMonster)
{
}

CQS_Jump* CQS_Jump::Create()
{
	CQS_Jump* pInstance = new CQS_Jump();
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed Created : CQS_Jump"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CQS_Jump::Free()
{
	__super::Free();
}


namespace {
	struct QS_JumpRegister {
		QS_JumpRegister() {
			CMonsterStateFactory::GetInstance()->Register(ENUM_CLASS(MONSTER_STATE_TYPE::QUEEN), ENUM_CLASS(QUEEN_STATE::JUMP), []()->CMonsterState* { return CQS_Jump::Create(); });
		};

	};
	static QS_JumpRegister  Register;
}