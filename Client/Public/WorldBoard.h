#pragma once
#include "MapObject.h"

NS_BEGIN(Engine)
class CColliderContainer;
NS_END

NS_BEGIN(Client)

class CWorldBoard final : public CMapObject
{
private:
	CWorldBoard(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CWorldBoard(const CWorldBoard& Prototype);
	virtual ~CWorldBoard() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;


private:
	CColliderContainer* m_pColliderContainer = { nullptr };

private:
	HRESULT Ready_Colliders();

public:
	static CWorldBoard*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

NS_END