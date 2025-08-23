#include "ClientPch.h"
#include "State_Hit_Light.h"
#include "PlayerPawn.h"
#include "StateFactory.h"

CState_Hit_Light::CState_Hit_Light()
{
}

HRESULT CState_Hit_Light::Initialize()
{
    m_iStateFlag = ENUM_CLASS(STATE_FLAG::HIT);

	return S_OK;
}

void CState_Hit_Light::Enter(CPlayerPawn* pPlayerPawn)
{
    m_iStateFlag = ENUM_CLASS(STATE_FLAG::HIT);

    HIT_DIR eHitDir = pPlayerPawn->Get_HirDir();

    Change_HitFlag(eHitDir);
}

void CState_Hit_Light::InputData(CPlayerPawn* pPlayerPawn, INPUT_MOVE_DESC MoveInput, INPUT_ACTION_DESC ActionInput)
{
    Change_OtherState(pPlayerPawn, MoveInput, ActionInput);
}

void CState_Hit_Light::Update(CPlayerPawn* pPlayerPawn, _float fTimeDelta)
{
}

void CState_Hit_Light::Exit(CPlayerPawn* pPlayerPawn)
{
    m_iStateFlag = ENUM_CLASS(STATE_FLAG::HIT);
}

void CState_Hit_Light::Change_HitFlag(HIT_DIR eHitDir)
{
    switch (eHitDir)
    {
    case HIT_DIR::FRONT:
        m_iStateFlag |= ENUM_CLASS(HIT_FLAG::FRONT);
        break;
    case HIT_DIR::BACK:
        m_iStateFlag |= ENUM_CLASS(HIT_FLAG::BACK);
        break;
    case HIT_DIR::RIGHT:
        m_iStateFlag |= ENUM_CLASS(HIT_FLAG::RIGHT);
        break;
    case HIT_DIR::LEFT:
        m_iStateFlag |= ENUM_CLASS(HIT_FLAG::LEFT);
        break;
    }
}

CState_Hit_Light* CState_Hit_Light::Create()
{
    CState_Hit_Light* pInstance = new CState_Hit_Light();
    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX(TEXT("Failed Created : CState_Hit_Light"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_Hit_Light::Free()
{
    __super::Free();

}


namespace {
    struct StateHitLightRegister {
        StateHitLightRegister() {
            CStateFactory::GetInstance()->Register(ENUM_CLASS(PLAYER_STATE::HIT_LIGHT), []()->CPlayerState* { return CState_Hit_Light::Create(); });
        };

    };
    static StateHitLightRegister Register;
}