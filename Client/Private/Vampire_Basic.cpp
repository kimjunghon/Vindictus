#include "ClientPch.h"
#include "Vampire_Basic.h"
#include "PawnObject.h"
#include "Body.h"
#include "VampireAI.h"

CVampire_Basic::CVampire_Basic(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
    : CVampire { pDevice, pDeviceContext }
{
}

CVampire_Basic::CVampire_Basic(const CVampire_Basic& Prototype)
    : CVampire { Prototype }
{
}

HRESULT CVampire_Basic::Initialize_Prototype()
{
    m_fAttackCoolTime = 5.f;
    m_fAttackRange = 50.f;
    m_fChaseRange = 40.f;
    m_fMinDistance = 30.f;

    return S_OK;
}

HRESULT CVampire_Basic::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    m_iStateFlag = ENUM_CLASS(STATE_FLAG::SPAWN);

    if (FAILED(Ready_AI()))
        return E_FAIL;

    if (FAILED(Ready_PawnObject()))
        return E_FAIL;

    m_fAttackTime = 0.f;

    //TEST
    m_pTargetTransform = static_cast<CTransform*>(m_pGameInstance->Get_Component(ENUM_CLASS(LAYERTYPE::NONSTATIC), TEXT("Layer_Player"), TEXT("Com_Transform"), 0));
    if (nullptr == m_pTargetTransform)
        return E_FAIL;

    return S_OK;
}

void CVampire_Basic::Priority_Update(_float fTimeDelta)
{
    for (auto& Pair : m_PawnObjects)
        Pair.second->Priority_Update(fTimeDelta);
}

void CVampire_Basic::Update(_float fTimeDelta)
{
    m_fAttackTime += fTimeDelta;

    m_pAI->Update();

    if(m_iStateFlag & ENUM_CLASS(STATE_FLAG::MOVE))
    {
        _vector vTargetPos = m_pTargetTransform->Get_State(STATE::POSITION);
        vTargetPos = XMVectorSetY(vTargetPos, XMVectorGetY(m_pTransformCom->Get_State(STATE::POSITION)));

        m_pTransformCom->LookAt(vTargetPos);
    }

    for (auto& Pair : m_PawnObjects)
        Pair.second->Update(fTimeDelta);
}

void CVampire_Basic::Late_Update(_float fTimeDelta)
{
    Compute_AnimPosition();

    for (auto& Pair : m_PawnObjects)
        Pair.second->Late_Update(fTimeDelta);
}

HRESULT CVampire_Basic::Render()
{
    return S_OK;
}

BT_STATE CVampire_Basic::CanOtherAction()
{
    if(m_pBody->AnimCanChange() || m_pBody->AnimIsFinished())
        return BT_STATE::FAILED;

    return BT_STATE::RUN;
}

BT_STATE CVampire_Basic::CanAttackRange()
{
    _float fDistance = XMVectorGetX(XMVector3Length(XMVectorSubtract(m_pTargetTransform->Get_State(STATE::POSITION), m_pTransformCom->Get_State(STATE::POSITION))));

    if (abs(fDistance) <= m_fAttackRange)
        return BT_STATE::SUCCESS;

    return BT_STATE::FAILED;
}

BT_STATE CVampire_Basic::Attack()
{
    m_iStateFlag = ENUM_CLASS(STATE_FLAG::ATTACK);
    m_fAttackTime = 0.f;

    return BT_STATE::SUCCESS;
}

BT_STATE CVampire_Basic::Chase()
{
    _float fDistance = XMVectorGetX(XMVector3Length(XMVectorSubtract(m_pTargetTransform->Get_State(STATE::POSITION), m_pTransformCom->Get_State(STATE::POSITION))));
    
    if (abs(fDistance) >= m_fChaseRange)
    {
        m_iStateFlag = ENUM_CLASS(STATE_FLAG::MOVE) | ENUM_CLASS(MOVE_FLAG::RUN_FRONT);

        return BT_STATE::SUCCESS;
    }

    return BT_STATE::FAILED;
}

BT_STATE CVampire_Basic::Patrol()
{    
    _float fDistance = XMVectorGetX(XMVector3Length(XMVectorSubtract(m_pTargetTransform->Get_State(STATE::POSITION), m_pTransformCom->Get_State(STATE::POSITION))));
    
    if (abs(fDistance) <= m_fMinDistance)
        m_iStateFlag = ENUM_CLASS(STATE_FLAG::MOVE) | ENUM_CLASS(MOVE_FLAG::RUN_BACK);
    else
        m_iStateFlag = ENUM_CLASS(STATE_FLAG::MOVE) | (rand()%2 == 0 ? ENUM_CLASS(MOVE_FLAG::RUN_LEFT) : ENUM_CLASS(MOVE_FLAG::RUN_RIGHT));
 
    return BT_STATE::SUCCESS;
}

HRESULT CVampire_Basic::Ready_AI()
{
    m_pAI = CVampireAI::Create(this);
    if (nullptr == m_pAI)
        return E_FAIL;

    return S_OK;
}

HRESULT CVampire_Basic::Ready_PawnObject()
{
    CBody::BODY_DESC BodyObjectDesc = {};
    BodyObjectDesc.pPawnMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    BodyObjectDesc.pStateFlag = &m_iStateFlag;

    if (FAILED(__super::Add_PawnObject(TEXT("Vampire_Basic_Body"), ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Vampire_Basic_Body"), &BodyObjectDesc)))
        return E_FAIL;

    m_pBody = static_cast<CBody*>(Find_PawnObject(TEXT("Vampire_Basic_Body")));

    m_pAnimMovement = m_pBody->Get_AnimMovementPtr();

    return S_OK;
}

void CVampire_Basic::Compute_AnimPosition()
{
    _vector vPosition = m_pTransformCom->Get_State(STATE::POSITION);
    _vector vAnimPosition = *m_pAnimMovement;

    _vector vLook = XMVector3Normalize(m_pTransformCom->Get_State(STATE::LOOK));
    _float fYaw = atan2f(XMVectorGetX(vLook), XMVectorGetZ(vLook));

    _matrix RotationMatrix = XMMatrixRotationY(fYaw);

    vAnimPosition = XMVector3Transform(vAnimPosition, RotationMatrix);

    vPosition = XMVectorSetW(XMVectorAdd(vPosition, vAnimPosition), 1.f);

    m_pTransformCom->Set_State(STATE::POSITION, vPosition);
}

CVampire_Basic* CVampire_Basic::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
    CVampire_Basic* pInstance = new CVampire_Basic(pDevice, pDeviceContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed Created : CVampire_Basic"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CVampire_Basic::Clone(void* pArg)
{
    CVampire_Basic* pInstance = new CVampire_Basic(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed Cloned : CVampire_Basic"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CVampire_Basic::Free()
{
    __super::Free();

    Safe_Release(m_pAI);
}
