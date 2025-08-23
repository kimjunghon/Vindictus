#pragma once
#include "Bounding.h"

NS_BEGIN(Engine)

class ENGINE_DLL CBoundingSphere final : public CBounding
{
public:
	typedef struct tagBoundingSphereDesc : public BOUNDING_DESC
	{
		_float		fRadius;
	}BOUNDING_SPHERE_DESC;

private:
	CBoundingSphere(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CBoundingSphere() = default;
public:
	const BoundingSphere* Get_Desc() const { return m_pDesc; }

public:
	HRESULT			Initialize(const BOUNDING_DESC* pDesc);
	virtual void	Update(_fmatrix WorldMatrix, _bool IsRotate) override;
	virtual _bool	Intersect(COLLIDER eType, CBounding* pTarget, _float* pDistance = nullptr, _float3* pNormal = nullptr) override;
#ifdef _DEBUG
public:
	virtual HRESULT Render(PrimitiveBatch<VertexPositionColor>* pBatch, _fvector vColor) override;
#endif

public:
	BoundingSphere* m_pOriginalDesc{};
	BoundingSphere* m_pDesc{};

public:
	static CBoundingSphere* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const BOUNDING_DESC* pDesc);
	virtual void			Free() override;

};

NS_END