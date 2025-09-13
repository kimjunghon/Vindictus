#include "Client_Defines.h"
#include "BehaviorTree.h"

NS_BEGIN(Client)

class CPuppy;

class CPuppyAI final : public CBehaviorTree
{
private:
	CPuppyAI();
	virtual ~CPuppyAI() = default;

public:
	virtual HRESULT Initialize(CPuppy* pControlledPuppy);

private:
	CPuppy* m_pControlledPuppy = { nullptr };

private:
	HRESULT Ready_Nodes();

public:
	static CPuppyAI*	Create(CPuppy* pControlledPuppy);
	virtual void		Free() override;
};

NS_END