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

    return S_OK;
}

void CQS_Burrow::Enter(CMonster* pMonster)
{
	for (_uint i = 0; i < ENUM_CLASS(BURROW_ATTACK::END); i++)
		m_IsBurrowAction[i] = true;

	m_eBurrowState = BURROW_STATE::BEGIN;

	m_iStateFlag = ENUM_CLASS(STATE_FLAG::BURROW) | ENUM_CLASS(BURROW_FLAG::BEGIN);
}

void CQS_Burrow::Update(CMonster* pMonster, _float fTimeDelta)
{
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
		m_iStateFlag = ENUM_CLASS(STATE_FLAG::BURROW) | ENUM_CLASS(BURROW_FLAG::END);
		pMonster->MoveToTarget(0.6f);
		return;
	}

	_uint iFlag = ENUM_CLASS(STATE_FLAG::BURROW);
	_uint iActionFlag = ENUM_CLASS(BURROW_FLAG::MOVE);

	_uint iRandomIndex = CanAttackIndex[rand() % CanAttackIndex.size()];

	m_IsBurrowAction[iRandomIndex] = false;

	m_iStateFlag = iFlag | (iActionFlag << iRandomIndex);

	pMonster->LookAtTarget();
}

void CQS_Burrow::Move(CMonster* pMonster, _float fTimeDelta)
{
	m_iStateFlag = ENUM_CLASS(STATE_FLAG::BURROW) | ENUM_CLASS(BURROW_FLAG::STAY);

	pMonster->MoveToTarget(0.05);
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