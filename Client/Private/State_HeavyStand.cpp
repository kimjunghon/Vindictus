#include "ClientPch.h"
#include "State_HeavyStand.h"
#include "PlayerPawn.h"
#include "StateFactory.h"

CState_HeavyStand::CState_HeavyStand()
{
}

HRESULT CState_HeavyStand::Initialize()
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;

	m_iStateFlag = ENUM_CLASS(STATE_FLAG::HEAVYSTAND);

	return S_OK;
}

void CState_HeavyStand::Enter(CPlayerPawn* pPlayerPawn)
{
	m_bHeavyStandEnd = false;
	ChangeActionFlag(ENUM_CLASS(HEAVYSTAND_FLAG::HEAVYSTAND_BEGIN));
}

void CState_HeavyStand::InputData(CPlayerPawn* pPlayerPawn, INPUT_MOVE_DESC MoveInput, INPUT_ACTION_DESC ActionInput)
{
	if (m_bHeavyStandEnd)
	{
		Change_OtherState(pPlayerPawn, MoveInput, ActionInput);
		return;
	}

	if (ActionInput.byAction & ENUM_CLASS(ACTION_INPUT::HEAVYSTAND) && pPlayerPawn->AnimCanChange())
	{
		ChangeActionFlag(ENUM_CLASS(HEAVYSTAND_FLAG::HEAVYSTAND_DURING));
	}
	else if(~ActionInput.byAction & ENUM_CLASS(ACTION_INPUT::HEAVYSTAND))
	{
		ChangeActionFlag(ENUM_CLASS(HEAVYSTAND_FLAG::HEAVYSTAND_END));
		m_bHeavyStandEnd = true;
	}
}

void CState_HeavyStand::Update(CPlayerPawn* pPlayerPawn, _float fTimeDelta)
{
}

void CState_HeavyStand::Exit(CPlayerPawn* pPlayerPawn)
{
	m_iStateFlag = ENUM_CLASS(STATE_FLAG::HEAVYSTAND);
}

CState_HeavyStand* CState_HeavyStand::Create()
{
	CState_HeavyStand* pInstance = new CState_HeavyStand();
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed Created : CState_HeavyStand"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CState_HeavyStand::Free()
{
	__super::Free();
}

namespace {
	struct StateHeavyStandRegister {
		StateHeavyStandRegister() {
			CStateFactory::GetInstance()->Register(ENUM_CLASS(PLAYER_STATE::HEAVYSTAND), []()->CPlayerState* { return CState_HeavyStand::Create(); });
		};
	};
	static StateHeavyStandRegister Register;
}
