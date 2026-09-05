#pragma once
#include "Component.h"

NS_BEGIN(Engine)

class ENGINE_DLL CTexture final : public CComponent
{
private:
	CTexture(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CTexture(const CTexture& Prototype);
	virtual ~CTexture() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pTextureFilePath, _uint iNumTextures);
	virtual HRESULT Initialize(void* pArg);

public:
	HRESULT Bind_Shader_Texture(class CShader* pShader, const _char* pConstantName, _uint iTextureIndex);
	HRESULT	Copy_Resource(_uint iTextureIndex, ID3D11Texture2D* pSourTexture);

private:
	vector<ID3D11ShaderResourceView*>	m_SPVs;
	_uint								m_iNumTextures = {};

public:
	static CTexture*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const _tchar* pTextureFilePath, _uint iNumTextures);
	virtual CComponent* Clone(void* pArg) override;
	virtual void		Free() override;
};

NS_END