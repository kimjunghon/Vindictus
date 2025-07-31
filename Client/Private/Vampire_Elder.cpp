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
	m_fAttackCoolTime = 10.f;
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

	if (FAILED(Ready_AI()))
		return E_FAIL;

	if (FAILED(Ready_PawnObject()))
		return E_FAIL;

	m_fAttackTime = 0.f;

	//TEST
	m_pTargetTransform = static_cast<CTransform*>(m_pGameInstance->Get_Component(ENUM_CLASS(LAYERTYPE::NONSTATIC), TEXT("Layer_Player"), TEXT("Com_Transform"), 0));
	if (nullptr == m_pTargetTransform)
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
	m_fAttackTime += fTimeDelta;

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

BT_STATE CVampire_Elder::CanOtherAction()
{
	if (m_pBody->AnimCanChange() || m_pBody->AnimIsFinished())
		return BT_STATE::FAILED;

	return BT_STATE::RUN;
}

BT_STATE CVampire_Elder::CanAttackRange()
{
	_float fDistance = XMVectorGetX(XMVector3Length(XMVectorSubtract(m_pTargetTransform->Get_State(STATE::POSITION), m_pTransformCom->Get_State(STATE::POSITION))));

	if (abs(fDistance) <= m_fAttackRange)
		return BT_STATE::SUCCESS;

	return BT_STATE::FAILED;
}

BT_STATE CVampire_Elder::Attack()
{
	_float fDistance = XMVectorGetX(XMVector3Length(XMVectorSubtract(m_pTargetTransform->Get_State(STATE::POSITION), m_pTransformCom->Get_State(STATE::POSITION))));

	if (abs(fDistance) <= m_fMinDistance)
		m_iStateFlag = ENUM_CLASS(STATE_FLAG::ATTACK) | ENUM_CLASS(ATTACK_FLAG::ATTACK_MELEE);
	else
		m_iStateFlag = ENUM_CLASS(STATE_FLAG::ATTACK) | ENUM_CLASS(ATTACK_FLAG::ATTACK_RANGE);

	m_fAttackTime = 0.f;

	return BT_STATE::SUCCESS;
}

BT_STATE CVampire_Elder::Chase()
{
	_float fDistance = XMVectorGetX(XMVector3Length(XMVectorSubtract(m_pTargetTransform->Get_State(STATE::POSITION), m_pTransformCom->Get_State(STATE::POSITION))));

	if (abs(fDistance) >= m_fChaseRange)
	{
		m_iStateFlag = ENUM_CLASS(STATE_FLAG::MOVE) | ENUM_CLASS(MOVE_FLAG::RUN_FRONT);

		return BT_STATE::SUCCESS;
	}

	return BT_STATE::FAILED;
}

BT_STATE CVampire_Elder::Patrol()
{
	_float fDistance = XMVectorGetX(XMVector3Length(XMVectorSubtract(m_pTargetTransform->Get_State(STATE::POSITION), m_pTransformCom->Get_State(STATE::POSITION))));

	if (abs(fDistance) <= m_fMinDistance)
		m_iStateFlag = ENUM_CLASS(STATE_FLAG::MOVE) | ENUM_CLASS(MOVE_FLAG::RUN_BACK);
	else
		m_iStateFlag = ENUM_CLASS(STATE_FLAG::MOVE) | (rand() % 2 == 0 ? ENUM_CLASS(MOVE_FLAG::RUN_LEFT) : ENUM_CLASS(MOVE_FLAG::RUN_RIGHT));

	return BT_STATE::SUCCESS;
}

HRESULT CVampire_Elder::Ready_AI()
{
	m_pAI = CVampireAI::Create(this);
	if (nullptr == m_pAI)
		return E_FAIL;

	return S_OK;
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
	_vector vPosition = m_pTransformCom->Get_State(STATE::POSITION);
	_vector vAnimPosition = *m_pAnimMovement;

	_vector vLook = XMVector3Normalize(m_pTransformCom->Get_State(STATE::LOOK));
	_float fYaw = atan2f(XMVectorGetX(vLook), XMVectorGetZ(vLook));

	_matrix RotationMatrix = XMMatrixRotationY(fYaw);

	vAnimPosition = XMVector3Transform(vAnimPosition, RotationMatrix);

	vPosition = XMVectorSetW(XMVectorAdd(vPosition, vAnimPosition), 1.f);

	m_pTransformCom->Set_State(STATE::POSITION, vPosition);
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

	Safe_Release(m_pAI);
}
