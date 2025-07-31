#include "ClientPch.h"
#include "Queen.h"
#include "Body.h"

CQueen::CQueen(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CMonster { pDevice, pDeviceContext }
{
}

CQueen::CQueen(const CQueen& Prototype)
	: CMonster { Prototype }
{
}

HRESULT CQueen::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CQueen::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_PawnObjects()))
		return E_FAIL;

	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(50.f, 0.f, 0.f, 1.f));

	return S_OK;
}

void CQueen::Priority_Update(_float fTimeDelta)
{
	for (auto& Pair : m_PawnObjects)
		Pair.second->Priority_Update(fTimeDelta);
}

void CQueen::Update(_float fTimeDelta)
{
	for (auto& Pair : m_PawnObjects)
		Pair.second->Update(fTimeDelta);
}

void CQueen::Late_Update(_float fTimeDelta)
{
	Compute_AnimPosition();

	for (auto& Pair : m_PawnObjects)
		Pair.second->Late_Update(fTimeDelta);
}

HRESULT CQueen::Render()
{
	return S_OK;
}

HRESULT CQueen::Ready_PawnObjects()
{
	CBody::BODY_DESC BodyObjectDesc = {};
	BodyObjectDesc.pPawnMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	BodyObjectDesc.pStateFlag = &m_iStateFlag;

	if (FAILED(__super::Add_PawnObject(TEXT("Vampire_Royal_Body"), ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Queen_Body"), &BodyObjectDesc)))
		return E_FAIL;

	m_pBody = static_cast<CBody*>(Find_PawnObject(TEXT("Vampire_Royal_Body")));

	m_pAnimMovement = m_pBody->Get_AnimMovementPtr();

	return S_OK;
}

void CQueen::Compute_AnimPosition()
{
}

CQueen* CQueen::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CQueen* pInstance = new CQueen(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed Created : CQueen"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CQueen::Clone(void* pArg)
{
	CQueen* pInstance = new CQueen(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CQueen"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CQueen::Free()
{
	__super::Free();
}
