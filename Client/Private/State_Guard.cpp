#include "ClientPch.h"
#include "State_Guard.h"
#include "PlayerPawn.h"
#include "StateFactory.h"

CState_Guard::CState_Guard()
{
}

HRESULT CState_Guard::Initialize()
{
	return S_OK;
}

void CState_Guard::Enter(CPlayerPawn* pPlayerPawn)
{
}

void CState_Guard::InputData(CPlayerPawn* pPlayerPawn, INPUT_MOVE_DESC MoveInput, INPUT_ACTION_DESC ActionInput)
{
}

void CState_Guard::Update(CPlayerPawn* pPlayerPawn, _float fTimeDelta)
{
}

void CState_Guard::Exit(CPlayerPawn* pPlayerPawn)
{
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
