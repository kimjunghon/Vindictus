#pragma once
#include "ColliderPawn.h"

using namespace Cat;

NS_BEGIN(Client)
class CBody;

class CCat final : public CColliderPawn
{
public:
	typedef struct tagPuppyDesc : GAMEOBJECT_DESC
	{
		_float3 vPosition;
		_int	iCellIndex;
	}CAT_DESC;

private:
	CCat(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CCat(const CCat& Prototype);
	virtual ~CCat() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CBody*			m_pBody = { nullptr };
	_uint			m_iStateFlag = {};

private:
	HRESULT			Ready_PawnObject();
	HRESULT			Ready_Collider();
	void			UpdateAnimFlag();
public:
	static CCat*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

NS_END