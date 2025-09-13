#include "ClientPch.h"
#include "State_Hit_Strong.h"
#include "PlayerPawn.h"
#include "StateFactory.h"

CState_Hit_Strong::CState_Hit_Strong()
{
}

HRESULT CState_Hit_Strong::Initialize()
{
    if (FAILED(__super::Initialize()))
        return E_FAIL;

    m_iStateFlag = ENUM_CLASS(STATE_FLAG::HIT);

    return S_OK;
}

void CState_Hit_Strong::Enter(CPlayerPawn* pPlayerPawn)
{
    m_iStateFlag = ENUM_CLASS(STATE_FLAG::HIT);

    DIR eHitDir = pPlayerPawn->Get_HirDir();

    Change_HitFlag(eHitDir);
}

void CState_Hit_Strong::InputData(CPlayerPawn* pPlayerPawn, INPUT_MOVE_DESC MoveInput, INPUT_ACTION_DESC ActionInput)
{
    if (m_iStateFlag & m_iDirFlag && pPlayerPawn->AnimIsFinished())
    {
        if (MoveInput.bMove)
        {
            pPlayerPawn->Compute_PlayerMoveDir();
            ChangeActionFlag(m_iDirFlag << 2);
        }
        else
            ChangeActionFlag(m_iDirFlag << 1);
    }
    else
        Change_OtherState(pPlayerPawn, MoveInput, ActionInput);
}

void CState_Hit_Strong::Update(CPlayerPawn* pPlayerPawn, _float fTimeDelta)
{
}

void CState_Hit_Strong::Exit(CPlayerPawn* pPlayerPawn)
{
    m_iStateFlag = ENUM_CLASS(STATE_FLAG::HIT);
}

void CState_Hit_Strong::Change_HitFlag(DIR eHitDir)
{
    switch (eHitDir)
    {
    case DIR::FRONT:
    {
        m_iStateFlag |= ENUM_CLASS(HIT_FLAG::STRONG_FRONT);
        m_iDirFlag = ENUM_CLASS(HIT_FLAG::STRONG_FRONT);
        break;
    }
    case DIR::BACK:
    {
        m_iStateFlag |= ENUM_CLASS(HIT_FLAG::STRONG_BACK);
        m_iDirFlag = ENUM_CLASS(HIT_FLAG::STRONG_BACK);
        break;
    }
    }
}

CState_Hit_Strong* CState_Hit_Strong::Create()
{
    CState_Hit_Strong* pInstance = new CState_Hit_Strong();
    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX(TEXT("Failed Created : CState_Hit_Strong"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_Hit_Strong::Free()
{
    __super::Free();
}

namespace {
    struct StateHitStrongRegister {
        StateHitStrongRegister() {
            CStateFactory::GetInstance()->Register(ENUM_CLASS(PLAYER_STATE::HIT_STRONG), []()->CPlayerState* { return CState_Hit_Strong::Create(); });
        };

    };
    static StateHitStrongRegister Register;
}