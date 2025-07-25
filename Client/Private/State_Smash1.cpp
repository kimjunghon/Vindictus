#include "ClientPch.h"
#include "State_Smash1.h"
#include "PlayerPawn.h"
#include "StateFactory.h"

CState_Smash1::CState_Smash1()
{
}

HRESULT CState_Smash1::Initialize()
{
	m_iStateFlag = ENUM_CLASS(STATE_FLAG::SMASH);

	return S_OK;
}

void CState_Smash1::Enter(CPlayerPawn* pPlayerPawn)
{
	ChangeActionFlag(ENUM_CLASS(SMASH_FLAG::SMASH1));
}

void CState_Smash1::InputData(CPlayerPawn* pPlayerPawn, INPUT_MOVE_DESC MoveInput, INPUT_ACTION_DESC ActionInput)
{
	Change_OtherState(pPlayerPawn, MoveInput, ActionInput);
}

void CState_Smash1::Update(CPlayerPawn* pPlayerPawn, _float fTimeDelta)
{
}

void CState_Smash1::Exit(CPlayerPawn* pPlayerPawn)
{
	m_iStateFlag = ENUM_CLASS(STATE_FLAG::SMASH);
}

CState_Smash1* CState_Smash1::Create()
{
	CState_Smash1* pInstance = new CState_Smash1();
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed Created : CState_Smash1"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CState_Smash1::Free()
{
	__super::Free();
}

namespace {
	struct StateSmash1Register {
		StateSmash1Register() {
			CStateFactory::GetInstance()->Register(ENUM_CLASS(PLAYER_STATE::SMASH1), []()->CPlayerState* { return CState_Smash1::Create(); });
		};

	};
	static StateSmash1Register RegisterIdle;
}