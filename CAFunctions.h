#ifndef CA_FUNCTIONS_H
#define CA_FUNCTIONS_H

#include "CATypes.h"
#include "CAException.h"
#include <vector>
#include <utility>
class Cell;

enum neighborindex
{
    TOPLEFT = 0,
    TOP = 1,
    TOPRIGHT = 2,
    LEFT = 3,
    RIGHT = 4,
    BOTTOMLEFT = 5,
    BOTTOM = 6,
    BOTTOMRIGHT = 7
};

int countSurroundingCellsWithValue(const std::vector<Cell *> &neighbors, const state_name &state);//count number of neighboring cells has states value greater than or equal to 1

int getSurroundingCellsAverageValue(const std::vector<Cell *> &neighbors, const state_name &state);//get average value of neighboring cells

std::vector<Cell*> get_neighbors(const grid_type &grid, int x, int y);//find the neighbors of the cell given by grid[x][y]

#endif // CA_FUNCTIONS_H
