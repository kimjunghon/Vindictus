#include "ClientPch.h"
#include "Glasgavelen.h"
#include "PawnObject.h"
#include "Body.h"
#include "GlasgavelenSword.h"

CGlasgavelen::CGlasgavelen(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CMonster { pDevice, pDeviceContext }
{
}

CGlasgavelen::CGlasgavelen(const CGlasgavelen& Prototype)
	: CMonster { Prototype }
{
}

HRESULT CGlasgavelen::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGlasgavelen::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_PawnObjects()))
		return E_FAIL;

	return S_OK;
}

void CGlasgavelen::Priority_Update(_float fTimeDelta)
{
	for (auto& Pair : m_PawnObjects)
		Pair.second->Priority_Update(fTimeDelta);
}

void CGlasgavelen::Update(_float fTimeDelta)
{
	for (auto& Pair : m_PawnObjects)
		Pair.second->Update(fTimeDelta);
}

void CGlasgavelen::Late_Update(_float fTimeDelta)
{
	for (auto& Pair : m_PawnObjects)
		Pair.second->Late_Update(fTimeDelta);
}

HRESULT CGlasgavelen::Render()
{
	return S_OK;
}

HRESULT CGlasgavelen::Spawn(MONSTER_SPAWN_DATA SpawnData)
{
	return S_OK;
}

BT_STATE CGlasgavelen::Attack()
{
	return BT_STATE();
}

BT_STATE CGlasgavelen::Chase()
{
	return BT_STATE();
}

BT_STATE CGlasgavelen::Patrol()
{
	return BT_STATE();
}

HRESULT CGlasgavelen::Ready_PawnObjects()
{
	CBody::BODY_DESC BodyObjectDesc = {};
	BodyObjectDesc.pPawnMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	BodyObjectDesc.pStateFlag = &m_iStateFlag;

	if (FAILED(__super::Add_PawnObject(TEXT("GlasgavelenBody"), ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Glasgavelen_Body"), &BodyObjectDesc)))
		return E_FAIL;

	CPawnObject::PAWNOBJECT_DESC PawnObjectDesc = {};
	PawnObjectDesc.pPawnMatrix = m_pTransformCom->Get_WorldMatrixPtr();

	if (FAILED(__super::Add_PawnObject(TEXT("GlasgavelenSword"), ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Glasgavelen_Sword"), &PawnObjectDesc)))
		return E_FAIL;

	m_pBody = static_cast<CBody*>(Find_PawnObject(TEXT("GlasgavelenBody")));
	if (nullptr == m_pBody)
		return E_FAIL;

	CGlasgavelenSword* pSword = static_cast<CGlasgavelenSword*>(Find_PawnObject(TEXT("GlasgavelenSword")));
	if (nullptr == pSword)
		return E_FAIL;

	if(FAILED(pSword->Bind_ParentBones(m_pBody->Get_ParentModelPtr())))
		return E_FAIL;

	return S_OK;
}

CGlasgavelen* CGlasgavelen::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CGlasgavelen* pInstance = new CGlasgavelen(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed Created : CGlasgavelen"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CGlasgavelen::Clone(void* pArg)
{
	CGlasgavelen* pInstance = new CGlasgavelen(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CGlasgavelen"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CGlasgavelen::Free()
{
	__super::Free();
}
