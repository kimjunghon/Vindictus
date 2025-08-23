#pragma once
#include "Client_Defines.h"
#include "Base.h"

NS_BEGIN(Engine)
class CModel;
NS_END

NS_BEGIN(Client)

class CAnimMachine abstract : public CBase
{
protected:
	CAnimMachine();
	virtual ~CAnimMachine() = default;

public:
	virtual void	Initialize();
	HRESULT			Set_Animation(CModel* pModelCom, _uint iState);
	HRESULT			Forcing_Set_Animation(CModel* pModelCom, _uint iState);
protected:
	unordered_map<_uint, ANIM_DATA> m_AnimDatas;

public:
	virtual void	Free() override;
};

NS_END