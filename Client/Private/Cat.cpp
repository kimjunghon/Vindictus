#include "ClientPch.h"
#include "Cat.h"
#include "Body.h"

CCat::CCat(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
    : CColliderPawn { pDevice, pDeviceContext }
{
}

CCat::CCat(const CCat& Prototype)
    : CColliderPawn { Prototype }
{
}

HRESULT CCat::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CCat::Initialize(void* pArg)
{
    if (nullptr == pArg)
        return E_FAIL;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_PawnObject()))
        return E_FAIL;

    if (FAILED(Ready_Collider()))
        return E_FAIL;

    CAT_DESC* pDesc = static_cast<CAT_DESC*>(pArg);

    _vector vPosition = XMVectorSetW(XMLoadFloat3(&pDesc->vPosition), 1.f);

    m_pTransformCom->Set_State(STATE::POSITION, vPosition);

    m_pTransformCom->RotateQuaternion(XMQuaternionRotationRollPitchYaw(0.f, XMConvertToRadians(90.f), 0.f));

    m_iStateFlag = ENUM_CLASS(STATE_FLAG::SLEEP);

    return S_OK;
}

void CCat::Priority_Update(_float fTimeDelta)
{
    for (auto& Pair : m_PawnObjects)
        Pair.second->Priority_Update(fTimeDelta);
}

void CCat::Update(_float fTimeDelta)
{
    for (auto& Pair : m_PawnObjects)
        Pair.second->Update(fTimeDelta);
}

void CCat::Late_Update(_float fTimeDelta)
{
    for (auto& Pair : m_PawnObjects)
        Pair.second->Late_Update(fTimeDelta);

    m_pColliderContainer->Update(this, m_pTransformCom->Get_WorldMatrix());
}

HRESULT CCat::Render()
{
    return S_OK;
}

HRESULT CCat::Ready_PawnObject()
{
    CBody::BODY_DESC CatBodyDesc = {};
    CatBodyDesc.pPawnMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    CatBodyDesc.pStateFlag = &m_iStateFlag;
    
    if (FAILED(CPawn::Add_PawnObject(TEXT("Cat_Body"), ENUM_CLASS(LEVEL::TOWN), TEXT("Prototype_GameObject_Cat_Body"), &CatBodyDesc)))
        return E_FAIL;
    m_pBody = static_cast<CBody*>(Find_PawnObject(TEXT("Cat_Body")));
    if (nullptr == m_pBody)
        return E_FAIL;

    return S_OK;
}

HRESULT CCat::Ready_Collider()
{
    CBoundingAABB::BOUNDING_AABB_DESC  AABBDesc = {};
    AABBDesc.vExtents = _float3(30.f, 30.f, 30.f);
    AABBDesc.vCenter = _float3(0.f, AABBDesc.vExtents.y, 0.f);

    if (FAILED(m_pColliderContainer->Add_Collider(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_AABB"),
        ENUM_CLASS(COLLIDER_CHANNEL::BOUNDING), ENUM_CLASS(COLLIDER_OWNER::OBJECT), &AABBDesc, nullptr)))
        return E_FAIL;

    CBoundingOBB::BOUNDING_OBB_DESC OBBDesc = {};
    OBBDesc.vAngles = _float3(0.f, 0.f, 0.f);
    OBBDesc.vExtents = _float3(20.f, 20.f, 20.f);
    OBBDesc.vCenter = _float3(0.f, OBBDesc.vExtents.y, 0.f);

    if (FAILED(m_pColliderContainer->Add_Collider(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_OBB"),
        ENUM_CLASS(COLLIDER_CHANNEL::INTERACTION), ENUM_CLASS(COLLIDER_OWNER::OBJECT), &OBBDesc, nullptr)))
        return E_FAIL;

    m_pColliderContainer->Bind_Collision_Callback(ENUM_CLASS(COLLIDER_CHANNEL::INTERACTION), 0, COLLIDER_STATE::BEGIN, [this](const CCollider::COLLISION_DATA& Data) {
        m_iStateFlag = ENUM_CLASS(STATE_FLAG::GROOMING);
        EVENT_DYEING_NPC Event;
        Event.IsNear = true;
        m_pGameInstance->Publish(ENUM_CLASS(EVENT_TYPE::STATIC), Event); });


    m_pColliderContainer->Bind_Collision_Callback(ENUM_CLASS(COLLIDER_CHANNEL::INTERACTION), 0, COLLIDER_STATE::END, [this](const CCollider::COLLISION_DATA& Data) {
        m_iStateFlag = ENUM_CLASS(STATE_FLAG::SLEEP);
        EVENT_DYEING_NPC Event;
        Event.IsNear = false;
        m_pGameInstance->Publish(ENUM_CLASS(EVENT_TYPE::STATIC), Event); });

    return S_OK;
}

void CCat::UpdateAnimFlag()
{
    _uint iState = ENUM_CLASS(STATE_FLAG::IDLE);

    _uint iRandomIndex = rand() % 5;

    m_iStateFlag = iState << iRandomIndex;
}

CCat* CCat::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
    CCat* pInstance = new CCat(pDevice, pDeviceContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed Created : CCat"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CCat::Clone(void* pArg)
{
    CCat* pInstance = new CCat(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed Cloned : CCat"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CCat::Free()
{
    __super::Free();
}
