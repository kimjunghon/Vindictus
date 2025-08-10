#include "EnginePch.h"
#include "UIObject.h"
#include "GameInstance.h"

CUIObject::CUIObject(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CGameObject {pDevice, pDeviceContext}
{
}

CUIObject::CUIObject(const CUIObject& Prototype)
	: CGameObject { Prototype }
{
}

HRESULT CUIObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUIObject::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	UIOBJECT_DESC* pDesc = static_cast<UIOBJECT_DESC*>(pArg);

	m_fX = pDesc->fX + pDesc->fOffsetX;
	m_fY = pDesc->fY + pDesc->fOffsetY;
	
	m_iDepth = pDesc->iDepth;
	m_fSizeX = pDesc->fSizeX;
	m_fSizeY = pDesc->fSizeY;
	m_fOffsetX = pDesc->fOffsetX;
	m_fOffsetY = pDesc->fOffsetY;

	D3D11_VIEWPORT			Viewport{};

	_uint			iNumViewports = { 1 };

	// GraphicDevice 초기화하며 RSSetViewPort로 세팅한 ViewPort 가져옴
	m_pDeviceContext->RSGetViewports(&iNumViewports, &Viewport);
	
	//XMMatrixLookAtLH가 XMMATRIX 반환, 그걸 Float4x4 변수에 저장(XMStore)
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(Viewport.Width, Viewport.Height, 0.f, UI_FAR));

	m_iWinSizeX = Viewport.Width;
	m_iWinSizeY = Viewport.Height;


	return S_OK;
}

void CUIObject::Priority_Update(_float fTimeDelta)
{
}

void CUIObject::Update(_float fTimeDelta)
{
}

void CUIObject::Late_Update(_float fTimeDelta)
{
}

HRESULT CUIObject::Render()
{
	return S_OK;
}

_bool CUIObject::IsPick(HWND hWnd)
{
	_float fX = m_fX;
	_float fY = m_fY;

	RECT	rcRect = { LONG(fX - (m_fSizeX * 0.5f)), LONG(fY - (m_fSizeY * 0.5f)), LONG(fX + (m_fSizeX * 0.5f)), LONG(fY + (m_fSizeY * 0.5f)) };

	POINT	ptMouse = m_pGameInstance->Get_MousePoint();

	return PtInRect(&rcRect, ptMouse);
}

HRESULT CUIObject::Begin()
{
	m_pTransformCom->Scale(_float3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(m_fX - (m_iWinSizeX * 0.5f), -m_fY + (m_iWinSizeY * 0.5f), (UI_FAR / (_float)m_iDepth), 1.f));

	return S_OK;
}


void CUIObject::Set_Position(_float fX, _float fY)
{
	m_fX = fX + m_fOffsetX;
	m_fY = fY + m_fOffsetY;
}

void CUIObject::Set_Offset(_float fOffsetX, _float fOffsetY)
{
	m_fOffsetX = fOffsetX;
	m_fOffsetY = fOffsetY;
}

void CUIObject::Free()
{
	__super::Free();

}
