#include "PlayerHPBar.h"

CPlayerHPBar::CPlayerHPBar(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CProgressBar { pDevice, pDeviceContext}
{
}

CPlayerHPBar::CPlayerHPBar(const CPlayerHPBar& Prototype)
	: CProgressBar{ Prototype }
{
}

HRESULT CPlayerHPBar::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayerHPBar::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_fRatio = 1.f;
	m_fCurrentRatio = 1.f;
	m_fFillSpeed = 0.5f;

	m_eType = PROGRESS_TYPE::HP;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CPlayerHPBar::Update(_float fTimeDelta)
{
	if(m_fCurrentRatio > m_fRatio)
	{
		m_fCurrentRatio = m_fRatio;
	}
	else if(m_fCurrentRatio < m_fRatio)
	{
		m_fCurrentRatio += m_fFillSpeed * fTimeDelta;
		if (m_fCurrentRatio >= m_fRatio)
			m_fCurrentRatio = m_fRatio;
	}
}

HRESULT CPlayerHPBar::Ready_Components()
{
	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Texture_GamePlay_PlayerHPBar"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))

	return S_OK;
}

HRESULT CPlayerHPBar::Ready_Children()
{
	UIOBJECT_DESC Children_Desc{};
	Children_Desc.fX = m_fX;
	Children_Desc.fY = m_fY;
	Children_Desc.fSizeX = m_fSizeX;
	Children_Desc.fSizeY = m_fSizeY;
	Children_Desc.fOffsetX = m_fOffsetX;
	Children_Desc.fOffsetY = m_fOffsetY;
	Children_Desc.iDepth = ENUM_CLASS(UI_DEPTH::THIRD);

	if (FAILED(CUIObject::Add_StaticTexture_Child(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_UIObject_PlayerHPBar_Lerp"), &Children_Desc)))
		return E_FAIL;

	Children_Desc.iDepth = ENUM_CLASS(UI_DEPTH::SECOND);
	if (FAILED(CUIObject::Add_DynamicTexture_Child(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_UIObject_Panel"),
		ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_GamePlay_PlayerHP_Bar_Back"), &Children_Desc)))
		return E_FAIL;

	return S_OK;
}

CPlayerHPBar* CPlayerHPBar::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CPlayerHPBar* pInstance = new CPlayerHPBar(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed Created : CPlayerHPBar"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CPlayerHPBar::Clone(void* pArg)
{
	CPlayerHPBar* pInstance = new CPlayerHPBar(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CPlayerHPBar"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayerHPBar::Free()
{
	__super::Free();
}
