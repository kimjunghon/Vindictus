#pragma once

#include "Client_Defines.h"
#include "Level.h"

NS_BEGIN(Client)

class CPool_Instance;

class CLevel_Queen final : public CLevel
{
private:
	CLevel_Queen(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CLevel_Queen() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void	Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CPool_Instance* m_pPool_Instance = { nullptr };

private:
	HRESULT Ready_Light();
	HRESULT Ready_Sky();
	HRESULT Ready_GameObjectToJson();
	HRESULT Ready_Camera();
	HRESULT Ready_DamageFont();
	HRESULT Ready_Player(const Value& Player);

	HRESULT	Ready_Effect();
	HRESULT Ready_PoolingMonster();
	HRESULT Ready_PoolingProjectile();
	HRESULT Ready_TriggerBox();

	HRESULT Ready_Map(const _wstring& strLayerTag);

public:
	static CLevel_Queen*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void			Free() override;
};

NS_END