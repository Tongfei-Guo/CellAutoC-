#ifndef CAWORLD_H
#define CAWORLD_H

#include <vector>
#include <functional>
#include <string>
#include <memory>
#include "CATypes.h"
#include "Cell.h"
class Model; //forward dependency


class CAWorld
{
public:
    CAWorld(const Model &model);
	CAWorld(const std::string &model_name);//no implemented, load built-in model
	CAWorld(const CAWorld &rhs);
	CAWorld(CAWorld &&rhs) noexcept;
	CAWorld &operator=(const CAWorld &rhs);
	CAWorld &operator=(CAWorld &&rhs) noexcept;
	~CAWorld(); // TODO : free cell memory.
	void step(unsigned steps);
	std::vector<int> print_world();
    void print_test();//don't delete until production.
	CAWorld &combine(const CAWorld &world, unsigned r_low, unsigned r_high, unsigned c_low, unsigned c_high);
	CAWorld &combine(CAWorld &&world, unsigned r_low, unsigned r_high, unsigned c_low, unsigned c_high);

private:
    unsigned width, height, grid_size;
	std::vector<std::vector<Cell*>> grid;
    static std::vector<std::function<int()>> diffX, diffY;
	void combine_error_check(const CAWorld &world, unsigned r_low, unsigned r_high, unsigned c_low, unsigned c_high);
    type_name type_initializer(const std::vector<std::pair<type_name, percentage>> &accum_dist);
    void fill_neighbors(std::vector<Cell*> &neighbors, int x, int y);
	void _step();
	void copy_grid(const CAWorld &rhs);
	void delete_grid();
};
#endif
