#pragma once
#include "Client_Defines.h"
#include "PawnObject.h"

NS_BEGIN(Engine)
class CModel;
class CShader;
NS_END

NS_BEGIN(Client)

class CPlayerAnimMachine;

class CPlayerBody final : public CPawnObject
{
public:
	typedef struct tagBodyDesc : public PAWNOBJECT_DESC
	{
		_uint* pStateFlag = { nullptr };
	}BODY_DESC;

private:
	CPlayerBody(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CPlayerBody(const CPlayerBody& Prototype);
	virtual ~CPlayerBody() = default;

public:
	virtual HRESULT		Initialize_Prototype() override;
	virtual HRESULT		Initialize(void* pArg) override;
	virtual void		Priority_Update(_float fTimeDelta) override;
	virtual void		Update(_float fTimeDelta) override;
	virtual void		Late_Update(_float fTimeDelta) override;
	virtual HRESULT		Render() override;

	_bool				AnimIsFinished() { return m_pModelCom->CurrentAnim_Finished(); }
	_bool				AnimCanChange() { return m_pModelCom->CanChangeAnimation(); }
	const _vector*		Get_AnimMovementPtr() { return m_pModelCom->Get_AnimMovementPtr(); }
	const _float4x4*	SocketCombinedMatrixPtr(const string& strSocektBoneName) { return m_pModelCom->Find_SocketBoneCombinedMatrix(strSocektBoneName); }
	CModel*				Get_ParentModelPtr() { return m_pModelCom; }

	void				EquipHead() { m_IsEquipHead = true; }
	void				UnEquipHead() { m_IsEquipHead = false; }
private:
	CModel*				m_pModelCom = { nullptr };
	CShader*			m_pShaderCom = { nullptr };
	CPlayerAnimMachine* m_pAnimMachine = { nullptr };

	_uint*				m_pStateFlag = {};
	_vector				m_vMoveDir = {};
	_bool				m_IsEquipHead = {};
private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CPlayerBody*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

NS_END