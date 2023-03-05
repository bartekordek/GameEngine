#pragma once

#include "gameengine/Import.hpp"

NAMESPACE_BEGIN( LOGLW )

class DebugSystemBase
{
public:
	DebugSystemBase();
	virtual void init() = 0;
	virtual void frame() = 0;
	~DebugSystemBase();
protected:
private:
};

NAMESPACE_END( LOGLW )