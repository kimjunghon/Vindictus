#pragma once
#include "Client_Defines.h"
#include "Pawn.h"

NS_BEGIN(Client)

class CCamera_Target;
class CPlayerState;
class CPlayerBody;
class CArmor;

class CPlayerPawn final : public CPawn
{
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
	void			Sprint() { m_fSpeedRatio = 5.f; }

	HRESULT			EquipArmor(const _wstring& strArmorTag, CArmor* pArmor, ARMOR_TYPE eArmorType);
	HRESULT			UnEquipArmor(ARMOR_TYPE eArmorType);
private:
	CPlayerBody*			m_pPlayerBody = { nullptr };
	CCamera_Target*			m_pCamera = { nullptr };
	_wstring				m_strEquipArmors[ENUM_CLASS(ARMOR_TYPE::END)] = {};

	INPUT_MOVE_DESC			m_MoveInput = {};
	INPUT_ACTION_DESC		m_ActionInput = {};
	INPUT_CAMERA_DESC		m_CameraInput = {};
	
	vector<CPlayerState*>	m_States;
	CPlayerState*			m_pCurrentState = { nullptr };

	_float					m_fSpeed = {};
	_float					m_fSpeedRatio = {};
	_uint					m_iStateFlag = {};
	_uint					m_iComboCount = {};
	_vector					m_vPlayerMoveDir = {};
	const _vector*			m_pAnimMovement = {};

private:
	HRESULT Ready_Components();
	HRESULT Ready_Camera();
	HRESULT Ready_PawnObjects();
	HRESULT Ready_PlayerBody();
	HRESULT Ready_Weapons();
	HRESULT Ready_Armors();
	HRESULT Ready_States();

	void	Compute_WorldMatrix();
	void	Bind_InputData(_float fTimeDelta);
	void	Move(_float fTimeDelta);

public:
	static CPlayerPawn*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

NS_END