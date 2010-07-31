/*
Copyright (c) 2005-2010 Lode Vandevenne
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

#pragma once

namespace lpi
{

/*
This class contains nothing that depends on OpenGL libraries. And this most
certainly cannot do things like drawing OpenGL objects or so.

It is merely a class representing whether or not there is an OpenGL context active and
if there is one active, it has a certain index associated with it, so that
you can recognise if it's the same as before, or if it's a new context.
*/
class GLContext
{
  private:
    bool active;
    int index;
  public:
    GLContext();
    ~GLContext();
    
    bool isActive() const; //is there an OpenGL context active?
    int getID() const; //unique ID of the current OpenGL context (only valid if isActive() is true)

    void onNewGLContext(); //call if a new context is made for the first time, or if one is replaced (then the previous is destroyed, no need to use onGLContextDestroyed in that case)
    void onGLContextDestroyed(); //call if the GL context is destroyed and no new one is available instead.
};


} //namespace lpi

