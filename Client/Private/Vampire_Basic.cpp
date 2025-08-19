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
    m_iNumAttacks = END;

    m_AttackCoolTime.resize(m_iNumAttacks, 0.f);
    m_AttackTime.resize(m_iNumAttacks, 0.f);

    m_AttackCoolTime[ATTACK_NORMAL] = 5.f;

    m_fAttackRange = 50.f;
    m_fChaseRange = 40.f;
    m_fMinDistance = 30.f;

    if (FAILED(Ready_AttackMapping()))
        return E_FAIL;

    return S_OK;
}

HRESULT CVampire_Basic::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    m_iStateFlag = ENUM_CLASS(STATE_FLAG::SPAWN);

    if (FAILED(Ready_PawnObject()))
        return E_FAIL;

    if(FAILED(Ready_Collider()))
        return E_FAIL;

    return S_OK;
}

void CVampire_Basic::Priority_Update(_float fTimeDelta)
{
    m_vPrevPosition = m_pTransformCom->Get_State(STATE::POSITION);

    for (auto& Pair : m_PawnObjects)
        Pair.second->Priority_Update(fTimeDelta);
}

void CVampire_Basic::Update(_float fTimeDelta)
{
    CMonster::Update_AttackCoolTime(fTimeDelta);

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

    __super::Update_Colliders(m_iStateFlag);

    for (auto& Pair : m_PawnObjects)
        Pair.second->Late_Update(fTimeDelta);

#ifdef _DEBUG
    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::NONBLEND, this)))
        return;
#endif
}

HRESULT CVampire_Basic::Render()
{
#ifdef _DEBUG
    for (auto& Pair : m_Colliders)
    {
    	for (auto& pCollider : Pair.second)
    	{
    		pCollider->Render();
    	}
    }
    //for (_uint i = 0; i < m_Colliders[COLLIDER_CHANNEL::ATTACK].size(); i++)
    //{
    //    m_AttackColliderCombinedMatrix[i] = XMMatrixMultiply(XMLoadFloat4x4(m_AttackColliderSocketMatrix[i]), m_pTransformCom->Get_WorldMatrix());
    //    m_Colliders[COLLIDER_CHANNEL::ATTACK][i]->Render();
    //}
#endif
    return S_OK;
}

HRESULT CVampire_Basic::Ready_PawnObject()
{
    CBody::BODY_DESC BodyObjectDesc = {};
    BodyObjectDesc.pPawnMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    BodyObjectDesc.pStateFlag = &m_iStateFlag;

    if (FAILED(__super::Add_PawnObject(TEXT("Vampire_Basic_Body"), ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Vampire_Basic_Body"), &BodyObjectDesc)))
        return E_FAIL;

    m_pBody = static_cast<CBody*>(Find_PawnObject(TEXT("Vampire_Basic_Body")));

    m_pAnimMovement = m_pBody->Get_AnimMovementPtr();

    return S_OK;
}

HRESULT CVampire_Basic::Ready_Collider()
{
    if (FAILED(Ready_Collider_Bounding()))
        return E_FAIL;

    if (FAILED(Ready_Collider_Body()))
        return E_FAIL;

    if (FAILED(Ready_Collider_Hit()))
        return E_FAIL;

    if (FAILED(Ready_Collider_Attack()))
        return E_FAIL;

    return S_OK;
}

HRESULT CVampire_Basic::Ready_Collider_Bounding()
{
    CBoundingAABB::BOUNDING_AABB_DESC  AABBDesc = {};
    AABBDesc.vExtents = _float3(30.f, 30.f, 30.f);
    AABBDesc.vCenter = _float3(0.f, AABBDesc.vExtents.y, 0.f);

    CCollider::COLLIDER_DESC ColliderDesc = {};
    ColliderDesc.eChannel = COLLIDER_CHANNEL::BOUNDING;
    ColliderDesc.eOwner = COLLIDER_OWNER::MONSTER;
    ColliderDesc.BoundingDesc = &AABBDesc;

    CCollider* pBoundingCollider = { nullptr };

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_AABB"),
        TEXT("Com_Collider_Bounding"), reinterpret_cast<CComponent**>(&pBoundingCollider), &ColliderDesc)))
        return E_FAIL;

    m_Colliders[COLLIDER_CHANNEL::BOUNDING].push_back(pBoundingCollider);

    return S_OK;
}

HRESULT CVampire_Basic::Ready_Collider_Body()
{
    CBoundingOBB::BOUNDING_OBB_DESC OBBDesc = {};
    OBBDesc.vAngles = _float3(0.f, 0.f, 0.f);
    OBBDesc.vExtents = _float3(15.f, 18.f, 15.f);
    OBBDesc.vCenter = _float3(0.f, OBBDesc.vExtents.y, 0.f);

    CCollider::COLLIDER_DESC ColliderDesc = {};
    ColliderDesc.eChannel = COLLIDER_CHANNEL::BODY;
    ColliderDesc.eOwner = COLLIDER_OWNER::MONSTER;
    ColliderDesc.BoundingDesc = &OBBDesc;

    CCollider* pBodyCollider = { nullptr };

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_OBB"),
        TEXT("Com_Collider_Body"), reinterpret_cast<CComponent**>(&pBodyCollider), &ColliderDesc)))
        return E_FAIL;

    m_Colliders[COLLIDER_CHANNEL::BODY].push_back(pBodyCollider);

    pBodyCollider->SetCollisionCallBack([this](const CCollider::COLLISION_DATA& Data) {
        this->OnCollisionBlock(Data); }
    );

    return S_OK;
}

HRESULT CVampire_Basic::Ready_Collider_Hit()
{
    m_Colliders[COLLIDER_CHANNEL::HIT].resize(ENUM_CLASS(HIT_COLLIDER::END), nullptr);
    m_HitColliderSocketMatrix.resize(ENUM_CLASS(HIT_COLLIDER::END), nullptr);
    m_HitColliderCombinedMatrix.resize(ENUM_CLASS(HIT_COLLIDER::END), XMMatrixIdentity());

    /* Com_Collider_Hit_Head */
    CBoundingOBB::BOUNDING_OBB_DESC OBBDesc = {};
    OBBDesc.vAngles = _float3(0.f, 0.f, XMConvertToRadians(90.f));
    OBBDesc.vExtents = _float3(4.f, 7.f, 4.f);
    OBBDesc.vCenter = _float3(0.f, 0.f, 0.f);

    if (FAILED(__super::Add_Collider_Hit(TEXT("Com_Collider_Hit_Head"), COLLIDER_OWNER::MONSTER, &OBBDesc, ENUM_CLASS(HIT_COLLIDER::HEAD), m_pBody->SocketCombinedMatrixPtr("ValveBiped.Bip01_Head"))))
        return E_FAIL;


    /* Com_Collider_Hit_Upper */
    OBBDesc.vAngles = _float3(0.f, 0.f, XMConvertToRadians(90.f));
    OBBDesc.vExtents = _float3(6.f, 10.f, 6.f);
    OBBDesc.vCenter = _float3(3.f, 0.f, 0.f);

    if (FAILED(__super::Add_Collider_Hit(TEXT("Com_Collider_Hit_Upper"), COLLIDER_OWNER::MONSTER, &OBBDesc, ENUM_CLASS(HIT_COLLIDER::BODY), m_pBody->SocketCombinedMatrixPtr("ValveBiped.Bip01_Spine1"))))
        return E_FAIL;

    return S_OK;
}

HRESULT CVampire_Basic::Ready_Collider_Attack()
{
    m_Colliders[COLLIDER_CHANNEL::ATTACK].resize(ENUM_CLASS(ATTACK_COLLIDER::END), nullptr);
    m_AttackColliderSocketMatrix.resize(ENUM_CLASS(ATTACK_COLLIDER::END), nullptr);
    m_AttackColliderCombinedMatrix.resize(ENUM_CLASS(ATTACK_COLLIDER::END), XMMatrixIdentity());

    /* Com_Collider_Attack_Sword */
    CBoundingOBB::BOUNDING_OBB_DESC OBBDesc = {};
    OBBDesc.vAngles = _float3(0.f, 0.f, XMConvertToRadians(90.f));
    OBBDesc.vExtents = _float3(30.f, 20.f, 30.f);
    OBBDesc.vCenter = _float3(0.f, 0.f, 0.f);

    if (FAILED(__super::Add_Collider_Attack(TEXT("Com_Collider_Attack_LeftHand"), COLLIDER_OWNER::MONSTER, &OBBDesc, ENUM_CLASS(ATTACK_COLLIDER::LEFT_HAND), m_pBody->SocketCombinedMatrixPtr("ValveBiped.Bip01_L_Hand"))))
        return E_FAIL;

    Bind_AttackCollisionCallback(ENUM_CLASS(ATTACK_COLLIDER::LEFT_HAND));

    /* Com_Collider_Attack_Shield */
    OBBDesc.vAngles = _float3(XMConvertToRadians(60.f), XMConvertToRadians(90.f), XMConvertToRadians(180.f));
    OBBDesc.vExtents = _float3(30.f, 20.f, 30.f);
    OBBDesc.vCenter = _float3(0.f, 0.f, 0.f);

    if (FAILED(__super::Add_Collider_Attack(TEXT("Com_Collider_Attack_RightHand"), COLLIDER_OWNER::MONSTER, &OBBDesc, ENUM_CLASS(ATTACK_COLLIDER::RIGHT_HAND), m_pBody->SocketCombinedMatrixPtr("ValveBiped.Bip01_R_Hand"))))
        return E_FAIL;

    Bind_AttackCollisionCallback(ENUM_CLASS(ATTACK_COLLIDER::RIGHT_HAND));

    return S_OK;
}

HRESULT CVampire_Basic::Ready_AttackMapping()
{
    _uint iFlag = ENUM_CLASS(STATE_FLAG::ATTACK);

    m_AttackMapping[iFlag].push_back({ ENUM_CLASS(ATTACK_COLLIDER::LEFT_HAND), false, 1.f, _float2(0.2f, 0.6f) });
    m_AttackMapping[iFlag].push_back({ ENUM_CLASS(ATTACK_COLLIDER::RIGHT_HAND), false, 1.f, _float2(0.2f, 0.6f) });

    return S_OK;
}

void CVampire_Basic::Compute_AnimPosition()
{
    _vector vAnimPosition = XMVectorSetY(*m_pAnimMovement, 0.f);

    _matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();

    _matrix PositionMatrix = XMMatrixTranslationFromVector(vAnimPosition);

    m_pTransformCom->MovePositionToMatrix(PositionMatrix, m_pNavigationCom);
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
}
