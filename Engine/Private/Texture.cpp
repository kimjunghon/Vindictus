#include "EnginePch.h"
#include "Texture.h"
#include "Shader.h"

CTexture::CTexture(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CComponent { pDevice, pDeviceContext }
{
}

CTexture::CTexture(const CTexture& Prototype)
	: CComponent		{ Prototype}
	, m_iNumTextures	{ Prototype.m_iNumTextures }
	, m_SPVs			{ Prototype.m_SPVs }
{
	for (auto& pSPV : m_SPVs)
		Safe_AddRef(pSPV);
}

HRESULT CTexture::Initialize_Prototype(const _tchar* pTextureFilePath, _uint iNumTextures)
{
	m_iNumTextures = iNumTextures;

	_tchar szExt[MAX_PATH] = {};

	_wsplitpath_s(pTextureFilePath, nullptr, 0, nullptr, 0, nullptr, 0, szExt, MAX_PATH);

	for (_uint i = 0; i < iNumTextures; i++)
	{
		_tchar szFullPath[MAX_PATH] = {};

		wsprintf(szFullPath, pTextureFilePath, i);

		ID3D11ShaderResourceView* pSPV = { nullptr };

		
		if (false == lstrcmp(szExt, TEXT(".dds")))
		{
			if (FAILED(CreateDDSTextureFromFile(m_pDevice, szFullPath, nullptr, &pSPV)))
				return E_FAIL;
		}
		else if (false == lstrcmp(szExt, TEXT(".tga")))
		{
			MSG_BOX(TEXT("Ext is Tga"));
			return E_FAIL;
		}
		else
			if (FAILED(CreateWICTextureFromFile(m_pDevice, szFullPath, nullptr, &pSPV)))
				return E_FAIL;

		m_SPVs.push_back(pSPV);
	}


	return S_OK;
}

HRESULT CTexture::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CTexture::Bind_Shader_Texture(CShader* pShader, const _char* pConstantName, _uint iTextureIndex)
{
	return pShader->Bind_SPV(pConstantName, m_SPVs[iTextureIndex]);
}

HRESULT CTexture::Copy_Resource(_uint iTextureIndex, ID3D11Texture2D* pSourTexture)
{
	if (iTextureIndex >= m_iNumTextures)
		return E_FAIL;
	ID3D11Resource* pDestResource = {};
	
	m_SPVs[iTextureIndex]->GetResource(&pDestResource);

	D3D11_TEXTURE2D_DESC srcDesc;
	((ID3D11Texture2D*)pDestResource)->GetDesc(&srcDesc);

	printf("Src: %dx%d Format=%d Mip=%d Array=%d Usage=%d Bind=0x%X Sample=%d\n",
		srcDesc.Width, srcDesc.Height, srcDesc.Format, srcDesc.MipLevels, srcDesc.ArraySize,
		srcDesc.Usage, srcDesc.BindFlags, srcDesc.SampleDesc.Count);

	m_pDeviceContext->CopyResource(pSourTexture, pDestResource);

	Safe_Release(pDestResource);

	return S_OK;
}

CTexture* CTexture::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const _tchar* pTextureFilePath, _uint iNumTextures)
{
	CTexture* pInstance = new CTexture(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype(pTextureFilePath, iNumTextures)))
	{
		MSG_BOX(TEXT("Failed Created : CTexture"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CTexture::Clone(void* pArg)
{
	CTexture* pInstance = new CTexture(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CTexture"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTexture::Free()
{
	__super::Free();

	for (auto& pSRV : m_SPVs)
		Safe_Release(pSRV);

	m_SPVs.clear();
}
