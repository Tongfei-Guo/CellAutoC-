#ifndef CELL_H
#define CELL_H

#include <vector>
#include <tuple>
#include <string>
#include <functional>
#include <map>
#include <deque>
#include <algorithm>
#include <memory>
#include "CATypes.h"
#include "CAException.h"
#include "Model.h"
#include <iostream>
class CellHistBounded; //forward dependency
class CellHistUnbounded; //forward dependency
class Cell
{
friend class CAWorld;
friend class CellHistBounded;
friend class CellHistUnbounded;
public:
	// Cell copy-control
    Cell(){}
	Cell(const Cell &) = default;
	Cell(Cell &&) noexcept = default;
	Cell& operator=(const Cell &) = default;
	Cell& operator=(Cell &&) noexcept = default;
	virtual ~Cell() = default;
	// auxiliary functions
	inline state_value &operator[](const state_name &state);
	inline void set_type(const type_name &rhs_type);
	inline const type_name &get_type() const;
	inline const std::map<state_name, state_value> & get_states() const;
	virtual inline Cell get_frame(unsigned i) const;
	virtual inline const unsigned frame_size() const;
    int x,y;

protected:
    static std::map<type_name, std::tuple<process_type, reset_type, init_type>> type_aux_funcs;
	std::map<state_name, state_value> states; // TODO : state can be arbitrary type?
    type_name type;
    // auxiliary functions
	static inline const type_name &_add_type(const std::pair<type_name, Model::grid_param_type_no_name> &pair);
	inline void _set_type(const type_name &rhs_type);
	inline const process_type&_call_process() const;
	inline const reset_type&_call_reset() const;
	inline const init_type&_call_init() const;
	virtual void prepare_process() {} // does nothing in the base class
	virtual inline void frame_resize(unsigned size);
	// the following functions are for deep copy&move between Cell type and its derived types.
	// _clone should always be called with a _move, it will be free within the immediate _move where it gets called.
	virtual inline Cell *_clone() const &;
	virtual inline Cell *_clone() && ;
	virtual inline void _move(Cell *cell);
	virtual inline void _move(CellHistBounded *cell);
	virtual inline void _move(CellHistUnbounded *cell);
};

class CellHistBounded : public Cell
{
friend class CellHistUnbounded;
friend class CAWorld;
public:
    // CellHistBounded copy control
    CellHistBounded() = default;
	CellHistBounded(unsigned buffersize) : Cell(), type_hist(buffersize), states_hist(buffersize) {}
	CellHistBounded(const CellHistBounded &) = default;
	CellHistBounded(CellHistBounded &&) noexcept = default;
	CellHistBounded& operator=(const CellHistBounded &) = default;
	CellHistBounded& operator=(CellHistBounded &&) noexcept = default;
	virtual ~CellHistBounded() final = default;
	virtual inline Cell get_frame(unsigned i) const final;
	virtual inline const unsigned frame_size() const final;
private:
	std::deque<type_name> type_hist;
	std::deque<std::map<state_name, state_value>> states_hist;
	// auxiliary functions
	virtual inline void prepare_process() final;
	virtual inline void frame_resize(unsigned size) final;
	// deep copy&move
	virtual inline CellHistBounded *_clone() const & final;
	virtual inline CellHistBounded *_clone() && final;
	virtual inline void _move(Cell *cell) final;
	virtual inline void _move(CellHistBounded *cell) final;
	virtual inline void _move(CellHistUnbounded *cell) final;
};

class CellHistUnbounded : public Cell
{
friend class CellHistBounded;
public:
    // CellHistUnbounded copy control
	CellHistUnbounded() = default;
	CellHistUnbounded(const CellHistUnbounded &) = default;
	CellHistUnbounded(CellHistUnbounded &&) noexcept = default;
	CellHistUnbounded& operator=(const CellHistUnbounded &) = default;
	CellHistUnbounded& operator=(CellHistUnbounded &&) noexcept = default;
	virtual ~CellHistUnbounded() final = default;
	virtual inline Cell get_frame(unsigned i) const final;
	virtual inline const unsigned frame_size() const final;
private:
	std::vector<type_name> type_hist;
	std::vector<std::map<state_name, state_value>> states_hist;
	// auxiliary functions
	virtual inline void prepare_process() final;
	virtual inline void frame_resize(unsigned size) final;
	// deep copy&move
	virtual inline CellHistUnbounded *_clone() const & final;
	virtual inline CellHistUnbounded *_clone() && final;
	virtual inline void _move(Cell *cell) final;
	virtual inline void _move(CellHistBounded *cell) final;
	virtual inline void _move(CellHistUnbounded *cell) final;
};

/* Cell inlines */
inline state_value &Cell::operator[](const state_name &state)
{
	return states[state];
}

inline void Cell::set_type(const type_name &rhs_type)
{
	if (type_aux_funcs.find(rhs_type) == type_aux_funcs.end())
    {
        throw nonexist_type("void Cell::set_type(const std::string &) funciton call : type " + rhs_type + " does not exist.");
	}
	type = rhs_type;
}

inline const type_name &Cell::get_type() const
{
	return type;
}

inline const type_name &Cell::_add_type(const std::pair<type_name, Model::grid_param_type_no_name> &pair)
{
	type_aux_funcs[pair.first] = std::make_tuple(std::get<1>(pair.second), std::get<2>(pair.second), std::get<3>(pair.second));
	return pair.first;
}

inline void Cell::_set_type(const type_name &rhs_type)
{
    type = rhs_type;
}


inline const process_type&Cell::_call_process() const
{
	return std::get<0>(type_aux_funcs.at(type));
}

inline const reset_type&Cell::_call_reset() const
{
	return std::get<1>(type_aux_funcs.at(type));
}

inline const init_type&Cell::_call_init() const
{
	return std::get<2>(type_aux_funcs.at(type));
}

inline const unsigned Cell::frame_size() const
{
    return 1;
}

inline void Cell::frame_resize(unsigned size)
{
    throw internal_error("void Cell::frame_resize(" + std::to_string(size) + ")" ); // this function shouldn't be called
}
inline Cell Cell::get_frame(unsigned i) const
{
    if (i == 0)
    {
        return (*this);
    }
    else
        throw internal_error("Cell Cell::get_frame(" + std::to_string(i) + ")" );
}

inline Cell *Cell::_clone() const &
{
	return new Cell(*this);
}

inline Cell *Cell::_clone() &&
{
	return new Cell(std::move(*this)); // do i need move here?
}

inline void Cell::_move(Cell *cell)
{
	type = std::move(cell->type);
	states = std::move(cell->states);
    delete cell;
}

inline void Cell::_move(CellHistBounded *cell)
{
	type = std::move(cell->type);
	states = std::move(cell->states);
	delete cell;
}
inline void Cell::_move(CellHistUnbounded *cell)
{
	type = std::move(cell->type);
	states = std::move(cell->states);
	delete cell;
}


inline const std::map<state_name, state_value> & Cell::get_states() const
{
    return states;
}





/* CellHistBounded inlines */
inline void CellHistBounded::prepare_process()
{
	type_hist.pop_front();
	type_hist.push_back(type);
	states_hist.pop_front();
	states_hist.push_back(states);
}

inline const unsigned CellHistBounded::frame_size() const
{
    return type_hist.size()+1;
}

inline void CellHistBounded::frame_resize(unsigned size)
{
    //this is only used for CellUnbounded intialization
    type_hist.resize(size-1);
    states_hist.resize(size-1);
}

inline Cell CellHistBounded::get_frame(unsigned i) const
{
    if (i == 0)
    {
        return (*this);
    }
    else if (i <= type_hist.size())
    {
        Cell cell;
        cell.type = type_hist[type_hist.size()-i];
        cell.states = states_hist[type_hist.size()-i];
        cell.x=x;
        cell.y=y;
        return cell;
    }
    else
        throw internal_error("Cell CellHistBounded::get_frame(" + std::to_string(i) + ")" );
}

inline CellHistBounded *CellHistBounded::_clone() const &
{
	return new CellHistBounded(*this);
}

inline CellHistBounded *CellHistBounded::_clone() &&
{
	return new CellHistBounded(std::move(*this));
}

inline void CellHistBounded::_move(Cell *cell)
{
	type = std::move(cell->type);
	states = std::move(cell->states);
	auto size = type_hist.size();
	type_hist = std::deque<type_name>(size);
	states_hist = std::deque<std::map<state_name, state_value>>(size);
	delete cell;
}

inline void CellHistBounded::_move(CellHistBounded *cell)
{
    auto diff_size = type_hist.size() - cell->type_hist.size();
	(*this) = std::move(*cell);
	while (diff_size != 0)
    {
        if (diff_size > 0)
        {
            --diff_size;
            type_hist.push_front(type_name());
            states_hist.push_front(std::map<state_name, state_value>());
        }
        else
        {
            ++diff_size;
            type_hist.pop_front();
            states_hist.pop_front();
        }
    }
    delete cell;
}

inline void CellHistBounded::_move(CellHistUnbounded *cell)
{
    type = std::move(cell->type);
	states = std::move(cell->states);
	auto size1 = type_hist.size(), size2 = cell->type_hist.size();
	type_hist = std::deque<type_name>(size1);
	states_hist = std::deque<std::map<state_name, state_value>>(size1);
	copy(cell->type_hist.crbegin(), cell->type_hist.crbegin() + std::min(size1, size2), type_hist.rbegin());
	copy(cell->states_hist.crbegin(), cell->states_hist.crbegin() + std::min(size1, size2), states_hist.rbegin());
    delete cell;
}

/*CellHistUnbounded inlines*/
void CellHistUnbounded::prepare_process()
{
	type_hist.push_back(type);
	states_hist.push_back(states);
}

inline const unsigned CellHistUnbounded::frame_size() const
{
    return type_hist.size()+1;
}

inline void CellHistUnbounded::frame_resize(unsigned size)
{

    if (type_hist.size() < size-1)
    {
        std::vector<type_name> temp_type_hist(size-1);
        std::copy(type_hist.crbegin(), type_hist.crend(), temp_type_hist.rbegin());
        type_hist = std::move(temp_type_hist);
        std::vector<std::map<state_name, state_value>> temp_states_hist(size-1);
        std::copy(states_hist.crbegin(), states_hist.crend(), temp_states_hist.rbegin());
        states_hist = std::move(temp_states_hist);
    }
}

inline Cell CellHistUnbounded::get_frame(unsigned i) const
{
    if (i == 0)
    {
        return (*this);
    }
    else if (i <= type_hist.size())
    {
        Cell cell;
        cell.type = type_hist[type_hist.size()-i];
        cell.states = states_hist[type_hist.size()-i];
        cell.x=x;
        cell.y=y;
        return cell;
    }
    else
        throw internal_error("Cell CellHistUnbounded::get_frame(" + std::to_string(i) + ")" );
}

inline CellHistUnbounded *CellHistUnbounded::_clone() const &
{
	return new CellHistUnbounded(*this);
}

inline CellHistUnbounded *CellHistUnbounded::_clone() &&
{
	return new CellHistUnbounded(std::move(*this));
}

inline void CellHistUnbounded::_move(Cell *cell)
{
    std::fill(type_hist.begin(), type_hist.end(), type_name(""));
    std::fill(states_hist.begin(), states_hist.end(), std::map<state_name, state_value>());
	type = std::move(cell->type);
	states = std::move(cell->states);
	delete cell;
}

inline void CellHistUnbounded::_move(CellHistBounded *cell)
{
    type = std::move(cell->type);
	states = std::move(cell->states);
	auto size1 = type_hist.size(), size2 = cell->type_hist.size();
	if (size1 > size2)
    {
        type_hist = std::vector<type_name>(size1 - size2);
	    states_hist = std::vector<std::map<state_name, state_value>>(size1 - size2);
    }
	copy(cell->type_hist.cbegin(), cell->type_hist.cend(), std::back_inserter(this->type_hist));
	copy(cell->states_hist.cbegin(), cell->states_hist.cend(), std::back_inserter(this->states_hist));
	delete cell;
}

inline void CellHistUnbounded::_move(CellHistUnbounded *cell)
{
	type = std::move(cell->type);
	states = std::move(cell->states);
	auto size1 = type_hist.size(), size2 = cell->type_hist.size();
	if (size1 > size2)
    {
        type_hist = std::vector<type_name>(size1 - size2);
	    states_hist = std::vector<std::map<state_name, state_value>>(size1 - size2);
    }
	copy(cell->type_hist.cbegin(), cell->type_hist.cend(), std::back_inserter(this->type_hist));
	copy(cell->states_hist.cbegin(), cell->states_hist.cend(), std::back_inserter(this->states_hist));
	delete cell;
}

#endif
