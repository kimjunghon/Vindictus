#pragma once
#include "Body.h"

class CPuppyBody final : public CBody
{
public:
	typedef struct tagPuppyBodyDesc : public BODY_DESC
	{
		_uint iPuppyTypeIndex;
	}PUPPY_BODY_DESC;
private:
	enum class PUPPY_TYPE { GRAY, WHITE, ARMOR };

private:
	CPuppyBody(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CPuppyBody(const CPuppyBody& Prototype);
	virtual ~CPuppyBody() = default;

public:
	virtual HRESULT		Initialize_Prototype() override;
	virtual HRESULT		Initialize(void* pArg) override;
	virtual void		Priority_Update(_float fTimeDelta) override;
	virtual void		Update(_float fTimeDelta) override;
	virtual void		Late_Update(_float fTimeDelta) override;
	virtual HRESULT		Render() override;
	virtual HRESULT		Render_Shadow() override;

private:
	HRESULT				Ready_Components(_uint iPuppyType);
	HRESULT				Bind_ShaderResources();

public:
	static CPuppyBody*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

