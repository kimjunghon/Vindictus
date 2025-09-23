#include "ClientPch.h"
#include "TriggerBox.h"

CTriggerBox::CTriggerBox(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CGameObject { pDevice, pDeviceContext }
{
}

CTriggerBox::CTriggerBox(const CTriggerBox& Prototype)
	: CGameObject { Prototype }
{
}

HRESULT CTriggerBox::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTriggerBox::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	TRIGGER_DESC* pDesc = static_cast<TRIGGER_DESC*>(pArg);

	m_Callback = pDesc->Callback;

	m_pTransformCom->Set_State(STATE::POSITION, pDesc->vPosition);

	if (FAILED(Ready_Component(pDesc->vSize)))
		return E_FAIL;

	return S_OK;
}

void CTriggerBox::Priority_Update(_float fTimeDelta)
{
}

void CTriggerBox::Update(_float fTimeDelta)
{
}

void CTriggerBox::Late_Update(_float fTimeDelta)
{
	m_pColliderContainer->Update(this, m_pTransformCom->Get_WorldMatrix());
}

HRESULT CTriggerBox::Render()
{
	return S_OK;
}

HRESULT CTriggerBox::Ready_Component(_float3 vSize)
{
	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_Container"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderContainer))))
		return E_FAIL;

	CBoundingAABB::BOUNDING_AABB_DESC  AABBDesc = {};
	AABBDesc.vExtents = _float3(vSize.x, vSize.y, vSize.z);
	AABBDesc.vCenter = _float3(0.f, 0.f, 0.f);

	if (FAILED(m_pColliderContainer->Add_Collider(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_AABB"),
		ENUM_CLASS(COLLIDER_CHANNEL::BOUNDING), ENUM_CLASS(COLLIDER_OWNER::OBJECT), &AABBDesc, nullptr)))
		return E_FAIL;

	CBoundingOBB::BOUNDING_OBB_DESC OBBDesc = {};
	OBBDesc.vAngles = _float3(0.f, 0.f, 0.f);
	OBBDesc.vExtents = _float3(vSize.x, vSize.y, vSize.z);
	OBBDesc.vCenter = _float3(0.f, 0.f, 0.f);

	if (FAILED(m_pColliderContainer->Add_Collider(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_OBB"),
		ENUM_CLASS(COLLIDER_CHANNEL::INTERACTION), ENUM_CLASS(COLLIDER_OWNER::OBJECT), &OBBDesc, nullptr)))
		return E_FAIL;

	if (FAILED(m_pColliderContainer->Bind_Collision_Callback(ENUM_CLASS(COLLIDER_CHANNEL::INTERACTION), 0, COLLIDER_STATE::BEGIN, [this](const CCollider::COLLISION_DATA& Data) {
		this->m_Callback();
		m_pColliderContainer->SetEnableAllColliderChannel(false);})))
		return E_FAIL;

	return S_OK;
}

CTriggerBox* CTriggerBox::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CTriggerBox* pInstance = new CTriggerBox(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed Created : CTriggerBox"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CTriggerBox::Clone(void* pArg)
{
	CTriggerBox* pInstance = new CTriggerBox(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CTriggerBox"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTriggerBox::Free()
{
	__super::Free();

	Safe_Release(m_pColliderContainer);
}
