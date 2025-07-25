#include "ClientPch.h"
#include "State_Idle.h"
#include "PlayerPawn.h"
#include "StateFactory.h"

CState_Idle::CState_Idle()
{
}

HRESULT CState_Idle::Initialize()
{
    m_iStateFlag |= ENUM_CLASS(STATE_FLAG::IDLE);

    m_fRestDelay = 10.f;

    return S_OK;
}

void CState_Idle::Enter(CPlayerPawn* pPlayerPawn)
{
    ChangeActionFlag(ENUM_CLASS(IDLE_FLAG::DEFAULT));

    m_fIdleTime = 0.f;
}

void CState_Idle::InputData(CPlayerPawn* pPlayerPawn, INPUT_MOVE_DESC MoveInput, INPUT_ACTION_DESC ActionInput)
{
    if (ActionInput.bAction)
    {
        Find_ActionState(pPlayerPawn, ActionInput.byAction);
    }
    else if (MoveInput.bMove)
    {
        pPlayerPawn->Change_State(ENUM_CLASS(PLAYER_STATE::MOVE));
    }
}

void CState_Idle::Update(CPlayerPawn* pPlayerPawn, _float fTimeDelta)
{
    m_fIdleTime += fTimeDelta;

    if (m_fIdleTime >= m_fRestDelay)
    {
        ChangeActionFlag(ENUM_CLASS(IDLE_FLAG::REST));
    }
}

void CState_Idle::Exit(CPlayerPawn* pPlayerPawn)
{
    m_iStateFlag = ENUM_CLASS(STATE_FLAG::IDLE);
}


CState_Idle* CState_Idle::Create()
{
    CState_Idle* pInstance = new CState_Idle();
    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX(TEXT("Failed Created : CState_Idle"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_Idle::Free()
{
    __super::Free();
}


namespace {
struct StateIdleRegister {
    StateIdleRegister() {
        CStateFactory::GetInstance()->Register(ENUM_CLASS(PLAYER_STATE::IDLE), []()->CPlayerState* { return CState_Idle::Create(); });
    };

};
static StateIdleRegister RegisterIdle;
}