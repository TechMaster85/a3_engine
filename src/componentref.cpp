#include "componentref.h"

#include "actor.h"

ComponentRef::ComponentRef(const luabridge::LuaRef &r) : ref(r) {
    hasOnStart = ref["OnStart"].isFunction();
    hasOnUpdate = ref["OnUpdate"].isFunction();
    hasOnLateUpdate = ref["OnLateUpdate"].isFunction();
    hasOnCollisionEnter = ref["OnCollisionEnter"].isFunction();
    hasOnCollisionExit = ref["OnCollisionExit"].isFunction();
    hasOnTriggerEnter = ref["OnTriggerEnter"].isFunction();
    hasOnTriggerExit = ref["OnTriggerExit"].isFunction();
    hasOnDestroy = ref["OnDestroy"].isFunction();
}

void ComponentRef::reportError(const std::string &error) const {
    Actor *actor = nullptr;
    if (ref["actor"].isUserdata()) {
        actor = ref["actor"].cast<Actor *>();
    }
    const std::string actorName = (actor != nullptr) ? actor->getName() : "?";
    std::cout << "\033[31m" << actorName << " : " << error << "\033[0m\n";
}
