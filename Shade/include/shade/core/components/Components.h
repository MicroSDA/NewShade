#pragma once
#include "shade/core/scripting/NativeScript.h"
#include "shade/core/camera/Camera.h"
#include "shade/core/mesh/Model3D.h"
#include "shade/core/transform/Transform3D.h"
#include "shade/core/transform/Transform2D.h"
#include "shade/core/environment/Environment.h"
#include "shade/core/environment/lighting/DirectLight.h"
#include "shade/core/environment/lighting/PointLight.h"
#include "shade/core/environment/lighting/SpotLight.h"
#include "shade/core/Types.h"
#include "shade/core/render/drawable/2d/Sprite.h"

namespace shade
{
	using NativeScriptComponent = NativeScript;
	using CameraComponent		= Shared<Camera>;
	using Model3DComponent		= Shared<Model3D>;
	using Transform3DComponent	= Transform3D;
	using Transform2DComponent	= Transform2D;
	using EnvironmentComponent	= Shared<Environment>;
	using DirectLightComponent	= Shared<DirectLight>;
	using PointLightComponent	= Shared<PointLight>;
	using SpotLightComponent	= Shared<SpotLight>;
	using SpriteComponent		= Shared<Sprite>;
}