#include "Model.h"
#include <vector>
#include <string>
#include <algorithm>
#include <iterator>

// TODO : should we warn user when grid type of the same name is being added?
Model::Model(world_param_type param,std::vector<grid_param_type> types,unsigned size,getcolor_type getcolor_func)
: world_param(param),getcolor(getcolor_func)
{
	for (grid_param_type &type : types)
	{
		_add_grid_type(type);
	}
	buffersize = size;
}

void Model::add_grid_type(grid_param_type type)
{
	_add_grid_type(type);
}

void Model::add_grid_type(const type_name &name, percentage percent, process_type process, reset_type reset, init_type init)
{
	grid_param_type_no_name param = grid_param_type_no_name(percent, process, reset, init);
	grid_types[name] = std::move(param);
}
