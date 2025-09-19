#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CGameInstance;
class CShader;

class CShadow final : public CBase
{
private:
	CShadow();
	virtual ~CShadow() = default;

public:
	const _float4x4*	Get_Transform_Float4x4(D3DTS eTransformState) const { return &m_Matrices[ENUM_CLASS(eTransformState)]; }


	/*const _float4x4*	Get_Shadow_ViewMatrix() const { return &m_ShadowViewMatrix; }
	const _float*		Get_ShadowLight_Far() const { return &m_fShadowLightFar; }
	HRESULT				Bind_Shadow_ProjMatrices(CShader* pShader, const _char* pConstantName);
	HRESULT				Bind_SplitDistance(CShader* pShader, const _char* pConstantName);
	HRESULT				Bind_Shadow_ProjMatrix(CShader* pShader, const _char* pMatrixConstantName, const _char* pIndexConstantName, _fvector vWorldPosition);*/

public:
	HRESULT Initialize(_float fViewportWidth, _float fViewportHeight);
	HRESULT	Update_ShadowLight(const SHADOW_LIGHT_DESC& ShadowLightDesc);
	void	Update(_fvector vTargetPosition);

private:
	CGameInstance*		m_pGameInstance = { nullptr };
	
	_float				m_fWidth = {};
	_float				m_fHeight = {};

	SHADOW_LIGHT_DESC	m_ShadowLightDesc = {};

	_vector				m_vDirection = {};
	_float				m_fDistance = {};
	_float4x4			m_Matrices[ENUM_CLASS(D3DTS::END)] = {};
	
	//_uint				m_iSplitIndex = {};
	//_float				m_fShadowLightFar = {};
	//_float				m_fSplitDistance[5] = {};
	//_matrix				m_CameraViewMatrix = {};
	//_float4x4			m_ShadowViewMatrix = {};
	//_float4x4			m_ShadowProjMatrices[4] = {};

	
private:
	/*_vector		Compute_Center(const _float4* pWorldPoints);
	void		Make_ShadowLight_SplitProj(const _float4* pWorldPoints);
	_matrix		Compute_ShadowLight_Proj(const _float4* pViewPoints, const _fmatrix ViewMatrixInv);
	_float		Compute_Split_Distance(_float fCameraNear, _float fCameraFar, _uint iIndex, _uint iNumSplit, _float fLambda);*/

public:
	static CShadow*		Create(_float fViewportWidth, _float fViewportHeight);
	virtual void		Free() override;
};

NS_END