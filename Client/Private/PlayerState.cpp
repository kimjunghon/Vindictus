#include "ClientPch.h"
#include "PlayerState.h"
#include "PlayerPawn.h"

CPlayerState::CPlayerState()
{
}

HRESULT CPlayerState::Initialize()
{
	return S_OK;
}

void CPlayerState::Enter(CPlayerPawn* pPlayerPawn)
{
}

void CPlayerState::InputData(CPlayerPawn* pPlayerPawn, INPUT_MOVE_DESC MoveInput, INPUT_ACTION_DESC ActionInput)
{
}

void CPlayerState::Update(CPlayerPawn* pPlayerPawn, _float fTimeDelta)
{
}

void CPlayerState::Exit(CPlayerPawn* pPlayerPawn)
{
}

void CPlayerState::Change_OtherState(CPlayerPawn* pPlayerPawn, INPUT_MOVE_DESC MoveInput, INPUT_ACTION_DESC ActionInput)
{
	if (ActionInput.bAction && pPlayerPawn->AnimCanChange())
	{
		Find_ActionState(pPlayerPawn, ActionInput.byAction);
	}
	else if (MoveInput.bMove && pPlayerPawn->AnimCanChange())
	{
		pPlayerPawn->Change_State(ENUM_CLASS(PLAYER_STATE::MOVE));
	}
	else if (pPlayerPawn->AnimIsFinished())
		pPlayerPawn->Change_State(ENUM_CLASS(PLAYER_STATE::IDLE));
}

void CPlayerState::Find_ActionState(CPlayerPawn* pPlayerPawn, _byte byAction)
{
	if(byAction & ENUM_CLASS(ACTION_INPUT::ATTACK))
	{
		pPlayerPawn->Change_State(ENUM_CLASS(PLAYER_STATE::ATTACK));
	}
	else if (byAction & ENUM_CLASS(ACTION_INPUT::GUARD))
	{
		pPlayerPawn->Change_State(ENUM_CLASS(PLAYER_STATE::GUARD));
	}
	else if (byAction & ENUM_CLASS(ACTION_INPUT::SMASH))
	{
		_uint iComboCount = pPlayerPawn->Get_ComboCount();
		
		_uint iSmashState = ENUM_CLASS(PLAYER_STATE::SMASH0) + iComboCount;
		
		pPlayerPawn->Change_State(ENUM_CLASS(iSmashState));
	}
	else if (byAction & ENUM_CLASS(ACTION_INPUT::ROLL))
	{
		pPlayerPawn->Change_State(ENUM_CLASS(PLAYER_STATE::ROLL));
	}
	else if (byAction & ENUM_CLASS(ACTION_INPUT::HEAVYSTAND))
	{
		pPlayerPawn->Change_State(ENUM_CLASS(PLAYER_STATE::HEAVYSTAND));
	}
}

void CPlayerState::ChangeActionFlag(_uint iOnFlag)
{
	m_iStateFlag &= ((1<<8) - 1);

	m_iStateFlag |= iOnFlag;
}

void CPlayerState::Free()
{
	__super::Free();
}
