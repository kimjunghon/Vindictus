#include "ClientPch.h"
#include "State_Smash2.h"
#include "PlayerPawn.h"
#include "StateFactory.h"

CState_Smash2::CState_Smash2()
{
}

HRESULT CState_Smash2::Initialize()
{
	m_iStateFlag = ENUM_CLASS(STATE_FLAG::SMASH);
	m_iActionFlag = ENUM_CLASS(SMASH_FLAG::SMASH2_0);
	m_iMaxComboCount = 2;

	return S_OK;
}

void CState_Smash2::Enter(CPlayerPawn* pPlayerPawn)
{
	ChangeActionFlag(ENUM_CLASS(SMASH_FLAG::SMASH2_0));

	m_iComboCount = 0;
}

void CState_Smash2::InputData(CPlayerPawn* pPlayerPawn, INPUT_MOVE_DESC MoveInput, INPUT_ACTION_DESC ActionInput)
{

	if (ActionInput.byAction & ENUM_CLASS(ACTION_INPUT::SMASH) && false == pPlayerPawn->AnimIsFinished() && pPlayerPawn->AnimCanChange())
	{
		if (MoveInput.bMove)
			pPlayerPawn->Compute_PlayerMoveDir();

		m_iComboCount++;
		if (m_iComboCount >= m_iMaxComboCount)
			m_iComboCount = m_iMaxComboCount;

		ChangeActionFlag(m_iActionFlag << m_iComboCount);
	}
	else
	{
		Change_OtherState(pPlayerPawn, MoveInput, ActionInput);
	}

}

void CState_Smash2::Update(CPlayerPawn* pPlayerPawn, _float fTimeDelta)
{
}

void CState_Smash2::Exit(CPlayerPawn* pPlayerPawn)
{
	m_iStateFlag = ENUM_CLASS(STATE_FLAG::SMASH);
	pPlayerPawn->Reset_ComboCount();
}

CState_Smash2* CState_Smash2::Create()
{
	CState_Smash2* pInstance = new CState_Smash2();
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed Created : CState_Smash2"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CState_Smash2::Free()
{
	__super::Free();
}

namespace {
	struct StateSmash2Register {
		StateSmash2Register() {
			CStateFactory::GetInstance()->Register(ENUM_CLASS(PLAYER_STATE::SMASH2), []()->CPlayerState* { return CState_Smash2::Create(); });
		};

	};
	static StateSmash2Register RegisterIdle;
}