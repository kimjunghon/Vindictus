#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CController_Manager final : public CBase
{
private:
	typedef unordered_map<_wstring, class CController*> CONTROLLERS;

private:
	CController_Manager();
	virtual ~CController_Manager() = default;

public:
	HRESULT MoveInput(_uint iChannelIndex, INPUT_MOVE_DESC* pOut);
	HRESULT ActionInput(_uint iChannelIndex, INPUT_ACTION_DESC* pOut);
	HRESULT CameraInput(_uint iChannelIndex, INPUT_CAMERA_DESC* pOut);

public:
	HRESULT				Add_Controller_ToManager(const _wstring& strControllerTag, class CController* pController);
	HRESULT				Change_Controller(_uint iChannelIndex, const _wstring& strControllerTag);
	HRESULT				Change_Controller(_uint iChannelIndex, class CController* pNewController);
	class CController*	Find_Controller(const _wstring& strControllerTag);
	void				Clear();

private:
	CONTROLLERS					m_Controllers;
	class CController*			m_pCurrentController[ENUM_CLASS(CONTROLLER_CHANNEL::END)] = { nullptr };

public:
	static CController_Manager* Create();
	virtual void				Free() override;
};

NS_END