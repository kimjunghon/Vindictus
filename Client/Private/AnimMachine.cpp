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

	auto iter = m_AnimDatas.find(iState);
	if (iter == m_AnimDatas.end())
		return E_FAIL;

	return pModelCom->Set_Animation(iter->second);
}

HRESULT CAnimMachine::Forcing_Set_Animation(CModel* pModelCom, _uint iState)
{
	if (nullptr == pModelCom)
		return E_FAIL;

	auto iter = m_AnimDatas.find(iState);
	if (iter == m_AnimDatas.end())
		return E_FAIL;

	return pModelCom->Forcing_Set_Animation(iter->second);
}

const ANIM_DATA* CAnimMachine::Get_AnimationData(_uint iState)
{
	auto iter = m_AnimDatas.find(iState);
	if (m_AnimDatas.end() == iter)
		return nullptr;

	return &iter->second;
}

void CAnimMachine::Free()
{
    __super::Free();

}
