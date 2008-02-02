/*
Copyright (c) 2005-2008 Lode Vandevenne
All rights reserved.

This file is part of Lode's Programming Interface.

Lode's Programming Interface is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Lode's Programming Interface is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Lode's Programming Interface.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "lpi_pathfind.h"

#include <list>
#include <iostream>

namespace
{
  struct Node
  {
    int x;
    int y;
    
    int f; //total cost, g + h
    int g; //cost from beginning to here
    int h; //estimated cost from here to end (heuristic)
    
    Node* parent;
    
    Node(int x, int y)
    {
      parent = 0;
      this->x = x;
      this->y = y;
    }
    
    Node(const Node& other)
    {
      *this = other;
    }
    
    Node()
    {
      parent = 0;
    }
  };
  
  bool operator==(const Node& a, const Node& b)
  {
    return a.x == b.x && a.y == b.y; //we only compare the coordinates, not the cost
  }
}

namespace lpi
{


//world is a 2D tilemap, with rectangular tiles that are either filled or empty, supporting positive and negative coordinates of tiles. x0y0x1y1 are the borders inside the world in which to work.
bool World_AStar::findPath(std::vector<PathFindPos>& o_path, const PathFindPos& start, const PathFindPos& end, const PathFindPos& corner0, const PathFindPos& corner1)
{
  Node startnode(start.x, start.y);
  Node endnode(end.x, end.y);
  
  struct NestedFunctions //functions nested inside this function for convenience
  {
    static int heuristic(const Node& current, const Node& end)
    {
      int hx = (end.x - current.x);
      int hy = (end.y - current.y);
      if(hx < 0) hx = -hx;
      if(hy < 0) hy = -hy;
      return 10 * (hx + hy);
    }
  };
  std::list<Node> open; //open list: squares to look at next. Using an std::list allows to insert new squares sorted by cost in it
  std::list<Node> closed; //closed list: squares we don't have to look at anymore for now
  size_t w = corner1.x - corner0.x, h = corner1.y - corner0.y;
  std::vector<bool> closed_table(w * h, false);
  
  startnode.g = 0;
  startnode.h = NestedFunctions::heuristic(startnode, endnode);
  startnode.f = startnode.g + startnode.h;
  startnode.parent = 0;
  open.push_back(startnode);
  
  bool path_exists;
  
  for(;;)
  {
    if(open.size() == 0)
    {
      //the open list is empty... no path is found
      path_exists = false;
      break;
    }
    
    Node current = open.front(); open.pop_front(); //take the one with lowest cost from the open list
    closed.push_back(current);
    closed_table[w * current.y + current.x] = true;
    
    
    if(current == endnode)
    {
      //the end is reached
      path_exists = true;
      break;
    }
    
    //find the adjendant squares
    for(int i = 0; i < 8; i++) //for each neighbor
    {
    
      Node neighbor(current);
      switch(i)
      {
        case 0: neighbor.y -= 1; break; // +-x->
        case 1: neighbor.x += 1; break; // y 704
        case 2: neighbor.y += 1; break; // | 3 1
        case 3: neighbor.x -= 1; break; // v 625
        case 4:
          neighbor.x += 1;
          neighbor.y -= 1;
          break;
        case 5:
          neighbor.x += 1;
          neighbor.y += 1;
          break;
        case 6:
          neighbor.x -= 1;
          neighbor.y += 1;
          break;
        case 7:
          neighbor.x -= 1;
          neighbor.y -= 1;
          break;
      }
      
      int extra_cost;
      if(i < 4) extra_cost = 10;
      else extra_cost = 14; //10 * sqrt(2)

      
      ///is the neigbhbor walkable?
      bool walkable = true;
      if(neighbor.x < corner0.x || neighbor.x >= corner1.x || neighbor.y < corner0.y || neighbor.y >= corner1.y) walkable = false;
      if(blocks(neighbor.x, neighbor.y)) walkable = false;
      //diagonal also blocks if two walls are in the way
      if(i >= 4)
      {
        if(blocks(neighbor.x, current.y)) walkable = false;
        if(blocks(current.x, neighbor.y)) walkable = false;
      }
      
      if(!walkable) continue;
      
      ///if the neighbor is already on the closed list, skip it
      bool already_on_closed_list = false;
      if(closed_table[w * neighbor.y + neighbor.x] == true) already_on_closed_list = true;
      /***for(std::list<Node>::iterator i = closed.begin(); i != closed.end(); i++)
      {
        if(neighbor == *i)
        {
        already_on_closed_list = true;
        break;
        }
      }***/
      
      if(already_on_closed_list) continue;
      
      neighbor.parent = &closed.back();
      
      neighbor.g = current.g + extra_cost;
      neighbor.h = NestedFunctions::heuristic(neighbor, endnode);
      neighbor.f = neighbor.g + neighbor.h; //the cost
      
      ///find if the neighbor is already on the open list
      bool better_one_already_on_open_list = false;
      for(std::list<Node>::iterator i = open.begin(); i != open.end(); )
      {
        if(neighbor == *i)
        {
          if(neighbor.f < (*i).f)
          {
            i = open.erase(i);
          }
          else
          {
            better_one_already_on_open_list = true;
            ++i;
          }
        }
        else ++i;
      }
      
      if(better_one_already_on_open_list) continue;
      
      ///place the neighbor node in the open list, correctly sorted by cost
      bool inserted = false;
      for(std::list<Node>::iterator i = open.begin(); i != open.end(); i++)
      {
        if(neighbor.f <= (*i).f)
        {
          open.insert(i, neighbor);
          inserted = true;
          break;
        }
      }
      if(!inserted) open.push_back(neighbor); //either open was empty, or neighbor had higher cost than everything in it, so insert it at the back now
    }
  }
  
  //The while loop is now over. Either a path is found, or there exists none. Now store the result in the return path.
  
  Node node = closed.back();
  o_path.push_back(PathFindPos(node.x, node.y));
  while(node.parent != 0)
  {
    node = *(node.parent);
    o_path.insert(o_path.begin(), PathFindPos(node.x, node.y));
  }
  
  return path_exists;
}

} //namespace lpi

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
///Test and example of pathfind
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#if 0 //LPI_PATHFIND_TEST

using namespace lpi;

namespace pathfindtest
{
  
  template<int SIZEX, int SIZEY>
  class TestWorld : public World_AStar
  {
    public:
    bool map[SIZEX][SIZEY];
    
    virtual bool blocks(int x, int y) const
    {
      return map[x][y];
    }
  };
  
  
  void test_pathfind()
  {
    const unsigned SIZE = 16;
    
    TestWorld<SIZE, SIZE> w;
    
    char display[SIZE][SIZE];
    
    for(size_t x = 0; x < SIZE; x++)
    for(size_t y = 0; y < SIZE; y++)
    {
      w.map[x][y] = !( ((x & y) == 0) || ((x - y) % 5 < 2) );
    }
    
    for(size_t x = 0; x < SIZE; x++)
    {
      for(size_t y = 0; y < SIZE; y++)
      {
        if(w.map[x][y]) display[x][y] = '#';
        else display[x][y] =  '.';
      }
    }
    
    std::vector<PathFindPos> path;
    
    bool path_exists = w.findPath(path, PathFindPos(0, 0), PathFindPos(SIZE - 1, SIZE - 1), PathFindPos(0, 0), PathFindPos(SIZE, SIZE));
    
    for(size_t i = 0; i < path.size(); i++)
    {
      display[path[i].x][path[i].y] = 'a' + i;
    }
    
    std::cout<<"\n";
    for(size_t x = 0; x < SIZE; x++)
    {
      for(size_t y = 0; y < SIZE; y++)
      {
        std::cout << display[x][y];
      }
      std::cout << "\n";
    }
    
    if(path_exists) std::cout << "\npath exists\n"; else std::cout << "\npath doesn't exist\n";
    std::cout << "path size: " << path.size() << "\n";
  }

  struct TEST
  {
    TEST()
    {
      test_pathfind();
    }
  } test;

}

#endif

