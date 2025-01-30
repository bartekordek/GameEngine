#pragma once

#include "SDL2Wrapper/IWindowFactory.hpp"
#include "CUL/Log/ILogger.hpp"
#include "CUL/STL_IMPORTS/STD_memory.hpp"

NAMESPACE_BEGIN( SDL2W )

class WindowCreatorConcrete final:
    public IWindowFactory
{
public:
    explicit WindowCreatorConcrete( CUL::LOG::ILogger* logger );

    IWindow* createWindow(
        const WindowData& winData,
        ISDL2Wrapper* wrapper ) override;

    ~WindowCreatorConcrete();

protected:
private:
    CUL::LOG::ILogger* m_logger = nullptr;


    WindowCreatorConcrete( const WindowCreatorConcrete& rhv ) = delete;
    WindowCreatorConcrete( WindowCreatorConcrete&& rhv ) = delete;
    WindowCreatorConcrete& operator=( const WindowCreatorConcrete& rhv ) = delete;
    WindowCreatorConcrete& operator=( WindowCreatorConcrete&& rhv ) = delete;
};

NAMESPACE_END( SDL2W )