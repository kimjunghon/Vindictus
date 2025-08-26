#pragma once
#include "MonsterState.h"

NS_BEGIN(Client)

using namespace Glasgavelen;

class CGS_WingBreak final : public CMonsterState
{
private:
	CGS_WingBreak();
	virtual ~CGS_WingBreak() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void	Enter(CMonster* pMonster) override;
	virtual void	Update(CMonster* pMonster, _float fTimeDelta) override;
	virtual void	Exit(CMonster* pMonster) override;

private:
	_float2			m_vChangeModelRange = {};
	_bool			m_IsModelChange = {};

public:
	static CGS_WingBreak*	Create();
	virtual void			Free() override;
};

NS_END