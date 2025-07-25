#pragma once
#include "GameObject.h"

NS_BEGIN(Engine)

class ENGINE_DLL CPawnObject abstract : public CGameObject
{
public:
	typedef struct tagPawnObjectDesc
	{
		const _float4x4* pPawnMatrix;
	}PAWNOBJECT_DESC;

protected:
	CPawnObject(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CPawnObject(const CPawnObject& Prototype);
	virtual ~CPawnObject() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	virtual void	Bind_PawnData(void* pData);

protected:
	const _float4x4* m_pPawnMatrix = { nullptr };

public:
	virtual CGameObject*	Clone(void* pArg) PURE;
	virtual void			Free() override;
};

NS_END