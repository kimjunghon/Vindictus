#include "ClientPch.h"
#include "GS_Down.h"
#include "Glasgavelen.h"

CGS_Down::CGS_Down()
{
}

_bool CGS_Down::CanStateChange(CMonster* pMonster)
{
	if (m_iDownPhase >= ENUM_CLASS(DOWN_PHASE::END))
		return pMonster->AnimCanChange() || pMonster->AnimIsFinished();

	return false;
}

HRESULT CGS_Down::Initialize()
{
	m_iStateFlag = ENUM_CLASS(STATE_FLAG::HIT);

	m_iDownFlag = ENUM_CLASS(HIT_FLAG::DOWN_BEGIN);

    return S_OK;
}

void CGS_Down::Enter(CMonster* pMonster)
{
	ChangeActionFlag(m_iDownFlag);

	m_iDownPhase = ENUM_CLASS(DOWN_PHASE::BEGIN);
}

void CGS_Down::Update(CMonster* pMonster, _float fTimeDelta)
{
	if (pMonster->AnimIsFinished() && m_iDownPhase < ENUM_CLASS(DOWN_PHASE::END))
	{
		m_iDownPhase++;

		_uint iActionFlag = m_iDownFlag << m_iDownPhase;

		ChangeActionFlag(iActionFlag);
	}
}

void CGS_Down::Exit(CMonster* pMonster)
{
	m_iStateFlag = ENUM_CLASS(STATE_FLAG::HIT);

	m_iDownPhase = ENUM_CLASS(DOWN_PHASE::BEGIN);
}

CGS_Down* CGS_Down::Create()
{
	CGS_Down* pInstance = new CGS_Down();
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed Created : CGS_Down"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CGS_Down::Free()
{
	__super::Free();
}

namespace {
	struct GS_DownRegister {
		GS_DownRegister() {
			CMonsterStateFactory::GetInstance()->Register(ENUM_CLASS(MONSTER_STATE_TYPE::GLASGAVELEN), ENUM_CLASS(GAVELEN_STATE::DOWN), []()->CMonsterState* { return CGS_Down::Create(); });
		};

	};
	static GS_DownRegister  Register;
}