#pragma once
#include "Client_Defines.h"
#include "Level.h"

NS_BEGIN(Client)

class CMonsterInstance;

class CLevel_Glasgavelen final : public CLevel
{
private:
	CLevel_Glasgavelen(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CLevel_Glasgavelen() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void	Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CMonsterInstance* m_pMonsterInstance = { nullptr };

private:
	HRESULT Ready_Light();
	HRESULT Ready_GameObjectToJson();
	HRESULT Ready_Player(const Value& Player);
	HRESULT Ready_PoolingMonster();

	HRESULT Ready_Map(const _wstring& strLayerTag);

public:
	static CLevel_Glasgavelen*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void				Free() override;
};


NS_END