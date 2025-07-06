#pragma once
#include "Event.h"

NS_BEGIN(Engine)

class CEventBus final : public CBase
{
private:
	typedef function<void(const CEvent&)> CallBack;
	typedef map<size_t, vector<CallBack>> SUBSCRIBERS;

private:
	CEventBus();
	CEventBus(const CEventBus& Prototype) = delete;
	virtual ~CEventBus() = default;

public:
	template<typename EVENT>
	void Subscribe(_uint iEventLevelIndex, function<void(const EVENT&)> Subscriber)
	{
		auto wrapper = [Subscriber](const CEvent& Event) {
			Subscriber(static_cast<const EVENT&>(Event)); };
		
		m_Subscribers[iEventLevelIndex][typeid(EVENT).hash_code()].push_back(wrapper);
	}

	void Publish(_uint iEventLevelIndex, const CEvent& Event)
	{
		auto& Vector = m_Subscribers[iEventLevelIndex][typeid(Event).hash_code()];
		for (auto& Callback : Vector)
			Callback(Event);
	}

public:
	HRESULT Initialize(_uint iNumLevels);
	void	Clear(_uint iClearLevel);

private:
	_uint					m_iNumLevels = {};
	SUBSCRIBERS*			m_Subscribers = { nullptr };

public:
	static CEventBus*	Create(_uint iNumLevels);
	virtual void		Free() override;
};

NS_END