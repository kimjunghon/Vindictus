#include "ProgressBar.h"
#include "GameInstance.h"

CProgressBar::CProgressBar(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CUIObject { pDevice, pDeviceContext }
{
}

CProgressBar::CProgressBar(const CProgressBar& Prototype)
	: CUIObject { Prototype }
{
}

HRESULT CProgressBar::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CProgressBar::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_fStartX = m_fX - m_fSizeX;
	m_fRatio = 0.f;
	m_fCurrentRatio = 0.f;
	m_fFillSpeed = 0.5f;

	m_pGameInstance->Subscribe<EVENT_PROGRESSBAR>(ENUM_CLASS(LEVEL::STATIC), [this](const EVENT_PROGRESSBAR& Event) {
		this->Event_ProgressBar(Event); });

	return S_OK;
}

void CProgressBar::Priority_Update(_float fTimeDelta)
{
}

void CProgressBar::Update(_float fTimeDelta)
{
	if (m_fRatio > m_fCurrentRatio)
	{
		m_fCurrentRatio += (m_fFillSpeed * fTimeDelta);
		if (m_fCurrentRatio > m_fRatio)
			m_fCurrentRatio = m_fRatio;
	}
}

void CProgressBar::Late_Update(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::UI, this)))
		return;
}

HRESULT CProgressBar::Render()
{
	__super::Begin();

	if (FAILED(m_pTransformCom->Bind_Shader_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Float("g_ProgressBarStartX", m_fStartX)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Float("g_ProgressBarSizeX", m_fSizeX)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Float("g_ProgressBarRatio", m_fCurrentRatio)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_Shader_Texture(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	m_pShaderCom->Begin(ENUM_CLASS(SHADER_VTXPOSTEX::PROGRESSBAR));

	m_pVIBufferCom->Bind_Resources();

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CProgressBar::Ready_TextureCom(_uint iTexturePrototypeLevelIndex, const _wstring& strTexturePrototypeTag)
{
	if (FAILED(CGameObject::Add_Component(iTexturePrototypeLevelIndex, strTexturePrototypeTag,
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CProgressBar::Ready_Components()
{
	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
		return E_FAIL;

	return S_OK;
}

void CProgressBar::Event_ProgressBar(const EVENT_PROGRESSBAR& Event)
{
	m_fRatio = Event.fRatio;
}

CProgressBar* CProgressBar::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CProgressBar* pInstance = new CProgressBar(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed Created : CProgressBar"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CProgressBar::Clone(void* pArg)
{
	CProgressBar* pInstance = new CProgressBar(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CProgressBar"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CProgressBar::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
