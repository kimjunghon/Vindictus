#include "ClientPch.h"
#include "QueenDoor.h"
#include "Effect.h"

CQueenDoor::CQueenDoor(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CMapObject { pDevice, pDeviceContext }
	, m_pPoolInstance { CPool_Instance::GetInstance()}
{
	Safe_AddRef(m_pPoolInstance);
}

CQueenDoor::CQueenDoor(const CQueenDoor& Prototype)
	: CMapObject { Prototype }
	, m_pPoolInstance { Prototype.m_pPoolInstance }
	, m_fEffectDelay { Prototype.m_fEffectDelay }
	, m_fDownSpeed { Prototype.m_fDownSpeed }
	, m_fMinY { Prototype.m_fMinY }
{
	Safe_AddRef(m_pPoolInstance);
}

HRESULT CQueenDoor::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_fEffectDelay = 1.f;
	m_fDownSpeed = 10.f;
	m_fMinY = -80.f;

	return S_OK;
}

HRESULT CQueenDoor::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Colliders()))
		return E_FAIL;

	m_vEffectPosition = m_pTransformCom->Get_State(STATE::POSITION);

	m_pGameInstance->Subscribe<EVENT_QUEEN_DEAD>(ENUM_CLASS(EVENT_TYPE::NONSTATIC), [this](const EVENT_QUEEN_DEAD& Event) { this->Event_QueenDead(Event); });

	return S_OK;
}

void CQueenDoor::Priority_Update(_float fTimeDelta)
{
	if (m_pGameInstance->Get_KeyDown(DIK_O))
		m_IsOpen = true;

}

void CQueenDoor::Update(_float fTimeDelta)
{
	if (m_IsDown)
		return;

	if (m_IsOpen)
		OpenDoor(fTimeDelta);
}

void CQueenDoor::Late_Update(_float fTimeDelta)
{
	if (m_IsDown)
		return;

	CMapObject::Late_Update(fTimeDelta);

	_matrix ColliderMatrix = XMMatrixTranslationFromVector(m_pTransformCom->Get_State(STATE::POSITION));

	m_pColliderContainer->Update(this, ColliderMatrix);
}

HRESULT CQueenDoor::Render()
{
	return CMapObject::Render();
}

HRESULT CQueenDoor::Ready_Colliders()
{
	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_Container"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderContainer))))
		return E_FAIL;

	CBoundingAABB::BOUNDING_AABB_DESC  AABBDesc = {};
	AABBDesc.vExtents = _float3(60.f, 200.f, 10.f);
	AABBDesc.vCenter = _float3(0.f, AABBDesc.vExtents.y, 0.f);

	if (FAILED(m_pColliderContainer->Add_Collider(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_AABB"),
		ENUM_CLASS(COLLIDER_CHANNEL::BOUNDING), ENUM_CLASS(COLLIDER_OWNER::OBJECT), &AABBDesc, nullptr)))
		return E_FAIL;

	CBoundingOBB::BOUNDING_OBB_DESC OBBDesc = {};
	OBBDesc.vAngles = _float3(0.f, 0.f, 0.f);
	OBBDesc.vExtents = _float3(60.f, 200.f, 10.f);
	OBBDesc.vCenter = _float3(0.f, OBBDesc.vExtents.y, 0.f);

	if (FAILED(m_pColliderContainer->Add_Collider(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_OBB"),
		ENUM_CLASS(COLLIDER_CHANNEL::BODY), ENUM_CLASS(COLLIDER_OWNER::OBJECT), &OBBDesc, nullptr)))
		return E_FAIL;

	return S_OK;
}

void CQueenDoor::OpenDoor(_float fTimeDelta)
{
	m_fCurrentEffectDelay += fTimeDelta;

	if (m_fCurrentEffectDelay >= m_fEffectDelay)
	{
		m_fCurrentEffectDelay = 0.f;
		CEffect::EFFECT_SPAWN_DESC SpawnDesc = {};
		SpawnDesc.SpawnWorldMatrix = XMMatrixTranslationFromVector(m_vEffectPosition);
		SpawnDesc.IsEmissive = false;

		m_pPoolInstance->Request_SpawnEffect(TEXT("DoorDust"), &SpawnDesc);
	}

	_vector vPosition = m_pTransformCom->Get_State(STATE::POSITION);
	_float fY = XMVectorGetY(vPosition);
	fY -= m_fDownSpeed * fTimeDelta;

	if (fY <= m_fMinY)
		m_IsDown = true;

	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetY(vPosition, fY));
}

void CQueenDoor::Event_QueenDead(const EVENT_QUEEN_DEAD& Event)
{
	m_IsOpen = true;
}

CQueenDoor* CQueenDoor::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CQueenDoor* pInstance = new CQueenDoor(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed Created : CQueenDoor"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CQueenDoor::Clone(void* pArg)
{
	CQueenDoor* pInstance = new CQueenDoor(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CQueenDoor"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CQueenDoor::Free()
{
	__super::Free();

	Safe_Release(m_pColliderContainer);
	Safe_Release(m_pPoolInstance);
}
