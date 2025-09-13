#pragma once
#include "ClientPch.h"
#include "GameObject.h"

NS_BEGIN(Client)

class CNpc abstract : public CGameObject
{
protected:
	CNpc(ID3D11Device* pDeviceContext);
	
};

NS_END