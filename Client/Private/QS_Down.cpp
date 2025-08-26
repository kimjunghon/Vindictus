#include "ClientPch.h"
#include "QS_Down.h"
#include "Monster.h"

CQS_Down::CQS_Down()
{
}

_bool CQS_Down::CanStateChange(CMonster* pMonster)
{
	if (m_iAnimPhase == ENUM_CLASS(DOWN_PHASE::END))
	{
		return (pMonster->AnimCanChange() || pMonster->AnimIsFinished());
	}

	return false;
}

HRESULT CQS_Down::Initialize()
{
	m_iStateFlag = ENUM_CLASS(STATE_FLAG::HIT);

	m_iStrongFlag = ENUM_CLASS(HIT_FLAG::DOWN_BEGIN);

	return S_OK;
}

void CQS_Down::Enter(CMonster* pMonster)
{
	ChangeActionFlag(ENUM_CLASS(HIT_FLAG::DOWN_BEGIN));

	m_iAnimPhase = ENUM_CLASS(DOWN_PHASE::BEGIN);

	pMonster->LookAtTarget();
}

void CQS_Down::Update(CMonster* pMonster, _float fTimeDelta)
{
	if(pMonster->AnimIsFinished() && m_iAnimPhase < ENUM_CLASS(DOWN_PHASE::END))
	{
		m_iAnimPhase++;

		_uint iActionFlag = m_iStrongFlag << m_iAnimPhase;
		
		ChangeActionFlag(iActionFlag);
	}
}

void CQS_Down::Exit(CMonster* pMonster)
{
	m_iAnimPhase = 0;
	m_iStateFlag = ENUM_CLASS(STATE_FLAG::HIT);
}

CQS_Down* CQS_Down::Create()
{
	CQS_Down* pInstance = new CQS_Down();
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed Created : CQS_Down"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CQS_Down::Free()
{
	__super::Free();
}

namespace {
	struct QS_DownRegister {
		QS_DownRegister() {
			CMonsterStateFactory::GetInstance()->Register(ENUM_CLASS(MONSTER_STATE_TYPE::QUEEN), ENUM_CLASS(QUEEN_STATE::DOWN), []()->CMonsterState* { return CQS_Down::Create(); });
		};

	};
	static QS_DownRegister   Register;
}