#include "ClientPch.h"
#include "BossHP.h"
#include "BossHPBar.h"

CBossHP::CBossHP(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CUI_Panel { pDevice, pDeviceContext }
{
}

CBossHP::CBossHP(const CBossHP& Prototype)
	: CUI_Panel { Prototype }
	, m_fLerpSpeed { Prototype.m_fLerpSpeed }
{
}

HRESULT CBossHP::Initialize_Prototype()
{
	m_fLerpSpeed = 0.5f;

	return S_OK;
}

HRESULT CBossHP::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Children()))
		return E_FAIL;
	    
	m_pGameInstance->Subscribe<EVENT_BIND_BOSSHP>(ENUM_CLASS(EVENT_TYPE::STATIC), [this](const EVENT_BIND_BOSSHP& Event) {
		this->Event_BindBossHP(Event); });

	return S_OK;
}

void CBossHP::Priority_Update(_float fTimeDelta)
{
	if(m_pCurrentBossHP)
		__super::Children_Priority_Update(fTimeDelta);
}

void CBossHP::Update(_float fTimeDelta)
{
	if (m_pCurrentBossHP)
		__super::Children_Update(fTimeDelta);
}

void CBossHP::Late_Update(_float fTimeDelta)
{
	if (m_pCurrentBossHP)
	{
		Update_BarData(fTimeDelta);
		__super::Children_Late_Update(fTimeDelta);
	}
}

HRESULT CBossHP::Render()
{
	return S_OK;
}

HRESULT CBossHP::Ready_Children()
{
	CTextureUI::TEXTURE_UI_DESC Children_Desc = {};
	Children_Desc.fX = m_fX;
	Children_Desc.fY = m_fY;
	Children_Desc.fSizeX = 650.f;
	Children_Desc.fSizeY = 42.f;
	Children_Desc.fOffsetX = 0.f;
	Children_Desc.fOffsetY = -350.f;
	Children_Desc.iDepth = ENUM_CLASS(UI_DEPTH::FORTH);
	Children_Desc.iTexturePrototypeLevelIndex = ENUM_CLASS(LEVEL::STATIC);
	Children_Desc.strTexturePrototypeTag = TEXT("Prototype_Component_Texture_GamePlay_BossHP");
	Children_Desc.IsBlend = false;
	Children_Desc.fAlpha = 1.f;

	if (FAILED(__super::Add_Child(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_UIObject_BossHPBar"), &Children_Desc, reinterpret_cast<CUIObject**>(&m_pHpBar))))
		return E_FAIL;

	Children_Desc.fSizeX = 652.f;
	Children_Desc.fSizeY = 44.f;
	Children_Desc.iDepth = ENUM_CLASS(UI_DEPTH::THIRD);
	Children_Desc.strTexturePrototypeTag = TEXT("Prototype_Component_Texture_GamePlay_BossHP_Back");

	if (FAILED(__super::Add_Child(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_UIObject_Panel"), &Children_Desc)))
		return E_FAIL;

	return S_OK;
}

void CBossHP::Event_BindBossHP(const EVENT_BIND_BOSSHP& Event)
{
	m_fLineHP = Event.m_fLineHP;
	m_fMaxBossHP = Event.m_fMaxBossHP;
	m_pCurrentBossHP = Event.m_pCurrentBossHP;

	m_iNumMaxLine = static_cast<_uint>(m_fMaxBossHP / m_fLineHP);
	m_iNumLine = m_iNumMaxLine;
	

	_float fCurrentLineHp = fmod(*m_pCurrentBossHP, m_fLineHP);

	m_fLineRatio = fCurrentLineHp / m_fLineHP;
	m_fBarRatio = m_fLineRatio;

	m_IsChangeLine = false;
}

void CBossHP::Update_BarData(_float fTimeDelta)
{
	if (*m_pCurrentBossHP <= 0.f)
	{
		m_pCurrentBossHP = nullptr;
		return;
	}

	_float fCurrentLineHp = fmod(*m_pCurrentBossHP, m_fLineHP);

	m_fLineRatio = fCurrentLineHp / m_fLineHP;

	if (m_fBarRatio < m_fLineRatio)
	{
		m_fBarRatio -= m_fLerpSpeed * fTimeDelta;
		if (m_fBarRatio <= 0.f)
		{
			m_fBarRatio = 1.f;
			m_IsChangeLine = true;
		}
	}
	else if (m_fBarRatio > m_fLineRatio)
	{
		m_fBarRatio -= m_fLerpSpeed * fTimeDelta;
		if (m_fBarRatio <= m_fLineRatio)
			m_fBarRatio = m_fLineRatio;
	}

	m_pHpBar->Set_Ratio(m_fBarRatio);

	if (m_IsChangeLine)
	{
		m_IsChangeLine = false;

		m_iNumLine = static_cast<_uint>(*m_pCurrentBossHP / m_fLineHP);

		if (m_iNumLine <= 0)
			m_pHpBar->Change_PassIndex(ENUM_CLASS(SHADER_VTXPOSTEX::PROGRESSBAR));

		m_pHpBar->Change_TextureIndex(static_cast<_uint>((static_cast<_float>(m_iNumLine) / static_cast<_float>(m_iNumMaxLine)) * 3.f));
	}
}

CBossHP* CBossHP::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{ 
	CBossHP* pInstance = new CBossHP(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed Created : CBossHP"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CBossHP::Clone(void* pArg)
{
	CBossHP* pInstance = new CBossHP(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CBossHP"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBossHP::Free()
{
	__super::Free();

	Safe_Release(m_pHpBar);
}
