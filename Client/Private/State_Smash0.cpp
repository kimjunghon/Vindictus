#include "ClientPch.h"
#include "State_Smash0.h"
#include "PlayerPawn.h"
#include "StateFactory.h"

CState_Smash0::CState_Smash0()
{
}

HRESULT CState_Smash0::Initialize()
{
    m_iStateFlag = ENUM_CLASS(STATE_FLAG::SMASH);

	m_fFullChargeTime = 0.7f;

    return S_OK;
}

void CState_Smash0::Enter(CPlayerPawn* pPlayerPawn)
{
    ChangeActionFlag(ENUM_CLASS(SMASH_FLAG::SMASH0_CHARGE));

    m_bSmashEnd = false;
	m_bCharging = false;
    m_fChargeTime = 0.f;
}

void CState_Smash0::InputData(CPlayerPawn* pPlayerPawn, INPUT_MOVE_DESC MoveInput, INPUT_ACTION_DESC ActionInput)
{
	if (m_bSmashEnd)
	{
		Change_OtherState(pPlayerPawn, MoveInput, ActionInput);
		return;
	}
	if (ActionInput.byAction & ENUM_CLASS(ACTION_INPUT::SMASH))
	{
		if (pPlayerPawn->AnimIsFinished())
		{
			ChangeActionFlag(ENUM_CLASS(SMASH_FLAG::SMASH0_CHARGE_END));
			m_bSmashEnd = true;
			m_fChargeTime = 0.f;
		}
		else
		{
			ChangeActionFlag(ENUM_CLASS(SMASH_FLAG::SMASH0_CHARGE));
			m_bCharging = true;
		}
	}
	else if (~ActionInput.byAction & ENUM_CLASS(ACTION_INPUT::SMASH))
	{
		if(m_fChargeTime >= 0.7f)
			ChangeActionFlag(ENUM_CLASS(SMASH_FLAG::SMASH0_CHARGE_END));	
		else
			ChangeActionFlag(ENUM_CLASS(SMASH_FLAG::SMASH0));

		m_bSmashEnd = true;
		m_fChargeTime = 0.f;
	}
}

void CState_Smash0::Update(CPlayerPawn* pPlayerPawn, _float fTimeDelta)
{
	if(m_bCharging)
		m_fChargeTime += fTimeDelta;
}

void CState_Smash0::Exit(CPlayerPawn* pPlayerPawn)
{
	m_iStateFlag = ENUM_CLASS(STATE_FLAG::SMASH);
	pPlayerPawn->Reset_ComboCount();
}

CState_Smash0* CState_Smash0::Create()
{
	CState_Smash0* pInstance = new CState_Smash0();
	if(FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed Created : CState_Smash0"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CState_Smash0::Free()
{
	__super::Free();
}

namespace {
	struct StateSmash0Register {
		StateSmash0Register() {
			CStateFactory::GetInstance()->Register(ENUM_CLASS(PLAYER_STATE::SMASH0), []()->CPlayerState* { return CState_Smash0::Create(); });
		};

	};
	static StateSmash0Register RegisterIdle;
}