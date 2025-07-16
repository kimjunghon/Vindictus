#pragma once
#include "Component.h"

NS_BEGIN(Engine)

class CMesh;
class CMeshMaterial;

class ENGINE_DLL CModel final : public CComponent
{
private:
	CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CModel(const CModel& Prototype);
	virtual ~CModel() = default;

public:
	virtual HRESULT Initialize_Prototype(MODELTYPE eModelType, const _char* pModelFilePath, _fmatrix PreTransformMatrix);

	virtual HRESULT Initialize(void* pArg);

	HRESULT			Render(class CShader* pShader);
	void			Bind_Shader_Material(class CShader* pShader, const _char* pConstantName, _uint iMaterialIndex, _uint iSRVIndex, _uint iTextureType);

	HRESULT			Save_Json(const _wstring& strJsonPath);
	HRESULT			MeshesToJson(ofstream& File, Json& Array);
	HRESULT			MaterialToJson(ofstream& File, Json& Array);
private:
	const aiScene*			m_pAIScene = { nullptr };
	Assimp::Importer		m_Importer = {};
	MODELTYPE				m_eModelType = {};
	_float4x4				m_PreTransformMatrix = {};

	_uint					m_iNumMeshes = {};
	vector<CMesh*>			m_Meshes;

	_uint					m_iNumMaterials = {};
	vector<CMeshMaterial*>	m_Materials;

private:
	HRESULT Ready_Meshes(_fmatrix PreTransformMatrix);
	HRESULT Ready_Meshes(Json& Data, _fmatrix PreTransformMatrix);
	HRESULT Ready_Materials(const _char* pModelFilePath);
	HRESULT Ready_Materials(Json& Data, const _char* pModelFilePath);

public:
	static CModel*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, MODELTYPE eModelType, const _char* pModelFilePath, _fmatrix PreTransformMatrix);
	virtual CComponent* Clone(void* pArg) override;
	virtual void		Free() override;
};

NS_END