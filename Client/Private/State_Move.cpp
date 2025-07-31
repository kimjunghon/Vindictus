#include "ClientPch.h"
#include "State_Move.h"
#include "PlayerPawn.h"
#include "StateFactory.h"

CState_Move::CState_Move()
{
}

HRESULT CState_Move::Initialize()
{
	m_iStateFlag = ENUM_CLASS(STATE_FLAG::MOVE);

	return S_OK;
}

void CState_Move::Enter(CPlayerPawn* pPlayerPawn)
{
	ChangeActionFlag(ENUM_CLASS(MOVE_FLAG::DEFAULT));

	pPlayerPawn->Run();

	m_bStop = false;
}

void CState_Move::InputData(CPlayerPawn* pPlayerPawn, INPUT_MOVE_DESC MoveInput, INPUT_ACTION_DESC ActionInput)
{

	if (m_bStop)
	{
		if(pPlayerPawn->AnimCanChange())
			pPlayerPawn->Change_State(ENUM_CLASS(PLAYER_STATE::IDLE));
	}
	else if (ActionInput.bAction)
	{
		Find_ActionState(pPlayerPawn, ActionInput.byAction);
	}
	else if (false == MoveInput.bMove)
	{
		if(m_iStateFlag & ENUM_CLASS(MOVE_FLAG::SPRINT))
		{
			ChangeActionFlag(ENUM_CLASS(MOVE_FLAG::STOP));
			m_bStop = true;
		}
		else
			pPlayerPawn->Change_State(ENUM_CLASS(PLAYER_STATE::IDLE));
	}
	else 
	{
		if (MoveInput.bSprint)
		{
			pPlayerPawn->Sprint();
			ChangeActionFlag(ENUM_CLASS(MOVE_FLAG::SPRINT));
		}
		else
		{
			pPlayerPawn->Run();
			ChangeActionFlag(ENUM_CLASS(MOVE_FLAG::DEFAULT));
		}
	}
}

void CState_Move::Update(CPlayerPawn* pPlayerPawn, _float fTimeDelta)
{
}

void CState_Move::Exit(CPlayerPawn* pPlayerPawn)
{
	m_iStateFlag = ENUM_CLASS(STATE_FLAG::MOVE);
}

CState_Move* CState_Move::Create()
{
	CState_Move* pInstance = new CState_Move();
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed Created : CState_Move"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Move::Free()
{
	__super::Free();
}

namespace{
struct StateMoveRegister {
	StateMoveRegister() {
		CStateFactory::GetInstance()->Register(ENUM_CLASS(PLAYER_STATE::MOVE), []()->CPlayerState* { return CState_Move::Create(); });
	};


};
static StateMoveRegister RegisterMove;
}