#include "EnginePch.h"
#include "Model.h"
#include "Mesh.h"
#include "MeshMaterial.h"
#include "Shader.h"
#include "Bone.h"
#include "Animation.h"

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
    , m_iNumAnimation { Prototype.m_iNumAnimation }
    , m_iRootBoneIndex{ Prototype.m_iRootBoneIndex }
    , m_eModelType { Prototype.m_eModelType }
    , m_Bounding { Prototype.m_Bounding }
    , m_OffsetMatrix { Prototype.m_OffsetMatrix }
{
    for (auto& pPrototypeBone : Prototype.m_Bones)
        m_Bones.push_back(pPrototypeBone->Clone());

    for (auto& pMesh : m_Meshes)
        Safe_AddRef(pMesh);

    for (auto& pMaterials : m_Materials)
        Safe_AddRef(pMaterials);

    for (auto& Pair : Prototype.m_Animations)
        m_Animations.emplace(Pair.first, Pair.second->Clone());
}

_float CModel::Get_CurrentAnimSpeed()
{
    if (nullptr == m_pCurrentAnimation)
        return 0.f;

    _float fSpeed = m_pCurrentAnimation->Get_AnimTickPerSecond() * m_CurrentAnimData.fAnimSpeed;

    return fSpeed;
}

HRESULT CModel::Initialize_Prototype(MODEL_TYPE eModelType, const _char* pModelFilePath, _fmatrix PreTransformMatrix)
{
    XMStoreFloat4x4(&m_PreTransformMatrix, PreTransformMatrix);

    _char szExt[MAX_PATH] = {};
    _splitpath_s(pModelFilePath, nullptr, 0, nullptr, 0, nullptr, 0, szExt, MAX_PATH);

    if (false == strcmp(szExt, ".fbx"))
    {
        m_eModelType = eModelType;

        _uint iFlag = { aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast };

        if (MODEL_TYPE::NONANIM == m_eModelType)
            iFlag |= aiProcess_PreTransformVertices;

        m_pAIScene = m_Importer.ReadFile(pModelFilePath, iFlag);
        if (nullptr == m_pAIScene)
            return E_FAIL;

        if (FAILED(Ready_Bones(m_pAIScene->mRootNode, -1)))
            return E_FAIL;

        if (FAILED(Ready_Meshes()))
            return E_FAIL;

        if (FAILED(Ready_Materials(pModelFilePath)))
            return E_FAIL;
        
        if (FAILED(Ready_Animation()))
            return E_FAIL;


    }
    else if (false == strcmp(szExt, ".dat"))
    {
        ifstream File(pModelFilePath, ios::binary);
        if (!File)
        {
            MSG_BOX(TEXT("Failed File Open"));
            return E_FAIL;
        }

        MODEL_INFO tModelInfo = {};

        File.read(reinterpret_cast<_char*>(&tModelInfo), sizeof(MODEL_INFO));

        m_eModelType = static_cast<MODEL_TYPE>(tModelInfo.iModelType);
        m_iNumMeshes = tModelInfo.iNumMeshes;
        m_iNumMaterials = tModelInfo.iNumMaterials;

        
        m_Bounding.vMinPosition = _float3(FLT_MAX, FLT_MAX, FLT_MAX);
        m_Bounding.vMaxPosition = _float3(FLT_MAX * -1.f, FLT_MAX * -1.f, FLT_MAX * -1.f);

        if (FAILED(Ready_Bones(File, -1)))
            return E_FAIL;

        if (FAILED(Ready_Meshes(File)))
            return E_FAIL;

        if (FAILED(Ready_Materials(File, pModelFilePath)))
            return E_FAIL;

        if (FAILED(Ready_Animation(File)))
            return E_FAIL;

        File.close();

        _float fCenterX = (m_Bounding.vMinPosition.x + m_Bounding.vMaxPosition.x) * 0.5f;
        _float fCenterY = (m_Bounding.vMinPosition.y + m_Bounding.vMaxPosition.y) * 0.5f;
        _float fCenterZ = (m_Bounding.vMinPosition.z + m_Bounding.vMaxPosition.z) * 0.5f;
        
        XMStoreFloat4x4(&m_OffsetMatrix, XMMatrixTranslation(fCenterX * -1.f, fCenterY * -1.f, fCenterZ * -1.f));
    }
    else
        return E_FAIL;


	return S_OK;
}

HRESULT CModel::Initialize(void* pArg)
{
    
	return S_OK;
}

HRESULT CModel::Render(_uint iMeshIndex)
{
    if (FAILED(m_Meshes[iMeshIndex]->Bind_Resources()))
        return E_FAIL;

    if (FAILED(m_Meshes[iMeshIndex]->Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CModel::Bind_Shader_Material(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex, _uint iSRVIndex, _uint iTextureType, _bool* hasSPV)
{
    if (iMeshIndex >= m_iNumMeshes)
        return E_FAIL;
    
    _uint iMaterialIndex = m_Meshes[iMeshIndex]->Get_MaterialIndex();

    if (iMaterialIndex >= m_iNumMaterials)
        return E_FAIL;

    if(hasSPV)
        *hasSPV = m_Materials[iMaterialIndex]->Bind_Material(pShader, pConstantName, iSRVIndex, iTextureType);
    else
        m_Materials[iMaterialIndex]->Bind_Material(pShader, pConstantName, iSRVIndex, iTextureType);

    return S_OK;
}

HRESULT CModel::Bind_BoneMatrices(CShader* pShader, const _char* pConstantName, _uint iMeshIndex)
{
    if (iMeshIndex >= m_iNumMeshes)
        return E_FAIL;

    return m_Meshes[iMeshIndex]->Bind_BoneMatrices(pShader, pConstantName, m_Bones);
}

HRESULT CModel::Bind_BoneMatrices(CShader* pShader, const _char* pConstantName, _uint iMeshIndex, vector<CBone*>& Bones)
{
    if (iMeshIndex >= m_iNumMeshes)
        return E_FAIL;

    return m_Meshes[iMeshIndex]->Bind_BoneMatrices(pShader, pConstantName, Bones);
}

HRESULT CModel::Bind_PoseBoneMatrices(CShader* pShader, const _char* pConstantName, _uint iMeshIndex)
{
    if (iMeshIndex >= m_iNumMeshes)
        return E_FAIL;

    return m_Meshes[iMeshIndex]->Bind_PoseBoneMatrices(pShader, pConstantName, m_Bones);
}

HRESULT CModel::Update_PoseCombinedTransformationMatrix()
{
    for (_uint i = 0; i < m_Bones.size(); i++)
    {
        m_Bones[i]->Update_PoseCombinedTransformationMatrix(m_PreTransformMatrix, m_Bones, m_OffsetMatrix);
    }
    return S_OK;
}

HRESULT CModel::Set_Animation(const ANIM_DATA& AnimData)
{
    if (m_CurrentAnimData.strAnimKey == AnimData.strAnimKey && false == m_IsFinished)
        return E_FAIL;

    CAnimation* pAnimation = Find_Animation(AnimData.strAnimKey);
    if (nullptr == pAnimation)
        return E_FAIL;

    if(nullptr == m_pCurrentAnimation)
        m_IsAnimChange = false;
    else
        m_IsAnimChange = true;

    m_pCurrentAnimation = pAnimation;

    m_pCurrentAnimation->Enter(m_IsAnimChange);

    m_IsAnimStart = true;

    m_CurrentAnimData = AnimData;

    return S_OK;
}

HRESULT CModel::Forcing_Set_Animation(const ANIM_DATA& AnimData)
{
    CAnimation* pAnimation = Find_Animation(AnimData.strAnimKey);
    if (nullptr == pAnimation)
        return E_FAIL;

    if (nullptr == m_pCurrentAnimation)
        m_IsAnimChange = false;
    else
        m_IsAnimChange = true;

    m_pCurrentAnimation = pAnimation;

    m_vPrevRootPosition = XMVectorSet(0.f, 0.f, 0.f, 1.f);

    m_vPrevRootRotation = XMQuaternionIdentity();

    m_pCurrentAnimation->Enter(m_IsAnimChange);

    m_IsAnimStart = true;

    m_CurrentAnimData = AnimData;

    return S_OK;
}

_bool CModel::Play_Animation(_float fTimeDelta)
{
    if(m_pCurrentAnimation)
    {
        m_IsFinished = false;

        if (m_IsAnimChange)
            m_pCurrentAnimation->Update_TransformationMatricesLerp(m_Bones, &m_IsAnimChange, fTimeDelta * m_CurrentAnimData.fAnimSpeed);
        else
        {
            m_pCurrentAnimation->Update_TransformationMatrices(m_Bones, m_CurrentAnimData.IsLoop, &m_IsFinished, fTimeDelta * m_CurrentAnimData.fAnimSpeed, &m_IsAnimStart);
        }
    }

    for(_uint i =0; i< m_Bones.size(); i++)
    {
        m_Bones[i]->Update_CombinedTransformationMatrix(m_PreTransformMatrix, m_Bones);

        if (m_iRootBoneIndex != -1 && i == m_iRootBoneIndex)
        {
            RootMotion();
        }
    }

    return m_IsFinished;
}

void CModel::Bind_ParentBone(vector<CBone*>& ParentBones)
{
    for (auto& pBone : m_Bones)
    {
        pBone->Update_CombinedTransformationMatrix(m_PreTransformMatrix, ParentBones);
    }

}

HRESULT CModel::Save_Binary(const _wstring& strSaveFilePath)
{
    std::ofstream File(strSaveFilePath, ios::binary);
    if (!File.is_open())
    {
        MSG_BOX(TEXT("Failed File Open"));
        return E_FAIL;
    }

    m_iNumMeshes = m_pAIScene->mNumMeshes;
    m_iNumMaterials = m_pAIScene->mNumMaterials;
    
    File.write(reinterpret_cast<_char*>(&m_eModelType), sizeof(_uint));
    File.write(reinterpret_cast<_char*>(&m_iNumMeshes), sizeof(_uint));
    File.write(reinterpret_cast<_char*>(&m_iNumMaterials), sizeof(_uint));

    if (FAILED(BonesToBinary(File, m_pAIScene->mRootNode)))
        return E_FAIL;

    if (FAILED(MeshesToBinary(File)))
        return E_FAIL;

    if (FAILED(MaterialToBinary(File)))
        return E_FAIL;

    if (FAILED(AnimationToBinary(File)))
        return E_FAIL;

    File.close();

    return S_OK;
}

HRESULT CModel::BonesToBinary(ofstream& File, const aiNode* pAINode)
{
    size_t iLength = strlen(pAINode->mName.data);
    _uint iNumChildren = pAINode->mNumChildren;
    
    _float4x4 TransformationMatrix = {  };

    memcpy(&TransformationMatrix, &pAINode->mTransformation, sizeof(_float4x4));
    XMStoreFloat4x4(&TransformationMatrix, XMMatrixTranspose(XMLoadFloat4x4(&TransformationMatrix)));

    File.write(reinterpret_cast<_char*>(&iLength), sizeof(size_t));
    File.write(pAINode->mName.data, sizeof(_char) * iLength);
    File.write(reinterpret_cast<_char*>(&TransformationMatrix), sizeof(_float4x4));
    File.write(reinterpret_cast<_char*>(&iNumChildren), sizeof(_uint));

    for (_uint i = 0; i < iNumChildren; i++)
        BonesToBinary(File, pAINode->mChildren[i]);

    return S_OK;
}

HRESULT CModel::MeshesToBinary(ofstream& File)
{
    for (_uint i = 0; i < m_iNumMeshes; i++)
    {
        aiMesh* pAIMesh = m_pAIScene->mMeshes[i];
        if (nullptr == pAIMesh)
            return E_FAIL;

        size_t  iMeshNameLength = strlen(pAIMesh->mName.data);

        File.write(reinterpret_cast<_char*>(&pAIMesh->mMaterialIndex), sizeof(_uint));
        File.write(reinterpret_cast<_char*>(&pAIMesh->mNumVertices), sizeof(_uint));
        File.write(reinterpret_cast<_char*>(&pAIMesh->mNumFaces), sizeof(_uint));

        File.write(reinterpret_cast<_char*>(&iMeshNameLength), sizeof(size_t));
        File.write(pAIMesh->mName.data, sizeof(_char) * iMeshNameLength);

        if(m_eModelType == MODEL_TYPE::NONANIM)
        {
            VTXMESH* pVertices = new VTXMESH[pAIMesh->mNumVertices];

            for (_uint i = 0; i < pAIMesh->mNumVertices; i++)
            {
                memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
                memcpy(&pVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
                memcpy(&pVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));
                memcpy(&pVertices[i].vBinormal, &pAIMesh->mBitangents[i], sizeof(_float3));
                memcpy(&pVertices[i].vTexcoord, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));
            }

//            for (_uint i = 0; i < pAIMesh->mNumVertices; i++)
//                File.write(reinterpret_cast<_char*>(&pVertices[i]), sizeof(VTXMESH));

            File.write(reinterpret_cast<_char*>(pVertices), sizeof(VTXMESH) * pAIMesh->mNumVertices);

            Safe_Delete_Array(pVertices);
        }
        else if(m_eModelType == MODEL_TYPE::ANIM)
        {   
            VTXANIMMESH* pVertices = new VTXANIMMESH[pAIMesh->mNumVertices];
            ZeroMemory(pVertices, sizeof(VTXANIMMESH) * pAIMesh->mNumVertices);


            for (_uint i = 0; i < pAIMesh->mNumVertices; i++)
            {
                memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
                memcpy(&pVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
                memcpy(&pVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));
                memcpy(&pVertices[i].vBinormal, &pAIMesh->mBitangents[i], sizeof(_float3));
                memcpy(&pVertices[i].vTexcoord, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));
            }
            
            File.write(reinterpret_cast<_char*>(&pAIMesh->mNumBones), sizeof(_uint));
            
            for (_uint i = 0; i < pAIMesh->mNumBones; i++)
            {
                aiBone* pAIBone = pAIMesh->mBones[i];
            
                for (size_t j = 0; j < pAIBone->mNumWeights; j++)
                {
                    aiVertexWeight	AIVertexWeight = pAIBone->mWeights[j];
            
                    /* i번째 뼈가 영향을 주는 j번째 정점의 정점버퍼상의 인덱스 */
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
            
                _float4x4 OffsetMatrix;
            
                memcpy(&OffsetMatrix, &pAIBone->mOffsetMatrix, sizeof(_float4x4));
                XMStoreFloat4x4(&OffsetMatrix, XMMatrixTranspose(XMLoadFloat4x4(&OffsetMatrix)));

                size_t  iBoneNameLength = strlen(pAIBone->mName.data);
            
                File.write(reinterpret_cast<_char*>(&OffsetMatrix), sizeof(_float4x4));
                File.write(reinterpret_cast<_char*>(&iBoneNameLength), sizeof(size_t));
                File.write(pAIBone->mName.data, sizeof(_char) * iBoneNameLength);
            }

            if (0 == pAIMesh->mNumBones)
            {
                _float4x4 OffsetMatrix;
                XMStoreFloat4x4(&OffsetMatrix, XMMatrixIdentity());

                File.write(reinterpret_cast<_char*>(&OffsetMatrix), sizeof(_float4x4));
            }
            

            File.write(reinterpret_cast<_char*>(pVertices), sizeof(VTXANIMMESH) * pAIMesh->mNumVertices);

            Safe_Delete_Array(pVertices);
        }

        for (_uint i = 0; i < pAIMesh->mNumFaces; i++)
        {
            aiFace AIFace = pAIMesh->mFaces[i];

            File.write(reinterpret_cast<_char*>(&AIFace.mIndices[0]), sizeof(_uint));
            File.write(reinterpret_cast<_char*>(&AIFace.mIndices[1]), sizeof(_uint));
            File.write(reinterpret_cast<_char*>(&AIFace.mIndices[2]), sizeof(_uint));
        }
    }

    return S_OK;
}

HRESULT CModel::MaterialToBinary(ofstream& File)
{
    for (_uint i = 0; i < m_iNumMaterials; i++)
    {
        aiMaterial* pAIMaterial = m_pAIScene->mMaterials[i];
        if (nullptr == pAIMaterial)
            return E_FAIL;

        for (_uint j = 1; j < AI_TEXTURE_TYPE_MAX; j++)
        {
            _uint iNumTextures = pAIMaterial->GetTextureCount(static_cast<aiTextureType>(j));

            File.write(reinterpret_cast<_char*>(&iNumTextures), sizeof(_uint));
            
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
                
                size_t iNameLength = strlen(strFileFullName.c_str());

                File.write(reinterpret_cast<_char*>(&iNameLength), sizeof(size_t));
                File.write(strFileFullName.c_str(), sizeof(_char) * iNameLength);
            }
        }
    }

    return S_OK;
}

HRESULT CModel::AnimationToBinary(ofstream& File)
{
    _uint iNumAnimations = m_pAIScene->mNumAnimations;

    File.write(reinterpret_cast<_char*>(&iNumAnimations), sizeof(_uint));

    for (_uint i = 0; i < iNumAnimations; i++)
    {
        aiAnimation* pAIAnimation = m_pAIScene->mAnimations[i];
        
        size_t iAnimNameLength = {};
        _char szAnimName[MAX_PATH] = {};

        const _char* pFullName = pAIAnimation->mName.data;
        const _char* pCutPoint = strchr(pFullName, '|');

        strcpy_s(szAnimName, pCutPoint + 1);
        iAnimNameLength = strlen(szAnimName);

        _uint iNumChannels = pAIAnimation->mNumChannels;

        _float fDuration = static_cast<_float>(pAIAnimation->mDuration);
        _float fTickPerSecond = static_cast<_float>(pAIAnimation->mTicksPerSecond);

        File.write(reinterpret_cast<_char*>(&iAnimNameLength), sizeof(size_t));
        File.write(szAnimName, sizeof(_char) * iAnimNameLength);

        File.write(reinterpret_cast<_char*>(&fDuration), sizeof(_float));
        File.write(reinterpret_cast<_char*>(&fTickPerSecond), sizeof(_float));

        File.write(reinterpret_cast<_char*>(&iNumChannels), sizeof(_uint));

        for (_uint j = 0; j < iNumChannels; j++)
        {
            aiNodeAnim* pAIChannel = pAIAnimation->mChannels[j];

            _char szName[MAX_PATH] = {};
            size_t iNameLength = {};
            
            strcpy_s(szName, pAIChannel->mNodeName.data);
            iNameLength = strlen(szName);

            File.write(reinterpret_cast<_char*>(&iNameLength), sizeof(size_t));
            File.write(szName, sizeof(_char) * iNameLength);

            _uint iNumKeyFrames = max(max(pAIChannel->mNumScalingKeys, pAIChannel->mNumRotationKeys), pAIChannel->mNumPositionKeys);

            File.write(reinterpret_cast<_char*>(&iNumKeyFrames), sizeof(_uint));

            _float3     vScale{};
            _float4     vRotation{};
            _float3     vTranslation{};

            for (size_t k = 0; k < iNumKeyFrames; k++)
            {
                KEYFRAME            KeyFrame{};

                if (k < pAIChannel->mNumScalingKeys)
                {
                    memcpy(&vScale, &pAIChannel->mScalingKeys[k].mValue, sizeof(_float3));
                    KeyFrame.fTrackPosition = static_cast<_float>(pAIChannel->mScalingKeys[k].mTime);
                }

                if (k < pAIChannel->mNumRotationKeys)
                {
                    vRotation.x = pAIChannel->mRotationKeys[k].mValue.x;
                    vRotation.y = pAIChannel->mRotationKeys[k].mValue.y;
                    vRotation.z = pAIChannel->mRotationKeys[k].mValue.z;
                    vRotation.w = pAIChannel->mRotationKeys[k].mValue.w;

                    KeyFrame.fTrackPosition = static_cast<_float>(pAIChannel->mRotationKeys[k].mTime);
                }

                if (k < pAIChannel->mNumPositionKeys)
                {
                    memcpy(&vTranslation, &pAIChannel->mPositionKeys[k].mValue, sizeof(_float3));
                    KeyFrame.fTrackPosition = static_cast<_float>(pAIChannel->mPositionKeys[k].mTime);
                }

                KeyFrame.vScale = vScale;
                KeyFrame.vRotation = vRotation;
                KeyFrame.vPosition = vTranslation;

                File.write(reinterpret_cast<_char*>(&KeyFrame), sizeof(KEYFRAME));
            }
        }
    }

    return S_OK;
}

_bool CModel::IsAnimationInRangeTrackPosition(_float2 vRangeTrackPosition)
{
    if (nullptr == m_pCurrentAnimation)
        return false;

    return m_pCurrentAnimation->Anim_InRangeOfTrackPositon(vRangeTrackPosition.x, vRangeTrackPosition.y);
}

_bool CModel::IsAnimationPassToTrackPosition(_float fTrackPosition)
{
    return m_pCurrentAnimation->Anim_PassToTrackPosition(fTrackPosition);
}

_bool CModel::CanChangeAnimation()
{
    if (nullptr == m_pCurrentAnimation)
        return true;

    return m_pCurrentAnimation->Anim_InRangeOfRatio(m_CurrentAnimData.vRange.x, m_CurrentAnimData.vRange.y);
}

void CModel::RootMotion()
{
    _vector vScale = {};
    _vector vRotation = {};
    _vector vPosition = {};
 
    XMMatrixDecompose(&vScale, &vRotation, &vPosition, m_Bones[m_iRootBoneIndex]->Get_CombinedTransformationMatrix());
 
    if (m_IsAnimStart)
    {
        m_vPrevRootPosition = vPosition;
        m_vPrevRootRotation = vRotation;
        m_IsAnimStart = false;
    }

    _vector vTempPrevPosition = m_vPrevRootPosition;
    _vector vTempPrevRotation = m_vPrevRootRotation;

    m_vPrevRootPosition = vPosition;
    m_vPrevRootRotation = vRotation;
    
    m_vAnimMovement = XMVectorZero();

    m_vAnimRotation = XMQuaternionIdentity();

    if(m_RootMotionOption.PositionX)
    {
        m_vAnimMovement = XMVectorSetX(m_vAnimMovement, XMVectorGetX(XMVectorSubtract(vPosition, vTempPrevPosition)));
        vPosition = XMVectorSetX(vPosition, 0.f);
    }
    if (m_RootMotionOption.PositionY)
    {
        m_vAnimMovement = XMVectorSetY(m_vAnimMovement, XMVectorGetY(XMVectorSubtract(vPosition, vTempPrevPosition)));
        vPosition = XMVectorSetY(vPosition, 0.f);
    }
    if (m_RootMotionOption.PositionZ)
    {
        m_vAnimMovement = XMVectorSetZ(m_vAnimMovement, XMVectorGetZ(XMVectorSubtract(vPosition, vTempPrevPosition)));
        vPosition = XMVectorSetZ(vPosition, 0.f);
    }
    if (m_RootMotionOption.Rotation)
    {
        m_vAnimRotation = XMQuaternionMultiply(vRotation, XMQuaternionInverse(vTempPrevRotation));
        //m_vAnimRotation = vRotation;

        if(m_RootMotionOption.RotationOnlyZ)
        {
            _matrix RotationMatrix = XMMatrixRotationQuaternion(vRotation);
            _vector vLook = XMVector3Normalize(XMVectorSetY(RotationMatrix.r[2], 0.f));
            _vector vRotationInverse = XMQuaternionRotationMatrix(XMMatrixLookAtLH(XMVectorZero(), vLook, XMVectorSet(0.f, 1.f, 0.f, 0.f)));

            vRotation = XMQuaternionMultiply(vRotation, vRotationInverse);
        }
        else
        {
            vRotation = XMQuaternionIdentity();
        }
    }

    _matrix CombinedTransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);

    m_Bones[m_iRootBoneIndex]->Set_CombinedTransformationMatrix(CombinedTransformationMatrix);
}

HRESULT CModel::Add_AnimNotify(const string& strAnimationTag, _float fTrackPosition, function<void()> Callback)
{
    CAnimation* pAnimation = Find_Animation(strAnimationTag);
    if (nullptr == pAnimation)
        return E_FAIL;

    if (FAILED(pAnimation->Add_Notify(fTrackPosition, Callback)))
        return E_FAIL;

    return S_OK;
}

const _float4x4* CModel::Find_SocketBoneCombinedMatrix(const string& strSocketBoneName)
{
    auto iter = find_if(m_Bones.begin(), m_Bones.end(), [&](CBone* pBone) {
        if (true == pBone->Compare_Name(strSocketBoneName.c_str()))
            return true;
        return false;
        });

    if(iter == m_Bones.end())
        return nullptr;

    return (*iter)->Get_CombinedTransformationMatrixPtr();
}

const _float4x4* CModel::Find_OffsetMatrix(const string& strSocketBoneName)
{
    for (auto& Mesh : m_Meshes)
    {
        const _float4x4* pOffsetMatrix = nullptr;

        pOffsetMatrix = Mesh->Find_OffsetMatrix(m_Bones, strSocketBoneName);

        if (pOffsetMatrix)
            return pOffsetMatrix;
    }

    return nullptr;
}

#ifdef _DEBUG
_bool CModel::Is_Pick(_fvector vLocalPickPosition, _fvector vLocalPickDir, _float& fDist)
{
    if (m_eModelType != MODEL_TYPE::NONANIM)
        return false;


    _bool IsHit = false;
    _float fMin_Dist = FLT_MAX;
    _float fCurrentDist = {};

    for (_uint i = 0; i < m_iNumMeshes; i++)
    {
        if (m_Meshes[i]->Is_Pick(vLocalPickPosition, vLocalPickDir, fCurrentDist))
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

HRESULT CModel::Ready_Bones(ifstream& File, _int iParentIndex)
{
    CBone* pBone = CBone::Create(File, iParentIndex);
    if (nullptr == pBone)
        return E_FAIL;

    m_Bones.push_back(pBone);
    
    if(m_iRootBoneIndex == -1)
    {
        if (pBone->Compare_Name("ValveBiped.Bip01") || pBone->Compare_Name("root") || pBone->Compare_Name("Bip01"))
            m_iRootBoneIndex = static_cast<_uint>(m_Bones.size() - 1);
    }

    _uint iIndex = static_cast<_uint>(m_Bones.size()) - 1;

    _uint iNumChildren = {};
    File.read(reinterpret_cast<_char*>(&iNumChildren), sizeof(_uint));
    
    for (_uint i = 0; i < iNumChildren; i++)
        Ready_Bones(File, iIndex);
    
    return S_OK;
}

HRESULT CModel::Ready_Meshes(ifstream& File)
{
    for (_uint i = 0; i < m_iNumMeshes; i++)
    {
        CMesh* pMesh = CMesh::Create(m_pDevice, m_pDeviceContext, m_eModelType ,File, m_Bones, XMLoadFloat4x4(&m_PreTransformMatrix), m_Bounding);
        if (nullptr == pMesh)
            return E_FAIL;

        m_Meshes.push_back(pMesh);
    }

    return S_OK;
}

HRESULT CModel::Ready_Materials(ifstream& File, const _char* pModelFilePath)
{
    for (_uint i = 0; i < m_iNumMaterials; i++)
    {
        CMeshMaterial* pMeshMaterial = CMeshMaterial::Create(m_pDevice, m_pDeviceContext, pModelFilePath, File);
        if (nullptr == pMeshMaterial)
            return E_FAIL;

        m_Materials.push_back(pMeshMaterial);
    }

    return S_OK;
}

HRESULT CModel::Ready_Animation(ifstream& File)
{
    File.read(reinterpret_cast<_char*>(&m_iNumAnimation), sizeof(_uint));

    
    for (_uint i = 0; i < m_iNumAnimation; i++)
    {
        size_t iAnimNameLength;
        _char szAnimName[MAX_PATH] = {};

        File.read(reinterpret_cast<_char*>(&iAnimNameLength), sizeof(size_t));
        File.read(szAnimName, sizeof(_char) * iAnimNameLength);

        CAnimation* pAnimation = CAnimation::Create(File, m_Bones);
        if (nullptr == pAnimation)
            return E_FAIL;

        m_Animations.emplace(szAnimName, pAnimation);
    }
    return S_OK;
}

CAnimation* CModel::Find_Animation(const string& strAnimationTag)
{
    auto Pair = m_Animations.find(strAnimationTag);

    if (Pair == m_Animations.end())
        return nullptr;

    return Pair->second;
}

HRESULT CModel::Ready_Meshes()
{
    m_iNumMeshes = m_pAIScene->mNumMeshes;

    for (_uint i = 0; i < m_iNumMeshes; i++)
    {
        CMesh* pMesh = CMesh::Create(m_pDevice, m_pDeviceContext, m_eModelType, m_pAIScene->mMeshes[i], m_Bones, XMLoadFloat4x4(&m_PreTransformMatrix));
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

HRESULT CModel::Ready_Bones(const aiNode* pAINode, _int iParentIndex)
{
    CBone* pBone = CBone::Create(pAINode, iParentIndex);
    if (nullptr == pBone)
        return E_FAIL;

    m_Bones.push_back(pBone);

    if (m_iRootBoneIndex == -1)
    {
        if (pBone->Compare_Name("ValveBiped.Bip01"))
            m_iRootBoneIndex = static_cast<_uint>(m_Bones.size() - 1);
    }

    _int iIndex = static_cast<_int>(m_Bones.size()) - 1;

    for (_uint i = 0; i < pAINode->mNumChildren; i++)
        Ready_Bones(pAINode->mChildren[i], iIndex);

    return S_OK;
}

HRESULT CModel::Ready_Animation()
{
    m_iNumAnimation = m_pAIScene->mNumAnimations;

    for (size_t i = 0; i < m_iNumAnimation; i++)
    {
        CAnimation* pAnimation = CAnimation::Create(m_pAIScene->mAnimations[i], m_Bones);
        if (nullptr == pAnimation)
            return E_FAIL;

        m_Animations.emplace(m_pAIScene->mAnimations[i]->mName.data, pAnimation);
    }

    return S_OK;
}

CModel* CModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, MODEL_TYPE eModelType, const _char* pModelFilePath, _fmatrix PreTransformMatrix)
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

    for (auto& pBone : m_Bones)
        Safe_Release(pBone);
    m_Bones.clear();

    for (auto& pMesh : m_Meshes)
        Safe_Release(pMesh);
    m_Meshes.clear();

    for (auto& pMaterials : m_Materials)
        Safe_Release(pMaterials);
    m_Materials.clear();

    for (auto& Pair : m_Animations)
        Safe_Release(Pair.second);
    m_Animations.clear();

    m_Importer.FreeScene();
}