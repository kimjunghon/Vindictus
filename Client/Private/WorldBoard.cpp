#include "ClientPch.h"
#include "WorldBoard.h"

CWorldBoard::CWorldBoard(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CMapObject { pDevice, pDeviceContext }
{
}

CWorldBoard::CWorldBoard(const CWorldBoard& Prototype)
	: CMapObject { Prototype }
{
}

HRESULT CWorldBoard::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CWorldBoard::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Colliders()))
		return E_FAIL;

	return S_OK;
}

void CWorldBoard::Priority_Update(_float fTimeDelta)
{
	CMapObject::Priority_Update(fTimeDelta);
}

void CWorldBoard::Update(_float fTimeDelta)
{
	CMapObject::Update(fTimeDelta);
}

void CWorldBoard::Late_Update(_float fTimeDelta)
{
	CMapObject::Late_Update(fTimeDelta);

    m_pColliderContainer->Update(this, m_pTransformCom->Get_WorldMatrix());
}

HRESULT CWorldBoard::Render()
{
	return CMapObject::Render();
}

HRESULT CWorldBoard::Ready_Colliders()
{
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_Container"),
        TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderContainer))))
        return E_FAIL;

    CBoundingAABB::BOUNDING_AABB_DESC  AABBDesc = {};
    AABBDesc.vExtents = _float3(40.f, 40.f, 40.f);
    AABBDesc.vCenter = _float3(0.f, AABBDesc.vExtents.y, 0.f);

    if (FAILED(m_pColliderContainer->Add_Collider(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_AABB"),
        ENUM_CLASS(COLLIDER_CHANNEL::BOUNDING), ENUM_CLASS(COLLIDER_OWNER::OBJECT), &AABBDesc, nullptr)))
        return E_FAIL;

    CBoundingOBB::BOUNDING_OBB_DESC OBBDesc = {};
    OBBDesc.vAngles = _float3(0.f, 0.f, 0.f);
    OBBDesc.vExtents = _float3(40.f, 40.f, 40.f);
    OBBDesc.vCenter = _float3(0.f, OBBDesc.vExtents.y, 0.f);

    if (FAILED(m_pColliderContainer->Add_Collider(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_OBB"),
        ENUM_CLASS(COLLIDER_CHANNEL::INTERACTION), ENUM_CLASS(COLLIDER_OWNER::OBJECT), &OBBDesc, nullptr)))
        return E_FAIL;

    m_pColliderContainer->Bind_Collision_Callback(ENUM_CLASS(COLLIDER_CHANNEL::INTERACTION), 0, COLLIDER_STATE::BEGIN, [this](const CCollider::COLLISION_DATA& Data) {
        EVENT_BOARD_NPC Event;
        Event.IsNear = true;
        m_pGameInstance->Publish(ENUM_CLASS(EVENT_TYPE::STATIC), Event); });


    m_pColliderContainer->Bind_Collision_Callback(ENUM_CLASS(COLLIDER_CHANNEL::INTERACTION), 0, COLLIDER_STATE::END, [this](const CCollider::COLLISION_DATA& Data) {
        EVENT_BOARD_NPC Event;
        Event.IsNear = false;
        m_pGameInstance->Publish(ENUM_CLASS(EVENT_TYPE::STATIC), Event); });

    return S_OK;
}

CWorldBoard* CWorldBoard::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
    CWorldBoard* pInstance = new CWorldBoard(pDevice, pDeviceContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed Created : CWorldBoard"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CWorldBoard::Clone(void* pArg)
{
    CWorldBoard* pInstance = new CWorldBoard(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed Cloned : CWorldBoard"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CWorldBoard::Free()
{
    __super::Free();

    Safe_Release(m_pColliderContainer);
}
