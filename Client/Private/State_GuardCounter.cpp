#include "ClientPch.h"
#include "State_GuardCounter.h"
#include "PlayerPawn.h"
#include "StateFactory.h"

CState_GuardCounter::CState_GuardCounter()
{
}

HRESULT CState_GuardCounter::Initialize()
{
	m_iStateFlag = ENUM_CLASS(STATE_FLAG::SMASH);

	return S_OK;
}

void CState_GuardCounter::Enter(CPlayerPawn* pPlayerPawn)
{
	ChangeActionFlag(ENUM_CLASS(SMASH_FLAG::SMASH_GUARD_COUNTER));
}

void CState_GuardCounter::InputData(CPlayerPawn* pPlayerPawn, INPUT_MOVE_DESC MoveInput, INPUT_ACTION_DESC ActionInput)
{
	Change_OtherState(pPlayerPawn, MoveInput, ActionInput);
}

void CState_GuardCounter::Update(CPlayerPawn* pPlayerPawn, _float fTimeDelta)
{
}

void CState_GuardCounter::Exit(CPlayerPawn* pPlayerPawn)
{
	m_iStateFlag = ENUM_CLASS(STATE_FLAG::SMASH);
	pPlayerPawn->Reset_ComboCount();
}

CState_GuardCounter* CState_GuardCounter::Create()
{
	CState_GuardCounter* pInstance = new CState_GuardCounter();
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed Created : CState_GuardCounter"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CState_GuardCounter::Free()
{
	__super::Free();
}


namespace {
	struct StateGuardCounterRegister {
		StateGuardCounterRegister() {
			CStateFactory::GetInstance()->Register(ENUM_CLASS(PLAYER_STATE::GUARD_COUNTER), []()->CPlayerState* { return CState_GuardCounter::Create(); });
		};

	};
	static StateGuardCounterRegister RegisterIdle;
}