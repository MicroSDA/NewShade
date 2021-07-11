#include "shade_pch.h"
#include "Environment.h"

shade::Environment::Environment(const shade::Environment::Type& type) :
	m_Type(type)
{
}

const shade::Environment::Type& shade::Environment::GetType() const
{
	return m_Type;
}