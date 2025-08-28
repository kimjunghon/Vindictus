#pragma once
#include "Vampire.h"

NS_BEGIN(Client)

class CVampire_Royal final : public CVampire
{
private:
	enum class HIT_COLLIDER { HEAD, UPPER, LOWER, END };
	enum class ATTACK_COLLIDER { SPEAR, END };

private:
	CVampire_Royal(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CVampire_Royal(const CVampire_Royal& Prototype);
	virtual ~CVampire_Royal() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_PawnObject();	
	HRESULT	Ready_Collider();
	HRESULT	Ready_Collider_Bounding();
	HRESULT	Ready_Collider_Body();
	HRESULT	Ready_Collider_Hit();
	HRESULT	Ready_Collider_Attack();

public:
	static CVampire_Royal*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

NS_END