#pragma once

#include <initializer_list>
#include <vector>
#include <iostream>
#include <tuple>

namespace world
{


struct Position
{
	int x, y;
};

struct Movement
{
	int x, y;
};


Position operator+(Position p, Movement m)
{
	return {p.x + m.x, p.y + m.y};
}


enum class State {Wall, Empty};


class Maze
{

public:

	Maze(std::initializer_list<std::vector<State>> list) : v(list)
	{}

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

	State cell(Position p) 
	{
		if (p. y < v.size())
			if (p.x < v[p.y].size())
				return v[p.y][p.x];
		return State::Wall;  
	}

	std::vector<int>
	measure_from(Position pos)
	{
		if (cell(pos) == State::Wall) return {100,100};
		int x = 0;
		while(cell(pos + Movement{x,0}) != State::Wall) x++;
		int y = 0;
		while(cell(pos + Movement{0,y}) != State::Wall) y++;
		return {x,y};
	}


	friend std::ostream& operator<<(std::ostream&, const Maze&);	
private:
	Position pos;
	std::vector<std::vector<State>> v;
};


std::ostream& operator<<(std::ostream& out, const Maze& m)
{
	out << "____________________\n";
	for(auto& row: m.v)
	{
		out << "|";	
		for (auto cell: row)
		{
			out << (cell == State::Wall ? 'O' : ' ');
		}
		out << "|\n";	
	}
	out << "--------------------" << std::endl;
	return out;
}



}