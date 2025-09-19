#include "ClientPch.h"
#include "EnergyBall.h"
#include "Effect_Trail.h"

CEnergyBall::CEnergyBall(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CProjectile { pDevice, pDeviceContext }
{
}

CEnergyBall::CEnergyBall(const CEnergyBall& Prototype)
	: CProjectile { Prototype }
	, m_fEffectTime{ Prototype.m_fEffectTime }
{
}

HRESULT CEnergyBall::Initialize_Prototype()
{
	m_fEffectTime = 0.2f;

	return S_OK;
}

HRESULT CEnergyBall::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Component()))
		return E_FAIL;

	return S_OK;
}

void CEnergyBall::Priority_Update(_float fTimeDelta)
{

}

void CEnergyBall::Update(_float fTimeDelta)
{
	m_fCurrentEffectTime += fTimeDelta;

	if(m_fCurrentEffectTime >= m_fEffectTime)
	{
		CEffect::EFFECT_SPAWN_DESC EffectDesc = {};
		EffectDesc.SpawnWorldMatrix = m_pTransformCom->Get_WorldMatrix();
		EffectDesc.IsEmissive = true;

		m_pPoolInstance->Request_SpawnEffect(TEXT("EnergyBall"), &EffectDesc);
		m_fCurrentEffectTime = 0.f;
	}

	Update_Bezier(fTimeDelta);

	CProjectile::Move(fTimeDelta);

	m_pVIBufferCom->Update(fTimeDelta);

	//Test
	if (XMVectorGetY(m_pTransformCom->Get_State(STATE::POSITION)) <= 0.f)
		ReturnToPool();
}

void CEnergyBall::Late_Update(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::BLEND, this)))
		return;
}

HRESULT CEnergyBall::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(ENUM_CLASS(SHADER_POINTPARTICE::LOOP));

	m_pVIBufferCom->Bind_Resources();

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CEnergyBall::Spawn(void* pArg)
{
	m_IsActive = true;

	m_pVIBufferCom->Reset();

	ENERGYBALL_DESC* pDesc = static_cast<ENERGYBALL_DESC*>(pArg);

	m_eType = pDesc->eType;
	m_fDamage = pDesc->fDamage;
	m_pTargetTransformCom = pDesc->pTargetTransform;

	m_pTransformCom->Set_State(STATE::POSITION, pDesc->StartPosition);
	
	m_fCurrentDistance = 0.f;
	m_IsCurveFinished = false;

	Init_Bezier();

	m_IsMove = true;

	CEffect_Trail::TRAIL_DESC TrailDesc = {};
	TrailDesc.IsSwing = &m_IsMove;
	TrailDesc.fLifeTime = 3.f;
	TrailDesc.pSocketMatrix = nullptr;
	TrailDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	TrailDesc.vLeftPosition = _float3(0.f, -5.f, 0.f);
	TrailDesc.vRightPosition = _float3(0.f, 5.f, 0.f);
	TrailDesc.fNodeUpdateTime = 0.f;
	TrailDesc.IsEmissive = true;

	m_pPoolInstance->Request_SpawnEffect(TEXT("EnergyTrail"), &TrailDesc);

	return S_OK;
}

void CEnergyBall::ReturnToPool()
{
	m_IsActive = false;

	m_pPoolInstance->ReturnPool(TEXT("EnergyBall"), this);
	m_IsMove = false;
	m_fCurrentEffectTime = 0.f;
	m_pTargetTransformCom = nullptr;
}

HRESULT CEnergyBall::Ready_Component()
{
	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::GLASGAVELEN), TEXT("Prototype_Component_Texture_EnergyBallSeed"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::GLASGAVELEN), TEXT("Prototype_Component_EffectBuffer_EnergyBallSeed"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxInstance_PointParitlce"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CEnergyBall::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_Shader_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_Shader_Texture(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Shader_Color(m_pShaderCom, "g_vSourceColor")))
		return E_FAIL;

	return S_OK;
}

void CEnergyBall::Init_Bezier()
{
	_vector vStartPosition = m_pTransformCom->Get_State(STATE::POSITION);
	_vector vFinishPosition = m_pTargetTransformCom->Get_State(STATE::POSITION);

	_vector vDir = XMVector3Normalize((XMVectorSubtract(vFinishPosition, vStartPosition)));
	_vector vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vDir);
	_vector vUp = XMVector3Normalize(XMVector3Cross(vDir, vRight));

	vFinishPosition = vFinishPosition + (XMVectorSet(0.f, 1.f, 0.f, 0.f) * 10.f);

	_float fLength = XMVectorGetX(XMVector3Length(XMVectorSubtract(vFinishPosition, vStartPosition)));

	_float fLookWieght = fLength * 0.2f;
	_float fUpWieght = fLength * 0.5f;

	m_vMovePoints[0] = vStartPosition;
	m_vMovePoints[1] = vStartPosition + (vDir * fLookWieght) + (XMVectorSet(0.f, 1.f, 0.f,0.f) * 150);
	m_vMovePoints[2] = vFinishPosition - (vDir * fLookWieght) + (XMVectorSet(0.f, 1.f, 0.f, 0.f) * 50);
	m_vMovePoints[3] = vFinishPosition;

	m_fMinDistance = XMVectorGetX(XMVector3Length(XMVectorSubtract(m_vMovePoints[1], m_vMovePoints[0])));


	for (_uint i = 0; i < 4; i++)
		m_vMovePoints[i] = XMVectorSetW(m_vMovePoints[i], 1.f);

	CProjectile::Ready_Bezier(4, m_vMovePoints);
}

void CEnergyBall::Update_Bezier(_float fTimeDelta)
{
	if (m_fCurrentDistance <= m_fMinDistance)
		return;

	_vector vCurrentPosition = m_pTransformCom->Get_State(STATE::POSITION);
	_vector vNewFinishPosition = m_pTargetTransformCom->Get_State(STATE::POSITION);

	_float fLength = XMVectorGetX(XMVector3Length(XMVectorSubtract(vNewFinishPosition, vCurrentPosition)));
	_vector vCurrentLook = XMVector3Normalize(XMVectorSubtract(vCurrentPosition, m_vPrevPos));

	_vector vTargetDir = XMVector3Normalize(XMVectorSubtract(vNewFinishPosition, m_vMovePoints[2]));
	_float fLookWieght = fLength * 0.4f;

	m_vMovePoints[3] = XMVectorLerp(m_vMovePoints[3], vNewFinishPosition, fTimeDelta);
	m_vMovePoints[2] = XMVectorLerp(m_vMovePoints[2], m_vMovePoints[3] - (vTargetDir * fLookWieght), fTimeDelta);
	m_vMovePoints[1] = XMVectorLerp(m_vMovePoints[1], m_vMovePoints[0] + (vCurrentLook * fLookWieght) + (XMVectorSet(0.f, 1.f, 0.f, 0.f) * 50), fTimeDelta);

	for (_uint i = 0; i < 4; i++)
		m_vMovePoints[i] = XMVectorSetW(m_vMovePoints[i], 1.f);

	CProjectile::Ready_Bezier(4, m_vMovePoints);
}

CEnergyBall* CEnergyBall::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CEnergyBall* pInstance = new CEnergyBall(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed Created : CEnergyBall"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CEnergyBall::Clone(void* pArg)
{
	CEnergyBall* pInstance = new CEnergyBall(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CEnergyBall"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEnergyBall::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
}
