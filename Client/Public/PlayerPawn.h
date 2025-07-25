#pragma once
#include "Client_Defines.h"
#include "Pawn.h"

NS_BEGIN(Client)

class CCamera_Target;
class CPlayerState;

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

	void			Change_State(_uint iStateIndex);
	_uint			Get_ComboCount() { return m_iComboCount; }
	void			Reset_ComboCount() { m_iComboCount = 0; }
	void			Increase_ComboCount() { m_iComboCount++; }
	_bool			AnimIsFinished();
	_bool			AnimCanChange();

private:
	CCamera_Target* m_pCamera = { nullptr };
	
	INPUT_MOVE_DESC			m_MoveInput;
	INPUT_ACTION_DESC		m_ActionInput;
	INPUT_CAMERA_DESC		m_CameraInput;
	
	vector<CPlayerState*>	m_States;
	CPlayerState*			m_pCurrentState = { nullptr };

	_uint					m_iStateFlag = {};
	_uint					m_iComboCount = {};

private:
	HRESULT Ready_Components();
	HRESULT Ready_Camera();
	HRESULT Ready_PawnObjects();
	HRESULT Ready_States();

	void	Bind_InputData();

public:
	static CPlayerPawn*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

NS_END