#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CShader;

class CMeshMaterial final : public CBase
{
private:
	CMeshMaterial(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CMeshMaterial() = default;

public:
	HRESULT Initialize_Assimp(const _char* pModelFilePath, const aiMaterial* pAIMaterial);
	HRESULT Initialize_Json(const _char* pModelFilePath, Json& Data);
	HRESULT Initialize_Binary(const _char* pModelFilePath, ifstream& File);

	_bool	Bind_Material(CShader* pShader, const _char* pConstantName, _uint iSRVIndex, _uint iTextureType);

private:
	ID3D11Device*						m_pDevice = { nullptr };
	ID3D11DeviceContext*				m_pDeviceContext = { nullptr };
	vector<ID3D11ShaderResourceView*>	m_SRVs[AI_TEXTURE_TYPE_MAX];

public:
	static CMeshMaterial*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const _char* pModelFilePath, const aiMaterial* pAIMaterial);
	static CMeshMaterial*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const _char* pModelFilePath, Json& Data);
	static CMeshMaterial*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const _char* pModelFilePath, ifstream& File);
	virtual void			Free() override;
};

NS_END