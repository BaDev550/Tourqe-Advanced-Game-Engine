#pragma once

#include "TAGE/World/Objects/Entity.h"
#include <btBulletDynamicsCommon.h>

namespace TAGE::Physics {
	class PhysicsContactListener
	{
    public:
        struct Contact {
            Entity* EntityA;
            Entity* EntityB;
            ContactType Type;
        };

        std::vector<Contact> Contacts;

        void Clear() { Contacts.clear(); }
        void CheckContacts(btDynamicsWorld* world, entt::registry& registry);
	};
}