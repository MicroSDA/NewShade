#pragma once
#include "shade/core/scripting/NativeScript.h"
#include "shade/core/camera/Camera.h"
#include "shade/core/Types.h"

namespace shade
{
	using NativeScriptComponent = NativeScript;
	using CameraComponent		= Shared<Camera>;
}