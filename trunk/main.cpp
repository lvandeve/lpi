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
#include "lpi_texture.h"
#include "lpi_screen_gl.h"
#include "lpi_gui.h"
#include "lpi_gui_text.h"
#include "lpi_gui_ex.h"
#include "lpi_input_sdl.h"
#include "lpi_gui_drawer_gl.h"
#include "lpi_gui_drawer_buffer.h"
#include "lpi_draw2d.h"
#include "lpi_draw2dgl.h"
#include "lpi_draw2d_buffer.h"
#include "lpi_audio.h"
#include "lpi_bignums.h"
#include "lpi_math.h"
#include "lpi_tools.h"
#include "lpi_gui_unittest.h"
#include "lpi_xml.h"
#include "lodepng.h"
#include "lodewav.h"
#include "lpi_gui_dynamic.h"
#include "lpi_gui_color.h"
#include "lpi_guipartdraw_int.h"
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
  lpi::gui::IGUIDrawer* drawer;
  
  SpawnText(const std::string& text, double x, double y, lpi::gui::IGUIDrawer* drawer, const lpi::ColorRGB& color = lpi::RGB_White)
  : x(x), y(y), text(text), prev_time(lpi::getSeconds()), color(color), drawer(drawer)
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
    drawer->drawText(text, (int)x, (int)y, lpi::Font(color));
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
  
  void addSpawn(const std::string& text, int x, int y, lpi::gui::IGUIDrawer* drawer, const lpi::ColorRGB& color = lpi::RGB_White)
  {
    spawns.push_back(new SpawnText(text, (double)x, (double)y, drawer, color));
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

int main(int, char*[]) //the arguments have to be given here, or DevC++ can't link to SDL for some reason
{//std::cout<<sizeof(lpi::gui::Element)<<std::endl;
  lpi::ScreenGL screen(width, height, false, false, "lpi GUI demo");
  lpi::gui::GUIDrawerGL guidrawer(&screen);
  
  lpi::gui::MainContainer c(guidrawer);
  
  lpi::gui::Button tb;
  tb.makeText(20, 530, "Save Built In GUI To PNGs", guidrawer);
  c.pushTop(&tb);
  lpi::gui::Button tb2;
  tb2.makeText(20, 540, "GUI PNGs to base 64 TXT", guidrawer);
  c.pushTop(&tb2);
  
  lpi::gui::Button tb_unittest;
  tb_unittest.makeText(20, 550, "Unit Test", guidrawer);
  c.pushTop(&tb_unittest);
  
  lpi::gui::Button tb_guitopng;
  tb_guitopng.makeText(20, 560, "Render gui to 'alternativerenderer.png'", guidrawer);
  c.pushTop(&tb_guitopng);
  
  lpi::gui::Window w1;
  w1.make(100, 100, 500, 500);
  w1.addTop(guidrawer);
  w1.addTitle("Window 1");
  w1.addCloseButton(guidrawer);
  w1.addResizer(guidrawer);
  w1.setColor(lpi::RGBA_Red(192));
  c.pushTop(&w1);

  lpi::gui::Window w2;
  w2.make(50, 50, 300, 300);
  w2.addTop(guidrawer);
  w2.addTitle("Window 2");
  w2.addCloseButton(guidrawer);
  w2.setColor(lpi::RGBA_White(224));
  w2.addResizer(guidrawer);
  c.pushTop(&w2);
  
  lpi::gui::Tabs tabs;
  tabs.resize(0, 0, w2.getSizeX(), w2.getSizeY() - 32);
  tabs.addTab("tab 1");
  tabs.addTab("tab 2");
  tabs.addTab("tab 3");
  tabs.addTab("tab 4");
  w2.pushTopAt(&tabs, 0, 16, lpi::gui::Sticky(0.0, 0, 0.1, 0, 1.0, 0, 1.0, 0));


  lpi::gui::Button sound_button;
  sound_button.makeTextPanel(20, 500, "sound", 64, 24);
  w1.pushTopAt(&sound_button, 15, 50);
  
  lpi::gui::Scrollbar hbar;
  hbar.makeHorizontal(716, 220, 200, 100, guidrawer);
  w1.pushTopAt(&hbar, 150, 210);
  
  lpi::gui::Scrollbar vbar;
  vbar.makeVertical(700, 20, 200, 100, guidrawer);
  w1.pushTopAt(&vbar, 350, 10);
  
  lpi::gui::Slider sli;
  sli.makeVertical(650, 20, 200, 100, guidrawer);
  w1.pushTopAt(&sli, 300, 10, lpi::gui::Sticky(0.5, 0, 0.1, 0, 0.5, sli.getSizeX(), 0.5, 0));
  
  lpi::gui::Slider sli2;
  sli2.makeHorizontal(650, 20, 200, 100, guidrawer);
  w1.pushTopAt(&sli2, 150, 240);
  

  lpi::gui::Checkbox cb;
  cb.make(500, 20);
  w1.pushTopAt(&cb, 10, 600);
  
  /*lpi::gui::BulletList bl;
  bl.make(500, 80, 8, 0, 24);
  c.pushTop(&bl);*/
  
  lpi::gui::Button wb;
  wb.makeTextPanel(0, 0, "window button", 64, 24);
  wb.autoTextSize(&guidrawer, 4);
  tabs.getTabContent(0).pushTopAt(&wb, 20, 50);
  //w2.pushTopAt(&wb, 20, 50);
  
  lpi::gui::Checkbox wbcb;
  wbcb.make(0, 0);
  wbcb.makeLabel("more", wbcb.getSizeX(), wbcb.getRelCenterY() - 4, lpi::FONT_White);
  tabs.getTabContent(0).pushTopAt(&wbcb, 20, 80);
  

  
  lpi::ColorRGB dyn_value0 = lpi::RGBA_Lightblue(224);
  int dyn_value1 = 12345;
  std::string dyn_value2 = "hello";
  bool dyn_value3 = true;
  double dyn_value4 = 1.5;
  float dyn_value5 = 60.5;
  lpi::gui::DynamicPage dyn;
  //dyn.setSizeXY(200, 20);
  dyn.resize(0,0,280,100);
  dyn.setTitle("dynamic page");
  dyn.addControl("color", new lpi::gui::DynamicColor(&dyn_value0));
  dyn.addControl("int", new lpi::gui::DynamicValue<int>(&dyn_value1));
  dyn.addControl("string", new lpi::gui::DynamicValue<std::string>(&dyn_value2));
  dyn.addControl("boolean", new lpi::gui::DynamicCheckbox(&dyn_value3));
  dyn.addControl("double", new lpi::gui::DynamicValue<double>(&dyn_value4));
  dyn.addControl("slider", new lpi::gui::DynamicSlider<float>(&dyn_value5, 0, 100, guidrawer));
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
  
  lpi::gui::HueCircleEditor_HSL_HS huecircle;
  huecircle.resize(0,0,120,120);
  tabs.getTabContent(2).pushTopAt(&huecircle, 20, 20, lpi::gui::Sticky(0.01,0, 0.0,0, 0.45,0, 0.45,0));
  lpi::gui::ColorSlidersRGB rgb;
  rgb.resize(0,0,120,64);
  tabs.getTabContent(2).pushTopAt(&rgb, 20, 150, lpi::gui::Sticky(0.01,0, 0.55,0, 0.45,0, 0.7,0));
  lpi::gui::FGBGColor fgbg;
  fgbg.resize(0, 0, 48, 48);
  tabs.getTabContent(2).pushTopAt(&fgbg, 144, 20, lpi::gui::Sticky(0.01,0, 0.8,0, 0.2,0, 1.0,0));
  lpi::gui::Canvas canvas(guidrawer);
  canvas.make(0, 0, 256, 256);
  tabs.getTabContent(2).pushTopAt(&canvas, 144, 100, lpi::gui::Sticky(0.55,0, 0.55,0, 1.0,0, 1.0,0));
  
  lpi::gui::MultiColorPalette palette;
  //palette.generateDefault();
  palette.generateVibrant16x16();
  palette.resize(0, 0, 96, 96);
  tabs.getTabContent(2).pushTopAt(&palette, 200, 4, lpi::gui::Sticky(0.55,0, 0.0,0, 1.0,0, 0.45,0));
  
  lpi::gui::Checkbox wcb;
  wcb.make(0, 0);
  w1.pushTopAt(&wcb, 20, 20);
  wcb.makeLabel("toggle scroll-zone", wcb.getSizeX(), wcb.getRelCenterY() - 4, lpi::FONT_White);
  
  std::vector<double> sound(30000);
  for(size_t i = 0; i < 30000; i++) sound[i] = 0.1 * std::sin(i / (30.0 * (40000.0-i)/30000.0));
  
  lpi::audioOpen(44100, 2048);

  tabs.setElementOver(true);
  
  lpi::GameTime gametime;
  gametime.init_fps();
  
  lpi::gui::ToolTipManager tooltips;
  tooltips.registerElement(&sound_button, "beware...\nthis button makes sound");
  tooltips.registerElement(&tb_guitopng, "Unlike the main GUI, which is rendered\nusing 2D triangles with OpenGL,\nthe alternative renderer is entirely in software.");
  
  lpi::gui::ColorEditorSynchronizer colorSynchronizer;
  colorSynchronizer.add(&rgb);
  colorSynchronizer.add(&fgbg);
  colorSynchronizer.add(&huecircle);
  colorSynchronizer.add(&palette);
  fgbg.setFG(lpi::RGBd_Gray);
  fgbg.setBG(lpi::RGBd_White);
  colorSynchronizer.setColor(fgbg.getFG()); //make synchronizer update the above change
  
  while(lpi::frame(true, true))
  {
    gametime.update();
    
    guidrawer.drawText("lpi GUI demo", 0, 0);
    guidrawer.print(gametime.fps(), 0, 8);
    
    guidrawer.drawEllipseCentered(600, 400, 100, 50, lpi::ColorRGB(128, 255, 128, 128), true);
    guidrawer.drawBezier(600,100, 700,100, 750,200, 550,150, lpi::RGB_Lightred);
    guidrawer.drawCircle(600, 400, 110, lpi::ColorRGB(128, 255, 128, 255), false);
    
    //guidrawer.drawTexture(&lpi::gui::builtInTexture[37], 0, 50);
    
    dyn.controlToValue();
    //dyn.valueToControl();
    
    c.draw(guidrawer);
    tooltips.draw(&c, guidrawer);
    c.handle(lpi::gSDLInput);
    colorSynchronizer.handle();
    
    canvas.leftColor = fgbg.getFG255();
    canvas.rightColor = fgbg.getBG255();
    
    spawns.draw();
    spawns.handle();
    
    if(wcb.isChecked()) w1.addScrollbars(guidrawer);
    else w1.removeScrollbars();
    
    if(sound_button.pressed(lpi::gSDLInput)) lpi::audioPlay(sound);
    
    if(tb_unittest.pressed(lpi::gSDLInput)) lpi::gui::unitTest();
    
    if(tb_guitopng.pressed(lpi::gSDLInput))
    {
      lpi::gui::GUIDrawerBuffer d;
      lpi::Drawer2DBuffer& drawer = dynamic_cast<lpi::Drawer2DBuffer&>(d.getDrawer());
      std::vector<unsigned char> buffer(width * height * 4, 128);
      drawer.setBuffer(&buffer[0], width, height);
      c.draw(d);
      
      LodePNG::encode("alternativerenderer.png", buffer, width, height);
    }
    
    if(tb.clicked(lpi::gSDLInput))
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

    if(tb2.clicked(lpi::gSDLInput))
    {
      lpi::binaryFileToBase64File("textures_small_new.txt", "textures_small_new.png", true);
      lpi::binaryFileToBase64File("icons_small_new.txt", "icons_small_new.png", true);
      lpi::binaryFileToBase64File("textures_small.txt", "textures_small.png", true);
      lpi::binaryFileToBase64File("icons_small.txt", "icons_small.png", true);
    }
    
    if(wb.mouseJustOver(lpi::gSDLInput)) spawns.addSpawn("just over", lpi::globalMouseX, lpi::globalMouseY, &guidrawer, lpi::RGB_Gray);
    if(wb.mouseJustLeft(lpi::gSDLInput)) spawns.addSpawn("just left", lpi::globalMouseX, lpi::globalMouseY, &guidrawer, lpi::RGB_White);
    if(wb.pressed(lpi::gSDLInput)) spawns.addSpawn("pressed", lpi::globalMouseX, lpi::globalMouseY, &guidrawer, lpi::ColorRGB(255, 0, 0));
    if(wb.clicked(lpi::gSDLInput)) spawns.addSpawn("clicked", lpi::globalMouseX, lpi::globalMouseY, &guidrawer, lpi::ColorRGB(255, 64, 64));
    if(wb.mouseDoubleClicked(lpi::gSDLInput)) spawns.addSpawn("double clicked", lpi::globalMouseX, lpi::globalMouseY, &guidrawer, lpi::ColorRGB(255, 128, 128));
    if(wb.mouseTripleClicked(lpi::gSDLInput)) spawns.addSpawn("triple clicked", lpi::globalMouseX, lpi::globalMouseY, &guidrawer, lpi::ColorRGB(255, 192, 192));
    if(wb.mouseQuadrupleClicked(lpi::gSDLInput)) spawns.addSpawn("quadruple clicked", lpi::globalMouseX, lpi::globalMouseY, &guidrawer, lpi::ColorRGB(255, 224, 224));
    if(wb.pressed(lpi::gSDLInput, lpi::MMB)) spawns.addSpawn("middle pressed", lpi::globalMouseX, lpi::globalMouseY, &guidrawer, lpi::ColorRGB(0, 255, 0));
    if(wb.clicked(lpi::gSDLInput, lpi::MMB)) spawns.addSpawn("middle clicked", lpi::globalMouseX, lpi::globalMouseY, &guidrawer, lpi::ColorRGB(64, 255, 64));
    if(wb.mouseDoubleClicked(lpi::gSDLInput, lpi::MMB)) spawns.addSpawn("middle double clicked", lpi::globalMouseX, lpi::globalMouseY, &guidrawer, lpi::ColorRGB(128, 255, 128));
    if(wb.mouseTripleClicked(lpi::gSDLInput, lpi::MMB)) spawns.addSpawn("middle triple clicked", lpi::globalMouseX, lpi::globalMouseY, &guidrawer, lpi::ColorRGB(192, 255, 192));
    if(wb.mouseQuadrupleClicked(lpi::gSDLInput, lpi::MMB)) spawns.addSpawn("middle quadruple clicked", lpi::globalMouseX, lpi::globalMouseY, &guidrawer, lpi::ColorRGB(224, 255, 224));
    if(wb.pressed(lpi::gSDLInput, lpi::RMB)) spawns.addSpawn("right pressed", lpi::globalMouseX, lpi::globalMouseY, &guidrawer, lpi::ColorRGB(0, 0, 255));
    if(wb.clicked(lpi::gSDLInput, lpi::RMB)) spawns.addSpawn("right clicked", lpi::globalMouseX, lpi::globalMouseY, &guidrawer, lpi::ColorRGB(64, 64, 255));
    if(wb.mouseDoubleClicked(lpi::gSDLInput, lpi::RMB)) spawns.addSpawn("right double clicked", lpi::globalMouseX, lpi::globalMouseY, &guidrawer, lpi::ColorRGB(128, 128, 255));
    if(wb.mouseTripleClicked(lpi::gSDLInput, lpi::RMB)) spawns.addSpawn("right triple clicked", lpi::globalMouseX, lpi::globalMouseY, &guidrawer, lpi::ColorRGB(192, 192, 255));
    if(wb.mouseQuadrupleClicked(lpi::gSDLInput, lpi::RMB)) spawns.addSpawn("right quadruple clicked", lpi::globalMouseX, lpi::globalMouseY, &guidrawer, lpi::ColorRGB(224, 224, 255));
    if(wb.mouseScrollUp(lpi::gSDLInput)) spawns.addSpawn("scrolled up", lpi::globalMouseX, lpi::globalMouseY, &guidrawer, lpi::RGB_Black);
    if(wb.mouseScrollDown(lpi::gSDLInput)) spawns.addSpawn("scrolled down", lpi::globalMouseX, lpi::globalMouseY, &guidrawer, lpi::RGB_Black);
    if(wbcb.isChecked())
    {
      if(wb.mouseJustDownElsewhere(lpi::gSDLInput)) spawns.addSpawn("pressed elsewhere", lpi::globalMouseX, lpi::globalMouseY, &guidrawer, lpi::RGB_Black);
      if(wb.mouseDown(lpi::gSDLInput)) guidrawer.drawText("down", lpi::globalMouseX, lpi::globalMouseY, lpi::FONT_White);
      if(wb.mouseDownHere(lpi::gSDLInput)) guidrawer.drawText("down here", lpi::globalMouseX, lpi::globalMouseY + 8, lpi::FONT_White);
      if(wb.mouseGrabbed(lpi::gSDLInput)) guidrawer.drawText("grabbed", lpi::globalMouseX, lpi::globalMouseY + 16, lpi::FONT_White);
      if(wb.mouseDownElsewhere(lpi::gSDLInput)) guidrawer.drawText("down elsewhere", lpi::globalMouseX, lpi::globalMouseY + 8, lpi::FONT_White);
    }
    
    screen.redraw();
    screen.cls(lpi::RGB_Darkgreen);
  }
  
  return 0;
}



