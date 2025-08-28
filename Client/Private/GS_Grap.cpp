#include "ClientPch.h"
#include "GS_Grap.h"
#include "Glasgavelen.h"

CGS_Grap::CGS_Grap()
{
}

_bool CGS_Grap::CanStateChange(CMonster* pMonster)
{
	if (m_eGrapPhase == GRAP_PHASE::END)
		return pMonster->AnimCanChange() || pMonster->AnimIsFinished();

	return false;
}

HRESULT CGS_Grap::Initialize()
{
	m_iStateFlag = ENUM_CLASS(STATE_FLAG::GRAP);

	return S_OK;
}

void CGS_Grap::Enter(CMonster* pMonster)
{
	m_eGrapPhase = GRAP_PHASE::BEGIN;

	ChangeActionFlag(ENUM_CLASS(GRAP_FLAG::TRY));

	m_IsChange = true;
}

void CGS_Grap::Update(CMonster* pMonster, _float fTimeDelta)
{
	if (m_IsChange)
	{
		m_IsChange = false;
		return;
	}

	if (m_eGrapPhase == GRAP_PHASE::BEGIN )
	{
		if (pMonster->AnimIsFinished())
		{
			if (static_cast<CGlasgavelen*>(pMonster)->IsGrap())
				ChangeActionFlag(ENUM_CLASS(GRAP_FLAG::SUCCESS));
			else
				ChangeActionFlag(ENUM_CLASS(GRAP_FLAG::FAIL));

			m_eGrapPhase = GRAP_PHASE::END;
		}

		else if (pMonster->IsReadyAttack())
			pMonster->LookAtTarget();
	}
}

void CGS_Grap::Exit(CMonster* pMonster)
{
	m_iStateFlag = ENUM_CLASS(STATE_FLAG::GRAP);
	static_cast<CGlasgavelen*>(pMonster)->SetIsGrap(false);
}

CGS_Grap* CGS_Grap::Create()
{
	CGS_Grap* pInstance = new CGS_Grap();
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed Created : CGS_Grap"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CGS_Grap::Free()
{
	__super::Free();
}

namespace {
	struct GS_GrapRegister {
		GS_GrapRegister() {
			CMonsterStateFactory::GetInstance()->Register(ENUM_CLASS(MONSTER_STATE_TYPE::GLASGAVELEN), ENUM_CLASS(GAVELEN_STATE::GRAP), []()->CMonsterState* { return CGS_Grap::Create(); });
		};

	};
	static GS_GrapRegister  Register;
}