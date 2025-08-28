#pragma once
#include "ColliderPawn.h"

using namespace Player;

NS_BEGIN(Client)

class CCamera_Target;
class CPlayerState;
class CPlayerBody;
class CArmor;
class CWeapon;
class CPlayerInstance;

class CPlayerPawn final : public CColliderPawn
{
public:
	typedef struct tagPlayerDesc : public GAMEOBJECT_DESC
	{
		_float3 vPosition;
		_int iCellIndex;
	}PLAYER_DESC;

private:
	enum class PLAYER_HIT_TYPE { FRONT, BACK, RIGHT, LEFT, };
	enum class HIT_COLLIDER { HEAD, UPPER, LOWER, L_ARM, R_ARM, L_LEG, R_LEG, END};
	enum class ATTACK_COLLIDER { SWORD, SHILED, LEFT_LEG, RIGHT_LEG, END};

private:
	CPlayerPawn(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CPlayerPawn(const CPlayerPawn& Prototype);
	virtual ~CPlayerPawn() = default;

public:
	_uint			Get_ComboCount() { return m_iComboCount; }
	void			Reset_ComboCount() { m_iComboCount = 0; }
	void			Increase_ComboCount() { m_iComboCount++; }
	void			Run() { m_fSpeedRatio = 1.f; }
	void			Sprint() { m_fSpeedRatio = 2.f; }
	DIR				Get_HirDir() { return m_eHitDir; }
	_bool			IsGaurdHit() { return m_iStateFlag & ENUM_CLASS(HIT_FLAG::GUARD); }
	_bool			IsGrap() { return m_IsGrap; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	_bool			AnimIsFinished();
	_bool			AnimCanChange();
	void			Change_State(_uint iStateIndex);
	void			Compute_PlayerMoveDir();
	void			Grap();
	
private:
	CPlayerInstance*			m_pPlayerInstance = { nullptr };
	CPlayerBody*				m_pPlayerBody = { nullptr };
	CCamera_Target*				m_pCamera = { nullptr };

	//Equip
	_wstring					m_strEquipWeapons[ENUM_CLASS(WEAPON_TYPE::END)] = {};
	_wstring					m_strEquipArmors[ENUM_CLASS(ARMOR_TYPE::END)] = {};
	CArmor*						m_pEquipArmors[ENUM_CLASS(ARMOR_TYPE::END)] = { nullptr };
	//State
	PLAYER_STATUS				m_Status = {};

	INPUT_MOVE_DESC				m_MoveInput = {};
	INPUT_ACTION_DESC			m_ActionInput = {};
	INPUT_CAMERA_DESC			m_CameraInput = {};

	vector<CPlayerState*>		m_States;
	CPlayerState*				m_pCurrentState = { nullptr };
	
	DIR							m_eHitDir = { DIR::END };

	_bool						m_IsGrap = {};
	GRAP_DATA					m_GrapData = {};
	_float4x4					m_GrapOffsetMatrix = {};
	_float4x4					m_GrapMatrix = {};
	_uint						m_iStateFlag = {};
	_uint						m_iComboCount = {};

	_float						m_fSpeed = {};
	_float						m_fSpeedRatio = {};

	//Player Look
	_vector						m_vPlayerRotationQuat = {};
	_vector						m_vPrevPlayerRotationQuat = {};
	_vector						m_vPlayerMoveDir = {};

	//Root Motion
	const _vector*				m_pAnimMovement = {};
	const _vector*				m_pAnimRotation = {};

private:
	HRESULT			Init_Level(_int iCellIndex, _float3 vStartPostion);
	HRESULT			Ready_Camera();
	HRESULT			Ready_PawnObjects();
	HRESULT			Ready_PlayerBody();
	HRESULT			Ready_Weapons();
	HRESULT			Ready_Armors();
	HRESULT			Ready_States();

	HRESULT			Ready_Collider();
	HRESULT			Ready_Collider_Bounding();
	HRESULT			Ready_Collider_Body();
	HRESULT			Ready_Collider_Hit();
	HRESULT			Ready_Collider_Attack();

	HRESULT			Add_Collider_Grap();
	HRESULT			Ready_AttackMapping();
	HRESULT			Add_AttackCollisionInfo(const string& strAnimName, _uint iAttackColliderIndex, ATTACK_TYPE eType, _float fAttackRatio, _float2 vTrackPosition);

	void			Compute_WorldMatrix();
	void			Bind_InputData(_float fTimeDelta);
	void			Move(_float fTimeDelta);

	HRESULT			EquipWeapon(CWeapon* pWeapon);
	HRESULT			UnEquipWeapon(_uint iWeaponTypeIndex);

	HRESULT			EquipArmor(CArmor* pArmor);
	HRESULT			UnEquipArmor(_uint iArmorTypeIndex);

	void			Event_ChangeWeapon(const EVENT_CHANGE_WEAPON& Event);
	void			Event_ChangeArmor(const EVENT_CHANGE_ARMOR& Event);

	void			OnCollisionHit(_uint iArmorIndex, const CCollider::COLLISION_DATA& CollisionData);
	
	void			OnCollisionGrap(const CCollider::COLLISION_DATA& CollisionData);
	void			EndCollisionGrap(const CCollider::COLLISION_DATA& CollisionData);

	void			Change_HitState(ATTACK_TYPE eAttackType, _fvector vPosition, _fvector vAttackPosition);

	void			Update_HitColliderEnable();

	HRESULT			Bind_HitCollisionCallback(HIT_COLLIDER eHitCollider, ARMOR_TYPE eArmor_Type);

	void			DecreaseArmorDurability(_uint iArmorIndex, _float fDamage);
public:
	static CPlayerPawn*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

NS_END