#include "ClientPch.h"
#include "State_Smash4.h"
#include "PlayerPawn.h"
#include "StateFactory.h"

CState_Smash4::CState_Smash4()
{
}

HRESULT CState_Smash4::Initialize()
{
	m_iStateFlag = ENUM_CLASS(STATE_FLAG::SMASH);
	
	return S_OK;
}

void CState_Smash4::Enter(CPlayerPawn* pPlayerPawn)
{
	ChangeActionFlag(ENUM_CLASS(SMASH_FLAG::SMASH4));
}

void CState_Smash4::InputData(CPlayerPawn* pPlayerPawn, INPUT_MOVE_DESC MoveInput, INPUT_ACTION_DESC ActionInput)
{
	Change_OtherState(pPlayerPawn, MoveInput, ActionInput);
}

void CState_Smash4::Update(CPlayerPawn* pPlayerPawn, _float fTimeDelta)
{
}

void CState_Smash4::Exit(CPlayerPawn* pPlayerPawn)
{
	m_iStateFlag = ENUM_CLASS(STATE_FLAG::SMASH);
	pPlayerPawn->Reset_ComboCount();
}

CState_Smash4* CState_Smash4::Create()
{
	CState_Smash4* pInstance = new CState_Smash4();
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed Created : CState_Smash4"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CState_Smash4::Free()
{
	__super::Free();
}


namespace {
	struct StateSmash4Register {
		StateSmash4Register() {
			CStateFactory::GetInstance()->Register(ENUM_CLASS(PLAYER_STATE::SMASH4), []()->CPlayerState* { return CState_Smash4::Create(); });
		};

	};
	static StateSmash4Register RegisterIdle;
}