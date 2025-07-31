#pragma once
#include "Client_Defines.h"
#include "Pawn.h"

NS_BEGIN(Client)

class CBody;

class CMonster abstract : public CPawn
{
protected:
	CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CMonster(const CMonster& Prototype);
	virtual ~CMonster() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	CBody*				m_pBody = { nullptr };
	const _vector*		m_pAnimMovement = {nullptr};
	_uint				m_iStateFlag = {};

public:
	virtual CGameObject*	Clone(void* pArg) PURE;
	virtual void			Free() override;
};

NS_END