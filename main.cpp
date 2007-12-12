/*
Copyright (c) 2005-2007 Lode Vandevenne
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

/*
This is an example file that uses lpi to create a GUI.

A game example may be added later in the form of the "Tower Wars" project.
*/

//copypastable compile commands
/*
g++ *.cpp -lSDL -lGL
g++ *.cpp -lSDL -lGL -ansi -pedantic
g++ *.cpp -lSDL -lGL -Wall -Wextra -pedantic -ansi
g++ *.cpp -lSDL -lGL -Wall -Wextra -pedantic -ansi -O3

./a.out
-g3
-pg
gdb
valgrind
gprof > gprof.txt
*/


#include <SDL/SDL.h>
#include "lpi_color.h"
#include "lpi_file.h"
#include "lpi_event.h"
#include "lpi_general.h"
#include "lpi_text.h"
#include "lpi_texture.h"
#include "lpi_screen.h"
#include "lpi_gui.h"
#include "lpi_draw2dgl.h"
#include "lpi_audio.h"
#include "lpi_bignums.h"
#include "lodepng.h"
#include "lodewav.h"
#include <vector>
#include <iostream>
#include <cmath>

int main(int, char*[]) //the arguments have to be given here, or DevC++ can't link to SDL for some reason
{
  lpi::screen(1024, 768, 0, "Lode's GUI system");
  
  lpi::gui::Container c;
  
  lpi::gui::Button sound_button;
  sound_button.makeTextPanel(20, 500, "sound");
  c.pushTop(&sound_button);
  
  lpi::gui::Scrollbar bar;
  bar.makeVertical(700, 20, 200);
  c.pushTop(&bar);
  
  lpi::gui::Slider sli;
  sli.makeVertical(650, 20, 200);
  c.pushTop(&sli);
  
  lpi::gui::Checkbox cb;
  cb.make(500, 20);
  c.pushTop(&cb);
  
  lpi::gui::BulletList bl;
  bl.make(500, 80, 8, 0, 24);
  c.pushTop(&bl);
  
  lpi::gui::Button tb;
  tb.makeText(20, 540, "Text Button");
  c.pushTop(&tb);
  
  lpi::gui::Button tb_unittest;
  tb_unittest.makeText(20, 550, "Unit Test");
  c.pushTop(&tb_unittest);
  
  lpi::gui::Window w;
  w.make(50, 50, 300, 300);
  w.addTop();
  w.addTitle("Window 1");
  w.addCloseButton();
  w.setColor(lpi::RGBA_White(192));
  c.pushTop(&w);
  
  lpi::gui::Button wb;
  wb.makeTextPanel(0, 0, "window button");
  wb.autoTextSize(4);
  wb.centerText();
  w.pushTopAt(&wb, 20, 20);
  
  lpi::gui::Window w2;
  w2.make(100, 100, 300, 300);
  w2.addTop();
  w2.addTitle("Window 2");
  w2.addCloseButton();
  w2.addResizer();
  w2.setColor(lpi::RGBA_Red(192));
  c.pushTop(&w2);
  
  lpi::gui::Checkbox wcb;
  wcb.make(0, 0);
  w2.pushTopAt(&wcb, 20, 20);
  
  std::vector<double> sound(30000);
  for(size_t i = 0; i < 30000; i++) sound[i] = 0.3 * std::sin(i / (30.0 * (40000.0-i)/30000.0));
  
  lpi::audioOpen(44100, 2048);
  
  while(lpi::frame(true, true))
  {
    SDL_Delay(5);
    
    lpi::print("lpi GUI demo");
    
    lpi::drawGradientDisk(600, 400, 100, lpi::ColorRGB(128, 255, 128, 255), lpi::ColorRGB(255, 128, 128, 128));
    
    lpi::gui::builtInTexture[37].draw(0, 50);
    
    c.draw();
    c.handle();
    
    if(sound_button.pressed()) lpi::audioPlay(sound);
    
    if(tb_unittest.pressed()) lpi::gui::unitTest();
    
    lpi::redraw();
    lpi::cls(lpi::RGB_Darkgreen);
  }
  
  
  return 0;
}



