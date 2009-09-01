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
#include "lpi_time.h"
#include "lpi_text.h"
#include "lpi_texture.h"
#include "lpi_screen.h"
#include "lpi_gui.h"
#include "lpi_gui_text.h"
#include "lpi_gui_ex.h"
#include "lpi_gui_input_sdl.h"
#include "lpi_gui_drawer_gl.h"
#include "lpi_draw2d.h"
#include "lpi_draw2dgl.h"
#include "lpi_audio.h"
#include "lpi_bignums.h"
#include "lpi_math.h"
#include "lpi_tools.h"
#include "lpi_gl.h"
#include "lpi_gui_unittest.h"
#include "lpi_xml.h"
#include "lodepng.h"
#include "lodewav.h"
#include "lpi_gui_dynamic.h"
#include "lpi_gui_color.h"
#include "lodepng.h"

#include <vector>
#include <iostream>
#include <cmath>

static const int width = 1024;
static const int height = 768;

struct SpawnText
{
  double x;
  double y;
  double vx;
  double vy;
  std::string text;
  double prev_time;
  lpi::ColorRGB color;
  
  SpawnText(std::string text, double x, double y, const lpi::ColorRGB& color = lpi::RGB_White)
  : x(x), y(y), text(text), prev_time(lpi::getSeconds()), color(color)
  {
    vx = lpi::getRandom(-1.0, 1.0);
    vy = lpi::getRandom(-1.0, 1.0);
    double len = std::sqrt(vx*vx+vy*vy);
    vx /= len;
    vy /= len;
    vx *= 100;
    vy *= 100;
  }
  
  void draw()
  {
    lpi::print(text, (int)x, (int)y, color);
  }
  
  void handle()
  {
    double dt = lpi::getSeconds() - prev_time;
    y += vy * (dt);
    x += vx * (dt);
    vy += 200.0 * dt;
    prev_time = lpi::getSeconds();
  }
};

struct SpawnTexts
{
  std::vector<SpawnText*> spawns;
  
  void addSpawn(const std::string& text, int x, int y, const lpi::ColorRGB& color = lpi::RGB_White)
  {
    spawns.push_back(new SpawnText(text, (double)x, (double)y, color));
  }
  
  void draw()
  {
    for(size_t i = 0; i < spawns.size(); i++)
      spawns[i]->draw();
  }
  
  void handle()
  {
    for(size_t i = 0; i < spawns.size(); i++)
    {
      spawns[i]->handle();
      if(spawns[i]->x < 0 || spawns[i]->x > width || spawns[i]->y < 0 || spawns[i]->y > height)
      {
        delete spawns[i];
        spawns.erase(spawns.begin() + i);
      }
    }
  }
};

SpawnTexts spawns;

void littletemporarytest()
{
  std::vector<unsigned char> buffer(512*512*4, 0);
  lpi::drawCircle(&buffer[0], 512, 512, 255, 255, 100, lpi::RGB_Lightblue);
  lpi::drawDisk(&buffer[0], 512, 512, 255, 255, 80, lpi::RGB_Lightred);
  lpi::drawBezier(&buffer[0],512,512, 60,10, 70,10, 75,20, 55,15, lpi::RGB_Yellow);
  lpi::drawFilledEllipse(&buffer[0], 512, 512, 255, 255, 150, 50, lpi::RGBA_Lightblue(128));
  LodePNG::encode("testje.png", buffer, 512, 512);
}

int main(int, char*[]) //the arguments have to be given here, or DevC++ can't link to SDL for some reason
{//std::cout<<sizeof(lpi::gui::Element)<<std::endl;//littletemporarytest();
  lpi::screen(width, height, 0, true, "lpi GUI demo");
  lpi::initFont();
  lpi::gui::GUIDrawerGL::init();
  
  lpi::gui::Container c;
  
  lpi::gui::Button tb;
  tb.makeText(20, 530, "Save Built In GUI To PNGs");
  c.pushTop(&tb);
  lpi::gui::Button tb2;
  tb2.makeText(20, 540, "GUI PNGs to base 64 TXT");
  c.pushTop(&tb2);
  
  lpi::gui::Button tb_unittest;
  tb_unittest.makeText(20, 550, "Unit Test");
  c.pushTop(&tb_unittest);
  
  lpi::gui::Window w1;
  w1.make(100, 100, 500, 500);
  w1.addTop();
  w1.addTitle("Window 1");
  w1.addCloseButton();
  w1.addResizer();
  w1.setColor(lpi::RGBA_Red(192));
  c.pushTop(&w1);

  lpi::gui::Window w2;
  w2.make(50, 50, 300, 300);
  w2.addTop();
  w2.addTitle("Window 2");
  w2.addCloseButton();
  w2.setColor(lpi::RGBA_White(224));
  w2.addResizer();
  c.pushTop(&w2);
  
  lpi::gui::Tabs tabs;
  tabs.resize(0, 0, w2.getSizeX(), w2.getSizeY() - 32);
  tabs.addTab("tab 1");
  tabs.addTab("tab 2");
  tabs.addTab("tab 3");
  tabs.addTab("tab 4");
  w2.pushTopAt(&tabs, 0, 16, lpi::gui::Sticky(0.0, 0, 0.1, 0, 1.0, 0, 0.9, 0));


  lpi::gui::Button sound_button;
  sound_button.makeTextPanel(20, 500, "sound");
  w1.pushTopAt(&sound_button, 15, 50);
  
  lpi::gui::Scrollbar hbar;
  hbar.makeHorizontal(716, 220, 200);
  w1.pushTopAt(&hbar, 150, 210);
  
  lpi::gui::Scrollbar vbar;
  vbar.makeVertical(700, 20, 200);
  w1.pushTopAt(&vbar, 350, 10);
  
  lpi::gui::Slider sli;
  sli.makeVertical(650, 20, 200);
  w1.pushTopAt(&sli, 300, 10, lpi::gui::Sticky(0.5, 0, 0.1, 0, 0.5, sli.getSizeX(), 0.5, 0));
  
  lpi::gui::Slider sli2;
  sli2.makeSmallHorizontal(650, 20, 200);
  w1.pushTopAt(&sli2, 150, 240);
  

  lpi::gui::Checkbox cb;
  cb.make(500, 20);
  w1.pushTopAt(&cb, 10, 600);
  
  /*lpi::gui::BulletList bl;
  bl.make(500, 80, 8, 0, 24);
  c.pushTop(&bl);*/
  
  lpi::gui::Button wb;
  wb.makeTextPanel(0, 0, "window button");
  wb.autoTextSize(4);
  wb.centerText();
  tabs.getTabContent(0).pushTopAt(&wb, 20, 50);
  //w2.pushTopAt(&wb, 20, 50);
  
  //lpi::ColorRGB dyn_value0 = lpi::RGBA_Lightblue(224);
  int dyn_value1 = 12345;
  std::string dyn_value2 = "hello";
  bool dyn_value3 = true;
  double dyn_value4 = 1.5;
  float dyn_value5 = 60.5;
  lpi::gui::DynamicPage dyn;
  //dyn.setSizeXY(200, 20);
  dyn.resize(0,0,280,100);
  dyn.setTitle("dynamic page");
  //dyn.addControl("color", new lpi::gui::DynamicColor(&dyn_value0));
  dyn.addControl("int", new lpi::gui::DynamicValue<int>(&dyn_value1));
  dyn.addControl("string", new lpi::gui::DynamicValue<std::string>(&dyn_value2));
  dyn.addControl("boolean", new lpi::gui::DynamicCheckbox(&dyn_value3));
  dyn.addControl("double", new lpi::gui::DynamicValue<double>(&dyn_value4));
  dyn.addControl("slider", new lpi::gui::DynamicSlider<float>(&dyn_value5, 0, 100));
  //dyn.resize(0,0,200,100);
  tabs.getTabContent(1).pushTopAt(&dyn, 10, 20);
  tabs.selectTab(1);

  lpi::gui::PVariable<int> tval1;
  tval1.make(0, 0, &dyn_value1);
  tabs.getTabContent(1).pushTopAt(&tval1, 10, 140);
  lpi::gui::PVariable<std::string> tval2;
  tval2.make(0, 0, &dyn_value2);
  tabs.getTabContent(1).pushTopAt(&tval2, 10, 148);
  lpi::gui::PVariable<bool> tval3;
  tval3.make(0, 0, &dyn_value3);
  tabs.getTabContent(1).pushTopAt(&tval3, 10, 156);
  lpi::gui::PVariable<double> tval4;
  tval4.make(0, 0, &dyn_value4);
  tabs.getTabContent(1).pushTopAt(&tval4, 10, 164);
  lpi::gui::PVariable<float> tval5;
  tval5.make(0, 0, &dyn_value5);
  tabs.getTabContent(1).pushTopAt(&tval5, 10, 172);
  
  lpi::gui::HueCircle_HSV_HS hsv;
  hsv.resize(0,0,120,120);
  hsv.setAdaptiveColor(lpi::RGBA_Lightred(192));
  tabs.getTabContent(2).pushTopAt(&hsv, 20, 50);
  lpi::ColorRGB color;
  lpi::gui::PColorPlane colorplane(&color);
  colorplane.resize(0, 0, 64, 48);
  //colorplane.color = lpi::ColorRGB(128, 255, 255, 192);
  tabs.getTabContent(2).pushTopAt(&colorplane, 144, 50);
  
  lpi::gui::Checkbox wcb;
  wcb.make(0, 0);
  w1.pushTopAt(&wcb, 20, 20);
  wcb.makeLabel("toggle scroll-zone", wcb.getSizeX(), wcb.getRelCenterY() - 4, lpi::TS_W);
  
  std::vector<double> sound(30000);
  for(size_t i = 0; i < 30000; i++) sound[i] = 0.1 * std::sin(i / (30.0 * (40000.0-i)/30000.0));
  
  lpi::audioOpen(44100, 2048);
  
  tabs.setElementOver(true);
  
  lpi::GameTime gametime;
  gametime.init_fps();
  
  while(lpi::frame(true, true))
  {
    gametime.update();
    
    lpi::print("lpi GUI demo");
    lpi::print(gametime.fps(), 0, 8);
    
    lpi::drawGradientEllipse(600, 400, 100, 50, lpi::ColorRGB(128, 255, 128, 255), lpi::ColorRGB(255, 128, 128, 128));
    lpi::drawBezier(600,100, 700,100, 750,200, 550,150, lpi::RGB_Lightred);
    lpi::drawCircle(600, 400, 110, lpi::ColorRGB(128, 255, 128, 255));
    
    lpi::gui::builtInTexture[37].draw(0, 50);
    
    dyn.controlToValue();
    //dyn.valueToControl();
    
    c.draw(lpi::gGUIDrawer);
    c.handle(lpi::gGUIInput);
    
    lpi::ColorHSV colorHSV(hsv.getValueAngle() * 255, hsv.getValueAxial() * 255, 255);
    color = lpi::HSVtoRGB(colorHSV);
    
    spawns.draw();
    spawns.handle();
    
    if(wcb.checked) w1.addScrollbars();
    else w1.removeScrollbars();
    
    if(sound_button.pressed(lpi::gGUIInput)) lpi::audioPlay(sound);
    
    if(tb_unittest.pressed(lpi::gGUIInput)) lpi::gui::unitTest();
    
    if(tb.clicked(lpi::gGUIInput))
    {
      lpi::xml::XMLTree tree;
      tree.parse(lpi::gui::builtInGuiData);
      
      for(size_t i = 0; i < tree.children.size(); i++)
      {
        if(tree.children[i]->content.name == "textures_small")
        {
          lpi::base64StringToBinaryFile("textures_small.png", tree.children[i]->content.value);
        }
        else if(tree.children[i]->content.name == "icons_small")
        {
          lpi::base64StringToBinaryFile("icons_small.png", tree.children[i]->content.value);
        }
      }
    }
    
    if(tb2.clicked(lpi::gGUIInput))
    {
      lpi::binaryFileToBase64File("textures_small_new.txt", "textures_small_new.png", true);
      lpi::binaryFileToBase64File("icons_small_new.txt", "icons_small_new.png", true);
      lpi::binaryFileToBase64File("textures_small.txt", "textures_small.png", true);
      lpi::binaryFileToBase64File("icons_small.txt", "icons_small.png", true);
    }
    
    if(wb.mouseJustOver(lpi::gGUIInput)) spawns.addSpawn("just over", lpi::globalMouseX, lpi::globalMouseY, lpi::RGB_Gray);
    if(wb.mouseJustLeft(lpi::gGUIInput)) spawns.addSpawn("just left", lpi::globalMouseX, lpi::globalMouseY, lpi::RGB_White);
    if(wb.pressed(lpi::gGUIInput)) spawns.addSpawn("pressed", lpi::globalMouseX, lpi::globalMouseY, lpi::ColorRGB(255, 0, 0));
    if(wb.clicked(lpi::gGUIInput)) spawns.addSpawn("clicked", lpi::globalMouseX, lpi::globalMouseY, lpi::ColorRGB(255, 64, 64));
    if(wb.mouseDoubleClicked(lpi::gGUIInput)) spawns.addSpawn("double clicked", lpi::globalMouseX, lpi::globalMouseY, lpi::ColorRGB(255, 128, 128));
    if(wb.mouseTripleClicked(lpi::gGUIInput)) spawns.addSpawn("triple clicked", lpi::globalMouseX, lpi::globalMouseY, lpi::ColorRGB(255, 192, 192));
    if(wb.mouseQuadrupleClicked(lpi::gGUIInput)) spawns.addSpawn("quadruple clicked", lpi::globalMouseX, lpi::globalMouseY, lpi::ColorRGB(255, 224, 224));
    if(wb.pressed(lpi::gGUIInput, lpi::gui::GUI_MMB)) spawns.addSpawn("middle pressed", lpi::globalMouseX, lpi::globalMouseY, lpi::ColorRGB(0, 255, 0));
    if(wb.clicked(lpi::gGUIInput, lpi::gui::GUI_MMB)) spawns.addSpawn("middle clicked", lpi::globalMouseX, lpi::globalMouseY, lpi::ColorRGB(64, 255, 64));
    if(wb.mouseDoubleClicked(lpi::gGUIInput, lpi::gui::GUI_MMB)) spawns.addSpawn("middle double clicked", lpi::globalMouseX, lpi::globalMouseY, lpi::ColorRGB(128, 255, 128));
    if(wb.mouseTripleClicked(lpi::gGUIInput, lpi::gui::GUI_MMB)) spawns.addSpawn("middle triple clicked", lpi::globalMouseX, lpi::globalMouseY, lpi::ColorRGB(192, 255, 192));
    if(wb.mouseQuadrupleClicked(lpi::gGUIInput, lpi::gui::GUI_MMB)) spawns.addSpawn("middle quadruple clicked", lpi::globalMouseX, lpi::globalMouseY, lpi::ColorRGB(224, 255, 224));
    if(wb.pressed(lpi::gGUIInput, lpi::gui::GUI_RMB)) spawns.addSpawn("right pressed", lpi::globalMouseX, lpi::globalMouseY, lpi::ColorRGB(0, 0, 255));
    if(wb.clicked(lpi::gGUIInput, lpi::gui::GUI_RMB)) spawns.addSpawn("right clicked", lpi::globalMouseX, lpi::globalMouseY, lpi::ColorRGB(64, 64, 255));
    if(wb.mouseDoubleClicked(lpi::gGUIInput, lpi::gui::GUI_RMB)) spawns.addSpawn("right double clicked", lpi::globalMouseX, lpi::globalMouseY, lpi::ColorRGB(128, 128, 255));
    if(wb.mouseTripleClicked(lpi::gGUIInput, lpi::gui::GUI_RMB)) spawns.addSpawn("right triple clicked", lpi::globalMouseX, lpi::globalMouseY, lpi::ColorRGB(192, 192, 255));
    if(wb.mouseQuadrupleClicked(lpi::gGUIInput, lpi::gui::GUI_RMB)) spawns.addSpawn("right quadruple clicked", lpi::globalMouseX, lpi::globalMouseY, lpi::ColorRGB(224, 224, 255));
    if(wb.mouseScrollUp(lpi::gGUIInput)) spawns.addSpawn("scrolled up", lpi::globalMouseX, lpi::globalMouseY, lpi::RGB_Black);
    if(wb.mouseScrollDown(lpi::gGUIInput)) spawns.addSpawn("scrolled down", lpi::globalMouseX, lpi::globalMouseY, lpi::RGB_Black);
    
    lpi::redraw();
    lpi::cls(lpi::RGB_Darkgreen);
  }
  
  return 0;
}



