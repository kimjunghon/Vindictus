#include "ClientPch.h"
#include "Vampire.h"

CVampire::CVampire(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CMonster { pDevice, pDeviceContext }
{
}

CVampire::CVampire(const CVampire& Prototype)
	: CMonster { Prototype }
	, m_fAttackCoolTime { Prototype.m_fAttackCoolTime }
	, m_fAttackRange { Prototype.m_fAttackRange }
	, m_fChaseRange { Prototype.m_fChaseRange }
	, m_fMinDistance { Prototype.m_fMinDistance }
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

BT_STATE CVampire::CanAttack()
{
	if (m_fAttackTime >= m_fAttackCoolTime)
		return BT_STATE::SUCCESS;

	return BT_STATE::FAILED;
}

BT_STATE CVampire::CanOtherAction()
{
	return BT_STATE();
}

BT_STATE CVampire::CanAttackRange()
{
	return BT_STATE();
}

BT_STATE CVampire::Attack()
{
	return BT_STATE();
}

BT_STATE CVampire::Chase()
{
	return BT_STATE();
}

BT_STATE CVampire::Patrol()
{
	return BT_STATE();
}

void CVampire::Free()
{
	__super::Free();
}
