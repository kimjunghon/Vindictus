#include "ClientPch.h"
#include "GavelenRock.h"
#include "Effect.h"

CGavelenRock::CGavelenRock(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CProjectile { pDevice, pDeviceContext }
{
}

CGavelenRock::CGavelenRock(const CGavelenRock& Prototype)
	: CProjectile { Prototype }
{
}

HRESULT CGavelenRock::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGavelenRock::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Component()))
		return E_FAIL;

	return S_OK;
}

void CGavelenRock::Priority_Update(_float fTimeDelta)
{
}

void CGavelenRock::Update(_float fTimeDelta)
{
	if (*m_pIsSwing && false == m_IsMove)
	{
		m_IsMove = true;
		Update_Bezier();
		*m_pIsSwing = false;

		m_pColliderContainer->SetEnableAllColliderChannel(true);
		m_pColliderContainer->SetDesc(ENUM_CLASS(COLLIDER_CHANNEL::ATTACK), 0, &m_CurrentAttackData);
	}

	if (m_IsMove)
		CProjectile::Move(fTimeDelta);
	else
		m_pTransformCom->Set_WorldMatrix(XMMatrixMultiply(XMLoadFloat4x4(m_pSocektMatrixPtr), XMLoadFloat4x4(m_pOwnerMatrixPtr)));

	//test
	if (XMVectorGetY(m_pTransformCom->Get_State(STATE::POSITION)) <= 0.f)
		ReturnToPool();

	m_pTransformCom->TurnQuaternion(XMQuaternionSlerp(XMQuaternionIdentity(), m_vRotateQuat, fTimeDelta));
}

void CGavelenRock::Late_Update(_float fTimeDelta)
{
	m_pColliderContainer->Update(this, m_pTransformCom->Get_WorldMatrix());

	if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::NONBLEND, this)))
		return;
}

HRESULT CGavelenRock::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		m_pModelCom->Bind_Shader_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE, 0);

		m_pShaderCom->Begin(ENUM_CLASS(SHADER_VTXMESH::DEFAULT));

		m_pModelCom->Render(i);

		m_pShaderCom->Bind_SPV("g_DiffuseTexture", nullptr);
	}

	return S_OK;
}

HRESULT CGavelenRock::Spawn(void* pArg)
{
	GAVELEN_ROCK_DESC* pDesc = static_cast<GAVELEN_ROCK_DESC*>(pArg);

	m_vRotateQuat = ComputeRandomRotateQuat();

	m_IsActive = true;

	m_CurrentAttackData.eAttackType = pDesc->eType;
	m_CurrentAttackData.fDamage = pDesc->fDamage;

	m_pTargetTransformCom = pDesc->pTargetTransform;

	m_pIsSwing = pDesc->pIsSwing;
	m_pSocektMatrixPtr = pDesc->pSocketMatrixPtr;
	m_pOwnerMatrixPtr = pDesc->pOwnerMatrixPtr;

	m_fCurrentDistance = 0.f;

	m_IsCurveFinished = false;

	m_LUT.clear();

	return S_OK;
}

void CGavelenRock::ReturnToPool()
{
	CEffect::EFFECT_SPAWN_DESC EffectDesc = {};
	EffectDesc.SpawnWorldMatrix = m_pTransformCom->Get_WorldMatrix();
	EffectDesc.IsEmissive = false;

	if (FAILED(m_pPoolInstance->Request_SpawnEffect(TEXT("Rock_Prefab"), &EffectDesc)))
		return;

	m_pColliderContainer->SetEnableAllColliderChannel(false);
	m_pColliderContainer->SetDesc(ENUM_CLASS(COLLIDER_CHANNEL::ATTACK), 0, nullptr);

	m_IsActive = false;

	m_pTargetTransformCom = nullptr;

	m_pIsSwing = nullptr;
	m_pSocektMatrixPtr = nullptr;
	m_pOwnerMatrixPtr = nullptr;
}

HRESULT CGavelenRock::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_Shader_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
		return E_FAIL;
	
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
		return E_FAIL;

	return S_OK;
}

HRESULT CGavelenRock::Ready_Component()
{
	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::GLASGAVELEN), TEXT("Prototype_Component_Model_GavelenRock"), 
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	CBoundingAABB::BOUNDING_AABB_DESC  AABBDesc = {};
	AABBDesc.vExtents = _float3(120.f, 120.f, 120.f);
	AABBDesc.vCenter = _float3(0.f, 0.f, 0.f);

	if (FAILED(m_pColliderContainer->Add_Collider(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_AABB"),
		ENUM_CLASS(COLLIDER_CHANNEL::BOUNDING), ENUM_CLASS(COLLIDER_OWNER::MONSTER), &AABBDesc, nullptr)))
		return E_FAIL;

	CBoundingOBB::BOUNDING_OBB_DESC OBBDesc = {};
	OBBDesc.vAngles = _float3(0.f, 0.f, 0.f);
	OBBDesc.vExtents = _float3(100.f, 80.f, 100.f);
	OBBDesc.vCenter = _float3(0.f, 0.f, 0.f);

	if (FAILED(m_pColliderContainer->Add_Collider(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_OBB"),
		ENUM_CLASS(COLLIDER_CHANNEL::ATTACK), ENUM_CLASS(COLLIDER_OWNER::MONSTER), &OBBDesc, nullptr)))
		return E_FAIL;

	if (FAILED(m_pColliderContainer->Bind_Collision_Callback(ENUM_CLASS(COLLIDER_CHANNEL::ATTACK), 0, COLLIDER_STATE::BEGIN, [this](const CCollider::COLLISION_DATA& Data) {
		this->OnCollisionAttack(Data); })))
		return E_FAIL;

	return S_OK;
}

void CGavelenRock::Update_Bezier()
{
	_vector vStartPosition = m_pTransformCom->Get_State(STATE::POSITION);
	_vector vFinishPosition = m_pTargetTransformCom->Get_State(STATE::POSITION);

	_vector vDir = XMVector3Normalize((XMVectorSubtract(vFinishPosition, vStartPosition)));
	_vector vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vDir);
	_vector vUp = XMVector3Normalize(XMVector3Cross(vDir, vRight));

	vFinishPosition = vFinishPosition + (vDir * 10.f);

	_float fLength = XMVectorGetX(XMVector3Length(XMVectorSubtract(vFinishPosition, vStartPosition)));

	_float fLookWieght = fLength * 0.5f;
	_float fUpWieght = fLength * 0.3f;

	_vector vPoints[3] = {};

	vPoints[0] = vStartPosition;
	vPoints[1] = vStartPosition + (vDir * fLookWieght) + (vUp * fUpWieght);
	vPoints[2] = vFinishPosition;

	CProjectile::Ready_Bezier(3, vPoints);
}

_vector CGavelenRock::ComputeRandomRotateQuat()
{
	_float fYaw = {};
	_float fPitch = {};
	_float fRoll = {};

	fYaw = m_pGameInstance->Rand(-1.f, 1.f);
	fPitch = m_pGameInstance->Rand(-1.f, 1.f);
	fRoll = m_pGameInstance->Rand(-1.f, 1.f);

	return XMQuaternionRotationRollPitchYaw(fPitch, fYaw, fRoll);
}

CGavelenRock* CGavelenRock::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CGavelenRock* pInstance = new CGavelenRock(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed Created : CGavelenRock"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CGavelenRock::Clone(void* pArg)
{
	CGavelenRock* pInstance = new CGavelenRock(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CGavelenRock"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CGavelenRock::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
