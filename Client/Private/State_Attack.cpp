#include "ClientPch.h"
#include "State_Attack.h"
#include "PlayerPawn.h"
#include "StateFactory.h"

CState_Attack::CState_Attack()
{
}

HRESULT CState_Attack::Initialize()
{
	m_iStateFlag = ENUM_CLASS(STATE_FLAG::ATTACK);

	return S_OK;
}

void CState_Attack::Enter(CPlayerPawn* pPlayerPawn)
{
	pPlayerPawn->Reset_ComboCount();

	pPlayerPawn->Increase_ComboCount();

	ChangeActionFlag(ENUM_CLASS(ATTACK_FLAG::COMBO1));
}

void CState_Attack::InputData(CPlayerPawn* pPlayerPawn, INPUT_MOVE_DESC MoveInput, INPUT_ACTION_DESC ActionInput)
{
	if (ActionInput.byAction & ENUM_CLASS(ACTION_INPUT::ATTACK) && pPlayerPawn->AnimCanChange())
	{
		_uint iComboCount = pPlayerPawn->Get_ComboCount();

		if (iComboCount >= 4)
			return;

		pPlayerPawn->Increase_ComboCount();
		iComboCount++;

		_uint iFlag = ENUM_CLASS(ATTACK_FLAG::COMBO1) << (iComboCount - 1);

		ChangeActionFlag(iFlag);

	}
	else if (ActionInput.bAction && pPlayerPawn->AnimCanChange())
	{
		Find_ActionState(pPlayerPawn, ActionInput.byAction);
	}
	else if (MoveInput.bMove && pPlayerPawn->AnimCanChange())
	{
		pPlayerPawn->Change_State(ENUM_CLASS(PLAYER_STATE::MOVE));
	}
	else if(pPlayerPawn->AnimIsFinished())
		pPlayerPawn->Change_State(ENUM_CLASS(PLAYER_STATE::IDLE));
}

void CState_Attack::Update(CPlayerPawn* pPlayerPawn, _float fTimeDelta)
{
}

void CState_Attack::Exit(CPlayerPawn* pPlayerPawn)
{
	m_iStateFlag = ENUM_CLASS(STATE_FLAG::ATTACK);
}

CState_Attack* CState_Attack::Create()
{
	CState_Attack* pInstance = new CState_Attack();
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed Created : CState_Attack"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Attack::Free()
{
	__super::Free();
}

namespace {
	struct StateAttackRegister {
		StateAttackRegister() {
			CStateFactory::GetInstance()->Register(ENUM_CLASS(PLAYER_STATE::ATTACK), []()->CPlayerState* { return CState_Attack::Create(); });
		};

	};
	static StateAttackRegister RegisterIdle;
}