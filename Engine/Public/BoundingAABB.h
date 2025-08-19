#pragma once
#include "Bounding.h"

NS_BEGIN(Engine)

class ENGINE_DLL CBoundingAABB final : public CBounding
{
public:
	typedef struct tagBoundingAABBDesc : public BOUNDING_DESC
	{
		_float3		vExtents;
	}BOUNDING_AABB_DESC;

private:
	CBoundingAABB(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CBoundingAABB() = default;

public:
	const BoundingBox* Get_Desc() const { return m_pDesc; }

public:
	HRESULT			Initialize(const BOUNDING_DESC* pDesc);
	virtual void	Update(_fmatrix WorldMatrix) override;
	virtual _bool	Intersect(COLLIDER eType, CBounding* pTarget, _float* pDistance = nullptr, _float3* pNormal = nullptr) override;

#ifdef _DEBUG
public:
	virtual HRESULT Render(PrimitiveBatch<VertexPositionColor>* pBatch, _fvector vColor) override;
#endif

private:
	BoundingBox* m_pOriginalDesc{};
	BoundingBox* m_pDesc{};

public:
	static CBoundingAABB*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const BOUNDING_DESC* pDesc);
	virtual void			Free() override;
};

NS_END