#include "tagepch.h"
#include "ScriptGlue.h"

#include "mono/jit/jit.h"
#include "mono/metadata/assembly.h"

namespace TAGE {
#define ADD_INTERNAL_CALL(icall) mono_add_internal_call("TAGE.InternalCalls::"#icall, (void*)icall)

	static void NativeLog(int logLevel, MonoString* string) {
		char* cStr = mono_string_to_utf8(string);
		std::string str(cStr);
		mono_free(cStr);

		switch (logLevel)
		{
		case 0:
			LOG_INFO(str);
			break;
		case 1:
			LOG_WARN(str);
			break;
		case 2:
			LOG_ERROR(str);
			break;
		default:
			LOG_INFO(str);
			break;
		}
	}

	static void NativeLog_Vector3(int logLevel, glm::vec3* vector) {
		switch (logLevel)
		{
		case 0:
			LOG_INFO(glm::to_string(*vector));
			break;
		case 1:
			LOG_WARN(glm::to_string(*vector));
			break;
		case 2:
			LOG_ERROR(glm::to_string(*vector));
			break;
		default:
			LOG_INFO(glm::to_string(*vector));
			break;
		}
	}

	void ScriptGlue::RegisterFunctions()
	{
		ADD_INTERNAL_CALL(NativeLog);
		ADD_INTERNAL_CALL(NativeLog_Vector3);
	}
}