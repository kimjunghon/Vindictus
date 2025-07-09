#include "Player.h"

CPlayer::CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CCollisionObject { pDevice, pDeviceContext }
{
}

CPlayer::CPlayer(const CPlayer& Prototype)
	: CCollisionObject { Prototype }
{
}

HRESULT CPlayer::Initialize_Prototype()
{
	return E_NOTIMPL;
}

HRESULT CPlayer::Initialize(void* pArg)
{
	return E_NOTIMPL;
}

void CPlayer::Priority_Update(_float fTimeDelta)
{
}

void CPlayer::Update(_float fTimeDelta)
{
}

void CPlayer::Late_Update(_float fTimeDelta)
{
}

HRESULT CPlayer::Render()
{
	return E_NOTIMPL;
}

HRESULT CPlayer::Ready_Component()
{
	return E_NOTIMPL;
}

CPlayer* CPlayer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	return nullptr;
}

CGameObject* CPlayer::Clone(void* pArg)
{
	return nullptr;
}

void CPlayer::Free()
{
}
