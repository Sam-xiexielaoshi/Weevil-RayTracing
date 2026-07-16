#pragma once

#include "../EditorContext.h"

class Panel
{
public:
	virtual ~Panel() = default;
	virtual void Draw(EditorContext& context) = 0;
};