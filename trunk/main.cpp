/*
Copyright (c) 2005-2007 Lode Vandevenne
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of Lode Vandevenne nor the names of his contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
using namespace lpi;


int main(int, char*[]) //the arguments have to be given here, or DevC++ can't link to SDL for some reason
{
  screen(1024, 768, 0, "Lode's GUI system");
  
  gui::Container c;
  
  gui::Button sound_button;
  sound_button.makeTextPanel(20, 500, "sound");
  c.pushTop(&sound_button);
  
  gui::Scrollbar bar;
  bar.makeVertical(700, 20, 200);
  c.pushTop(&bar);
  
  gui::Slider sli;
  sli.makeVertical(650, 20, 200);
  c.pushTop(&sli);
  
  gui::Checkbox cb;
  cb.make(500, 20);
  c.pushTop(&cb);
  
  gui::BulletList bl;
  bl.make(500, 80, 8, 0, 24);
  c.pushTop(&bl);
  
  gui::Button tb;
  tb.makeText(20, 540, "Text Button");
  c.pushTop(&tb);
  
  gui::Button tb_unittest;
  tb_unittest.makeText(20, 550, "Unit Test");
  c.pushTop(&tb_unittest);
  
  gui::Window w;
  w.make(50, 50, 300, 300);
  w.addTop();
  w.addTitle("Window 1");
  w.addCloseButton();
  w.setColor(RGBA_White(192));
  c.pushTop(&w);
  
  gui::Button wb;
  wb.makeTextPanel(0, 0, "window button");
  wb.autoTextSize(4);
  wb.centerText();
  w.pushTopAt(&wb, 20, 20);
  
  gui::Window w2;
  w2.make(100, 100, 300, 300);
  w2.addTop();
  w2.addTitle("Window 2");
  w2.addCloseButton();
  w2.addResizer();
  w2.setColor(RGBA_Red(192));
  c.pushTop(&w2);
  
  gui::Checkbox wcb;
  wcb.make(0, 0);
  w2.pushTopAt(&wcb, 20, 20);
  
  std::vector<double> sound(30000);
  for(size_t i = 0; i < 30000; i++) sound[i] = 0.3 * std::sin(i / (30.0 * (40000.0-i)/30000.0));
  
  audioOpen(44100, 2048);
  
  while(frame(true, true))
  {
    SDL_Delay(5);
    
    print("lpi GUI demo");
    
    drawGradientDisk(600, 400, 100, ColorRGB(128, 255, 128, 255), ColorRGB(255, 128, 128, 128));
    
    gui::builtInTexture[37].draw(0, 50);
    
    c.draw();
    c.handle();
    
    if(sound_button.pressed()) audioPlay(sound);
    
    if(tb_unittest.pressed()) lpi::gui::unitTest();
    
    redraw();
    cls(RGB_Darkgreen);
  }
  
  
  return 0;
}



