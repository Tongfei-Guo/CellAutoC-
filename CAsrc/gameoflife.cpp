//test purpose, don't delete until production
#include <random>
#include "Model.h"
#include "Cell.h"
#include "CAWorld.h"
#include "CATypes.h"
#include "CAFunctions.h"
#include <chrono>
#include <iostream>
int main()
{
    auto process = process_type([] (const grid_type &grid, Cell *self)
    {
        std::vector<Cell*> neighbors = get_neighbors(grid, self->x, self->y);
        auto cnt=countSurroundingCellsWithValue(neighbors, "wasAlive");
        (*self)["alive"]=(cnt==3 || (cnt==2 && (*self)["alive"]));
    });
    auto reset = reset_type([](Cell *self)
    {
        (*self)["wasAlive"]=(*self)["alive"];
    });
    auto init = init_type([](Cell *self)
    {
        (*self)["alive"] = (rand()>RAND_MAX/2);
    });
    auto getcolor = getcolor_type([](Cell *self)
    {
    	if ((*self)["alive"]==0)
            return 1;
        else
            return 0;
    });

/*
    std::vector<bitcolor> palette = {
    {68, 36, 52, 255},
    {255, 255, 255, 255}
    };
    */

    //CAWorld world1(Model(world_param_type(100, 50, 6), { grid_param_type("Wall", 100, process, reset, init) },0));
    //CAWorld world2(Model(world_param_type(100, 50, 6), { grid_param_type("Wall", 100, process, reset, init) },1));
    CAWorld world(Model(world_param_type(96, 64, 6), { grid_param_type("living", 100, process, reset, init) },1, getcolor));
    //world.AddMeasure(new CADistributionMeasure());
	auto start = std::chrono::high_resolution_clock::now();
	world.forall_step(1000);
    auto elapsed = std::chrono::high_resolution_clock::now() - start;
    auto nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed).count();
    std::cout << nanoseconds << "nanoseconds\n";
    auto timestamp = world.get_timestamps();
    world.print_test(timestamp, 0);
    world.AddMeasureAndRun(new CADistributionMeasure("Final Dist"));

    for(auto m: world.GetMeasures())
        std::cout<<"========="<<m->GetName()<<"========="<<std::endl<<m->Str_All()<<std::endl;

	return 0;
}
