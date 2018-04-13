#include <random>
#include "Model.h"
#include "Cell.h"
#include "CAWorld.h"
#include "CATypes.h"
int main()
{
    auto process = process_type([](Cell *self, std::vector<Cell*> neighbors)
    {
        int surrounding = Cell::countSurroundingCellsWithValue(neighbors, "wasOpen");
        (*self)["open"] = ((*self)["wasOpen"] && surrounding >=4 || surrounding >= 6);
    });
    auto reset = reset_type([](Cell *self)
    {
        (*self)["wasOpen"] = (*self)["open"];
    });
    auto init = init_type([](Cell *self)
    {
        (*self)["open"] = (((double) rand() / (RAND_MAX)) > 0.4);
    });
    Model model(std::make_tuple(50, 50, 6), {std::make_tuple("Wall", 100, process, reset, init)});
    CAWorld world(model);
    for (int i = 0; i != 10; ++i)
        world.step();
    world.print_world();
	return 0;
}

