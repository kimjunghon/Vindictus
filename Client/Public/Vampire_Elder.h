#pragma once
#include "Vampire.h"

NS_BEGIN(Client)

class CVampire_Elder final : public CVampire
{
private:
	enum class HIT_COLLIDER { HEAD, UPPER, LOWER, END };
	enum class ATTACK_COLLIDER { LEFT_HAND, RIGHT_HAND, END };

private:
	CVampire_Elder(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CVampire_Elder(const CVampire_Elder& Prototype);
	virtual ~CVampire_Elder() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual BT_STATE	Attack() override;
	HRESULT				Ready_PawnObject();
	HRESULT				Ready_Collider();
	HRESULT				Ready_Collider_Bounding();
	HRESULT				Ready_Collider_Body();
	HRESULT				Ready_Collider_Hit();
	HRESULT				Ready_Collider_Attack();
	HRESULT				Ready_AttackMapping();

public:
	static CVampire_Elder*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

NS_END