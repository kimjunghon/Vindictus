#pragma once
#include "Component.h"
#include "Collider.h"

NS_BEGIN(Engine)

class CGameObject;

class ENGINE_DLL CColliderContainer final : public CComponent
{
private:
	typedef unordered_map<_uint, vector<CCollider*>> COLLIDER;
	typedef unordered_map<_uint, vector<const _float4x4*>> COLLIDER_BONE;

private:
	CColliderContainer(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CColliderContainer(const CColliderContainer& Prototype);
	virtual ~CColliderContainer() = default;
	
public:	
	virtual HRESULT				Initialize_Prototype();
	virtual HRESULT				Initialize(void* pArg) override;
	void						Update(CGameObject* pOwner, _fmatrix WorldMatrix);

	HRESULT						Add_Collider(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, _uint iChannel, _uint iOwner,  CBounding::BOUNDING_DESC* pDesc, const _float4x4* pBoneMatrix);
	HRESULT						Bind_Collision_Callback(_uint iChannel, _uint iColliderIndex, COLLIDER_STATE eState, CCollider::Collision_CallBack Callback);
	
	void						SetDesc(_uint iChannel, _uint iColliderIndex, void* pDesc);
	void						SetEnable(_uint iChannel, _uint iColliderIndex, _bool IsEnable);

	void						SetEnableAllColliderChannel(_bool IsEnable);
	void						SetEnableColliderChannel(_uint iChannel, _bool IsEnable);

	HRESULT						Change_Collider_BoneMatrix(_uint iChannel, _uint iColliderIndex, const _float4x4* pChangeBoneMatrix);

private:
	unordered_set<_uint>		m_ColliderChannels;

	COLLIDER					m_Colliders;
	COLLIDER_BONE				m_ColliderBoneMatrices;

#ifdef _DEBUG
	_uint iRenderIndex = {};
#endif

private:
	void						Update_Collider(_uint iChannel, _uint iIndex, CCollider* pCollider, _fmatrix WorldMatrix, CGameObject* pOwner);
	HRESULT						Add_BoneMatrix(_uint iChannel, const _float4x4* pBoneMatrix);
	
	vector<CCollider*>*			Find_Colliders(_uint iChannel);
	vector<const _float4x4*>*	Find_BoneMatrices(_uint iChannel);

public:
	static CColliderContainer*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CComponent*			Clone(void* pArg) override;
	virtual void				Free() override;
};

NS_END