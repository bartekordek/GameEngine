#pragma once

#include "DebugUtil/DebugSystemBase.hpp"

NAMESPACE_BEGIN(LOGLW)

class DebugSystemDX12 final: public DebugSystemBase
{
public:
protected:
private:

	// Inherited via DebugSystemBase
	virtual void init() override;
	virtual void frame() override;
};

NAMESPACE_END(END)