#include "ClientPch.h"
#include "Queen.h"
#include "Body.h"
#include "QueenAI.h"

CQueen::CQueen(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CMonster { pDevice, pDeviceContext }
{
}

CQueen::CQueen(const CQueen& Prototype)
	: CMonster { Prototype }
	, m_fNearAttackCoolTime { Prototype.m_fNearAttackCoolTime }
{
}

HRESULT CQueen::Initialize_Prototype()
{
	m_iNumAttacks = END;

	m_AttackCoolTime.resize(m_iNumAttacks, 0.f);
	m_AttackTime.resize(m_iNumAttacks, 0.f);

	m_AttackCoolTime[DOUBLE] = 30.f;
	m_AttackCoolTime[SWOOP] = 30.f;
	m_AttackCoolTime[POISON] = 25.f;
	m_AttackCoolTime[LEFTHAND] = 20.f;
	m_AttackCoolTime[RIGHTHAND] = 20.f;

	m_fAttackRange = 100.f;
	m_fChaseRange = 80.f;
	m_fMinDistance = 50.f;

	m_fNearAttackCoolTime = 1.f;

	return S_OK;
}

HRESULT CQueen::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_PawnObjects()))
		return E_FAIL;

	if (FAILED(Ready_AI()))
		return E_FAIL;

	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(50.f, 0.f, 0.f, 1.f));

	m_iStateFlag = ENUM_CLASS(STATE_FLAG::ATTACK) | ENUM_CLASS(ATTACK_FLAG::TURN_LEFT);

	m_fCurrentRotation = XMQuaternionIdentity();
	
	return S_OK;
}

void CQueen::Priority_Update(_float fTimeDelta)
{
	for (auto& Pair : m_PawnObjects)
		Pair.second->Priority_Update(fTimeDelta);
}

void CQueen::Update(_float fTimeDelta)
{
	CMonster::Update_AttackCoolTime(fTimeDelta);
	Check_Near(fTimeDelta);

	m_pAI->Update();

	if (m_iStateFlag & ENUM_CLASS(STATE_FLAG::MOVE))
	{
		_vector vTargetPos = m_pTargetTransform->Get_State(STATE::POSITION);
		vTargetPos = XMVectorSetY(vTargetPos, XMVectorGetY(m_pTransformCom->Get_State(STATE::POSITION)));

		m_pTransformCom->LookAt(vTargetPos);

	}

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

BT_STATE CQueen::Attack()
{
	_uint iAttackFlag = ENUM_CLASS(ATTACK_FLAG::DOUBLE) << m_iCurrentAttack;

	m_iStateFlag = ENUM_CLASS(STATE_FLAG::ATTACK) | iAttackFlag;

	m_AttackTime[m_iCurrentAttack] = 0.f;

	return BT_STATE::SUCCESS;
}

BT_STATE CQueen::Chase()
{
	_float fDistance = XMVectorGetX(XMVector3Length(XMVectorSubtract(m_pTargetTransform->Get_State(STATE::POSITION), m_pTransformCom->Get_State(STATE::POSITION))));

	if (abs(fDistance) >= m_fChaseRange)
	{
		m_iStateFlag = ENUM_CLASS(STATE_FLAG::MOVE) | ENUM_CLASS(MOVE_FLAG::RUN);
		return BT_STATE::SUCCESS;
	}

	return BT_STATE::FAILED;
}

BT_STATE CQueen::Patrol()
{
	return BT_STATE::FAILED;
}

BT_STATE CQueen::CanBurrow()
{
	return BT_STATE();
}

BT_STATE CQueen::Burrow()
{
	return BT_STATE();
}

BT_STATE CQueen::CanNearAttack()
{
	if (m_fNearAttackTime >= m_fNearAttackCoolTime)
	{
		return BT_STATE::SUCCESS;
	}

	return BT_STATE::FAILED;
}

BT_STATE CQueen::NearAttack()
{
	_vector vTargetDir = XMVector3Normalize(XMVectorSetY(XMVectorSubtract(m_pTargetTransform->Get_State(STATE::POSITION), m_pTransformCom->Get_State(STATE::POSITION)), 0.f));
	_vector vLook = XMVectorSetY(m_pTransformCom->Get_State(STATE::LOOK), 0.f);
	_vector vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook));
	
	_float fLookDot = XMVectorGetX(XMVector3Dot(vLook, vTargetDir));
	_float fRightDot = XMVectorGetX(XMVector3Dot(vRight, vTargetDir));
	
	_float fComparisonRadian = cosf(XMConvertToRadians(70.f));

	_uint iAttackFlag = {};

	if (fLookDot >= fComparisonRadian)
		iAttackFlag = ENUM_CLASS(ATTACK_FLAG::TURN_RIGHT);
		//	iAttackFlag = ENUM_CLASS(ATTACK_FLAG::MELLE);
	else if (fLookDot <= -fComparisonRadian)
		iAttackFlag = ENUM_CLASS(ATTACK_FLAG::TURN_RIGHT);
		//iAttackFlag = ENUM_CLASS(ATTACK_FLAG::TAIL);
	else if (fRightDot >= 0.f)
		iAttackFlag = ENUM_CLASS(ATTACK_FLAG::TURN_RIGHT);
	else
		iAttackFlag = ENUM_CLASS(ATTACK_FLAG::TURN_LEFT);
		
	m_iStateFlag = ENUM_CLASS(STATE_FLAG::ATTACK) | iAttackFlag;
	m_fNearAttackTime = 0.f;

	return BT_STATE::SUCCESS;
}

BT_STATE CQueen::IsLook()
{
	_vector vTargetDir = XMVector3Normalize(XMVectorSetY(XMVectorSubtract(m_pTargetTransform->Get_State(STATE::POSITION), m_pTransformCom->Get_State(STATE::POSITION)), 0.f));
	_vector vLook = XMVectorSetY(m_pTransformCom->Get_State(STATE::LOOK), 0.f);

	_float fLookDot = XMVectorGetX(XMVector3Dot(vLook, vTargetDir));

	_float fComparisonRadian = cosf(XMConvertToRadians(70.f));

	if (fLookDot >= fComparisonRadian)
		return BT_STATE::SUCCESS;

	return BT_STATE::FAILED;
}

BT_STATE CQueen::LookForPlayer()
{
	m_IsLookForPlayer = true;

	

	return BT_STATE::SUCCESS;
}

BT_STATE CQueen::Idle()
{
	m_iStateFlag = ENUM_CLASS(STATE_FLAG::IDLE) | ENUM_CLASS(IDLE_FLAG::THREAT);

	return BT_STATE::SUCCESS;
}

HRESULT CQueen::Ready_PawnObjects()
{
	CBody::BODY_DESC BodyObjectDesc = {};
	BodyObjectDesc.pPawnMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	BodyObjectDesc.pStateFlag = &m_iStateFlag;

	if (FAILED(__super::Add_PawnObject(TEXT("Queen_Body"), ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Queen_Body"), &BodyObjectDesc)))
		return E_FAIL;

	m_pBody = static_cast<CBody*>(Find_PawnObject(TEXT("Queen_Body")));

	m_pAnimMovement = m_pBody->Get_AnimMovementPtr();
	m_pAnimRotation = m_pBody->Get_AnimRotationPtr();

	return S_OK;
}

HRESULT CQueen::Ready_AI()
{
	m_pAI = CQueenAI::Create(this);
	if (nullptr == m_pAI)
		return E_FAIL;

	return S_OK;
}

void CQueen::Compute_AnimPosition()
{
	_vector vAnimPosition = XMVectorSetY(*m_pAnimMovement, 0.f);
	_vector vAnimRotation = *m_pAnimRotation;
	
	_matrix RotationMatrix = XMMatrixRotationQuaternion(*m_pAnimRotation);
	
	{
		_vector vLook = XMVector3Normalize(RotationMatrix.r[2]);
		_float fYaw = atan2f(XMVectorGetX(vLook), XMVectorGetZ(vLook));
		
		RotationMatrix = XMMatrixRotationY(fYaw);
	}


	_matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();
	
	_matrix PositionMatrix = XMMatrixTranslationFromVector(vAnimPosition);

	WorldMatrix = (PositionMatrix * RotationMatrix) * WorldMatrix;

	m_pTransformCom->Set_WorldMatrix(WorldMatrix);
}

void CQueen::Check_Near(_float fTimeDelta)
{
	_float fDistance = XMVectorGetX(XMVector3Length(XMVectorSubtract(m_pTargetTransform->Get_State(STATE::POSITION), m_pTransformCom->Get_State(STATE::POSITION))));

	if (abs(fDistance) <= 70.f)
	{
		m_fNearAttackTime += fTimeDelta;
	}
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
