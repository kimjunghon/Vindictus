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
	void Subscribe(_uint iEventTypeIndex, function<void(const EVENT&)> Subscriber)
	{
		if (iEventTypeIndex >= ENUM_CLASS(EVENT_TYPE::END))
		{
			MSG_BOX(TEXT("Failed Subscribe Event"));
			return;
		}

		auto wrapper = [Subscriber](const CEvent& Event) {
			Subscriber(static_cast<const EVENT&>(Event)); };
		
		m_Subscribers[iEventTypeIndex][typeid(EVENT).hash_code()].push_back(wrapper);
	}

	void Publish(_uint iEventTypeIndex, const CEvent& Event)
	{
		auto& Vector = m_Subscribers[iEventTypeIndex][typeid(Event).hash_code()];
		for (auto& Callback : Vector)
			Callback(Event);
	}

public:
	HRESULT Initialize();
	void	Clear();

private:
	SUBSCRIBERS*			m_Subscribers = { nullptr };

public:
	static CEventBus*	Create();
	virtual void		Free() override;
};

NS_END