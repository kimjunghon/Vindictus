#pragma once
#include "ColliderPawn.h"

NS_BEGIN(Engine)
class CBehaviorTree;
NS_END

using namespace Puppy;

NS_BEGIN(Client)
class CBody;

class CPuppy final : public CColliderPawn
{
public:
	typedef struct tagPuppyDesc : GAMEOBJECT_DESC
	{
		_uint iPuppyTypeIndex;
		_float3 vPosition;
		_int iCellIndex;
	}PUPPY_DESC;

private:
	CPuppy(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CPuppy(const CPuppy& Prototype);
	virtual ~CPuppy() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	BT_STATE		CanOtherAction();
	BT_STATE		IsNear();
	BT_STATE		Idle();

private:
	_uint			m_iStateFlag = {};
	CBody*			m_pBody = { nullptr };
	CTransform*		m_pTargetTransform = { nullptr };
	CBehaviorTree*	m_pAI = { nullptr };

	const _vector* m_pAnimMovement = {};
	const _vector* m_pAnimRotation = {};

private:
	HRESULT			Ready_PawnObject(_uint iPuppyType);
	HRESULT			Ready_AI();
	HRESULT			Ready_Collider();
	void			Compute_WorldMatrix();

public:
	static CPuppy*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

NS_END