#include "ClientPch.h"
#include "State_Hit_HeavyStand.h"
#include "PlayerPawn.h"
#include "StateFactory.h"


CState_Hit_HeavyStand::CState_Hit_HeavyStand()
{
}

HRESULT CState_Hit_HeavyStand::Initialize()
{
    if (FAILED(__super::Initialize()))
        return E_FAIL;

    m_iStateFlag = ENUM_CLASS(STATE_FLAG::HIT);

    m_fCanCounterTime = 1.f;

    return S_OK;
}

void CState_Hit_HeavyStand::Enter(CPlayerPawn* pPlayerPawn)
{
    m_iStateFlag |= ENUM_CLASS(HIT_FLAG::HEAVYSTAND);

    m_fHitTime = 0.f;
}

void CState_Hit_HeavyStand::InputData(CPlayerPawn* pPlayerPawn, INPUT_MOVE_DESC MoveInput, INPUT_ACTION_DESC ActionInput)
{
    Change_OtherState(pPlayerPawn, MoveInput, ActionInput);
}

void CState_Hit_HeavyStand::Update(CPlayerPawn* pPlayerPawn, _float fTimeDelta)
{
}

void CState_Hit_HeavyStand::Exit(CPlayerPawn* pPlayerPawn)
{
    m_iStateFlag = ENUM_CLASS(STATE_FLAG::HIT);
}

CState_Hit_HeavyStand* CState_Hit_HeavyStand::Create()
{
    CState_Hit_HeavyStand* pInstance = new CState_Hit_HeavyStand();
    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX(TEXT("Failed Created : CState_Hit_HeavyStand"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_Hit_HeavyStand::Free()
{
    __super::Free();
}


namespace {
    struct StateHitHeavyStandRegister {
        StateHitHeavyStandRegister() {
            CStateFactory::GetInstance()->Register(ENUM_CLASS(PLAYER_STATE::HIT_HEAVYSTAND), []()->CPlayerState* { return CState_Hit_HeavyStand::Create(); });
        };

    };
    static StateHitHeavyStandRegister Register;
}