#include "ClientPch.h"
#include "State_Smash3.h"
#include "PlayerPawn.h"
#include "StateFactory.h"

CState_Smash3::CState_Smash3()
{
}

HRESULT CState_Smash3::Initialize()
{
	m_iStateFlag = ENUM_CLASS(STATE_FLAG::SMASH);
	m_iActionFlag = ENUM_CLASS(SMASH_FLAG::SMASH3_0);
	m_iMaxComboCount = 2;

	m_fKeepTime = 0.5f;

	return S_OK;
}

void CState_Smash3::Enter(CPlayerPawn* pPlayerPawn)
{
	ChangeActionFlag(ENUM_CLASS(SMASH_FLAG::SMASH3_0));

	m_iComboCount = 0;

	m_bReadySmash = false;
}

void CState_Smash3::InputData(CPlayerPawn* pPlayerPawn, INPUT_MOVE_DESC MoveInput, INPUT_ACTION_DESC ActionInput)
{
	if (ActionInput.byAction & ENUM_CLASS(ACTION_INPUT::SMASH) || m_bReadySmash)
	{

		if (pPlayerPawn->AnimCanChange())
		{
			m_bReadySmash = false;

			if (MoveInput.bMove)
				pPlayerPawn->Compute_PlayerMoveDir();

			m_iComboCount++;
			if (m_iComboCount >= m_iMaxComboCount)
				m_iComboCount = m_iMaxComboCount;

			ChangeActionFlag(m_iActionFlag << m_iComboCount);
		}
		else
		{
			if (false == m_bReadySmash)
			{
				m_bReadySmash = true;
				m_fCurrentKeepTime = 0.f;
			}
		}
	}
	else
	{
		Change_OtherState(pPlayerPawn, MoveInput, ActionInput);
	}
}

void CState_Smash3::Update(CPlayerPawn* pPlayerPawn, _float fTimeDelta)
{
	if (m_bReadySmash)
	{
		m_fCurrentKeepTime += fTimeDelta;
		if (m_fCurrentKeepTime >= m_fKeepTime)
			m_bReadySmash = false;
	}
}

void CState_Smash3::Exit(CPlayerPawn* pPlayerPawn)
{
	m_iStateFlag = ENUM_CLASS(STATE_FLAG::SMASH);
	pPlayerPawn->Reset_ComboCount();
}

CState_Smash3* CState_Smash3::Create()
{
	CState_Smash3* pInstance = new CState_Smash3();
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed Created : CState_Smash3"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CState_Smash3::Free()
{
	__super::Free();
}

namespace {
	struct StateSmash2Register {
		StateSmash2Register() {
			CStateFactory::GetInstance()->Register(ENUM_CLASS(PLAYER_STATE::SMASH3), []()->CPlayerState* { return CState_Smash3::Create(); });
		};

	};
	static StateSmash2Register RegisterIdle;
}