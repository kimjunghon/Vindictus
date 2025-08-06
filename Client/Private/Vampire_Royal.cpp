#include "ClientPch.h"
#include "Vampire_Royal.h"
#include "Body.h"
#include "VampireAI.h"

CVampire_Royal::CVampire_Royal(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CVampire { pDevice, pDeviceContext }
{
}

CVampire_Royal::CVampire_Royal(const CVampire_Royal& Prototype)
	: CVampire { Prototype }
{
}

HRESULT CVampire_Royal::Initialize_Prototype()
{
	m_iNumAttacks = END;

	m_AttackCoolTime.resize(m_iNumAttacks, 0.f);
	m_AttackTime.resize(m_iNumAttacks, 0.f);

	m_AttackCoolTime[ATTACK_NORMAL] = 7.f;
	
	m_fAttackRange = 70.f;
	m_fChaseRange = 50.f;
	m_fMinDistance = 30.f;

	return S_OK;
}

HRESULT CVampire_Royal::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_iStateFlag = ENUM_CLASS(STATE_FLAG::SPAWN);

	if (FAILED(Ready_PawnObject()))
		return E_FAIL;

	return S_OK;
}

void CVampire_Royal::Priority_Update(_float fTimeDelta)
{
	for (auto& Pair : m_PawnObjects)
		Pair.second->Priority_Update(fTimeDelta);
}

void CVampire_Royal::Update(_float fTimeDelta)
{
	CMonster::Update_AttackCoolTime(fTimeDelta);

	m_pAI->Update();

	if (m_iStateFlag & ENUM_CLASS(STATE_FLAG::MOVE) || m_iStateFlag & ENUM_CLASS(STATE_FLAG::ATTACK))
	{
		_vector vTargetPos = m_pTargetTransform->Get_State(STATE::POSITION);
		vTargetPos = XMVectorSetY(vTargetPos, XMVectorGetY(m_pTransformCom->Get_State(STATE::POSITION)));

		m_pTransformCom->LookAt(vTargetPos);
	}

	for (auto& Pair : m_PawnObjects)
		Pair.second->Update(fTimeDelta);
}

void CVampire_Royal::Late_Update(_float fTimeDelta)
{
	Compute_AnimPosition();

	for (auto& Pair : m_PawnObjects)
		Pair.second->Late_Update(fTimeDelta);
}

HRESULT CVampire_Royal::Render()
{
	return S_OK;
}

HRESULT CVampire_Royal::Ready_PawnObject()
{
	CBody::BODY_DESC BodyObjectDesc = {};
	BodyObjectDesc.pPawnMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	BodyObjectDesc.pStateFlag = &m_iStateFlag;

	if (FAILED(__super::Add_PawnObject(TEXT("Vampire_Royal_Body"), ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Vampire_Royal_Body"), &BodyObjectDesc)))
		return E_FAIL;

	m_pBody = static_cast<CBody*>(Find_PawnObject(TEXT("Vampire_Royal_Body")));

	m_pAnimMovement = m_pBody->Get_AnimMovementPtr();

	return S_OK;
}

void CVampire_Royal::Compute_AnimPosition()
{
	_vector vAnimPosition = XMVectorSetY(*m_pAnimMovement, 0.f);

	_matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();

	_matrix PositionMatrix = XMMatrixTranslationFromVector(vAnimPosition);

	m_pTransformCom->Set_WorldMatrix((PositionMatrix)*WorldMatrix);
}

CVampire_Royal* CVampire_Royal::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CVampire_Royal* pInstance = new CVampire_Royal(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed Created : CVampire_Royal"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CVampire_Royal::Clone(void* pArg)
{
	CVampire_Royal* pInstance = new CVampire_Royal(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CVampire_Royal"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CVampire_Royal::Free()
{
	__super::Free();

}
