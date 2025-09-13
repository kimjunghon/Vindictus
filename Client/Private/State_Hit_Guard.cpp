#include "ClientPch.h"
#include "State_Hit_Guard.h"
#include "PlayerPawn.h"
#include "StateFactory.h"

CState_Hit_Guard::CState_Hit_Guard()
{
}

HRESULT CState_Hit_Guard::Initialize()
{
    if (FAILED(__super::Initialize()))
        return E_FAIL;

    m_iStateFlag = ENUM_CLASS(STATE_FLAG::HIT);

    m_fCanCounterTime = 1.f;

    return S_OK;
}

void CState_Hit_Guard::Enter(CPlayerPawn* pPlayerPawn)
{
    m_iStateFlag |= ENUM_CLASS(HIT_FLAG::GUARD);

    m_fHitTime = 0.f;
}

void CState_Hit_Guard::InputData(CPlayerPawn* pPlayerPawn, INPUT_MOVE_DESC MoveInput, INPUT_ACTION_DESC ActionInput)
{
    if (m_fHitTime <= m_fCanCounterTime)
    {
        if (ActionInput.byAction & ENUM_CLASS(ACTION_INPUT::SMASH))
            pPlayerPawn->Change_State(ENUM_CLASS(PLAYER_STATE::GUARD_COUNTER));
    }
    else if(pPlayerPawn->AnimIsFinished())
        Change_OtherState(pPlayerPawn, MoveInput, ActionInput);
}

void CState_Hit_Guard::Update(CPlayerPawn* pPlayerPawn, _float fTimeDelta)
{
    m_fHitTime += fTimeDelta;
}

void CState_Hit_Guard::Exit(CPlayerPawn* pPlayerPawn)
{
    m_iStateFlag = ENUM_CLASS(STATE_FLAG::HIT);
}

CState_Hit_Guard* CState_Hit_Guard::Create()
{
    CState_Hit_Guard* pInstance = new CState_Hit_Guard();
    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX(TEXT("Failed Created : CState_Hit_Guard"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_Hit_Guard::Free()
{
    __super::Free();
}


namespace {
    struct StateHitGuardRegister {
        StateHitGuardRegister() {
            CStateFactory::GetInstance()->Register(ENUM_CLASS(PLAYER_STATE::HIT_GUARD), []()->CPlayerState* { return CState_Hit_Guard::Create(); });
        };

    };
    static StateHitGuardRegister Register;
}