#include "ClientPch.h"
#include "VS_HitStrong.h"
#include "Monster.h"

CVS_HitStrong::CVS_HitStrong()
{
}

_bool CVS_HitStrong::CanStateChange(CMonster* pMonster)
{
	if (m_iAnimPhase == ENUM_CLASS(STRONG_PHASE::UP))
	{
		return (pMonster->AnimCanChange() || pMonster->AnimIsFinished());
	}

	return false;
}

HRESULT CVS_HitStrong::Initialize()
{
	m_iStateFlag = ENUM_CLASS(STATE_FLAG::HIT);

	m_iStrongFlag = ENUM_CLASS(HIT_FLAG::STRONG_BEGIN);

	return S_OK;
}

void CVS_HitStrong::Enter(CMonster* pMonster)
{
	ChangeActionFlag(ENUM_CLASS(HIT_FLAG::STRONG_BEGIN));

	m_iAnimPhase = ENUM_CLASS(STRONG_PHASE::BEGIN);

	pMonster->LookAtTarget();
}

void CVS_HitStrong::Update(CMonster* pMonster, _float fTimeDelta)
{
	if(pMonster->AnimIsFinished() && m_iAnimPhase < ENUM_CLASS(STRONG_PHASE::UP))
	{
		m_iAnimPhase++;

		_uint iActionFlag = m_iStrongFlag << m_iAnimPhase;

		ChangeActionFlag(iActionFlag);
	}
}

void CVS_HitStrong::Exit(CMonster* pMonster)
{
	m_iAnimPhase = 0;
	m_iStateFlag = ENUM_CLASS(STATE_FLAG::HIT);
}

CVS_HitStrong* CVS_HitStrong::Create()
{
	CVS_HitStrong* pInstance = new CVS_HitStrong();
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed Created : CVS_HitStrong"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CVS_HitStrong::Free()
{
	__super::Free();
}

namespace {
	struct VS_HitStrongRegister {
		VS_HitStrongRegister() {
			CMonsterStateFactory::GetInstance()->Register(ENUM_CLASS(MONSTER_STATE_TYPE::VAMPIRE), ENUM_CLASS(VAMPIRE_STATE::HIT_STRONG), []()->CMonsterState* { return CVS_HitStrong::Create(); });
		};

	};
	static VS_HitStrongRegister   Register;
}