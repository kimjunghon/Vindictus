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

HRESULT CMeshMaterial::Initialize(const _char* pModelFilePath, const aiMaterial* pAIMaterial)
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
			sprintf_s(szFullPath, "%s%s%s%s", szDrive, szDir, szFileName, szExt);
			
			_tchar szTemp[MAX_PATH] = {};

			MultiByteToWideChar(CP_UTF8, 0, szFullPath, -1, szTemp, MAX_PATH);

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

HRESULT CMeshMaterial::Initialize(const _char* pModelFilePath, Json& Data)
{
	//for (_uint i = 1; i < AI_TEXTURE_TYPE_MAX; i++)
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
			
			sprintf_s(szFullFilePath, "%s%s%s", szDrive, szDir,szFileName);

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


void CMeshMaterial::Bind_Material(CShader* pShader, const _char* pConstantName, _uint iSRVIndex, _uint iTextureType)
{
	pShader->Bind_SPV(pConstantName, m_SRVs[iSRVIndex][iTextureType]);
}

CMeshMaterial* CMeshMaterial::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const _char* pModelFilePath, const aiMaterial* pAIMaterial)
{
	CMeshMaterial* pInstance = new CMeshMaterial(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize(pModelFilePath, pAIMaterial)))
	{
		MSG_BOX(TEXT("Failed to Created : CMeshMaterial"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CMeshMaterial* CMeshMaterial::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const _char* pModelFilePath, Json& Data)
{
	CMeshMaterial* pInstance = new CMeshMaterial(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize(pModelFilePath, Data)))
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
