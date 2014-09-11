#pragma once

#include <initializer_list>
#include <vector>
#include <iostream>
#include <tuple>
#include <cassert> 
#include <stdexcept>
#include <algorithm>
#include <ctime>

#include <random>
#include <cmath>

namespace mazeworld
{

auto noise()
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::normal_distribution<> d(0,2);
    return static_cast<int>( std::round(d(gen)));
}

struct Position
{
	int x, y;
};


struct Movement
{
	int x, y;
};


inline Position operator+(Position p, Movement m)
{
	return {p.x + m.x, p.y + m.y};
}


enum class State {Undef, Wall, Empty};


/**
 */
class Maze
{

public:

	Maze(std::initializer_list<std::vector<State>> list)
	{		
		_height = list.size( );
		assert(_height > 0);
		_width = begin(list)->size();
		assert(_width > 0);

		for (auto& r: list)
		{
			assert(r.size() == _width);
			cells.insert(begin(cells), begin(r), end(r));
		}
	}

    Maze(size_t w = 1, size_t h = 1)
	: _width(w)
	, _height(h)
	{
		if (_width == 0 || _height == 0)
			throw std::runtime_error("Cannot initialize empty maze");
		cells.resize(w*h, State::Undef);
	}

	size_t width() {return _width;}
	size_t height() {return _height;}

	void set_position(Position p)
	{
		pos = p;
	}

	bool move(Movement m)
	{
		if (cell(pos + m) == State::Wall)
			return false;
		pos = pos + m;
		return true;
	}

	State cell(Position p) const 
	{
		if (p.x < _width && p.y < _height )
			return cells[p.y*_width + p.x];
		return State::Wall;  
	}

	void set_cell(Position p, State s) 
	{
		if (p.x < _width && p.y < _height )
			cells[p.y*_width + p.x] = s;
	}	

	std::vector<int>
	measure_from(Position pos, bool nc = false)
	{
		if (cell(pos) == State::Wall) return {100,100};
		int x = 0;
		while(cell(pos + Movement{x,0}) != State::Wall) x++;
		int y = 0;
		while(cell(pos + Movement{0,y}) != State::Wall) y++;
		if (nc)
			return {x + noise(),y + noise()};
		else
			return {x,y};
	}


	friend std::ostream& operator<<(std::ostream&, const Maze&);	


private:
	Position pos;
	size_t _width;
	size_t _height;
	std::vector<State> cells;
};






std::tuple<Maze, Position, std::vector<Movement>> 
generate_maze(size_t w, size_t h)
{
	Maze maze(w, h);

	std::srand ( unsigned ( std::time(0) ) );

	std::vector<Movement> moves{{0,-1}, {-1,0}, {0,1}, {1,0}};
	std::vector<Movement> surr{{-1,-1}, {-1,0}, {-1,1}, 
					          {0,-1},          {0, 1},
					      	  {1, -1}, {1,0}, {1,1}};

	std::vector<Movement> longets_path;		
	Position best_start_pos;		      	  
    for (int i = 0; i < maze.width(); ++i)
    {
    	for (int j = 0; j < maze.height(); ++j)
    	{
    		Position pos{i, j};
    			
			int c = 0;
			for(auto sr: surr)
				if (maze.cell(pos + sr) == State::Empty)
					c++;
			if (c > 2) continue;
			std::vector<Movement> path;				      	  
    
			while(true)
			{
				std::vector<Movement> possible_moves;
				std::copy_if(std::begin(moves), std::end(moves), 
							 std::back_inserter(possible_moves),
		                     [&](Movement x) { return maze.cell(pos + x) == State::Undef;});

				if (possible_moves.empty()) break;
				std::random_shuffle(std::begin(possible_moves), std::end(possible_moves));
				bool stop = true;
				for(auto m: possible_moves)
				{
					int c = 0;
					for(auto sr: surr)
						if (maze.cell(pos + m + sr) == State::Empty)
							c++;
					if (c < 3)
					{
						path.push_back(m);
						pos = pos + m;
						maze.set_cell(pos, State::Empty);
						stop = false;
						break;	
					}
				}				
				if (stop) break;
			}

			if (path.size() > longets_path.size())
			{
				std::swap( path, longets_path);
				best_start_pos = {i, j};
			}
		}
	}
	for (int i = 0; i < maze.width(); ++i)
    {
    	for (int j = 0; j < maze.height(); ++j)
    	{
    		if (maze.cell({i,j}) == State::Undef)
    			maze.set_cell({i,j}, State::Wall);
    	}
    }
	return std::make_tuple(maze, best_start_pos, longets_path);
}



std::ostream& operator<<(std::ostream& out, const Maze& m)
{
	out << '|' << std::string(m._width, '_') << std::endl;
	for(int i = 0; i < m._height; ++i)
	{
		out << "|";	
		for (int j = 0; j < m._width; ++j)
		{
			switch(m.cell({j,i}))
			{
				case State::Undef: out << "*"; break;
				case State::Wall:  out << "W"; break;
				case State::Empty: out << " "; break;
			}			
		}
		out << "|\n";	
	}
	out << '|' << std::string(m._width, '_') << '|' << std::endl;	
	return out;
}



}
