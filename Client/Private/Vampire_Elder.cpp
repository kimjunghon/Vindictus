#include "ClientPch.h"
#include "Vampire_Elder.h"
#include "Body.h"
#include "VampireAI.h"

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
	m_iNumAttacks = END;

	m_AttackCoolTime.resize(m_iNumAttacks, 0.f);
	m_AttackTime.resize(m_iNumAttacks, 0.f);

	m_AttackCoolTime[ATTACK_NORMAL] = 10.f;

	m_fAttackRange = 150.f;
	m_fChaseRange = 120.f;
	m_fMinDistance = 50.f;

	return S_OK;
}

HRESULT CVampire_Elder::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_iStateFlag = ENUM_CLASS(STATE_FLAG::SPAWN);

	if (FAILED(Ready_PawnObject()))
		return E_FAIL;

	return S_OK;
}

void CVampire_Elder::Priority_Update(_float fTimeDelta)
{
	for (auto& Pair : m_PawnObjects)
		Pair.second->Priority_Update(fTimeDelta);
}

void CVampire_Elder::Update(_float fTimeDelta)
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

void CVampire_Elder::Late_Update(_float fTimeDelta)
{
	Compute_AnimPosition();

	for (auto& Pair : m_PawnObjects)
		Pair.second->Late_Update(fTimeDelta);
}

HRESULT CVampire_Elder::Render()
{
	return S_OK;
}


BT_STATE CVampire_Elder::Attack()
{
	_float fDistance = XMVectorGetX(XMVector3Length(XMVectorSubtract(m_pTargetTransform->Get_State(STATE::POSITION), m_pTransformCom->Get_State(STATE::POSITION))));

	if (abs(fDistance) <= m_fMinDistance)
		m_iStateFlag = ENUM_CLASS(STATE_FLAG::ATTACK) | ENUM_CLASS(ATTACK_FLAG::MELEE);
	else
		m_iStateFlag = ENUM_CLASS(STATE_FLAG::ATTACK) | ENUM_CLASS(ATTACK_FLAG::RANGE);

	m_AttackTime[m_iCurrentAttack] = 0.f;

	return BT_STATE::SUCCESS;
}

HRESULT CVampire_Elder::Ready_PawnObject()
{
	CBody::BODY_DESC BodyObjectDesc = {};
	BodyObjectDesc.pPawnMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	BodyObjectDesc.pStateFlag = &m_iStateFlag;

	if (FAILED(__super::Add_PawnObject(TEXT("Vampire_Elder_Body"), ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Vampire_Elder_Body"), &BodyObjectDesc)))
		return E_FAIL;

	m_pBody = static_cast<CBody*>(Find_PawnObject(TEXT("Vampire_Elder_Body")));

	m_pAnimMovement = m_pBody->Get_AnimMovementPtr();

	return S_OK;
}

void CVampire_Elder::Compute_AnimPosition()
{
	_vector vAnimPosition = XMVectorSetY(*m_pAnimMovement, 0.f);

	_matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();

	_matrix PositionMatrix = XMMatrixTranslationFromVector(vAnimPosition);

	m_pTransformCom->Set_WorldMatrix((PositionMatrix)*WorldMatrix);
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
