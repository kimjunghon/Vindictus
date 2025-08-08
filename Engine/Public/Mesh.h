#pragma once
#include "VIBuffer.h"

NS_BEGIN(Engine)

class CBone;
class CShader;

class ENGINE_DLL CMesh final : public CVIBuffer
{
private:
	CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CMesh(const CMesh& Prototype);
	virtual ~CMesh() = default;

public:
	_uint			Get_MaterialIndex() { return m_iMaterialIndex; }

public:
	virtual HRESULT Initialize_Prototype_Assimp(MODELTYPE eType, const aiMesh* pAIMesh, const vector<CBone*>& Bones, _fmatrix PreTransformMatrix);
	virtual HRESULT Initialize_Prototype_Binary(MODELTYPE eType, ifstream& File, const vector<CBone*>& Bones, _fmatrix PreTransformMatrix);
	virtual HRESULT Initialize(void* pArg) override;
	
	HRESULT			Bind_BoneMatrices(CShader* pShader, const _char* pConstantName, const vector<CBone*>& Bones);

#ifdef _DEBUG
	_bool			Is_Pick(_fvector vLocalPickPosition, _fvector vLocalPickDir, _float& fDist);
#endif

private:
	_char				m_szName[MAX_PATH] = {};
	_uint				m_iMaterialIndex = {};

	_uint				m_iNumBones = {};
	_float4x4			m_BoneMatrices[MAX_BONES] = {};
	vector<_int>		m_BoneIndices;
	vector<_float4x4>	m_OffsetMatrices;

#ifdef _DEBUG
	vector<VTXMESH>		m_Vertices;
	vector<_uint>		m_Indices;
#endif
	
private:
	HRESULT Ready_Vertices_For_NonAnim_Assimp(const aiMesh* pAIMesh, _fmatrix PreTransformMatrix);
	HRESULT Ready_Vertices_For_Anim_Assimp(const aiMesh* pAIMesh, const vector<CBone*>& Bones);

	HRESULT Ready_Vertices_For_NonAnim_Binary(ifstream& File, _fmatrix PreTransformMatrix);
	HRESULT Ready_Vertices_For_Anim_Binary(ifstream& File, const vector<CBone*>& Bones);

public:
	static CMesh*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, MODELTYPE eType, const aiMesh* pAIMesh, const vector<CBone*>& Bones, _fmatrix PreTransformMatrix);
	static CMesh*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, MODELTYPE eType, ifstream& File, const vector<CBone*>& Bones, _fmatrix PreTransformMatrix);
	virtual CComponent* Clone(void* pArg) override;
	virtual void		Free() override;
};

NS_END