#pragma once
#include "Client_Defines.h"
#include "Pawn.h"
#include "Collider.h"

NS_BEGIN(Engine)
class CNavigation;
NS_END

NS_BEGIN(Client)

class CCamera_Target;
class CPlayerState;
class CPlayerBody;
class CArmor;
class CWeapon;
class CPlayerInstance;

class CPlayerPawn final : public CPawn
{
public:
	typedef struct tagPlayerDesc : public GAMEOBJECT_DESC
	{
		_float3 vPosition;
		_int iCellIndex;
	}PLAYER_DESC;

private:
	enum class HIT_COLLIDER { HEAD, UPPER, LOWER, L_ARM, R_ARM, L_LEG, R_LEG, END};
	enum class ATTACK_COLLIDER { SWORD, SHILED, LEFT_LEG, RIGHT_LEG, END};
	typedef unordered_map<COLLIDER_CHANNEL, vector<CCollider*>> COLLIDER;

private:
	CPlayerPawn(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CPlayerPawn(const CPlayerPawn& Prototype);
	virtual ~CPlayerPawn() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void			Change_State(_uint iStateIndex);
	_uint			Get_ComboCount() { return m_iComboCount; }
	void			Reset_ComboCount() { m_iComboCount = 0; }
	void			Increase_ComboCount() { m_iComboCount++; }
	_bool			AnimIsFinished();
	_bool			AnimCanChange();
	void			Compute_PlayerMoveDir();
	void			Run() { m_fSpeedRatio = 1.f; }
	void			Sprint() { m_fSpeedRatio = 2.f; }

private:
	CPlayerInstance*			m_pPlayerInstance = { nullptr };
	CNavigation*				m_pNavigation = { nullptr };
	CPlayerBody*				m_pPlayerBody = { nullptr };
	CCamera_Target*				m_pCamera = { nullptr };

	//Collider
	COLLIDER					m_Colliders;
	vector<const _float4x4*>	m_HitColliderSocketMatrix;
	vector<_matrix>				m_HitColliderCombinedMatrix;
	vector<const _float4x4*>	m_AttackColliderSocketMatrix;
	vector<_matrix>				m_AttackColliderCombinedMatrix;
	HIT_TYPE					m_eHitType = { HIT_TYPE::END };
	//Equip
	_wstring					m_strEquipWeapons[ENUM_CLASS(WEAPON_TYPE::END)] = {};
	_wstring					m_strEquipArmors[ENUM_CLASS(ARMOR_TYPE::END)] = {};

	//State
	INPUT_MOVE_DESC				m_MoveInput = {};
	INPUT_ACTION_DESC			m_ActionInput = {};
	INPUT_CAMERA_DESC			m_CameraInput = {};

	vector<CPlayerState*>		m_States;
	CPlayerState*				m_pCurrentState = { nullptr };
	
	_uint						m_iStateFlag = {};
	_uint						m_iComboCount = {};

	_float						m_fSpeed = {};
	_float						m_fSpeedRatio = {};

	//Player Look
	_vector						m_vPlayerRoationQuat = {};
	_vector						m_vPrevPlayerRotationQuat = {};
	_vector						m_vPlayerMoveDir = {};

	//Root Motion
	const _vector*				m_pAnimMovement = {};
	const _vector*				m_pAnimRotation = {};

private:
	void		Update_ColliderMatrix();

	HRESULT		Init_Level(_int iCellIndex, _float3 vStartPostion);
	HRESULT		Ready_Camera();
	HRESULT		Ready_PawnObjects();
	HRESULT		Ready_PlayerBody();
	HRESULT		Ready_Weapons();
	HRESULT		Ready_Armors();
	HRESULT		Ready_States();

	HRESULT		Ready_Collider();
	HRESULT		Ready_Collider_Bounding();
	HRESULT		Ready_Collider_Body();
	HRESULT		Ready_Collider_Hit();
	HRESULT		Add_Collider_Hit(const _wstring& strColliderTag, CBoundingOBB::BOUNDING_OBB_DESC* pDesc, _uint iColliderIndex, const _float4x4* pSocketCombinedMatrix);
	HRESULT		Ready_Collider_Attack();
	HRESULT		Add_Collider_Attack(const _wstring& strColliderTag,CBoundingOBB::BOUNDING_OBB_DESC* pDesc, _uint iColliderIndex, const _float4x4* pSocketCombinedMatrix);

	void		Compute_WorldMatrix();
	void		Bind_InputData(_float fTimeDelta);
	void		Move(_float fTimeDelta);

	HRESULT		EquipWeapon(CWeapon* pWeapon);
	HRESULT		UnEquipWeapon(_uint iWeaponTypeIndex);

	HRESULT		EquipArmor(CArmor* pArmor);
	HRESULT		UnEquipArmor(_uint iArmorTypeIndex);

	void		Event_ChangeWeapon(const EVENT_CHANGE_WEAPON& Event);
	void		Event_ChangeArmor(const EVENT_CHANGE_ARMOR& Event);

	void		OnCollisionHit(const CCollider::COLLISION_DATA& CollisionData);

public:
	static CPlayerPawn*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

NS_END