#include "ClientPch.h"
#include "State_Guard.h"
#include "PlayerPawn.h"
#include "StateFactory.h"

CState_Guard::CState_Guard()
{
}

HRESULT CState_Guard::Initialize()
{
	m_iStateFlag = ENUM_CLASS(STATE_FLAG::GUARD);

	return S_OK;
}

void CState_Guard::Enter(CPlayerPawn* pPlayerPawn)
{
	m_bGuardEnd = false;
	m_bGuardBegin = true;

	ChangeActionFlag(ENUM_CLASS(GUARD_FLAG::GUARD_BEGIN));
}

void CState_Guard::InputData(CPlayerPawn* pPlayerPawn, INPUT_MOVE_DESC MoveInput, INPUT_ACTION_DESC ActionInput)
{
	if (m_bGuardEnd)
	{
		Change_OtherState(pPlayerPawn, MoveInput, ActionInput);
		return;
	}

	if (ActionInput.byAction & ENUM_CLASS(ACTION_INPUT::HEAVYSTAND) && pPlayerPawn->AnimCanChange())
	{
		pPlayerPawn->Change_State(ENUM_CLASS(PLAYER_STATE::HEAVYSTAND));
	}
	else if (ActionInput.byAction & ENUM_CLASS(ACTION_INPUT::GUARD) && pPlayerPawn->AnimCanChange())
	{
		if (m_bGuardBegin)
		{
			m_bGuardBegin = false;
			ChangeActionFlag(ENUM_CLASS(GUARD_FLAG::GUARD_DURING));
		}
		else
			ChangeActionFlag(ENUM_CLASS(GUARD_FLAG::GUARD_DURING));
		
		if(ActionInput.byAction & ENUM_CLASS(ACTION_INPUT::ATTACK) && m_iStateFlag & ENUM_CLASS(GUARD_FLAG::GUARD_DURING))
			ChangeActionFlag(ENUM_CLASS(GUARD_FLAG::GUARD_ATTACK));
	}
	else if(~ActionInput.byAction & ENUM_CLASS(ACTION_INPUT::GUARD))
	{
		ChangeActionFlag(ENUM_CLASS(GUARD_FLAG::GUARD_END));
		m_bGuardEnd = true;
	}
}

void CState_Guard::Update(CPlayerPawn* pPlayerPawn, _float fTimeDelta)
{

}

void CState_Guard::Exit(CPlayerPawn* pPlayerPawn)
{
	m_iStateFlag = ENUM_CLASS(STATE_FLAG::GUARD);

	m_bGuardBegin = false;
}

CState_Guard* CState_Guard::Create()
{
	CState_Guard* pInstance = new CState_Guard();
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed Created : CState_Guard"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CState_Guard::Free()
{
	__super::Free();
}

namespace {
	struct StateGuardRegister {
		StateGuardRegister() {
			CStateFactory::GetInstance()->Register(ENUM_CLASS(PLAYER_STATE::GUARD), []()->CPlayerState* { return CState_Guard::Create(); });
		};
	};
	static StateGuardRegister RegisterMove;
}
