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

	m_fX = pDesc->fX;
	m_fY = pDesc->fY;
	m_iHeight = pDesc->iHeight;
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


HRESULT CUIObject::Add_StaticTexture_Child(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, UIOBJECT_DESC& UIChildDesc)
{
	CUIObject* pUIObject = dynamic_cast<CUIObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, iPrototypeLevelIndex, strPrototypeTag, &UIChildDesc));;
	if (nullptr == pUIObject)
		return E_FAIL;

	m_Children.push_back(pUIObject);

	return S_OK;
}

HRESULT CUIObject::Add_DynamicTexture_Child(_uint iUIPrototypeLevelIndex, const _wstring& strUIPrototypeTag, _uint iTexturePrototypeLevelIndex, const _wstring& strTexturePrototypeTag, UIOBJECT_DESC& UIChildDesc)
{
	CUIObject* pUIObject = dynamic_cast<CUIObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, iUIPrototypeLevelIndex, strUIPrototypeTag, &UIChildDesc));;
	if (nullptr == pUIObject)
		return E_FAIL;

	if (FAILED(pUIObject->Ready_TextureCom(iTexturePrototypeLevelIndex, strTexturePrototypeTag)))
		return E_FAIL;

	m_Children.push_back(pUIObject);

	return S_OK;
}

HRESULT CUIObject::Ready_TextureCom(_uint iTexturePrototypeLevelIndex, const _wstring& strTexturePrototypeTag)
{
	return S_OK;
}

HRESULT CUIObject::Begin()
{
	m_pTransformCom->Scale(_float3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(m_fX - m_fOffsetX - (m_iWinSizeX * 0.5f), -m_fY - m_fOffsetY + (m_iWinSizeY * 0.5f), (UI_FAR / (_float)m_iHeight), 1.f));

	return S_OK;
}

HRESULT CUIObject::Update_ChildPosition(_float fX, _float fY)
{
	m_fX = fX;
	m_fY = fY;

	for (auto Child: m_Children)
		Child->Update_ChildPosition(fX, fY);

	return S_OK;
}

void CUIObject::Children_Priority_Update(_float fTimeDelta)
{
	if (m_Children.size() <= 0)
		return;

	for (auto Child : m_Children)
		Child->Priority_Update(fTimeDelta);
}

void CUIObject::Children_Update(_float fTimeDelta)
{
	if (m_Children.size() <= 0)
		return;

	for (auto Child : m_Children)
		Child->Update(fTimeDelta);
}

void CUIObject::Children_Late_Update(_float fTimeDelta)
{
	if (m_Children.size() <= 0)
		return;

	for (auto Child : m_Children)
		Child->Late_Update(fTimeDelta);
}

HRESULT CUIObject::Children_Render()
{
	if (m_Children.size() <= 0)
		return E_FAIL;

	for (auto Child : m_Children)
	{
		if (FAILED(Child->Render()))
			return E_FAIL;
	}

	return S_OK;
}

void CUIObject::Free()
{
	__super::Free();

	for (auto& Child : m_Children)
		Safe_Release(Child);

	m_Children.clear();
}
