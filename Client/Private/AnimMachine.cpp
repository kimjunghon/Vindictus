#include "ClientPch.h"
#include "AnimMachine.h"
#include "Model.h"

CAnimMachine::CAnimMachine()
{
}

void CAnimMachine::Initialize()
{
}

HRESULT CAnimMachine::Set_Animation(CModel* pModelCom, _uint iState)
{
	if (nullptr == pModelCom)
		return E_FAIL;

	return 	pModelCom->Set_Animation(m_AnimDatas[iState]);
}

HRESULT CAnimMachine::Forcing_Set_Animation(CModel* pModelCom, _uint iState)
{
	if (nullptr == pModelCom)
		return E_FAIL;

	return 	pModelCom->Forcing_Set_Animation(m_AnimDatas[iState]);
}

void CAnimMachine::Free()
{
    __super::Free();

}
