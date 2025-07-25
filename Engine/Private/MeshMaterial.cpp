#include "EnginePch.h"
#include "MeshMaterial.h"
#include "Shader.h"

CMeshMaterial::CMeshMaterial(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: m_pDevice { pDevice }
	, m_pDeviceContext { pDeviceContext }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
}

HRESULT CMeshMaterial::Initialize_Assimp(const _char* pModelFilePath, const aiMaterial* pAIMaterial)
{
	for (_uint i = 1; i < AI_TEXTURE_TYPE_MAX; i++)
	{
		_uint iNumTextures = pAIMaterial->GetTextureCount(static_cast<aiTextureType>(i));

		for (_uint j = 0; j < iNumTextures; j++)
		{
			_char szDrive[MAX_PATH] = {};
			_char szDir[MAX_PATH] = {};

			_splitpath_s(pModelFilePath, szDrive, MAX_PATH, szDir, MAX_PATH, nullptr, 0, nullptr, 0);

			aiString strTexturePath;

			if (FAILED(pAIMaterial->GetTexture(static_cast<aiTextureType>(i), j, &strTexturePath)))
				return E_FAIL;
		
			_char szFileName[MAX_PATH] = {};
			_char szExt[MAX_PATH] = {};

			_splitpath_s(strTexturePath.data, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);
			

			_char szFullPath[MAX_PATH] = {};
			sprintf_s(szFullPath, "%s%s%s%s%s", szDrive, szDir, "Textures/", szFileName, szExt);
			
			_tchar szTemp[MAX_PATH] = {};

			MultiByteToWideChar(CP_UTF8, 0, szFullPath, static_cast<_int>(strlen(szFullPath)), szTemp, MAX_PATH);

			ID3D11ShaderResourceView* pSRV = { nullptr };

			HRESULT     hr = {};

			if (false == strcmp(szExt, ".dds"))
			{
				hr = CreateDDSTextureFromFile(m_pDevice, szTemp, nullptr, &pSRV);
			}
			else if(false == strcmp(szExt, ".tga"))
				return E_FAIL;
			else
			{
				hr = CreateWICTextureFromFile(m_pDevice, szTemp, nullptr, &pSRV);
			}

			if (FAILED(hr))
				continue;

			m_SRVs[i].push_back(pSRV);
		}
	}

	return S_OK;
}

HRESULT CMeshMaterial::Initialize_Json(const _char* pModelFilePath, Json& Data)
{
	_uint iIndex = {1};
	for(auto& TextureType : Data)
	{
		_uint iNumTextures = TextureType["NumTextures"];

		if (iNumTextures <= 0)
		{
			iIndex++;
			continue;
		}

		for (auto& TexturePath : TextureType["Textures"])
		{
			string TextureFileName = TexturePath["FilePath"];

			_char szFileName[MAX_PATH] = {};
			strcpy_s(szFileName, TextureFileName.c_str());

			_char szExt[MAX_PATH] = {};
			_splitpath_s(szFileName, nullptr, 0, nullptr, 0, nullptr, 0, szExt, MAX_PATH);

			_char szFullDrivePath[MAX_PATH] = {};
			
			strcpy_s(szFullDrivePath, pModelFilePath);

			_char szDrive[MAX_PATH] = {};
			_char szDir[MAX_PATH] = {};
			_splitpath_s(szFullDrivePath, szDrive, MAX_PATH, szDir, MAX_PATH, nullptr, 0, nullptr, 0);

			_char szFullFilePath[MAX_PATH] = {};
			
			sprintf_s(szFullFilePath, "%s%s%s%s", szDrive, szDir, "Textures/", szFileName);

			_tchar szWideFullFilePath[MAX_PATH] = {};

			MultiByteToWideChar(CP_UTF8, 0, szFullFilePath, -1, szWideFullFilePath, MAX_PATH);

			ID3D11ShaderResourceView* pSRV = { nullptr };

			HRESULT     hr = {};

			if (false == strcmp(szExt, ".dds"))
			{
				hr = CreateDDSTextureFromFile(m_pDevice, szWideFullFilePath, nullptr, &pSRV);
			}
			else if (false == strcmp(szExt, ".tga"))
				return E_FAIL;
			else
			{
				hr = CreateWICTextureFromFile(m_pDevice, szWideFullFilePath, nullptr, &pSRV);
			}

			if (FAILED(hr))
				continue;

			m_SRVs[iIndex].push_back(pSRV);
		}
		iIndex++;
	}

	return S_OK;
}

HRESULT CMeshMaterial::Initialize_Binary(const _char* pModelFilePath, ifstream& File)
{
	for (_uint i = 1; i < AI_TEXTURE_TYPE_MAX; i++)
	{
		_uint iNumTextures = {};
		File.read(reinterpret_cast<_char*>(&iNumTextures), sizeof(_uint));

		for (_uint j = 0; j < iNumTextures; j++)
		{
			size_t iFileNameLength = {};
			File.read(reinterpret_cast<_char*>(&iFileNameLength), sizeof(size_t));

			_char TextureFileName[MAX_PATH] = {};
			File.read(TextureFileName, sizeof(_char) * iFileNameLength);

			_char szExt[MAX_PATH] = {};
			_splitpath_s(TextureFileName, nullptr, 0, nullptr, 0, nullptr, 0, szExt, MAX_PATH);

			const _char* pCutPoint = strstr(pModelFilePath, "Models");
			size_t PostLength = strlen(pModelFilePath)- strlen(pCutPoint);

			_char szDrivePath[MAX_PATH] = {};

			strncpy_s(szDrivePath, pModelFilePath, PostLength);

			_char szFullFilePath[MAX_PATH] = {};

			sprintf_s(szFullFilePath, "%s%s%s", szDrivePath, "Models/Textures/", TextureFileName);

			_tchar szWideFullFilePath[MAX_PATH] = {};

			MultiByteToWideChar(CP_UTF8, 0, szFullFilePath, static_cast<_int>(strlen(szFullFilePath)), szWideFullFilePath, MAX_PATH);

			ID3D11ShaderResourceView* pSRV = { nullptr };

			HRESULT     hr = {};

			if (false == strcmp(szExt, ".dds"))
			{
				hr = CreateDDSTextureFromFile(m_pDevice, szWideFullFilePath, nullptr, &pSRV);
			}
			else if (false == strcmp(szExt, ".tga"))
				return E_FAIL;
			else
			{
				hr = CreateWICTextureFromFile(m_pDevice, szWideFullFilePath, nullptr, &pSRV);
			}

			if (FAILED(hr))
				continue;

			m_SRVs[i].push_back(pSRV);
		}
	}

	return S_OK;
}


_bool CMeshMaterial::Bind_Material(CShader* pShader, const _char* pConstantName, _uint iSRVIndex, _uint iTextureType)
{
	if (m_SRVs[iSRVIndex].size() <= 0)
	{
		return false;
	}

	pShader->Bind_SPV(pConstantName, m_SRVs[iSRVIndex][iTextureType]);
	
	return true;
}

CMeshMaterial* CMeshMaterial::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const _char* pModelFilePath, const aiMaterial* pAIMaterial)
{
	CMeshMaterial* pInstance = new CMeshMaterial(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Assimp(pModelFilePath, pAIMaterial)))
	{
		MSG_BOX(TEXT("Failed to Created : CMeshMaterial"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CMeshMaterial* CMeshMaterial::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const _char* pModelFilePath, Json& Data)
{
	CMeshMaterial* pInstance = new CMeshMaterial(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Json(pModelFilePath, Data)))
	{
		MSG_BOX(TEXT("Failed to Created : CMeshMaterial"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CMeshMaterial* CMeshMaterial::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const _char* pModelFilePath, ifstream& File)
{
	CMeshMaterial* pInstance = new CMeshMaterial(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Binary(pModelFilePath, File)))
	{
		MSG_BOX(TEXT("Failed to Created : CMeshMaterial"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMeshMaterial::Free()
{
	__super::Free();

	Safe_Release(m_pDevice);
	Safe_Release(m_pDeviceContext);

	for (auto& SRVs : m_SRVs)
	{
		for (auto& pSRV : SRVs)
			Safe_Release(pSRV);
		SRVs.clear();
	}
}
