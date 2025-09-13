#include "ClientPch.h"
#include "Puppy.h"
#include "PuppyBody.h"
#include "PuppyAI.h"

CPuppy::CPuppy(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CColliderPawn { pDevice, pDeviceContext }
{
}

CPuppy::CPuppy(const CPuppy& Prototype)
	: CColliderPawn { Prototype }
{
}

HRESULT CPuppy::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPuppy::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	PUPPY_DESC* pDesc = static_cast<PUPPY_DESC*>(pArg);

	if (FAILED(Ready_PawnObject(pDesc->iPuppyTypeIndex)))
		return E_FAIL;

	if (FAILED(Ready_AI()))
		return E_FAIL;

	m_pNavigationCom = m_pGameInstance->Clone_CurrentNavigation(pDesc->iCellIndex);
	if (nullptr == m_pNavigationCom)
		return E_FAIL;

	_vector vPosition = XMVectorSetW(XMLoadFloat3(&pDesc->vPosition), 1.f);

	m_pTransformCom->Set_State(STATE::POSITION, vPosition);

	m_pTargetTransform = static_cast<CTransform*>(m_pGameInstance->Get_Component(ENUM_CLASS(LAYER_TYPE::NONSTATIC), TEXT("Layer_Player"), TEXT("Com_Transform"), 0));
	if (nullptr == m_pTargetTransform)
		return E_FAIL;

	return S_OK;
}

void CPuppy::Priority_Update(_float fTimeDelta)
{
	m_vPrevPosition = m_pTransformCom->Get_State(STATE::POSITION);

	for (auto& Pair : m_PawnObjects)
		Pair.second->Priority_Update(fTimeDelta);
}

void CPuppy::Update(_float fTimeDelta)
{
	_float fY = XMVectorGetY(m_pTransformCom->Get_State(STATE::POSITION));
	_vector vPosition = XMVectorSetY(m_pTargetTransform->Get_State(STATE::POSITION), fY);
	m_pTransformCom->LookAt(vPosition);

	m_pAI->Update();

	for (auto& Pair : m_PawnObjects)
		Pair.second->Update(fTimeDelta);
}

void CPuppy::Late_Update(_float fTimeDelta)
{
	Compute_WorldMatrix();

	for (auto& Pair : m_PawnObjects)
		Pair.second->Late_Update(fTimeDelta);

	m_pColliderContainer->Update(this, m_pTransformCom->Get_WorldMatrix());
}

HRESULT CPuppy::Render()
{
	return S_OK;
}

BT_STATE CPuppy::CanOtherAction()
{
	if (m_pBody->AnimCanChange())
		return BT_STATE::FAILED;

	return BT_STATE::SUCCESS;
}

BT_STATE CPuppy::IsNear()
{
	_float fDistance = XMVectorGetX(XMVector3Length(XMVectorSubtract(m_pTransformCom->Get_State(STATE::POSITION), m_pTargetTransform->Get_State(STATE::POSITION))));

	if (fDistance <= 40.f)
		return BT_STATE::FAILED;

	if(fDistance <= 60.f)
		m_iStateFlag = ENUM_CLASS(STATE_FLAG::WALK);
	else
		m_iStateFlag = ENUM_CLASS(STATE_FLAG::RUN);

	return BT_STATE::SUCCESS;
}

BT_STATE CPuppy::Idle()
{
	_uint iState = ENUM_CLASS(STATE_FLAG::SIT_DOWN);
	
	_uint iRandomIndex = rand() % 4;

	m_iStateFlag = iState << iRandomIndex;

	return BT_STATE::SUCCESS;
}

HRESULT CPuppy::Ready_PawnObject(_uint iPuppyType)
{
	CPuppyBody::PUPPY_BODY_DESC PuppyBodyDesc = {};
	PuppyBodyDesc.pPawnMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	PuppyBodyDesc.pStateFlag = &m_iStateFlag;
	PuppyBodyDesc.iPuppyTypeIndex = iPuppyType;

	if (FAILED(CPawn::Add_PawnObject(TEXT("Puppy_Body"), ENUM_CLASS(LEVEL::TOWN), TEXT("Prototype_GameObject_Puppy_Body"), &PuppyBodyDesc)))
		return E_FAIL;

	m_pBody = static_cast<CPuppyBody*>(Find_PawnObject(TEXT("Puppy_Body")));
	if (nullptr == m_pBody)
		return E_FAIL;

	m_pAnimMovement = m_pBody->Get_AnimMovementPtr();
	if (nullptr == m_pAnimMovement)
		return E_FAIL;

	m_pAnimRotation = m_pBody->Get_AnimRotationPtr();
	if (nullptr == m_pAnimRotation)
		return E_FAIL;

	return S_OK;
}

HRESULT CPuppy::Ready_AI()
{
	m_pAI = CPuppyAI::Create(this);
	if (nullptr == m_pAI)
		return E_FAIL;
	return S_OK;
}

HRESULT CPuppy::Ready_Collider()
{
	return S_OK;
}

void CPuppy::Compute_WorldMatrix()
{
	_vector vAnimPosition = *m_pAnimMovement;

	_matrix PositionMatrix = XMMatrixTranslationFromVector(vAnimPosition);

	m_pTransformCom->MovePositionToMatrix(PositionMatrix, m_pNavigationCom);
}

CPuppy* CPuppy::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CPuppy* pInstance = new CPuppy(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed Created : CPuppy"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CPuppy::Clone(void* pArg)
{
	CPuppy* pInstance = new CPuppy(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed Created : CPuppy"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPuppy::Free()
{
	__super::Free();

	Safe_Release(m_pAI);

}
