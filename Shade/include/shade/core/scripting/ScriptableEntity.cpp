#include "shade_pch.h"
#include "ScriptableEntity.h"

shade::ScriptableEntity::~ScriptableEntity()
{
	SHADE_INFO("ScriptableEntity id: {0} has been destroyed.", static_cast<uint32_t>(m_Entity))
}

void shade::ScriptableEntity::SetUpdate(const bool& isUpdate)
{
	m_IsUpdate = isUpdate;
}

const bool& shade::ScriptableEntity::IsUpdate() const
{
	return m_IsUpdate;
}