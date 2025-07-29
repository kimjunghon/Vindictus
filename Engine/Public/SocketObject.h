#pragma once
#include "PawnObject.h"

NS_BEGIN(Engine)

class CModel;

class ENGINE_DLL CSocketObject abstract : public CPawnObject
{
public:
	typedef struct tagSocketObjectDesc : public PAWNOBJECT_DESC
	{
		const _float4x4* pSocketMatrix;
	}SOCKETOBJECT_DESC;
	
protected:
	CSocketObject(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CSocketObject(const CSocketObject& Prototype);
	virtual ~CSocketObject() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	const _float4x4*	m_pSocketMatrix = { nullptr };
	_float4x4			m_CombinedMatrix = { }; 

public:
	virtual CGameObject*	Clone(void* pArg) PURE;
	virtual void			Free() override;
};

NS_END