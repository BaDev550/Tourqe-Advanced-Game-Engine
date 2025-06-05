#pragma once

#include "Event.h"

namespace TAGE {
	class WindowCloseEvent : public Event {
	public:
		virtual const char* GetName() const override { return "WindowClose"; }
		virtual EEventType GetEventType() const override { return EEventType::WindowClose; }
	};

    class WindowResizeEvent : public Event {
    public:
        WindowResizeEvent(uint32_t width, uint32_t height)
            : m_Width(width), m_Height(height) {
        }

        uint32_t GetWidth() const { return m_Width; }
        uint32_t GetHeight() const { return m_Height; }

        virtual EEventType GetEventType() const override { return EEventType::WindowResize; }
        virtual const char* GetName() const override { return "WindowResizeEvent"; }

    private:
        uint32_t m_Width, m_Height;
    };
}