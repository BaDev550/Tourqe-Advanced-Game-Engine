#pragma once

#include "Event.h"

namespace TAGE {
    class Start : Event {
    public:
        Start() {}

        virtual EEventType GetEventType() const override { return EEventType::AppStarted; }
        virtual const char* GetName() const override { return "StartEvent"; }
    };

    class Tick : Event {
    public:
        Tick(float deltatime) : _DeltaTime(deltatime) {}

        virtual EEventType GetEventType() const override { return EEventType::AppTick; }
        virtual const char* GetName() const override { return "TickEvent"; }
    private:
        float _DeltaTime;
    };
}