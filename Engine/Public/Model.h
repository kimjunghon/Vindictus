#pragma once
#include "Component.h"

NS_BEGIN(Engine)

class CMesh;
class CMeshMaterial;
class CBone;
class CAnimation;

class ENGINE_DLL CModel final : public CComponent
{
private:
	CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CModel(const CModel& Prototype);
	virtual ~CModel() = default;

public:
	_uint			Get_NumMeshes() const { return m_iNumMeshes; }

	_float4x4		Get_PreTransformMatrix() const { return m_PreTransformMatrix; }
	void			Set_PreTransformMatrix(_fmatrix PreTransformMatrix) { XMStoreFloat4x4(&m_PreTransformMatrix, PreTransformMatrix); }

public:
	virtual HRESULT Initialize_Prototype(MODELTYPE eModelType, const _char* pModelFilePath, _fmatrix PreTransformMatrix);
	virtual HRESULT Initialize(void* pArg);
	HRESULT			Render(_uint iMeshIndex);

	HRESULT			Bind_Shader_Material(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex, _uint iSRVIndex, _uint iTextureType, _bool* hasSPV = nullptr);
	HRESULT			Bind_BoneMatrices(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex);

	HRESULT			Set_Animation(const ANIM_DATA& AnimData);
	_bool			Play_Animation(_float fTimeDelta);

	HRESULT			Save_Binary(const _wstring& strSaveFilePath);
	HRESULT			BonesToBinary(ofstream& File, const aiNode* pAINode);
	HRESULT			MeshesToBinary(ofstream& File);
	HRESULT			MaterialToBinary(ofstream& File);
	HRESULT			AnimationToBinary(ofstream& File);

public:
	_bool			CanChangeAnimation();
	_bool			CurrentAnim_Finished() { return m_IsFinished; }

private:
	const aiScene*				m_pAIScene = { nullptr };
	Assimp::Importer			m_Importer = {};
	MODELTYPE					m_eModelType = {};
	_float4x4					m_PreTransformMatrix = {};

private:
	_uint						m_iNumMeshes = {};
	vector<CMesh*>				m_Meshes;

private:
	_uint						m_iNumMaterials = {};
	vector<CMeshMaterial*>		m_Materials;

private:
	vector<CBone*>				m_Bones;
	_int						m_iRootBoneIndex = { -1};

private:
	CAnimation*					m_pCurrentAnimation = { nullptr };
	string						m_strCurrentAnimName = {};
	_uint						m_iNumAnimation = {};
	map<string, CAnimation*>	m_Animations;
	
	ANIM_DATA					m_CurrentAnimData = {};
	_bool						m_bAnimChange = {};	
	_bool						m_IsFinished = {};

	
private:
	CAnimation* Find_Animation(const string& strAnimationTag);
#pragma region FBX
private:
	HRESULT Ready_Meshes();
	HRESULT Ready_Materials(const _char* pModelFilePath);
	HRESULT Ready_Bones(const aiNode* pAINode, _int iParentIndex);
	HRESULT Ready_Animation();
#pragma endregion

#pragma region BINARY
private:
	HRESULT Ready_Bones(ifstream& File, _int iParentIndex);
	HRESULT Ready_Meshes(ifstream& File);
	HRESULT Ready_Materials(ifstream& File, const _char* pModelFilePath);
	HRESULT Ready_Animation(ifstream& File);
#pragma endregion

public:
	static CModel*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, MODELTYPE eModelType, const _char* pModelFilePath, _fmatrix PreTransformMatrix);
	virtual CComponent* Clone(void* pArg) override;
	virtual void		Free() override;
};

NS_END