#pragma once
#include "GameObject.h"

NS_BEGIN(Engine)

class ENGINE_DLL CCollisionObject abstract : public CGameObject
{
private:
	CCollisionObject(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CCollisionObject(const CCollisionObject& Prototype);
	virtual ~CCollisionObject() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render();

public:
	const BoundingBox& Get_BoundingBox() { return m_BoundingBox; }

private:
	BoundingBox				m_BoundingBox;

private:
	BoundingBox	Compute_BoundingVolume() const;

public:
	virtual CGameObject*	Clone(void* pArg) PURE;
	virtual void			Free() override;
};

NS_END