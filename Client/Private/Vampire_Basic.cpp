#include "ClientPch.h"
#include "Vampire_Basic.h"
#include "PawnObject.h"
#include "Body.h"
#include "VampireAI.h"
#include "MonsterState.h"

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
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    m_eType = MONSTER_TYPE::VAMPIRE_BASIC;

    m_iNumAttacks = ENUM_CLASS(VAMPIRE_ATTACK::END);

    m_AttackCoolTime.resize(m_iNumAttacks, 0.f);
    m_AttackTime.resize(m_iNumAttacks, 0.f);

    m_AttackCoolTime[ENUM_CLASS(VAMPIRE_ATTACK::ATTACK_NORMAL)] = 5.f;

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

    if (FAILED(Ready_VampireState(MONSTER_TYPE::VAMPIRE_BASIC)))
        return E_FAIL;

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

    m_pCurrentState->Update(this, fTimeDelta);

    Bind_StateFlag();

    for (auto& Pair : m_PawnObjects)
        Pair.second->Update(fTimeDelta);
}

void CVampire_Basic::Late_Update(_float fTimeDelta)
{
    Compute_AnimPosition();

    __super::Update_Colliders(m_pTransformCom->Get_WorldMatrix(), m_iStateFlag);

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
    AABBDesc.vExtents = _float3(50.f, 50.f, 50.f);
    AABBDesc.vCenter = _float3(0.f, AABBDesc.vExtents.y, 0.f);

    if (FAILED(__super::Add_Collider_Bounding(TEXT("Com_Collider_Bounding"), COLLIDER_OWNER::MONSTER, &AABBDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CVampire_Basic::Ready_Collider_Body()
{
    CBoundingOBB::BOUNDING_OBB_DESC OBBDesc = {};
    OBBDesc.vAngles = _float3(0.f, 0.f, 0.f);
    OBBDesc.vExtents = _float3(18.f, 30.f, 18.f);
    OBBDesc.vCenter = _float3(0.f, OBBDesc.vExtents.y, 0.f);

    if (FAILED(__super::Add_Collider_Body(TEXT("Com_Collider_Body"), COLLIDER_OWNER::MONSTER, &OBBDesc)))
        return E_FAIL;

    if (FAILED(__super::Bind_Collision_Callback(COLLIDER_CHANNEL::BODY, 0, COLLIDER_STATE::BEGIN, [this](const CCollider::COLLISION_DATA& Data) {
        this->OnCollisionBlock(Data); })))
        return E_FAIL;

    if (FAILED(__super::Bind_Collision_Callback(COLLIDER_CHANNEL::BODY, 0, COLLIDER_STATE::DURING, [this](const CCollider::COLLISION_DATA& Data) {
        this->OnCollisionBlock(Data); })))
        return E_FAIL;

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
    OBBDesc.vExtents = _float3(15.f, 15.f, 15.f);
    OBBDesc.vCenter = _float3(0.f, 0.f, 0.f);

    if (FAILED(__super::Add_Collider_Hit(TEXT("Com_Collider_Hit_Head"), COLLIDER_OWNER::MONSTER, &OBBDesc, ENUM_CLASS(HIT_COLLIDER::HEAD), m_pBody->SocketCombinedMatrixPtr("ValveBiped.Bip01_Head"))))
        return E_FAIL;

    /* Com_Collider_Hit_Upper */
    OBBDesc.vAngles = _float3(0.f, 0.f, XMConvertToRadians(90.f));
    OBBDesc.vExtents = _float3(30.f, 30.f, 30.f);
    OBBDesc.vCenter = _float3(0.f, 0.f, 0.f);

    if (FAILED(__super::Add_Collider_Hit(TEXT("Com_Collider_Hit_Upper"), COLLIDER_OWNER::MONSTER, &OBBDesc, ENUM_CLASS(HIT_COLLIDER::UPPER), m_pBody->SocketCombinedMatrixPtr("ValveBiped.Bip01_Spine2"))))
        return E_FAIL;

    /* Com_Collider_Hit_Upper */
    OBBDesc.vAngles = _float3(0.f, 0.f, XMConvertToRadians(90.f));
    OBBDesc.vExtents = _float3(30.f, 30.f, 30.f);
    OBBDesc.vCenter = _float3(-OBBDesc.vExtents.y, 0.f, 0.f);

    if (FAILED(__super::Add_Collider_Hit(TEXT("Com_Collider_Hit_Lower"), COLLIDER_OWNER::MONSTER, &OBBDesc, ENUM_CLASS(HIT_COLLIDER::LOWER), m_pBody->SocketCombinedMatrixPtr("ValveBiped.Bip01_Spine"))))
        return E_FAIL;


    for (_uint i = 0; i < ENUM_CLASS(HIT_COLLIDER::END); i++)
    {
        if (FAILED(__super::Bind_Collision_Callback(COLLIDER_CHANNEL::HIT, i, COLLIDER_STATE::BEGIN, [this](const CCollider::COLLISION_DATA& Data) {
            this->OnCollisionHit(Data); })))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CVampire_Basic::Ready_Collider_Attack()
{
    m_Colliders[COLLIDER_CHANNEL::ATTACK].resize(ENUM_CLASS(ATTACK_COLLIDER::END), nullptr);
    m_AttackColliderSocketMatrix.resize(ENUM_CLASS(ATTACK_COLLIDER::END), nullptr);
    m_AttackColliderCombinedMatrix.resize(ENUM_CLASS(ATTACK_COLLIDER::END), XMMatrixIdentity());

    CBoundingOBB::BOUNDING_OBB_DESC OBBDesc = {};
    OBBDesc.vAngles = _float3(0.f, 0.f, XMConvertToRadians(90.f));
    OBBDesc.vExtents = _float3(30.f, 20.f, 30.f);
    OBBDesc.vCenter = _float3(0.f, 0.f, 0.f);

    if (FAILED(__super::Add_Collider_Attack(TEXT("Com_Collider_Attack_LeftHand"), COLLIDER_OWNER::MONSTER, &OBBDesc, ENUM_CLASS(ATTACK_COLLIDER::LEFT_HAND), m_pBody->SocketCombinedMatrixPtr("ValveBiped.Bip01_L_Hand"))))
        return E_FAIL;

    if (FAILED(__super::Bind_Collision_Callback(COLLIDER_CHANNEL::ATTACK, ENUM_CLASS(ATTACK_COLLIDER::LEFT_HAND), COLLIDER_STATE::BEGIN, [this](const CCollider::COLLISION_DATA& Data) {
        this->OnCollisionAttack(Data); })))
        return E_FAIL;

    OBBDesc.vAngles = _float3(XMConvertToRadians(60.f), XMConvertToRadians(90.f), XMConvertToRadians(180.f));
    OBBDesc.vExtents = _float3(30.f, 20.f, 30.f);
    OBBDesc.vCenter = _float3(0.f, 0.f, 0.f);

    if (FAILED(__super::Add_Collider_Attack(TEXT("Com_Collider_Attack_RightHand"), COLLIDER_OWNER::MONSTER, &OBBDesc, ENUM_CLASS(ATTACK_COLLIDER::RIGHT_HAND), m_pBody->SocketCombinedMatrixPtr("ValveBiped.Bip01_R_Hand"))))
        return E_FAIL;

    if (FAILED(__super::Bind_Collision_Callback(COLLIDER_CHANNEL::ATTACK, ENUM_CLASS(ATTACK_COLLIDER::RIGHT_HAND), COLLIDER_STATE::BEGIN, [this](const CCollider::COLLISION_DATA& Data) {
        this->OnCollisionAttack(Data); })))
        return E_FAIL;

    DisableColliderChannel(COLLIDER_CHANNEL::ATTACK);

    return S_OK;
}

HRESULT CVampire_Basic::Ready_AttackMapping()
{
    _uint iFlag = ENUM_CLASS(STATE_FLAG::ATTACK);
    
    m_AttackMapping[iFlag].push_back({ ENUM_CLASS(ATTACK_COLLIDER::RIGHT_HAND), ATTACK_TYPE::STRONG, 1.f, _float2(70.f, 75.f)});
    m_AttackMapping[iFlag].push_back({ ENUM_CLASS(ATTACK_COLLIDER::LEFT_HAND), ATTACK_TYPE::STRONG, 1.f, _float2(89.f, 94.f) });
    
    return S_OK;
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
