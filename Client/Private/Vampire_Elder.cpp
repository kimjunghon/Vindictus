#include "ClientPch.h"
#include "Vampire_Elder.h"
#include "Body.h"
#include "VampireAI.h"
#include "MonsterState.h"

CVampire_Elder::CVampire_Elder(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CVampire { pDevice, pDeviceContext }
{
}

CVampire_Elder::CVampire_Elder(const CVampire_Elder& Prototype)
	: CVampire { Prototype }
{
}

HRESULT CVampire_Elder::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_eType = MONSTER_TYPE::VAMPIRE_ELDER;

	m_iNumAttacks = ENUM_CLASS(VAMPIRE_ATTACK::END);

	m_AttackCoolTime.resize(m_iNumAttacks, 0.f);
	m_AttackTime.resize(m_iNumAttacks, 0.f);

	m_AttackCoolTime[ENUM_CLASS(VAMPIRE_ATTACK::ATTACK_NORMAL)] = 10.f;

	m_fAttackRange = 150.f;
	m_fChaseRange = 120.f;
	m_fMinDistance = 50.f;

	return S_OK;
}

HRESULT CVampire_Elder::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_VampireState(MONSTER_TYPE::VAMPIRE_ELDER)))
		return E_FAIL;

	if (FAILED(Ready_PawnObject()))
		return E_FAIL;

	if (FAILED(Ready_Collider()))
		return E_FAIL;

	if (FAILED(CMonster::Ready_AnimNotify("../Bin/Resources/AnimDatas/Vampire_Elder_AnimData.json")))
		return E_FAIL;

	return S_OK;
}

void CVampire_Elder::Priority_Update(_float fTimeDelta)
{
	m_vPrevPosition = m_pTransformCom->Get_State(STATE::POSITION);

	for (auto& Pair : m_PawnObjects)
		Pair.second->Priority_Update(fTimeDelta);
}

void CVampire_Elder::Update(_float fTimeDelta)
{
	CMonster::Update_AttackCoolTime(fTimeDelta);

	m_pAI->Update();

	m_pCurrentState->Update(this, fTimeDelta);

	Bind_StateFlag();

	if (false == m_IsActive)
		return;

	for (auto& Pair : m_PawnObjects)
		Pair.second->Update(fTimeDelta);
}

void CVampire_Elder::Late_Update(_float fTimeDelta)
{
	Compute_AnimPosition();

	for (auto& Pair : m_PawnObjects)
		Pair.second->Late_Update(fTimeDelta);

	__super::Update_Colliders(m_pTransformCom->Get_WorldMatrix());

#ifdef _DEBUG
	if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::NONBLEND, this)))
		return;
#endif

}

HRESULT CVampire_Elder::Render()
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

HRESULT CVampire_Elder::Ready_PawnObject()
{
	CBody::BODY_DESC BodyObjectDesc = {};
	BodyObjectDesc.pPawnMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	BodyObjectDesc.pStateFlag = &m_iStateFlag;

	if (FAILED(__super::Add_PawnObject(TEXT("Vampire_Elder_Body"), ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Vampire_Elder_Body"), &BodyObjectDesc)))
		return E_FAIL;

	m_pBody = static_cast<CBody*>(Find_PawnObject(TEXT("Vampire_Elder_Body")));

	m_pAnimMovement = m_pBody->Get_AnimMovementPtr();

	return S_OK;
}

HRESULT CVampire_Elder::Ready_Collider()
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

HRESULT CVampire_Elder::Ready_Collider_Bounding()
{
	CBoundingAABB::BOUNDING_AABB_DESC  AABBDesc = {};
	AABBDesc.vExtents = _float3(50.f, 50.f, 50.f);
	AABBDesc.vCenter = _float3(0.f, AABBDesc.vExtents.y, 0.f);

	if (FAILED(__super::Add_Collider_Bounding(TEXT("Com_Collider_Bounding"), COLLIDER_OWNER::MONSTER, &AABBDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CVampire_Elder::Ready_Collider_Body()
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

HRESULT CVampire_Elder::Ready_Collider_Hit()
{
	m_Colliders[COLLIDER_CHANNEL::HIT].resize(ENUM_CLASS(HIT_COLLIDER::END), nullptr);
	m_HitColliderSocketMatrix.resize(ENUM_CLASS(HIT_COLLIDER::END), nullptr);
	m_HitColliderCombinedMatrix.resize(ENUM_CLASS(HIT_COLLIDER::END), XMMatrixIdentity());

	CBoundingOBB::BOUNDING_OBB_DESC OBBDesc = {};
	OBBDesc.vAngles = _float3(0.f, 0.f, 0.f);
	OBBDesc.vExtents = _float3(18.f, 30.f, 18.f);
	OBBDesc.vCenter = _float3(0.f, OBBDesc.vExtents.y, 0.f);

	if (FAILED(__super::Add_Collider_Hit(TEXT("Com_Collider_Hit_Head"), COLLIDER_OWNER::MONSTER, &OBBDesc, ENUM_CLASS(HIT_COLLIDER::HEAD), m_pBody->SocketCombinedMatrixPtr("ValveBiped.Bip01_Head"))))
		return E_FAIL;

	/* Com_Collider_Hit_Upper */
	OBBDesc.vAngles = _float3(0.f, 0.f, XMConvertToRadians(90.f));
	OBBDesc.vExtents = _float3(25.f, 25.f, 25.f);
	OBBDesc.vCenter = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Collider_Hit(TEXT("Com_Collider_Hit_Upper"), COLLIDER_OWNER::MONSTER, &OBBDesc, ENUM_CLASS(HIT_COLLIDER::UPPER), m_pBody->SocketCombinedMatrixPtr("ValveBiped.Bip01_Spine2"))))
		return E_FAIL;

	/* Com_Collider_Hit_Upper */
	OBBDesc.vAngles = _float3(0.f, 0.f, XMConvertToRadians(90.f));
	OBBDesc.vExtents = _float3(25.f, 25.f, 25.f);
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

HRESULT CVampire_Elder::Ready_Collider_Attack()
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

	return S_OK;
}

void CVampire_Elder::CreateFireBall(ATTACK_TYPE eType, _float fAttackRatio)
{
}

void CVampire_Elder::ThrowFireBall()
{
}

HRESULT CVampire_Elder::Add_AttackCollisionNotify(const string& strAnimName, _uint iAttackColliderIndex, ATTACK_TYPE eType, _float fAttackRatio, _float2 vTrackPosition)
{
	if (!strcmp(strAnimName.c_str(), "Attack_FireBall"))
		Add_FireBallNotify(eType, fAttackRatio, vTrackPosition);
	else
	{
		if (FAILED(m_pBody->Add_AnimNotify(strAnimName, vTrackPosition.x, [this, iAttackColliderIndex, eType, fAttackRatio]() {
			this->m_Colliders[COLLIDER_CHANNEL::ATTACK][iAttackColliderIndex]->SetEnable(true);
			m_CurrentAttackData.iAttackID = m_iStateFlag + (reinterpret_cast<size_t>(this) << 1);
			m_CurrentAttackData.eAttackType = eType;
			m_CurrentAttackData.fDamage = m_Status.fAttackDamage * fAttackRatio;
			m_CurrentAttackData.vAttackPosition = m_pTransformCom->Get_State(STATE::POSITION);
			this->m_Colliders[COLLIDER_CHANNEL::ATTACK][iAttackColliderIndex]->Set_Desc(&m_CurrentAttackData);
			})))
			return E_FAIL;

		if (FAILED(m_pBody->Add_AnimNotify(strAnimName, vTrackPosition.y, [this, iAttackColliderIndex]() {
			this->m_Colliders[COLLIDER_CHANNEL::ATTACK][iAttackColliderIndex]->SetEnable(false);
			this->m_Colliders[COLLIDER_CHANNEL::ATTACK][iAttackColliderIndex]->Set_Desc(nullptr);
			})))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CVampire_Elder::Add_FireBallNotify(ATTACK_TYPE eType, _float fAttackRatio, _float2 vTrackPosition)
{
	if (FAILED(m_pBody->Add_AnimNotify("Attack_FireBall", vTrackPosition.x, [this, eType, fAttackRatio]() {
		this->CreateFireBall(eType, fAttackRatio);
		})))
		return E_FAIL;

	if (FAILED(m_pBody->Add_AnimNotify("Attack_FireBall", vTrackPosition.y, [this]() {
		this->ThrowFireBall();
		})))
		return E_FAIL;

	return S_OK;
}

CVampire_Elder* CVampire_Elder::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CVampire_Elder* pInstance = new CVampire_Elder(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed Created : CVampire_Elder"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CVampire_Elder::Clone(void* pArg)
{
	CVampire_Elder* pInstance = new CVampire_Elder(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CVampire_Elder"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CVampire_Elder::Free()
{
	__super::Free();

}
