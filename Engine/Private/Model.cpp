#include "EnginePch.h"
#include "Model.h"
#include "Mesh.h"
#include "MeshMaterial.h"
#include "Shader.h"

CModel::CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CComponent{ pDevice ,pDeviceContext }
{
}

CModel::CModel(const CModel& Prototype)
	: CComponent{ Prototype }
    , m_iNumMeshes { Prototype.m_iNumMeshes }
    , m_Meshes {Prototype.m_Meshes }
    , m_iNumMaterials { Prototype.m_iNumMaterials }
    , m_Materials { Prototype.m_Materials }
    , m_pAIScene { Prototype.m_pAIScene }
    , m_PreTransformMatrix { Prototype.m_PreTransformMatrix}
{
    for (auto& pMesh : m_Meshes)
        Safe_AddRef(pMesh);

    for (auto& pMaterials : m_Materials)
        Safe_AddRef(pMaterials);
}


HRESULT CModel::Initialize_Prototype(MODELTYPE eModelType, const _char* pModelFilePath, _fmatrix PreTransformMatrix)
{
    m_eModelType = eModelType;

    XMStoreFloat4x4(&m_PreTransformMatrix, PreTransformMatrix);

    _char szExt[MAX_PATH] = {};
    _splitpath_s(pModelFilePath, nullptr, 0, nullptr, 0, nullptr, 0, szExt, MAX_PATH);

    if (false == strcmp(szExt, ".fbx"))
    {
        _uint iFlag = { aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast };

        if (MODELTYPE::NONANIM == m_eModelType)
            iFlag |= aiProcess_PreTransformVertices;

        m_pAIScene = m_Importer.ReadFile(pModelFilePath, iFlag);
        if (nullptr == m_pAIScene)
            return E_FAIL;

        if (FAILED(Ready_Meshes(PreTransformMatrix)))
            return E_FAIL;

        if (FAILED(Ready_Materials(pModelFilePath)))
            return E_FAIL;

    }
    else if (false == strcmp(szExt, ".json"))
    {
        ifstream File(pModelFilePath);
        if (!File)
        {
            MSG_BOX(TEXT("Failed File Open"));
            return E_FAIL;
        }

        Json Data;
        File >> Data;

        if (FAILED(Ready_Meshes(Data, PreTransformMatrix)))
            return E_FAIL;

        if (FAILED(Ready_Materials(Data, pModelFilePath)))
            return E_FAIL;

        File.close();
    }
    else 
        return E_FAIL;

	return S_OK;
}

HRESULT CModel::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CModel::Render(class CShader* pShader)
{
    for (auto& pMesh : m_Meshes)
    {
        Bind_Shader_Material(pShader, "g_Texture", pMesh->Get_MaterialIndex(), aiTextureType_DIFFUSE, 0);
        pShader->Begin(0);
        pMesh->Bind_Resources();
        pMesh->Render();
    }

    return S_OK;
}

void CModel::Bind_Shader_Material(CShader* pShader, const _char* pConstantName, _uint iMaterialIndex, _uint iSRVIndex, _uint iTextureType)
{
    if (iMaterialIndex >= m_iNumMaterials)
        return;

    m_Materials[iMaterialIndex]->Bind_Material(pShader, pConstantName, iSRVIndex, iTextureType);
}

HRESULT CModel::Save_Json(const _wstring& strJsonPath)
{
    std::ofstream File(strJsonPath);
    if (!File.is_open())
    {
        MSG_BOX(TEXT("Failed File Open"));
        return E_FAIL;
    }

    Json Array;
    Array["Model"]["Meshes"] = Json::array();
    Array["Model"]["Materials"] = Json::array();

    m_iNumMeshes = m_pAIScene->mNumMeshes;
    Array["Model"]["NumMeshes"] = m_iNumMeshes;
    if (FAILED(MeshesToJson(File, Array["Model"]["Meshes"])))
        return E_FAIL;

    m_iNumMaterials = m_pAIScene->mNumMaterials;
    Array["Model"]["NumMaterials"] = m_iNumMaterials;
    if (FAILED(MaterialToJson(File, Array["Model"]["Materials"])))
        return E_FAIL;

    std::string jsonStr = Array.dump(4);

    if (jsonStr.empty())
    {
        MSG_BOX(TEXT("JSON string is empty, nothing to write."));
        return E_FAIL;
    }

    if(File.fail())
    {
        MSG_BOX(TEXT("File Wrong"));
        return E_FAIL;
    }
    File << Array.dump(4);

    File.close();

    return S_OK;
}

HRESULT CModel::MeshesToJson(ofstream& File, Json& Array)
{
    Json Meshes;

    for (_uint i = 0; i < m_iNumMeshes; i++)
    {
        aiMesh* pAIMesh = m_pAIScene->mMeshes[i];
        if (nullptr == pAIMesh)
            return E_FAIL;

        Json MeshInfo;

        MeshInfo["MaterialIndex"] = pAIMesh->mMaterialIndex;
        MeshInfo["NumVertices"] = pAIMesh->mNumVertices;
        MeshInfo["NumFaces"] = pAIMesh->mNumFaces;

        Json Vertices = Json::array();

        for (_uint i = 0; i < pAIMesh->mNumVertices; i++)
        {
            Json Vertex = Json{
            {"Position",    {pAIMesh->mVertices[i].x, pAIMesh->mVertices[i].y, pAIMesh->mVertices[i].z}},
            {"Normal",      {pAIMesh->mNormals[i].x, pAIMesh->mNormals[i].y, pAIMesh->mNormals[i].z}},
            {"Tangent",     {pAIMesh->mTangents[i].x, pAIMesh->mTangents[i].y, pAIMesh->mTangents[i].z}},
            {"Binormal",    {pAIMesh->mBitangents[i].x, pAIMesh->mBitangents[i].y, pAIMesh->mBitangents[i].z}},
            {"Texcoord",    {pAIMesh->mTextureCoords[0][i].x, pAIMesh->mTextureCoords[0][i].y}}
            };

            Vertices.push_back(Vertex);
        }

        MeshInfo["Vertices"] = Vertices;

        Json Indices = Json::array();

        for (_uint i = 0; i < pAIMesh->mNumFaces; i++)
        {
            aiFace AIFace = pAIMesh->mFaces[i];

            Json Index;
            Index["Index"] = Json{ AIFace.mIndices[0], AIFace.mIndices[1], AIFace.mIndices[2] };

            Indices.push_back(Index);
        }

        MeshInfo["Indices"] = Indices;

        Meshes.push_back(MeshInfo);
    }

    Array = Meshes;

    return S_OK;
}

HRESULT CModel::MaterialToJson(ofstream& File, Json& Array)
{
    Json Materials;

    for (_uint i = 0; i < m_iNumMaterials; i++)
    {
        aiMaterial* pAIMaterial = m_pAIScene->mMaterials[i];
        if (nullptr == pAIMaterial)
            return E_FAIL;

        Json MaterialArray = Json::array();

        for (_uint j = 1; j < AI_TEXTURE_TYPE_MAX; j++)
        {   
            _uint iNumTextures = pAIMaterial->GetTextureCount(static_cast<aiTextureType>(j));

            Json TextureInfo;

            TextureInfo["NumTextures"] = iNumTextures;
            
            Json Textures = Json::array();

            for (_uint k = 0; k < iNumTextures; k++)
            {
                aiString strTexturePath;

                if (FAILED(pAIMaterial->GetTexture(static_cast<aiTextureType>(j), k, &strTexturePath)))
                    return E_FAIL;

                _char szFileName[MAX_PATH] = {};
                _char szExt[MAX_PATH] = {};

                _splitpath_s(strTexturePath.data, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);
                strcat_s(szFileName, MAX_PATH, szExt);
                string strFileFullName = szFileName;

                Json Texture;

                Texture["FilePath"] = strFileFullName;

                Textures.push_back(Texture);

            }

            TextureInfo["Textures"] = Textures;

            MaterialArray.push_back(TextureInfo);
        }

        Materials.push_back(MaterialArray);
    }

    Array = Materials;

    return S_OK;
}

HRESULT CModel::Ready_Meshes(_fmatrix PreTransformMatrix)
{
    m_iNumMeshes = m_pAIScene->mNumMeshes;

    for (_uint i = 0; i < m_iNumMeshes; i++)
    {
        CMesh* pMesh = CMesh::Create(m_pDevice, m_pDeviceContext, m_pAIScene->mMeshes[i], PreTransformMatrix);
        if (nullptr == pMesh)
            return E_FAIL;

        m_Meshes.push_back(pMesh);
    }

    return S_OK;
}

HRESULT CModel::Ready_Meshes(Json& Data, _fmatrix PreTransformMatrix)
{
    m_iNumMeshes = Data["Model"]["NumMeshes"].get<_uint>();

    for (auto& MeshData : Data["Model"]["Meshes"])
    {
        CMesh* pMesh = CMesh::Create(m_pDevice, m_pDeviceContext, MeshData, PreTransformMatrix);
        if (nullptr == pMesh)
            return E_FAIL;

        m_Meshes.push_back(pMesh);
    }

    return S_OK;
}

HRESULT CModel::Ready_Materials(const _char* pModelFilePath)
{
    m_iNumMaterials = m_pAIScene->mNumMaterials;
    
    for (_uint i = 0; i < m_iNumMaterials; i++)
    {
        CMeshMaterial* pMeshMaterial = CMeshMaterial::Create(m_pDevice, m_pDeviceContext, pModelFilePath, m_pAIScene->mMaterials[i]);
        if (nullptr == pMeshMaterial)
            return E_FAIL;

        m_Materials.push_back(pMeshMaterial);
    }
    return S_OK;
}

HRESULT CModel::Ready_Materials(Json& Data, const _char* pModelFilePath)
{
    m_iNumMaterials = Data["Model"]["NumMaterials"].get<_uint>();
    
    for (auto& MaterialData : Data["Model"]["Materials"])
    {
        CMeshMaterial* pMeshMaterial = CMeshMaterial::Create(m_pDevice, m_pDeviceContext, pModelFilePath, MaterialData);
        if (nullptr == pMeshMaterial)
            return E_FAIL;

        m_Materials.push_back(pMeshMaterial);
    }
    return S_OK;
}

CModel* CModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, MODELTYPE eModelType, const _char* pModelFilePath, _fmatrix PreTransformMatrix)
{
    CModel* pInstance = new CModel(pDevice, pDeviceContext);

    if (FAILED(pInstance->Initialize_Prototype(eModelType, pModelFilePath, PreTransformMatrix)))
    {
        MSG_BOX(TEXT("Failed to Created : CModel"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CComponent* CModel::Clone(void* pArg)
{
    CModel* pInstance = new CModel(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CModel"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CModel::Free()
{
    __super::Free();

    for (auto& pMesh : m_Meshes)
        Safe_Release(pMesh);
    m_Meshes.clear();

    for (auto& pMaterials : m_Materials)
        Safe_Release(pMaterials);
    m_Materials.clear();

    m_Importer.FreeScene();
}