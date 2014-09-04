#include "mcl.hpp"
#include "mazeworld.hpp"
#include <iostream>
#include <cmath>

using namespace std;


namespace world = mazeworld;


std::ostream& operator<<(std::ostream& out, const mcl::MCL<world::Position>& mcl)
{
	for (auto p: mcl.particles)
	{
		out << "p[" << p.state.x << ", " << p.state.y << "] = " 
			<< p.weight << std::endl;
	}
	return out;
}


int main()
{
	static const world::State W = world::State::Wall;
	static const world::State E = world::State::Empty;

	{
		auto test_maze = mazeworld::generate_maze(10, 10);
		std::cout << get<0>(test_maze);
		return 0;
	}

	world::Maze maze{{E, E, E},
  		             {E, W, E},
        	         {E, W, E}};


    world::Position pos{0, 2};
    std::vector<world::Movement> track{{0,-1}, {0,-1}, {1, 0}};    	      	


    mcl::MCL<world::Position> locator{{0,0},{1,0},{2,0},{0,1},{2,1},{0,2},{2,2}};
    
    std::cout << "Sensor from (0,1): " << maze.measure_from({0,1})[0] << ", "
    	 << maze.measure_from({0,1})[1] 
    	 << std::endl;


    std::cout << maze;

    for (auto& m: track)
    {
    	pos = pos + m;
	    locator([&maze, pos](auto p){
	    			return mcl::weight_function(maze.measure_from(pos), 
	    								   maze.measure_from(p));}, 
	    		[m](auto p){return p + m;});    	      

	    std::cout << "================" << std::endl;
		std::cout << locator << std::endl;	
    }

    

}