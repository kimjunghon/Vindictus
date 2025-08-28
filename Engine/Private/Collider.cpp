#include "EnginePch.h"
#include "Collider.h"
#include "GameInstance.h"

CCollider::CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CComponent { pDevice, pDeviceContext }
{
}

CCollider::CCollider(const CCollider& Prototype)
	: CComponent { Prototype }
	, m_eType{ Prototype.m_eType }
#ifdef _DEBUG
	, m_pBatch{ Prototype.m_pBatch }
	, m_pEffect{ Prototype.m_pEffect }
	, m_pInputLayout{ Prototype.m_pInputLayout }
	, m_IsColl{ Prototype.m_IsColl }
#endif
{
#ifdef _DEBUG
	Safe_AddRef(m_pInputLayout);
#endif
}
HRESULT CCollider::Initialize_Prototype(COLLIDER eType)
{
	m_eType = eType;

#ifdef _DEBUG
	m_pBatch = new PrimitiveBatch<VertexPositionColor>(m_pDeviceContext);
	m_pEffect = new BasicEffect(m_pDevice);

	m_pEffect->SetVertexColorEnabled(true);

	const void* pShaderByteCode = { nullptr };
	size_t		iShaderByteCodeLength = {};

	m_pEffect->GetVertexShaderBytecode(&pShaderByteCode, &iShaderByteCodeLength);

	if (FAILED(m_pDevice->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount,
		pShaderByteCode, iShaderByteCodeLength, &m_pInputLayout)))
		return E_FAIL;
#endif

	return S_OK;
}

HRESULT CCollider::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	COLLIDER_DESC* pDesc = static_cast<COLLIDER_DESC*>(pArg);
	
	m_iOwner = pDesc->iOwner;
	m_iChannel = pDesc->iChannel;

	switch (m_eType)
	{
	case COLLIDER::AABB:
		m_pBounding = CBoundingAABB::Create(m_pDevice, m_pDeviceContext, pDesc->BoundingDesc);
		break;
	case COLLIDER::OBB:
		m_pBounding = CBoundingOBB::Create(m_pDevice, m_pDeviceContext, pDesc->BoundingDesc);
		break;
	case COLLIDER::SPHERE:
		m_pBounding = CBoundingSphere::Create(m_pDevice, m_pDeviceContext, pDesc->BoundingDesc);
		break;
	}

	return S_OK;
}

void CCollider::Update(_fmatrix WorldMatrix, _bool IsRotate)
{
	m_pBounding->Update(WorldMatrix, IsRotate);
}

void CCollider::SetEnable(_bool IsEnable)
{
	m_IsEnable = IsEnable;

	if (false == m_IsEnable)
		m_CollisionColliders.clear();
}

void CCollider::OnCollision(COLLIDER_STATE eState, const COLLISION_DATA& Data)
{
	if (m_CallBack[ENUM_CLASS(eState)])
		m_CallBack[ENUM_CLASS(eState)](Data);
}

_bool CCollider::IsColliding(CCollider* pCollider)
{
	auto iter = m_CollisionColliders.find(pCollider);
	if (iter == m_CollisionColliders.end())
		return false;

	return true;
}

void CCollider::EraseCollisionCollider(CCollider* pCollider)
{
	auto iter = m_CollisionColliders.find(pCollider);
	if (iter != m_CollisionColliders.end())
		m_CollisionColliders.erase(iter);
}

_bool CCollider::Intersect(CCollider* pOtherCollider, _float* pDistance, _float3* pNormal)
{
#ifdef _DEBUG
	return m_IsColl = m_pBounding->Intersect(pOtherCollider->m_eType, pOtherCollider->m_pBounding, pDistance, pNormal);
#else
	return m_pBounding->Intersect(pOtherCollider->m_eType, pOtherCollider->m_pBounding, pDistance, pNormal);
#endif
}

_bool CCollider::Intersect_BoundingChannel(CCollider* pOtherCollider)
{
	return Intersect(pOtherCollider);
}
void CCollider::Check_Collision(COLLISION_DATA& Data)
{
	if (false == m_IsEnable)
		return;

	if (false == Data.IsEnable)
	{
		if(IsColliding(Data.pCollider))
		{
			OnCollision(COLLIDER_STATE::END, Data);
			EraseCollisionCollider(Data.pCollider);
		}
		return;
	}

	_float  fDistance = {};
	_float3 vNormal = {};

	if (Intersect(Data.pCollider, &fDistance, &vNormal))
	{
		Data.BlockData.fDistance = fDistance;
		Data.BlockData.vNormal = vNormal;

		if (false == IsColliding(Data.pCollider))
		{
			m_CollisionColliders.insert(Data.pCollider);
			OnCollision(COLLIDER_STATE::BEGIN, Data);
		}
		else
			OnCollision(COLLIDER_STATE::DURING, Data);
	}
	else
	{
		if (IsColliding(Data.pCollider))
		{
			OnCollision(COLLIDER_STATE::END, Data);
			EraseCollisionCollider(Data.pCollider);
		}
	}
}

#ifdef _DEBUG
HRESULT CCollider::Render()
{
	m_pEffect->SetWorld(XMMatrixIdentity());
	m_pEffect->SetView(m_pGameInstance->Get_Transform_Matrix(D3DTS::VIEW));
	m_pEffect->SetProjection(m_pGameInstance->Get_Transform_Matrix(D3DTS::PROJ));

	m_pDeviceContext->IASetInputLayout(m_pInputLayout);
	m_pEffect->Apply(m_pDeviceContext);

	m_pBatch->Begin();

	m_pBounding->Render(m_pBatch, m_IsColl == true ? XMVectorSet(1.f, 0.f, 0.f, 1.f) : XMVectorSet(0.f, 1.f, 0.f, 1.f));

	m_pBatch->End();

	return S_OK;
}
#endif

CCollider* CCollider::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, COLLIDER eType)
{
	CCollider* pInstance = new CCollider(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype(eType)))
	{
		MSG_BOX(TEXT("Failed to Created : CCollider"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CCollider::Clone(void* pArg)
{
	CCollider* pInstance = new CCollider(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CCollider"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CCollider::Free()
{
	__super::Free();

#ifdef _DEBUG
	if (false == m_IsCloned)
	{
		Safe_Delete(m_pEffect);
		Safe_Delete(m_pBatch);
	}

	Safe_Release(m_pInputLayout);
#endif

	Safe_Release(m_pBounding);

	m_CollisionColliders.clear();
}
