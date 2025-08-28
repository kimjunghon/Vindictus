#include "EnginePch.h"
#include "Shader.h"
#include "Mesh.h"
#include "Bone.h"


CMesh::CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CVIBuffer { pDevice, pDeviceContext }
{
}

CMesh::CMesh(const CMesh& Prototype)
	: CVIBuffer { Prototype }
	, m_iMaterialIndex { Prototype.m_iMaterialIndex }
#ifdef _DEBUG
	, m_Vertices{ Prototype.m_Vertices }
	, m_Indices {Prototype.m_Indices }
#endif
{
}

HRESULT CMesh::Initialize_Prototype_Assimp(MODEL_TYPE eType, const aiMesh* pAIMesh, const vector<CBone*>& Bones, _fmatrix PreTransformMatrix)
{
	strcpy_s(m_szName, pAIMesh->mName.data);

	m_iMaterialIndex = pAIMesh->mMaterialIndex;
	m_iNumVertices = pAIMesh->mNumVertices;
	m_iNumIndices = pAIMesh->mNumFaces * 3;
	m_iIndexStride = 4;
	m_iNumVertexBuffers = 1;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_ePrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	
	HRESULT hr = MODEL_TYPE::NONANIM == eType ? Ready_Vertices_For_NonAnim_Assimp(pAIMesh, PreTransformMatrix) : Ready_Vertices_For_Anim_Assimp(pAIMesh, Bones);

	if (FAILED(hr))
		return E_FAIL;
	D3D11_BUFFER_DESC	IBDesc{};

	IBDesc.ByteWidth = m_iNumIndices * m_iIndexStride;
	IBDesc.Usage = D3D11_USAGE_DEFAULT;
	IBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	IBDesc.CPUAccessFlags = 0;
	IBDesc.MiscFlags = 0;
	IBDesc.StructureByteStride = m_iIndexStride;

	_uint* pIndices = new _uint[m_iNumIndices];

	_uint iIndex = {};

	for (_uint i = 0; i < pAIMesh->mNumFaces; i++)
	{
		aiFace AIFace = pAIMesh->mFaces[i];

		pIndices[iIndex++] = AIFace.mIndices[0];
		pIndices[iIndex++] = AIFace.mIndices[1];
		pIndices[iIndex++] = AIFace.mIndices[2];
	}

	D3D11_SUBRESOURCE_DATA IBInitialData{};
	IBInitialData.pSysMem = pIndices;

	if (FAILED(m_pDevice->CreateBuffer(&IBDesc, &IBInitialData, &m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

	return S_OK;
}

HRESULT CMesh::Initialize_Prototype_Binary(MODEL_TYPE eType, ifstream& File, const vector<CBone*>& Bones, _fmatrix PreTransformMatrix, MODEL_BOUNDING& ModelBounding)
{
	MESH_INFO tMeshInfo = {};
	size_t iMeshNameLenghth = {};

	File.read(reinterpret_cast<_char*>(&tMeshInfo), sizeof(MESH_INFO));
	File.read(reinterpret_cast<_char*>(&iMeshNameLenghth), sizeof(size_t));
	File.read(m_szName, sizeof(_char) * iMeshNameLenghth);

	m_iMaterialIndex = tMeshInfo.iMaterialIndex;
	m_iNumVertices = tMeshInfo.iNumVertices;
	m_iNumIndices = tMeshInfo.iNumFaces * 3;
	m_iIndexStride = 4;
	m_iNumVertexBuffers = 1;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_ePrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	HRESULT hr = MODEL_TYPE::NONANIM == eType ? Ready_Vertices_For_NonAnim_Binary(File, PreTransformMatrix, ModelBounding) : Ready_Vertices_For_Anim_Binary(File, Bones, ModelBounding);

	if (FAILED(hr))
		return E_FAIL;

	D3D11_BUFFER_DESC	IBDesc{};
	IBDesc.ByteWidth = m_iNumIndices * m_iIndexStride;
	IBDesc.Usage = D3D11_USAGE_DEFAULT;
	IBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	IBDesc.CPUAccessFlags = 0;
	IBDesc.MiscFlags = 0;
	IBDesc.StructureByteStride = m_iIndexStride;

	_uint* pIndices = new _uint[m_iNumIndices];

	_uint iIndex = {};

	for (_uint i = 0; i < tMeshInfo.iNumFaces; i++)
	{
		File.read(reinterpret_cast<_char*>(&pIndices[iIndex++]), sizeof(_uint));
		File.read(reinterpret_cast<_char*>(&pIndices[iIndex++]), sizeof(_uint));
		File.read(reinterpret_cast<_char*>(&pIndices[iIndex++]), sizeof(_uint));
	}

#ifdef _DEBUG
	if(eType == MODEL_TYPE::NONANIM)
	{
		m_Indices.resize(m_iNumIndices);
		memcpy(m_Indices.data(), pIndices, sizeof(_uint) * m_iNumIndices);
	}
#endif

	D3D11_SUBRESOURCE_DATA IBInitialData{};
	IBInitialData.pSysMem = pIndices;

	if (FAILED(m_pDevice->CreateBuffer(&IBDesc, &IBInitialData, &m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

	return S_OK;
}


HRESULT CMesh::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CMesh::Bind_BoneMatrices(CShader* pShader, const _char* pConstantName, const vector<CBone*>& Bones)
{
	for (_uint i = 0; i < m_iNumBones; i++)
	{
		XMStoreFloat4x4(&m_BoneMatrices[i], XMMatrixMultiply(XMLoadFloat4x4(&m_OffsetMatrices[i]), Bones[m_BoneIndices[i]]->Get_CombinedTransformationMatrix()));
	}

	return pShader->Bind_Matrices(pConstantName, m_BoneMatrices, m_iNumBones);
}

HRESULT CMesh::Bind_PoseBoneMatrices(CShader* pShader, const _char* pConstantName, const vector<CBone*>& Bones)
{
	for (_uint i = 0; i < m_iNumBones; i++)
	{
		XMStoreFloat4x4(&m_BoneMatrices[i], XMMatrixMultiply(XMLoadFloat4x4(&m_OffsetMatrices[i]), Bones[m_BoneIndices[i]]->Get_PoseCombinedTransformationMatrix()));
	}

	return pShader->Bind_Matrices(pConstantName, m_BoneMatrices, m_iNumBones);
}

const _float4x4* CMesh::Find_OffsetMatrix(vector<CBone*>& Bones, const string& strSocketBoneName)
{
	for (_uint i = 0; i < m_iNumBones; i++)
	{
		if (Bones[m_BoneIndices[i]]->Compare_Name(strSocketBoneName.c_str()))
			return &m_OffsetMatrices[i];
	}

	return nullptr;
}

#ifdef _DEBUG
_bool CMesh::Is_Pick(_fvector vLocalPickPosition, _fvector vLocalPickDir, _float& fDist)
{
	_uint iIndex = {};

	_bool IsHit = false;
	_float fMin_Dist = FLT_MAX;

	while(iIndex < m_iNumIndices)
	{	
		_vector vVertexPosition[3] = {
		XMLoadFloat3(&(m_Vertices[m_Indices[iIndex++]].vPosition)),
		XMLoadFloat3(&(m_Vertices[m_Indices[iIndex++]].vPosition)),
		XMLoadFloat3(&(m_Vertices[m_Indices[iIndex++]].vPosition)),
		};

		_float fCurrentDist = {};

		if (TriangleTests::Intersects(vLocalPickPosition, vLocalPickDir, vVertexPosition[0], vVertexPosition[1], vVertexPosition[2], fCurrentDist))
		{
			IsHit = true;
			if (fCurrentDist <= fMin_Dist)
				fMin_Dist = fCurrentDist;
		}
	}

	if (IsHit)
	{
		fDist = fMin_Dist;
		return true;
	}

	return false;
}
#endif

HRESULT CMesh::Ready_Vertices_For_NonAnim_Assimp(const aiMesh* pAIMesh, _fmatrix PreTransformMatrix)
{
	m_iVertexStride = sizeof(VTXMESH);

	D3D11_BUFFER_DESC VBDesc{};

	VBDesc.ByteWidth = m_iNumVertices * m_iVertexStride;
	VBDesc.Usage = D3D11_USAGE_DEFAULT;
	VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;;
	VBDesc.CPUAccessFlags = 0;
	VBDesc.MiscFlags = 0;
	VBDesc.StructureByteStride = m_iVertexStride;

	VTXMESH* pVertices = new VTXMESH[m_iNumVertices];

	for (_uint i = 0; i < m_iNumVertices; i++)
	{
		memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vPosition, XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vPosition), PreTransformMatrix));

		memcpy(&pVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vNormal, XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vNormal), PreTransformMatrix));

		memcpy(&pVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));
		memcpy(&pVertices[i].vBinormal, &pAIMesh->mBitangents[i], sizeof(_float3));
		memcpy(&pVertices[i].vTexcoord, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));
	}

	D3D11_SUBRESOURCE_DATA VBInitialData{};
	VBInitialData.pSysMem = pVertices;

	if (FAILED(m_pDevice->CreateBuffer(&VBDesc, &VBInitialData, &m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	return S_OK;
}

HRESULT CMesh::Ready_Vertices_For_Anim_Assimp(const aiMesh* pAIMesh, const vector<CBone*>& Bones)
{
	m_iVertexStride = sizeof(VTXANIMMESH);

	D3D11_BUFFER_DESC VBDesc{};
	VBDesc.ByteWidth = m_iNumVertices * m_iVertexStride;
	VBDesc.Usage = D3D11_USAGE_DEFAULT;
	VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VBDesc.CPUAccessFlags = 0;
	VBDesc.MiscFlags = 0;
	VBDesc.StructureByteStride = m_iVertexStride;

	VTXANIMMESH* pVertices = new VTXANIMMESH[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXANIMMESH) * pAIMesh->mNumVertices);

	for (_uint i = 0; i < m_iNumVertices; i++)
	{
		memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
		memcpy(&pVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
		memcpy(&pVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));
		memcpy(&pVertices[i].vBinormal, &pAIMesh->mBitangents[i], sizeof(_float3));
		memcpy(&pVertices[i].vTexcoord, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));
	}

	m_iNumBones = pAIMesh->mNumBones;

	for (_uint i = 0; i < m_iNumBones; i++)
	{
		aiBone* pAIBone = pAIMesh->mBones[i];
		_float4x4 OffsetMatrix;

		memcpy(&OffsetMatrix, &pAIBone->mOffsetMatrix, sizeof(_float4x4));

		XMStoreFloat4x4(&OffsetMatrix, XMMatrixTranspose(XMLoadFloat4x4(&OffsetMatrix)));

		m_OffsetMatrices.push_back(OffsetMatrix);

		_uint iBoneIndex = {0};

		auto Bone = find_if(Bones.begin(), Bones.end(), [&](CBone* pBone)->_bool {
			if (pBone->Compare_Name(pAIBone->mName.data))
				return true;
			iBoneIndex++;

			return false;
			});

		m_BoneIndices.push_back(iBoneIndex);

		for (_uint j = 0; j < pAIBone->mNumWeights; j++)
		{
			aiVertexWeight AIVertexWeight = pAIBone->mWeights[j];

			if (0.f == pVertices[AIVertexWeight.mVertexId].vBlendWeight.x)
			{
				pVertices[AIVertexWeight.mVertexId].vBlendIndex.x = i;
				pVertices[AIVertexWeight.mVertexId].vBlendWeight.x = AIVertexWeight.mWeight;
			}
			else if (0.f == pVertices[AIVertexWeight.mVertexId].vBlendWeight.y)
			{
				pVertices[AIVertexWeight.mVertexId].vBlendIndex.y = i;
				pVertices[AIVertexWeight.mVertexId].vBlendWeight.y = AIVertexWeight.mWeight;
			}
			else if (0.f == pVertices[AIVertexWeight.mVertexId].vBlendWeight.z)
			{
				pVertices[AIVertexWeight.mVertexId].vBlendIndex.z = i;
				pVertices[AIVertexWeight.mVertexId].vBlendWeight.z = AIVertexWeight.mWeight;
			}
			else
			{
				pVertices[AIVertexWeight.mVertexId].vBlendIndex.w = i;
				pVertices[AIVertexWeight.mVertexId].vBlendWeight.w = AIVertexWeight.mWeight;
			}
		}
	}

	if (0 == m_iNumBones)
	{
		m_iNumBones = 1;

		_uint iBoneIndex = { 0 };

		auto iter = find_if(Bones.begin(), Bones.end(), [&](CBone* pBone)->_bool {
			if (true == pBone->Compare_Name(m_szName))
				return true;

			iBoneIndex++;

			return false;
			});

		m_BoneIndices.push_back(iBoneIndex);

		_float4x4 OffsetMatrix;
		XMStoreFloat4x4(&OffsetMatrix, XMMatrixIdentity());

		m_OffsetMatrices.push_back(OffsetMatrix);
	}

	D3D11_SUBRESOURCE_DATA	VBInitialData{};
	VBInitialData.pSysMem = pVertices;

	if (FAILED(m_pDevice->CreateBuffer(&VBDesc, &VBInitialData, &m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	return S_OK;
}

HRESULT CMesh::Ready_Vertices_For_NonAnim_Binary(ifstream& File, _fmatrix PreTransformMatrix, MODEL_BOUNDING& ModelBounding)
{
	m_iVertexStride = sizeof(VTXMESH);

	D3D11_BUFFER_DESC VBDesc{};

	VBDesc.ByteWidth = m_iNumVertices * m_iVertexStride;
	VBDesc.Usage = D3D11_USAGE_DEFAULT;
	VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;;
	VBDesc.CPUAccessFlags = 0;
	VBDesc.MiscFlags = 0;
	VBDesc.StructureByteStride = m_iVertexStride;

	VTXMESH* pVertices = new VTXMESH[m_iNumVertices];

	for (_uint i = 0; i < m_iNumVertices; i++)
	{
		File.read(reinterpret_cast<_char*>(&pVertices[i]), sizeof(VTXMESH));
		XMStoreFloat3(&pVertices[i].vPosition, XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vPosition), PreTransformMatrix));
		XMStoreFloat3(&pVertices[i].vNormal, XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vNormal), PreTransformMatrix));

	}

	_float fMinX = ModelBounding.vMinPosition.x;
	_float fMinY = ModelBounding.vMinPosition.y;
	_float fMinZ = ModelBounding.vMinPosition.z;

	_float fMaxX = ModelBounding.vMaxPosition.x;
	_float fMaxY = ModelBounding.vMaxPosition.y;
	_float fMaxZ = ModelBounding.vMaxPosition.z;

	for(_uint i = 0; i< m_iNumVertices; i++)
	{

		if (fMinX > pVertices[i].vPosition.x)
			fMinX = pVertices[i].vPosition.x;

		if (fMinY > pVertices[i].vPosition.y)
			fMinY = pVertices[i].vPosition.y;

		if (fMinZ > pVertices[i].vPosition.z)
			fMinZ = pVertices[i].vPosition.z;

		if (fMaxX < pVertices[i].vPosition.x)
			fMaxX = pVertices[i].vPosition.x;

		if (fMaxY < pVertices[i].vPosition.y)
			fMaxY = pVertices[i].vPosition.y;

		if (fMaxZ < pVertices[i].vPosition.z)
			fMaxZ = pVertices[i].vPosition.z;
	}

	ModelBounding.vMinPosition = _float3(fMinX, fMinY, fMinZ);
	ModelBounding.vMaxPosition = _float3(fMaxX, fMaxY, fMaxZ);


#ifdef _DEBUG
	m_Vertices.resize(m_iNumVertices);
	memcpy(m_Vertices.data(), pVertices, sizeof(VTXMESH) * m_iNumVertices);
#endif

	D3D11_SUBRESOURCE_DATA VBInitialData{};
	VBInitialData.pSysMem = pVertices;

	if (FAILED(m_pDevice->CreateBuffer(&VBDesc, &VBInitialData, &m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	return S_OK;
}

HRESULT CMesh::Ready_Vertices_For_Anim_Binary(ifstream& File, const vector<CBone*>& Bones, MODEL_BOUNDING& ModelBounding)
{
	File.read(reinterpret_cast<_char*>(&m_iNumBones), sizeof(_uint));

	for (_uint i = 0; i < m_iNumBones; i++)
	{
		_float4x4 OffsetMatrix;
		size_t iBoneNameLength;
		_char	szBoneName[MAX_PATH] = {};

		File.read(reinterpret_cast<_char*>(&OffsetMatrix), sizeof(_float4x4));
		File.read(reinterpret_cast<_char*>(&iBoneNameLength), sizeof(size_t));
		File.read(szBoneName, sizeof(_char) * iBoneNameLength);

		m_OffsetMatrices.push_back(OffsetMatrix);

		_uint iBoneIndex = {};

		auto	iter = find_if(Bones.begin(), Bones.end(), [&](CBone* pBone)->_bool
			{
				if (true == pBone->Compare_Name(szBoneName))
					return true;

				iBoneIndex++;

				return false;
			});

		m_BoneIndices.push_back(iBoneIndex);
	}

	if (0 == m_iNumBones)
	{
		m_iNumBones = 1;

		_uint	iBoneIndex = { 0 };

		auto	iter = find_if(Bones.begin(), Bones.end(), [&](CBone* pBone)->_bool
			{
				if (true == pBone->Compare_Name(m_szName))
					return true;

				iBoneIndex++;

				return false;
			});

		m_BoneIndices.push_back(iBoneIndex);

		_float4x4		OffsetMatrix;
		XMStoreFloat4x4(&OffsetMatrix, XMMatrixIdentity());

		m_OffsetMatrices.push_back(OffsetMatrix);
	}

	m_iVertexStride = sizeof(VTXANIMMESH);
	
	D3D11_BUFFER_DESC VBDesc{};

	VBDesc.ByteWidth = m_iNumVertices * m_iVertexStride;
	VBDesc.Usage = D3D11_USAGE_DEFAULT;
	VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;;
	VBDesc.CPUAccessFlags = 0;
	VBDesc.MiscFlags = 0;
	VBDesc.StructureByteStride = m_iVertexStride;

	VTXANIMMESH* pVertices = new VTXANIMMESH[m_iNumVertices];
	
	File.read(reinterpret_cast<_char*>(pVertices), sizeof(VTXANIMMESH) * m_iNumVertices);

	_float fMinX = ModelBounding.vMinPosition.x;
	_float fMinY = ModelBounding.vMinPosition.y;
	_float fMinZ = ModelBounding.vMinPosition.z;

	_float fMaxX = ModelBounding.vMaxPosition.x;
	_float fMaxY = ModelBounding.vMaxPosition.y;
	_float fMaxZ = ModelBounding.vMaxPosition.z;

	for (_uint i = 0; i < m_iNumVertices; i++)
	{

		if (fMinX > pVertices[i].vPosition.x)
			fMinX = pVertices[i].vPosition.x;

		if (fMinY > pVertices[i].vPosition.y)
			fMinY = pVertices[i].vPosition.y;

		if (fMinZ > pVertices[i].vPosition.z)
			fMinZ = pVertices[i].vPosition.z;

		if (fMaxX < pVertices[i].vPosition.x)
			fMaxX = pVertices[i].vPosition.x;

		if (fMaxY < pVertices[i].vPosition.y)
			fMaxY = pVertices[i].vPosition.y;

		if (fMaxZ < pVertices[i].vPosition.z)
			fMaxZ = pVertices[i].vPosition.z;
	}

	ModelBounding.vMinPosition = _float3(fMinX, fMinY, fMinZ);
	ModelBounding.vMaxPosition = _float3(fMaxX, fMaxY, fMaxZ);

	D3D11_SUBRESOURCE_DATA VBInitialData{};
	VBInitialData.pSysMem = pVertices;

	if (FAILED(m_pDevice->CreateBuffer(&VBDesc, &VBInitialData, &m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	return S_OK;
}

CMesh* CMesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, MODEL_TYPE eType, const aiMesh* pAIMesh, const vector<CBone*>& Bones, _fmatrix PreTransformMatrix)
{
	CMesh* pInstance = new CMesh(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype_Assimp(eType, pAIMesh, Bones, PreTransformMatrix)))
	{
		MSG_BOX(TEXT("Failed Created : CMesh"));
		Safe_Release(pInstance);
	}
	return pInstance;
}


CMesh* CMesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, MODEL_TYPE eType, ifstream& File, const vector<CBone*>& Bones, _fmatrix PreTransformMatrix, MODEL_BOUNDING& ModelBounding)
{
	CMesh* pInstance = new CMesh(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype_Binary(eType, File, Bones, PreTransformMatrix, ModelBounding)))
	{
		MSG_BOX(TEXT("Failed Created : CMesh"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CMesh::Clone(void* pArg)
{
	CMesh* pInstance = new CMesh(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CMesh"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMesh::Free()
{
	__super::Free();

}
