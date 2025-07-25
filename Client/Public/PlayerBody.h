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
private:
	CPlayerBody(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CPlayerBody(const CPlayerBody& Prototype);
	virtual ~CPlayerBody() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	virtual void	Bind_PawnData(void* pData) override;

	_bool			AnimIsFinished() { return m_pModelCom->CurrentAnim_Finished(); }
	_bool			AnimCanChange() { return m_pModelCom->CanChangeAnimation(); }
private:
	CModel*				m_pModelCom = { nullptr };
	CShader*			m_pShaderCom = { nullptr };
	CPlayerAnimMachine* m_pAnimMachine = { nullptr };

	_uint				m_iStateFlag = {};
private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CPlayerBody*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

NS_END