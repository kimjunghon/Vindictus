#pragma once
#include "Client_Defines.h"
#include "UIObject.h"

NS_BEGIN(Engine)
class CTexture;
class CVIBuffer;
class CShader;
NS_END

NS_BEGIN(Client)

class CWeapon;
class CArmor;
class CPlayerInstance;
class CColorPoint;

class CPalette final : public CUIObject
{
private:
	CPalette(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CPalette(const CPalette& Prototype);
	virtual ~CPalette() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void			Set_DyeingItem(pair<ITEM_TYPE, _uint>* pItemType);
	void			Change_DyeingPart(_uint iPartIndex);
	void			Clear();
	void			Dyeing();
private:
	CColorPoint*			m_pColorPoint = {nullptr};
	CColorPoint*			m_pSelectColorPoint = { nullptr };
	CPlayerInstance*		m_pPlayerInstance = { nullptr };
	ID3D11Texture2D*		m_pPaletteSample = { nullptr };

	CTexture*				m_pTextureCom = { nullptr };
	CVIBuffer*				m_pVIBufferCom = { nullptr };
	CShader*				m_pShaderCom = { nullptr };

	CWeapon*				m_pDyeingWeapon = { nullptr };
	CArmor*					m_pDyeingArmor = { nullptr };

	DYEING_DATAS*			m_pDyeingDatas = { nullptr };

	_uint					m_iDyeMaterial = {};
	_uint					m_iSelectPartIndex = {};

	_float3					m_vDyeColor = {};

	_uint					m_iPaletteWidth = {};
	_uint					m_iPaletteHeight = {};

	_bool					m_IsMouseOn = {};
	_uint*					m_pPixels = { nullptr };

private:
	HRESULT			Ready_Components();
	HRESULT			Ready_Palette();
	HRESULT			Ready_ColorPoint();
	HRESULT			Bind_ShaderResources();

	virtual _bool	IsPick(HWND hWnd) override;
	void			Save_PaletteData();
	_float3			PaletteColor(_uint iX, _uint iY);
	
public:
	static CPalette*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

NS_END