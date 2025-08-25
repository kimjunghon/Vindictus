#include "ClientPch.h"
#include "State_Hit_Middle.h"
#include "PlayerPawn.h"
#include "StateFactory.h"

CState_Hit_Middle::CState_Hit_Middle()
{
}

HRESULT CState_Hit_Middle::Initialize()
{
    m_iStateFlag = ENUM_CLASS(STATE_FLAG::HIT);

    return S_OK;
}

void CState_Hit_Middle::Enter(CPlayerPawn * pPlayerPawn)
{
    m_iStateFlag = ENUM_CLASS(STATE_FLAG::HIT);

    DIR eHitDir = pPlayerPawn->Get_HirDir();

    Change_HitFlag(eHitDir);
}

void CState_Hit_Middle::InputData(CPlayerPawn* pPlayerPawn, INPUT_MOVE_DESC MoveInput, INPUT_ACTION_DESC ActionInput)
{
    if (m_iStateFlag & m_iDirFlag && pPlayerPawn->AnimIsFinished())
    {
        ChangeActionFlag(m_iDirFlag << 1);
    }
    else
        Change_OtherState(pPlayerPawn, MoveInput, ActionInput);
}

void CState_Hit_Middle::Update(CPlayerPawn* pPlayerPawn, _float fTimeDelta)
{
}

void CState_Hit_Middle::Exit(CPlayerPawn* pPlayerPawn)
{
    m_iStateFlag = ENUM_CLASS(STATE_FLAG::HIT);
}

void CState_Hit_Middle::Change_HitFlag(DIR eHitDir)
{
    switch (eHitDir)
    {
    case DIR::FRONT:
    {
        m_iStateFlag |= ENUM_CLASS(HIT_FLAG::MIDDLE_FRONT);
        m_iDirFlag = ENUM_CLASS(HIT_FLAG::MIDDLE_FRONT);
        break;
    }
    case DIR::BACK:
    {
        m_iStateFlag |= ENUM_CLASS(HIT_FLAG::MIDDLE_BACK);
        m_iDirFlag = ENUM_CLASS(HIT_FLAG::MIDDLE_BACK);
        break;
    }
    }
}

CState_Hit_Middle* CState_Hit_Middle::Create()
{
    CState_Hit_Middle* pInstance = new CState_Hit_Middle();
    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX(TEXT("Failed Created : CState_Hit_Middle"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_Hit_Middle::Free()
{
    __super::Free();
}


namespace {
    struct StateHitMiddleRegister {
        StateHitMiddleRegister() {
            CStateFactory::GetInstance()->Register(ENUM_CLASS(PLAYER_STATE::HIT_MIDDLE), []()->CPlayerState* { return CState_Hit_Middle::Create(); });
        };

    };
    static StateHitMiddleRegister Register;
}