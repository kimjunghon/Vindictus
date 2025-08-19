#include "ClientPch.h"
#include "Vampire.h"
#include "VampireAI.h"
#include "Navigation.h"
#include "Body.h"

CVampire::CVampire(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CMonster { pDevice, pDeviceContext }
{
}

CVampire::CVampire(const CVampire& Prototype)
	: CMonster { Prototype }
{
}

HRESULT CVampire::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CVampire::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_AI()))
		return E_FAIL;

	return S_OK;
}

void CVampire::Priority_Update(_float fTimeDelta)
{
}

void CVampire::Update(_float fTimeDelta)
{
}

void CVampire::Late_Update(_float fTimeDelta)
{
}

HRESULT CVampire::Render()
{
	return S_OK;
}

HRESULT CVampire::Spawn(MONSTER_SPAWN_DATA SpawnData)
{
	m_IsActive = true;

	m_iStateFlag = ENUM_CLASS(STATE_FLAG::SPAWN);

	m_pBody->Forcing_Play_Animation();

	m_pNavigationCom = m_pGameInstance->Clone_CurrentNavigation(SpawnData.iCellIndex);

	_vector vPosition = XMVectorSetW(XMLoadFloat3(&SpawnData.vPosition), 1.f);
	
	m_pTransformCom->Set_State(STATE::POSITION, vPosition);

	return S_OK;
}

BT_STATE CVampire::Attack()
{
	m_iStateFlag = ENUM_CLASS(STATE_FLAG::ATTACK);

	m_AttackTime[m_iCurrentAttack] = 0.f;

	return BT_STATE::SUCCESS;
}

BT_STATE CVampire::Chase()
{
	_float fDistance = XMVectorGetX(XMVector3Length(XMVectorSubtract(m_pTargetTransform->Get_State(STATE::POSITION), m_pTransformCom->Get_State(STATE::POSITION))));

	if (abs(fDistance) >= m_fChaseRange)
	{
		m_iStateFlag = ENUM_CLASS(STATE_FLAG::MOVE) | ENUM_CLASS(MOVE_FLAG::FRONT);

		return BT_STATE::SUCCESS;
	}

	return BT_STATE::FAILED;
}

BT_STATE CVampire::Patrol()
{
	_float fDistance = XMVectorGetX(XMVector3Length(XMVectorSubtract(m_pTargetTransform->Get_State(STATE::POSITION), m_pTransformCom->Get_State(STATE::POSITION))));

	if (abs(fDistance) <= m_fMinDistance)
		m_iStateFlag = ENUM_CLASS(STATE_FLAG::MOVE) | ENUM_CLASS(MOVE_FLAG::BACK);
	else
		m_iStateFlag = ENUM_CLASS(STATE_FLAG::MOVE) | (rand() % 2 == 0 ? ENUM_CLASS(MOVE_FLAG::LEFT) : ENUM_CLASS(MOVE_FLAG::RIGHT));

	return BT_STATE::SUCCESS;
}

HRESULT CVampire::Ready_AI()
{
	m_pAI = CVampireAI::Create(this);

	if (nullptr == m_pAI)
		return E_FAIL;

	return S_OK;
}

void CVampire::Free()
{
	__super::Free();

}
