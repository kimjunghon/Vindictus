#pragma once
#include "Client_Defines.h"
#include "UIObject.h"

NS_BEGIN(Engine)
class CShader;
class CVIBuffer_Dynamic;
class CTexture;
NS_END

NS_BEGIN(Client)

class CPool_Instance;

class CDamageFont final : public CUIObject
{
public:
	typedef struct tagDamageDesc
	{
		COLLIDER_OWNER eOwner;
		_uint iDamage;
		_vector vPosition;
	}DAMAGE_DESC;

private:
	typedef struct tagFontData
	{
		_float2 vSize;
		_float4 vUV;
	}ATLAS_DATA;

	typedef unordered_map<_char, ATLAS_DATA> FONT_DATAS;

private:
	CDamageFont(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CDamageFont(const CDamageFont& Prototype);
	virtual ~CDamageFont() = default;

public:
	virtual HRESULT Initialize_Prototype(const _char* pFontDataFilePath);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	
	HRESULT			Spawn(void* pArg);

private:
	CPool_Instance*		m_pPoolInstance = { nullptr };
	CVIBuffer_Dynamic*	m_pVIBufferCom = { nullptr };
	CShader*			m_pShaderCom = { nullptr };
	CTexture*			m_pTextureCom = { nullptr };

	_uint				m_iDamage = {};
	_float				m_fLifeTime = {};
	_float				m_fCurrentLifeTime = {};

	vector<VTXPOSTEX>	m_Vertices;
	FONT_DATAS			m_MonsterFontDatas;
	FONT_DATAS			m_PlayerFontDatas;

private:
	HRESULT		Ready_Components();
	HRESULT		Bind_ShaderResources();

	HRESULT		Ready_FontDatas(const _char* pFontDataFilePath);
	void		Add_MonsterData(_uint iTextureSizeX, _uint iTextureSizeY, _uint iStartX, _uint iEndX, _uint iStartY, _uint iEndY, _char chFont);
	void		Add_PlayerData(_uint iTextureSizeX, _uint iTextureSizeY, _uint iStartX, _uint iEndX, _uint iStartY, _uint iEndY, _char chFont);
	void		Update_Buffer(COLLIDER_OWNER eOwner);

public:
	static CDamageFont*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const _char* pFontDataFilePath);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

NS_END