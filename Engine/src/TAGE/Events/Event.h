#pragma once

#include <functional>
#include <string>
#include "TAGE/Common/TEnums.h"

namespace TAGE {

#define EVENT_CLASS_TYPE(type) static EEventType GetStaticType() { return EEventType::type; }\
									virtual EEventType GetEventType() const override { return GetStaticType(); }\
									virtual const char* GetName() const override { return #type; }

#define EVENT_CLASS_CATEGORY(category) virtual int GetCategoryFlags() const override { return category; }

	class Event {
	public:
		virtual ~Event() = default;
		virtual EEventType GetEventType() const = 0;
		virtual const char* GetName() const = 0;
		virtual int GetCategoryFlags() const = 0;
		virtual std::string ToString() const { return GetName(); }
		bool IsInCategory(EventCategory category)
		{
			return GetCategoryFlags() & category;
		}

		bool bIsHandled = false;
	};

	class EventDispatcher {
		template<typename T>
		using EventFn = std::function<bool(T&)>;
	public:
		EventDispatcher(Event& event) : _Event(event) {}

		template<typename T, typename F>
		bool Dispatch(const F& func)
		{
			if (_Event.GetEventType() == T::GetStaticType())
			{
				_Event.bIsHandled |= func(static_cast<T&>(_Event));
				return true;
			}
			return false;
		}

	private:
		Event& _Event;
	};

	inline std::ostream& operator<<(std::ostream& os, const Event& e)
	{
		return os << e.ToString();
	}
}