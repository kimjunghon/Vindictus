#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CShader;
class CModel;
NS_END

NS_BEGIN(Client)

class CCharacter abstract : public CGameObject
{
public:
	typedef struct tagCharacterDesc : public GAMEOBJECT_DESC
	{
		_uint iModelIndex;
		_wstring strModelTag;
	}CHARACTER_DESC;

protected:
	CCharacter(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CCharacter(const CCharacter& Prototype);
	virtual ~CCharacter() = default;

public:
	void			Update_PawnWorldMatrix(_float4x4& PawnWorldMatrix);
	void			Update_AnimData(const ANIM_DATA& AnimData);
	_bool			CurrentAnim_InRangeOfRatio(_float fBeginRatio, _float fEndRatio) { return m_pModelCom->CurrentAnim_InRangeOfRatio(fBeginRatio, fEndRatio); }
	_bool			CurrentAnim_Finished() { return m_pModelCom->CurrentAnim_Finished(); }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	CShader*	m_pShaderCom = { nullptr };
	CModel*		m_pModelCom = { nullptr };
	_float4x4	m_WorldMatrix = {};

	_bool		m_CanPlayAnim;

	ANIM_DATA	m_AnimData;

protected:
	HRESULT Ready_Components(_uint iModelIndex, const _wstring& strModelTag);
	HRESULT Bind_ShaderResources();

public:
	virtual CGameObject*	Clone(void* pArg) PURE;
	virtual void			Free() override;
};

NS_END