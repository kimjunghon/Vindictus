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

private:
	CBoundingOBB(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CBoundingOBB() = default;

public:
	const BoundingOrientedBox* Get_Desc() const { return m_pDesc; }

public:
	HRESULT			Initialize(const BOUNDING_DESC* pDesc);
	virtual void	Update(_fmatrix WorldMatrix) override;
	virtual _bool	Intersect(COLLIDER eType, CBounding* pTarget) override;
#ifdef _DEBUG
public:
	virtual HRESULT Render(PrimitiveBatch<VertexPositionColor>* pBatch, _fvector vColor) override;
#endif

private:
	BoundingOrientedBox* m_pOriginalDesc{};
	BoundingOrientedBox* m_pDesc{};

public:
	static CBoundingOBB*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const BOUNDING_DESC* pDesc);
	virtual void			Free() override;
};

NS_END