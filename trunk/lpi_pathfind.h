/*
Copyright (c) 2005-2007 Lode Vandevenne
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of Lode Vandevenne nor the names of his contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef LPI_PATHFIND_H_INCLUDED
#define LPI_PATHFIND_H_INCLUDED

#include <vector>

namespace lpi
{

struct PathFindPos //for 2D world pathfind algorithms
{
  int x;
  int y;
  
  PathFindPos(int x, int y)
  {
    this->x = x;
    this->y = y;
  }
  
  PathFindPos(){}
};

inline bool operator==(const PathFindPos& a, const PathFindPos& b)
{
  return a.x == b.x && a.y == b.y;
}

class World_AStar //the algorithm is implemented in this class that you need to inherit from
{
  public:
    virtual bool blocks(int x, int y) const = 0;
    
    virtual ~World_AStar(){}

    /*
    world is a 2D tilemap, with rectangular tiles that are either filled or empty, supporting positive
    and negative coordinates of tiles. corner0,corner1 are the borders inside the world in which to work.
    the output path vector must be empty when you give it as parameter
    */
    bool findPath(std::vector<PathFindPos>& o_path, const PathFindPos& start, const PathFindPos& end, const PathFindPos& corner0, const PathFindPos& corner1);

    /*
    A* algorithm description - from GameDev.net article:
    
    1. Add the starting square (or node) to the open list. 
    2. Repeat the following: 
    a) Look for the lowest F cost square on the open list. We refer to this as the current square. 
    b) Switch it to the closed list. 
    c) For each of the 8 squares adjacent to this current square … 
    If it is not walkable or if it is on the closed list, ignore it. Otherwise do the following.
    If it isn't on the open list, add it to the open list. Make the current square the parent of this square. Record the F, G, and H costs of the square.
    If it is on the open list already, check to see if this path to that square is better, using G cost as the measure. A lower G cost means that this is a better path. If so, change the parent of the square to the current square, and recalculate the G and F scores of the square. If you are keeping your open list sorted by F score, you may need to resort the list to account for the change.
    
    d) Stop when you: 
    -Add the target square to the closed list, in which case the path has been found (see note below), or
    -Fail to find the target square, and the open list is empty. In this case, there is no path.
    3. Save the path. Working backwards from the target square, go from each square to its parent square until you reach the starting square. That is your path.
    */

    static void makeDirectionPath(std::vector<PathFindPos>& path)
    {
      for(int i = 0; i < int(path.size() - 1); i++)
      {
        path[i].x = path[i + 1].x - path[i].x;
        path[i].y = path[i + 1].y - path[i].y;
      }
      
      path.pop_back();
    }
};

void test_pathfind();

} //namespace lpi
#endif //LPI_ALGORITHMS_H_INCLUDED
