#pragma once
#include "Bounding.h"

NS_BEGIN(Engine)

class ENGINE_DLL CBoundingOBB final : public CBounding
{
public:
	typedef struct tagBoundingOBBDesc : public BOUNDING_DESC
	{
		_float3		vExtents;
		_float3		vAngles;
	}BOUNDING_OBB_DESC;

	typedef struct tagOBBDesc
	{
		_float3		vCenter;
		_float3		vAlignDir[3];
		_float3		vCenterDir[3];
	}OBBDESC;

private:
	CBoundingOBB(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CBoundingOBB() = default;

public:
	const BoundingOrientedBox* Get_Desc() const { return m_pDesc; }

public:
	HRESULT			Initialize(const BOUNDING_DESC* pDesc);
	virtual void	Update(_fmatrix WorldMatrix, _bool IsRotate) override;
	virtual _bool	Intersect(COLLIDER eType, CBounding* pTarget, _float* pDisance = nullptr, _float3* pNormal = nullptr, _float3* pCollisionPos = nullptr) override;
#ifdef _DEBUG
public:
	virtual HRESULT Render(PrimitiveBatch<VertexPositionColor>* pBatch, _fvector vColor) override;
#endif

private:
	BoundingOrientedBox* m_pOriginalDesc{};
	BoundingOrientedBox* m_pDesc{};

private:
	_bool Intersect_OBB(const CBoundingOBB* pTarget, _float* pDistance, _float3* pNormal, _float3* pCollisionPos);
	OBBDESC Compute_OBBDesc() const;

public:
	static CBoundingOBB*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const BOUNDING_DESC* pDesc);
	virtual void			Free() override;
};

NS_END