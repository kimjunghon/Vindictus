#include "ClientPch.h"
#include "State_Roll.h"
#include "PlayerPawn.h"
#include "StateFactory.h"

CState_Roll::CState_Roll()
{
}

HRESULT CState_Roll::Initialize()
{
	m_iStateFlag = ENUM_CLASS(STATE_FLAG::ROLL);

	return S_OK;
}

void CState_Roll::Enter(CPlayerPawn* pPlayerPawn)
{
	m_eAnimPhase = ANIM_PHASE::BEGIN;

	ChangeActionFlag(ENUM_CLASS(ROLL_FLAG::ROLL_BEGIN));
}

void CState_Roll::InputData(CPlayerPawn* pPlayerPawn, INPUT_MOVE_DESC MoveInput, INPUT_ACTION_DESC ActionInput)
{
	if (pPlayerPawn->AnimCanChange())
		Change_OtherState(pPlayerPawn, MoveInput, ActionInput);
}

void CState_Roll::Update(CPlayerPawn* pPlayerPawn, _float fTimeDelta)
{
//	if (m_eAnimPhase != ANIM_PHASE::END && pPlayerPawn->AnimIsFinished())
//	{
//		if(m_eAnimPhase == ANIM_PHASE::BEGIN)
//		{
//			m_eAnimPhase = ANIM_PHASE::DURING;
//			ChangeActionFlag(ENUM_CLASS(ROLL_FLAG::ROLL_END));
//		}
//		else
//			m_eAnimPhase = ANIM_PHASE::END;
//
//	}
}

void CState_Roll::Exit(CPlayerPawn* pPlayerPawn)
{
	m_iStateFlag = ENUM_CLASS(STATE_FLAG::ROLL);
}

CState_Roll* CState_Roll::Create()
{
	CState_Roll* pInstance = new CState_Roll();
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed Created : CState_Roll"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Roll::Free()
{
	__super::Free();
}

namespace {
	struct StateRollRegister {
		StateRollRegister() {
			CStateFactory::GetInstance()->Register(ENUM_CLASS(PLAYER_STATE::ROLL), []()->CPlayerState* { return CState_Roll::Create(); });
		};
	};
	static StateRollRegister Register;
}