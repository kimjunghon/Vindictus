#pragma once
#include "Client_Defines.h"
#include "PawnObject.h"

NS_BEGIN(Engine)
class CModel;
class CShader;
NS_END

NS_BEGIN(Client)

class CAnimMachine;

class CBody abstract : public CPawnObject
{
public:
	typedef struct tagBodyDesc : public PAWNOBJECT_DESC
	{
		_uint* pStateFlag = { nullptr };
	}BODY_DESC;

protected:
	CBody(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CBody(const CBody& Prototype);
	virtual ~CBody() = default;

public:
	virtual HRESULT		Add_AnimNotify(const string& strAnimName, _float fTrackPosition, function<void()> Callback);

	_bool				IsAnimationInRangeTrackPosition(_float2 vRange) { return m_pModelCom->IsAnimationInRangeTrackPosition(vRange); }
	_bool				IsAnimationPassToTrackPosition(_float fTrackPosition) { return m_pModelCom->IsAnimationPassToTrackPosition(fTrackPosition); }
	_bool				AnimIsFinished() { return m_pModelCom->CurrentAnim_Finished(); }
	_bool				AnimCanChange() { return m_pModelCom->CanChangeAnimation(); }
	const _vector*		Get_AnimMovementPtr() const { return m_pModelCom->Get_AnimMovementPtr(); }
	const _vector*		Get_AnimRotationPtr() const { return m_pModelCom->Get_AnimRotationPtr(); }
	const _float4x4*	SocketCombinedMatrixPtr(const string& strSocektBoneName) const { return m_pModelCom->Find_SocketBoneCombinedMatrix(strSocektBoneName); }
	const _float4x4*	OffsetMatrixPtr(const string& strSocketBoneName) const { return m_pModelCom->Find_OffsetMatrix(strSocketBoneName); }
	CModel*				Get_ParentModelPtr() const { return m_pModelCom; }
	_float				Get_CurrentAnimSpeed() { return m_pModelCom->Get_CurrentAnimSpeed(); }
	virtual const _matrix Get_BodyCombinedMatrix() const { return XMMatrixIdentity(); }
	virtual const _float4x4* Get_BodyCombinedMatrixPtr() const { return nullptr; }
	const MODEL_BOUNDING& Get_ModelBounding() const { return m_pModelCom->Get_ModelBounding(); }
public:
	virtual HRESULT		Initialize_Prototype() override;
	virtual HRESULT		Initialize(void* pArg) override;
	virtual void		Priority_Update(_float fTimeDelta) override;
	virtual void		Update(_float fTimeDelta) override;
	virtual void		Late_Update(_float fTimeDelta) override;
	virtual HRESULT		Render() override;

	HRESULT				Forcing_Play_Animation();

protected:
	CAnimMachine*	m_pAnimMachine = { nullptr };
	CModel*			m_pModelCom = { nullptr };
	CShader*		m_pShaderCom = { nullptr };
	_uint*			m_pStateFlag = {};

public:
	virtual CGameObject*	Clone(void* pArg) PURE;
	virtual void			Free() override;
};

NS_END