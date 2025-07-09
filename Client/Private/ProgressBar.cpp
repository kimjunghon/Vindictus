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

	m_fStartX = g_iWinSizeX - (m_fX + m_fOffsetX + (m_fSizeX * 0.5f));
	
	m_pGameInstance->Subscribe<EVENT_PROGRESSBAR>(ENUM_CLASS(LEVEL::STATIC), [this](const EVENT_PROGRESSBAR& Event) {
		this->Event_ProgressBar(Event); });

	return S_OK;
}

void CProgressBar::Priority_Update(_float fTimeDelta)
{
	__super::Children_Priority_Update(fTimeDelta);
}

void CProgressBar::Update(_float fTimeDelta)
{

}

void CProgressBar::Late_Update(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::UI, this)))
		return;

	__super::Children_Late_Update(fTimeDelta);
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


	if (FAILED(m_pShaderCom->Bind_Float("g_fStartX", m_fStartX)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Float("g_fSizeX", m_fSizeX)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Float("g_fProgressBarRatio", m_fCurrentRatio)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_Shader_Texture(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	m_pShaderCom->Begin(ENUM_CLASS(SHADER_VTXPOSTEX::PROGRESSBAR));

	m_pVIBufferCom->Bind_Resources();

	m_pVIBufferCom->Render();

	return S_OK;
}


void CProgressBar::Event_ProgressBar(const EVENT_PROGRESSBAR& Event)
{
	if(Event.eType == m_eType)
		m_fRatio = Event.fRatio;
}


void CProgressBar::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
