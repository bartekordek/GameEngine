#pragma once

#include "gameengine/Import.hpp"
#include "CUL/GenericUtils/NonCopyable.hpp"

namespace LOGLW
{
class CScene
{
public:
    CUL_NONCOPYABLE( CScene )
protected:
private:
    CScene();
    ~CScene();

    friend class CSceneStore;
};
}  // namespace LOGLW