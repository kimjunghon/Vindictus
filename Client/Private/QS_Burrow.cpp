#include "ClientPch.h"
#include "QS_Burrow.h"
#include "Monster.h"

CQS_Burrow::CQS_Burrow()
{
}

_bool CQS_Burrow::CanStateChange(CMonster* pMonster)
{
	if (m_eBurrowState == BURROW_STATE::END)
	{
		return (pMonster->AnimCanChange() || pMonster->AnimIsFinished());
	}

	return false;
}

HRESULT CQS_Burrow::Initialize()
{
	m_iStateFlag = ENUM_CLASS(STATE_FLAG::BURROW);
    return S_OK;
}

void CQS_Burrow::Enter(CMonster* pMonster)
{
	for (_uint i = 0; i < ENUM_CLASS(BURROW_ATTACK::END); i++)
		m_IsBurrowAction[i] = true;

	m_eBurrowState = BURROW_STATE::BEGIN;

	ChangeActionFlag(ENUM_CLASS(BURROW_FLAG::BEGIN));
}

void CQS_Burrow::Update(CMonster* pMonster, _float fTimeDelta)
{
	if (pMonster->IsReadyAttack())
		pMonster->TurnToTarget(fTimeDelta * 2.f);

	if (pMonster->AnimCanChange() || pMonster->AnimIsFinished())
	{
		switch (m_eBurrowState)
		{
		case BURROW_STATE::BEGIN:
			m_eBurrowState = BURROW_STATE::DURING;
			break;
		case BURROW_STATE::DURING:
			Update_During(pMonster, fTimeDelta);
			break;
		case BURROW_STATE::END:
			break;
		}
	}

}

void CQS_Burrow::Exit(CMonster* pMonster)
{
	m_eBurrowState = BURROW_STATE::BEGIN;
	m_iStateFlag = ENUM_CLASS(STATE_FLAG::BURROW);
}

void CQS_Burrow::Update_During(CMonster* pMonster, _float fTimeDelta)
{
	if (pMonster->IsNear(pMonster->Get_AttackRange()))
		Attack(pMonster, fTimeDelta);
	else
		Move(pMonster, fTimeDelta);
}

void CQS_Burrow::Attack(CMonster* pMonster, _float fTimeDelta)
{
	vector<_uint> CanAttackIndex;

	for (_uint i = 0; i < ENUM_CLASS(BURROW_ATTACK::END); i++)
	{
		if (m_IsBurrowAction[i])
			CanAttackIndex.push_back(i);
	}

	if (CanAttackIndex.empty())
	{
		m_eBurrowState = BURROW_STATE::END;

		ChangeActionFlag(ENUM_CLASS(BURROW_FLAG::END));
		pMonster->MoveToTarget(0.6f);
		return;
	}

	_uint iRandomIndex = CanAttackIndex[rand() % CanAttackIndex.size()];

	_uint iActionFlag = ENUM_CLASS(BURROW_FLAG::MOVE) << iRandomIndex;

	m_IsBurrowAction[iRandomIndex] = false;

	ChangeActionFlag(iActionFlag);

	pMonster->LookAtTarget();
}

void CQS_Burrow::Move(CMonster* pMonster, _float fTimeDelta)
{
	ChangeActionFlag(ENUM_CLASS(BURROW_FLAG::STAY));
	pMonster->MoveToTarget(0.05f);
}

CQS_Burrow* CQS_Burrow::Create()
{
	CQS_Burrow* pInstance = new CQS_Burrow();
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed Created : CQS_Burrow"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CQS_Burrow::Free()
{
	__super::Free();
}

namespace {
	struct QS_BurrowRegister {
		QS_BurrowRegister() {
			CMonsterStateFactory::GetInstance()->Register(ENUM_CLASS(MONSTER_STATE_TYPE::QUEEN), ENUM_CLASS(QUEEN_STATE::BURROW), []()->CMonsterState* { return CQS_Burrow::Create(); });
		};

	};
	static QS_BurrowRegister   Register;
}