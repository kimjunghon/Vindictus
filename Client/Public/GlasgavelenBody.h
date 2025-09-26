#pragma once
#include "Body.h"

NS_BEGIN(Engine)
class CTexture;
NS_END


NS_BEGIN(Client)

class CAnimMachine;

class CGlasgavelenBody final : public CBody
{
private:
	CGlasgavelenBody(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CGlasgavelenBody(const CGlasgavelenBody& Prototype);
	virtual ~CGlasgavelenBody() = default;

public:
	void				BrokenWing() { m_IsBrokenWing = true; }

	virtual HRESULT		Add_AnimNotify(const string& strAnimName, _float fTrackPosition, function<void()> Callback) override;

public:
	virtual HRESULT		Initialize_Prototype() override;
	virtual HRESULT		Initialize(void* pArg) override;
	virtual void		Priority_Update(_float fTimeDelta) override;
	virtual void		Update(_float fTimeDelta) override;
	virtual void		Late_Update(_float fTimeDelta) override;
	virtual HRESULT		Render() override;
	virtual HRESULT		Render_Shadow() override;

	void				Change_BrokenModel();

private:
	ROOTMOTION_OPTION	m_DefaultOption = {};
	ROOTMOTION_OPTION	m_RotationOption = {};

	_uint				m_iRotationFlag = {};

	CModel*				m_pBrokenModelCom = {};

	_bool				m_IsBrokenWing = {};
	CTexture*			m_pDissolveTexture = { nullptr };

private:
	void	Update_BrokenWing(_float fTimeDelta);
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();
	HRESULT Bind_ShaderResources_Dissolve();

public:
	static CGlasgavelenBody*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;
};

NS_END