#include "ClientPch.h"
#include "State_Hit.h"
#include "PlayerPawn.h"
#include "StateFactory.h"

CState_Hit::CState_Hit()
{
}

HRESULT CState_Hit::Initialize()
{
    m_iStateFlag |= ENUM_CLASS(STATE_FLAG::HIT);

	return S_OK;
}

void CState_Hit::Enter(CPlayerPawn* pPlayerPawn)
{
    HIT_TYPE eHit_Type = {};
    eHit_Type = pPlayerPawn->Get_HitType();

    Change_HitFlag(eHit_Type);
}

void CState_Hit::InputData(CPlayerPawn* pPlayerPawn, INPUT_MOVE_DESC MoveInput, INPUT_ACTION_DESC ActionInput)
{
    Change_OtherState(pPlayerPawn, MoveInput, ActionInput);
}

void CState_Hit::Update(CPlayerPawn* pPlayerPawn, _float fTimeDelta)
{
}

void CState_Hit::Exit(CPlayerPawn* pPlayerPawn)
{
    m_iStateFlag = ENUM_CLASS(STATE_FLAG::ATTACK);
}

void CState_Hit::Change_HitFlag(HIT_TYPE eHitType)
{
    switch (eHitType)
    {
    case HIT_TYPE::FRONT:
        m_iStateFlag |= ENUM_CLASS(HIT_FLAG::HIT_FRONT);
        break;
    case HIT_TYPE::BACK:
        m_iStateFlag |= ENUM_CLASS(HIT_FLAG::HIT_BACK);
        break;
    case HIT_TYPE::LEFT:
        m_iStateFlag |= ENUM_CLASS(HIT_FLAG::HIT_RIGHT);
        break;
    case HIT_TYPE::RIGHT:
        m_iStateFlag |= ENUM_CLASS(HIT_FLAG::HIT_LEFT);
        break;
    case HIT_TYPE::STRONG:
        m_iStateFlag |= ENUM_CLASS(HIT_FLAG::HIT_STRONG);
        break;
    }
}

CState_Hit* CState_Hit::Create()
{
    CState_Hit* pInstance = new CState_Hit();
    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX(TEXT("Failed Created : CState_Hit"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_Hit::Free()
{
    __super::Free();
}


namespace {
    struct StateIdleRegister {
        StateIdleRegister() {
            CStateFactory::GetInstance()->Register(ENUM_CLASS(PLAYER_STATE::HIT), []()->CPlayerState* { return CState_Hit::Create(); });
        };

    };
    static StateIdleRegister RegisterIdle;
}