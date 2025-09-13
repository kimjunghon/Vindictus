#include "ClientPch.h"
#include "State_Grap.h"
#include "PlayerPawn.h"
#include "StateFactory.h"

CState_Grap::CState_Grap()
{
}

HRESULT CState_Grap::Initialize()
{
    if (FAILED(__super::Initialize()))
        return E_FAIL;

    m_iStateFlag = ENUM_CLASS(STATE_FLAG::HIT);

	return S_OK;
}

void CState_Grap::Enter(CPlayerPawn* pPlayerPawn)
{
    ChangeActionFlag(ENUM_CLASS(HIT_FLAG::GRAP));
}

void CState_Grap::InputData(CPlayerPawn* pPlayerPawn, INPUT_MOVE_DESC MoveInput, INPUT_ACTION_DESC ActionInput)
{
    if(false == pPlayerPawn->IsGrap() && m_iStateFlag & (ENUM_CLASS(HIT_FLAG::STRONG_FRONT_ROLLUP) | ENUM_CLASS(HIT_FLAG::STRONG_FRONT_UP)))
        Change_OtherState(pPlayerPawn, MoveInput, ActionInput);
    else
    {
        if (m_iStateFlag & ENUM_CLASS(HIT_FLAG::GRAP_END) && pPlayerPawn->AnimIsFinished())
        {
            if (MoveInput.bMove)
            {
                pPlayerPawn->Compute_PlayerMoveDir();
                ChangeActionFlag(ENUM_CLASS(HIT_FLAG::STRONG_FRONT_ROLLUP));
            }
            else
                ChangeActionFlag(ENUM_CLASS(HIT_FLAG::STRONG_FRONT_UP));
        }
        else if (false == pPlayerPawn->IsGrap() && m_iStateFlag & ENUM_CLASS(HIT_FLAG::GRAP))
        {

            ChangeActionFlag(ENUM_CLASS(HIT_FLAG::GRAP_END));
        }
    }
}

void CState_Grap::Update(CPlayerPawn* pPlayerPawn, _float fTimeDelta)
{
    if (pPlayerPawn->IsGrap())
        pPlayerPawn->Grap();
}

void CState_Grap::Exit(CPlayerPawn* pPlayerPawn)
{
}

CState_Grap* CState_Grap::Create()
{
    CState_Grap* pInstance = new CState_Grap();
    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX(TEXT("Failed Created : CState_Grap"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_Grap::Free()
{
    __super::Free();
}

namespace {
    struct StateGrapRegister {
        StateGrapRegister() {
            CStateFactory::GetInstance()->Register(ENUM_CLASS(PLAYER_STATE::GRAP), []()->CPlayerState* { return CState_Grap::Create(); });
        };

    };
    static StateGrapRegister Register;
}