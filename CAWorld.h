#ifndef CAWORLD_H
#define CAWORLD_H

#include <vector>
#include <functional>
#include <string>
#include "CATypes.h"
#include "Cell.h"
class Model; //forward dependency
class CAWorld
{
public:
    CAWorld(const Model &model);
	CAWorld(const std::string &model_name);//no implemented, load built-in model
    void step();
    void print_world();

private:
    int width = 0, height = 0, grid_size = 0;
	std::vector<std::vector<Cell>> grid;
    std::vector<std::function<int()>> diffX = std::vector<std::function<int()>>(8), diffY = std::vector<std::function<int()>>(8);
    type_no type_initializer(std::vector<std::pair<type_no, percentage>> &accum_dist);
    void fill_neighbors(std::vector<Cell *> &neighbors, int x, int y);
};
#endif
