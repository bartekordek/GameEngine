#pragma once

#include "CUL/STL_IMPORTS/STD_cstdint.hpp"
#include "CUL/STL_IMPORTS/STD_list.hpp"
#include "CUL/STL_IMPORTS/STD_mutex.hpp"

class Snake final
{
public:
    enum class HeadDirection: uint8_t
    {
        NORTH = 0,
        SOUTH,
        EAST,
        WEST,
        UNDEFINED
    };

    struct Pos
    {
        int row = 0;
        int col = 0;

        bool operator==( const Pos& rhv ) const
        {
            return row == rhv.row && col == rhv.col;
        }

    };

    Snake( int row, int col );

    bool nextMoveIsGameOver( int row, int col ) const;
    void move( bool increase = false );
    bool isFieldSnake( const Pos& position ) const;
    void reset();

    ~Snake();

    HeadDirection m_headDirection = HeadDirection::UNDEFINED;
protected:
private:

    Pos calculateNextPosition() const;

    mutable std::mutex m_snakeMtx;
    std::list<Pos> m_snakePositions;
};