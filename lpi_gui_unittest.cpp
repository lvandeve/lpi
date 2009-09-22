#include "lpi_gui_unittest.h"
#include "lpi_unittest.h"
#include "lpi_event.h"
#include "lpi_gui_input_sdl.h"
#include "lpi_screen.h"
#include "lpi_gui.h"
#include "lpi_gui_ex.h"
#include "lpi_gui_text.h"

namespace lpi
{
namespace gui
{

class GUIInputDebug : public GUIInputSDL
{
  private:
    int x;
    int y;
    bool lmb;
    bool rmb;
  public:
    void debugSetMousePos(int x, int y)
    {
      this->x = x;
      this->y = y;
    }
    void debugSetLMB(bool set)
    {
      this->lmb = set;
    }
    void debugSetRMB(bool set)
    {
      this->rmb = set;
    }
  public:
    //check position of mouse
    virtual int mouseX() const
    {
      return x;
    }
    virtual int mouseY() const
    {
      return y;
    }
    //check the state of the 3 buttons of mouse
    virtual bool mouseButtonDown(GUIMouseButton button) const
    {
      if(button == GUI_LMB)
        return lmb;
      else return rmb;
    }
};

#define LUT_MY_RESET \
{\
  testinput.debugSetLMB(0);\
  testinput.debugSetRMB(0);\
  testinput.debugSetMousePos(0, 0);\
}

GUIInputDebug testinput;

void unitTest()
{
  lpi::ScreenGL screen(1024, 768, 0, false, "Unit Testing...");
  lpi::gui::GUIDrawerGL dummydrawer(&screen);
  
  LUT_START_UNIT_TEST
  
  //I had a bug where when making a new BulletList and using "set" to give it a value, "check" wasn't returning the correct value until mouseOver
  LUT_CASE("set bulletlist")
    BulletList test1;
    test1.make(0, 0, 5, 0, 0);
    test1.set(3);
    LUT_SUB_ASSERT_FALSE(test1.check() != 3)
    test1.set(1);
    LUT_SUB_ASSERT_FALSE(test1.check() != 1)
  LUT_CASE_END
  
  LUT_CASE("mouseOver and mouseDown on Dummy")
    Dummy dummy;
    dummy.resize(0, 0, 50, 50);
    testinput.debugSetMousePos(100, 100);
    dummy.handle(testinput);
    LUT_SUB_ASSERT_TRUE(!dummy.mouseOver(testinput))
    testinput.debugSetMousePos(1, 1);
    dummy.handle(testinput);
    LUT_SUB_ASSERT_TRUE(dummy.mouseOver(testinput))
    LUT_SUB_ASSERT_TRUE(!dummy.mouseDown(testinput))
    testinput.debugSetLMB(1);
    dummy.handle(testinput);
    LUT_SUB_ASSERT_TRUE(dummy.mouseDown(testinput))
    testinput.debugSetMousePos(100, 100);
    dummy.handle(testinput);
    LUT_SUB_ASSERT_TRUE(!dummy.mouseDown(testinput))
  LUT_CASE_END
  
  LUT_CASE("mouseJustDown")
    Dummy dummy;
    dummy.resize(0, 0, 50, 50);
    dummy.handle(testinput);
    LUT_SUB_ASSERT_TRUE(!dummy.mouseJustDown(testinput))
    testinput.debugSetMousePos(100, 100);
    testinput.debugSetLMB(1);
    dummy.handle(testinput);
    LUT_SUB_ASSERT_TRUE(!dummy.mouseJustDown(testinput))
    testinput.debugSetMousePos(20, 20);
    dummy.handle(testinput);
    LUT_SUB_ASSERT_TRUE(dummy.mouseJustDown(testinput)) //the mouse moved over the element, the mouse button was already down since being outside the element, so it's now down over the element for the first time
    LUT_SUB_ASSERT_TRUE(!dummy.mouseJustDown(testinput)) //now it may not return true anymore!
    testinput.debugSetLMB(0);
    dummy.handle(testinput);
    LUT_SUB_ASSERT_TRUE(!dummy.mouseJustDown(testinput))
    testinput.debugSetLMB(1);
    dummy.handle(testinput);
    LUT_SUB_ASSERT_TRUE(dummy.mouseJustDown(testinput))
    LUT_SUB_ASSERT_TRUE(!dummy.mouseJustDown(testinput)) //now it may not return true anymore!
  LUT_CASE_END
  
  LUT_CASE("mouseJustDownHere")
    Dummy dummy;
    dummy.resize(0, 0, 50, 50);
    dummy.handle(testinput);
    LUT_SUB_ASSERT_TRUE(!dummy.mouseJustDownHere(testinput))
    testinput.debugSetMousePos(100, 100);
    testinput.debugSetLMB(1);
    dummy.handle(testinput);
    LUT_SUB_ASSERT_TRUE(!dummy.mouseJustDownHere(testinput))
    testinput.debugSetMousePos(20, 20);
    dummy.handle(testinput);
    LUT_SUB_ASSERT_TRUE(!dummy.mouseJustDownHere(testinput))
    testinput.debugSetLMB(0);
    dummy.handle(testinput);
    LUT_SUB_ASSERT_TRUE(!dummy.mouseJustDownHere(testinput))
    testinput.debugSetLMB(1);
    dummy.handle(testinput);
    LUT_SUB_ASSERT_TRUE(dummy.mouseJustDownHere(testinput))
    LUT_SUB_ASSERT_TRUE(!dummy.mouseJustDownHere(testinput)) //now it may not return true anymore!
  LUT_CASE_END
  
  LUT_CASE("mouseJustDownHere on top of window")
    Container c(dummydrawer);
    Window w;
    w.make(0, 0, 100, 100);
    c.pushTopAt(&w, 0, 0);
    
    w.addTop(dummydrawer);
    c.handle(testinput);c.handle(testinput);
    
    int mx = w.top.getCenterX();
    int my = w.top.getCenterY();
    c.handle(testinput);c.handle(testinput);
    LUT_SUB_ASSERT_TRUE(!w.top.mouseJustDownHere(testinput))
    testinput.debugSetMousePos(100, 100);
    testinput.debugSetLMB(1);
    c.handle(testinput);c.handle(testinput);
    LUT_SUB_ASSERT_TRUE(!w.top.mouseJustDownHere(testinput))
    testinput.debugSetMousePos(mx, my);
    c.handle(testinput);c.handle(testinput);
    LUT_SUB_ASSERT_TRUE(!w.top.mouseJustDownHere(testinput))
    testinput.debugSetLMB(0);
    c.handle(testinput);c.handle(testinput);
    LUT_SUB_ASSERT_TRUE(!w.top.mouseJustDownHere(testinput))
    testinput.debugSetLMB(1);
    c.handle(testinput);c.handle(testinput);
    LUT_SUB_ASSERT_TRUE(w.top.mouseJustDownHere(testinput))
    LUT_SUB_ASSERT_TRUE(!w.top.mouseJustDownHere(testinput)) //now it may not return true anymore!
  LUT_CASE_END
  
  LUT_CASE("mouseGrabbed")
    Dummy dummy;
    dummy.resize(0, 0, 50, 50);
    dummy.handle(testinput);
    LUT_SUB_ASSERT_TRUE(!dummy.mouseGrabbed(testinput)) //mouse not down yet
    testinput.debugSetMousePos(10, 10);
    testinput.debugSetLMB(1);
    dummy.handle(testinput);
    LUT_SUB_ASSERT_TRUE(dummy.mouseGrabbed(testinput)) //mouse down on element so it's grabbed
    testinput.debugSetMousePos(100, 100);
    dummy.handle(testinput);
    LUT_SUB_ASSERT_TRUE(dummy.mouseGrabbed(testinput)) //mouse moved away but still down ==> still grabbed
    LUT_SUB_ASSERT_TRUE(dummy.mouseGetGrabX() == 10 && dummy.mouseGetGrabY() == 10)
    testinput.debugSetLMB(0);
    dummy.handle(testinput);
    LUT_SUB_ASSERT_TRUE(!dummy.mouseGrabbed(testinput)) //mouse button up ==> not grabbed anymore
    testinput.debugSetLMB(1);
    dummy.handle(testinput);
    LUT_SUB_ASSERT_TRUE(!dummy.mouseGrabbed(testinput)) //mouse down somewhere else not on element ==> not grabbed
    testinput.debugSetMousePos(15, 15);
    dummy.handle(testinput);
    LUT_SUB_ASSERT_TRUE(!dummy.mouseGrabbed(testinput)) //mouse still down and moving over element, but it wasn't down HERE, so not grabbed
    /*//now repeat the test to see if states can properly be reused
    testinput.debugSetLMB(0);
    dummy.handle(testinput);
    LUT_SUB_ASSERT_TRUE(!dummy.mouseGrabbed(testinput))
    testinput.debugSetLMB(1);
    dummy.handle(testinput);
    LUT_SUB_ASSERT_TRUE(dummy.mouseGrabbed(testinput)) //mouse down on element so it's grabbed
    testinput.debugSetMousePos(100, 100);
    dummy.handle(testinput);
    LUT_SUB_ASSERT_TRUE(dummy.mouseGrabbed(testinput)) //mouse moved away but still down ==> still grabbed
    LUT_SUB_ASSERT_TRUE(dummy.mouseGetGrabX() == 15 && dummy.mouseGetGrabY() == 15)
    testinput.debugSetLMB(0);
    dummy.handle(testinput);
    LUT_SUB_ASSERT_TRUE(!dummy.mouseGrabbed(testinput)) //mouse button up ==> not grabbed anymore
    testinput.debugSetLMB(1);
    dummy.handle(testinput);
    LUT_SUB_ASSERT_TRUE(!dummy.mouseGrabbed(testinput)) //mouse down somewhere else not on element ==> not grabbed
    testinput.debugSetMousePos(11, 11);
    dummy.handle(testinput);
    LUT_SUB_ASSERT_TRUE(!dummy.mouseGrabbed(testinput)) //mouse still down and moving over element, but it wasn't down HERE, so not grabbed*/
  LUT_CASE_END
  
  //the fact that the container and the window handles the elements, influences the result
  LUT_CASE("mouseGrabbed on top of window")
    Container c(dummydrawer);
    Window w;
    w.make(0, 0, 100, 100);
    c.pushTopAt(&w, 0, 0);
    
    w.addTop(dummydrawer);
    c.handle(testinput);c.handle(testinput);
    
    int mx = w.top.getCenterX();
    int my = w.top.getCenterY();
 
    c.handle(testinput);c.handle(testinput);
    LUT_SUB_ASSERT_TRUE(!w.top.mouseGrabbed(testinput)) //mouse not down yet
    testinput.debugSetMousePos(mx, my);
    testinput.debugSetLMB(1);
    c.handle(testinput);c.handle(testinput);
    LUT_SUB_ASSERT_TRUE(w.top.mouseGrabbed(testinput)) //mouse down on element so it's grabbed
    testinput.debugSetMousePos(200, 200);
    c.handle(testinput);c.handle(testinput);
    LUT_SUB_ASSERT_TRUE(w.top.mouseGrabbed(testinput)) //mouse moved away but still down ==> still grabbed
    LUT_SUB_ASSERT_TRUE(w.top.mouseGetGrabX() == mx && w.top.mouseGetGrabY() == my)
    testinput.debugSetLMB(0);
    c.handle(testinput);c.handle(testinput);
    LUT_SUB_ASSERT_TRUE(!w.top.mouseGrabbed(testinput)) //mouse button up ==> not grabbed anymore
    testinput.debugSetLMB(1);
    c.handle(testinput);c.handle(testinput);
    LUT_SUB_ASSERT_TRUE(w.top.mouseGrabbed(testinput)) //mouse down somewhere else not on element ==> normally not grabbed, BUT, the window moves!! ==> under mouse and grabbed again
  LUT_CASE_END
  
  LUT_CASE("dragging a window")
    Container c(dummydrawer);
    Window w;
    w.make(0, 0, 100, 100);
    c.pushTopAt(&w, 0, 0);
    
    w.addTop(dummydrawer);
    c.handle(testinput);c.handle(testinput);
    
    int mx = w.top.getCenterX();
    int my = w.top.getCenterY();
    
    int wx1 = w.getX0();
    int wy1 = w.getY0();
    
    testinput.debugSetMousePos(mx, my); //mouse above top bar of the window
    testinput.debugSetLMB(1);
    c.handle(testinput);c.handle(testinput);
    
    int wx2 = w.getX0();
    int wy2 = w.getY0();
    
    LUT_SUB_ASSERT_TRUE(wx1 == wx2 && wy1 == wy2)
    
    testinput.debugSetMousePos(mx + 100, my + 50);
    c.handle(testinput);c.handle(testinput);
    int wx3 = w.getX0();
    int wy3 = w.getY0();
    
    LUT_SUB_ASSERT_FALSE(wx3 == wx1 && wy3 == wy1)
    LUT_SUB_ASSERT_TRUE(wx3 == wx1 + 100 && wy3 == wy1 + 50)
    LUT_APPEND_MSG << "wx1: " << wx1 << "wy1: " << wy1 << "wx2: " << wx2 << "wy2: " << wy2 << "wx3: " << wx3 << "wy3: " << wy3;
  LUT_CASE_END
  
  //functions like mouseJustDown and mouseJustDownHere may not influence each other's result, so both can return true once
  LUT_CASE("mouse independence")
    Dummy dummy;
    dummy.resize(0, 0, 50, 50);
    testinput.debugSetMousePos(10, 10); //mouse above
    dummy.handle(testinput);
    LUT_SUB_ASSERT_TRUE(!dummy.mouseJustDown(testinput))
    LUT_SUB_ASSERT_TRUE(!dummy.mouseJustDownHere(testinput))
    testinput.debugSetLMB(1); //press button
    dummy.handle(testinput);
    //now both must return true
    LUT_SUB_ASSERT_TRUE(dummy.mouseJustDown(testinput))
    LUT_SUB_ASSERT_TRUE(dummy.mouseJustDownHere(testinput))
    //but of course now both must return false
    LUT_SUB_ASSERT_TRUE(!dummy.mouseJustDown(testinput))
    LUT_SUB_ASSERT_TRUE(!dummy.mouseJustDownHere(testinput))
  LUT_CASE_END
  
  //when boolean mouseActive is true, mouseOver should work, when it's false, mouseOver shout NOT work
  LUT_CASE("mouseOver when active and not active")
    Dummy dummy;
    dummy.resize(0, 0, 50, 50);
    testinput.debugSetMousePos(1, 1);
    dummy.handle(testinput);
    LUT_SUB_ASSERT_TRUE(dummy.mouseOver(testinput))
    dummy.setElementOver(true);
    LUT_SUB_ASSERT_TRUE(!dummy.mouseOver(testinput))
    //note: containers like a gui::Window have to take control of the mouseActive boolean of the elements in it
  LUT_CASE_END
    
  
  //the selfActivate system (used by text input boxes) didn't work anymore, so I added this test to make sure it won't break anymore
  LUT_CASE("selfActivate of InputLine")
    InputLine line;
    line.make(0, 0, 10);
    
    //initially it's not active
    line.handle(testinput); //handle line all the time to let the selfActivate system work
    LUT_SUB_ASSERT_TRUE(!line.isControlActive())
    
    //move mouse above it and click
    testinput.debugSetMousePos(1, 1);
    testinput.debugSetLMB(1);
    line.handle(testinput);
    //it must be active now!
    LUT_SUB_ASSERT_TRUE(line.isControlActive())
    
    //now release the mouse button
    testinput.debugSetLMB(0);
    line.handle(testinput);
    //it should still be active!
    LUT_SUB_ASSERT_TRUE(line.isControlActive())
    
    //move mouse away
    testinput.debugSetMousePos(100, 100);
    line.handle(testinput);
    //it should still be active!
    LUT_SUB_ASSERT_TRUE(line.isControlActive())
    
    //move mouse away from it and click
    testinput.debugSetMousePos(100, 100);
    testinput.debugSetLMB(1);
    line.handle(testinput);
    //it may not be active anymore after clicking on a location outside (???????)
    LUT_SUB_ASSERT_TRUE(!line.isControlActive())
  LUT_CASE_END
  
  
  LUT_CASE("selfActivate of InputLine in Window")
    Container c(dummydrawer);
    Window w;
    w.make(0, 0, 100, 100);
    c.pushTopAt(&w, 0, 0);
    
    InputLine line;
    line.make(0, 0, 10);
    w.pushTopAt(&line, 0, 0);
    
    //initially it's not active
    c.handle(testinput); //handle line all the time to let the selfActivate system work
    LUT_SUB_ASSERT_TRUE(!line.isControlActive())
    
    //move mouse above it and click
    testinput.debugSetMousePos(line.getCenterX(), line.getCenterY());
    testinput.debugSetLMB(1);
    c.handle(testinput);
    //it must be active now!
    LUT_SUB_ASSERT_TRUE(line.isControlActive())
    
    //now release the mouse button
    testinput.debugSetLMB(0);
    c.handle(testinput);
    //it should still be active!
    LUT_SUB_ASSERT_TRUE(line.isControlActive())
    
    //move mouse away
    testinput.debugSetMousePos(100, 100);
    c.handle(testinput);
    //it should still be active!
    LUT_SUB_ASSERT_TRUE(line.isControlActive())
    
    //move mouse away from it and click
    testinput.debugSetMousePos(100, 100);
    testinput.debugSetLMB(1);
    c.handle(testinput);
    //it may not be active anymore after clicking on a location outside (???????)
    LUT_SUB_ASSERT_TRUE(!line.isControlActive())
  LUT_CASE_END
  
  //two things on a container, both at the same location, first the one is totallyEnabled and the other totallyDisabled, then vica versa, then each time only the enabled one may return true to mouseOver
  LUT_CASE("enabled and disabled on each other")
    Container c(dummydrawer);
    Window w;
    w.make(0, 0, 100, 100);
    c.pushTopAt(&w, 0, 0);
    
    Dummy dummy1;
    dummy1.resize(0, 0, 50, 50);
    w.pushTopAt(&dummy1, 0, 0);
    
    Dummy dummy2;
    dummy2.resize(0, 0, 50, 50);
    w.pushTopAt(&dummy2, 0, 0);
    
    testinput.debugSetMousePos(dummy1.getCenterX(), dummy1.getCenterY()); //doesn't matter if you take center of dummy1 or dummy2, both are exactly at same position
    
    dummy1.totallyEnable();
    dummy2.totallyDisable();
    
    c.handle(testinput);
    
    LUT_SUB_ASSERT_TRUE(dummy1.mouseOver(testinput))
    LUT_SUB_ASSERT_TRUE(!dummy2.mouseOver(testinput))
    
    dummy1.totallyDisable();
    dummy2.totallyEnable();
    
    c.handle(testinput);
    
    LUT_SUB_ASSERT_TRUE(!dummy1.mouseOver(testinput))
    LUT_SUB_ASSERT_TRUE(dummy2.mouseOver(testinput))
  LUT_CASE_END
  
  //dragging a window that has a top
  LUT_CASE("dragging of a window")
  
    Container c(dummydrawer);
    Window w;
    w.make(0, 0, 100, 100);
    w.addTop(dummydrawer);
    c.pushTopAt(&w, 0, 0);
    
    int mx = w.top.getCenterX();
    int my = w.top.getCenterY();
    
    //grab the window
    testinput.debugSetMousePos(mx, my);
    testinput.debugSetLMB(1);
    c.handle(testinput);c.handle(testinput);
    
    //move
    testinput.debugSetMousePos(200, 200);
    c.handle(testinput);c.handle(testinput);
    
    //now the window must still be below the mouse
    LUT_SUB_ASSERT_TRUE(w.mouseOver(testinput))
  LUT_CASE_END
  
  //when there are two windows, A and B, and you're dragging one over the other, but you drag the mouse fast and the mousepointer goes to a location not over the dragged window (the dragged window will be under the mouse again soon), then the other window may not swap to top
  LUT_CASE("fast dragging of one window over another")
  
    Container c(dummydrawer);
    Window A;
    A.make(0, 0, 100, 100);
    A.addTop(dummydrawer);
    c.pushTopAt(&A, 0, 0);
    
    Window B;
    B.make(0, 0, 100, 100);
    B.addTop(dummydrawer);
    c.pushTopAt(&B, 200, 0);
    
    int ax = A.top.getCenterX();
    int ay = A.top.getCenterY();
    int bx = B.top.getCenterX();
    int by = B.top.getCenterY();
    
    //grab the window A
    testinput.debugSetMousePos(ax, ay);
    testinput.debugSetLMB(1);
    
    c.handle(testinput);c.handle(testinput);
    
    //move it over window B
    testinput.debugSetMousePos(bx, by);
    
    c.handle(testinput);c.handle(testinput);
    
    //now window A must be over window B, so window A must have mouseOver, window B not
    
    LUT_SUB_ASSERT_TRUE(A.mouseOver(testinput))
    LUT_SUB_ASSERT_TRUE(!B.mouseOver(testinput))
  
  LUT_CASE_END
  
  //dragging a window that has a top that is shifted: when starting to drag, the window may not "jump" (but I once had)
  LUT_CASE("dragging of a window with shifted top")
  
    Container c(dummydrawer);
    Window w;
    w.make(0, 0, 100, 100);
    w.addTop(dummydrawer); //could cause jump of two pixels when the bug was there
    c.pushTopAt(&w, 0, 0);
    
    int mx = w.top.getCenterX();
    int my = w.top.getCenterY();
    
    //grab the window
    testinput.debugSetMousePos(mx, my);
    testinput.debugSetLMB(1);
    
    c.handle(testinput);
    
    LUT_SUB_ASSERT_TRUE(w.getX0() == 0 && w.getY0() == 0); //NOT shifted over two pixels
  LUT_CASE_END
  
  //two things on a container, both at the same location, first the one is totallyEnabled and the other totallyDisabled, then vica versa, then each time only the enabled one may return true to mouseOver
  LUT_CASE("mouseOver works in nested containers")
    Container c(dummydrawer);
    
    Window w; //window is nested container in c
    w.make(0, 0, 100, 100);
    c.pushTopAt(&w, 0, 0);
    
    Dummy dummy;
    dummy.resize(0, 0, 50, 50);
    w.pushTopAt(&dummy, 0, 0);
    
    testinput.debugSetMousePos(dummy.getCenterX(), dummy.getCenterY());
    c.handle(testinput); //handle only ONCE, no multiple times to make things "settle"
    
    LUT_SUB_ASSERT_TRUE(dummy.mouseOver(testinput)) //the mouse really is over the dummy, and we called handle on the top container so that all input is up to date, so mouseOver of the nested element in it should return true
  LUT_CASE_END
  
  LUT_CASE("mouseOver on button on window in Tabs, behind other window")
    Container c(dummydrawer);

    Window w2;
    w2.make(50, 50, 300, 300);
    w2.addTop(dummydrawer);
    w2.addTitle("Window 2");
    w2.addCloseButton(dummydrawer);
    w2.setColor(lpi::RGBA_White(192));
    w2.addResizer(dummydrawer);
    c.pushTop(&w2);
    
    Tabs tabs;
    tabs.resize(0, 0, w2.getSizeX(), w2.getSizeY() - 32);
    tabs.addTab("tab 1");
    tabs.addTab("tab 2");
    tabs.addTab("tab 3");
    tabs.addTab("tab 4");
    w2.pushTopAt(&tabs, 0, 16, lpi::gui::Sticky(0.1, 0, 0.1, 0, 0.9, 0, 0.5, 0));
    
    Window w1;
    w1.make(0, 0, 500, 500);
    w1.addTop(dummydrawer);
    w1.addTitle("Window 1");
    w1.addCloseButton(dummydrawer);
    w1.addResizer(dummydrawer);
    w1.setColor(lpi::RGBA_Red(192));
    c.pushTop(&w1);
    
    Button wb;
    wb.makeTextPanel(0, 0, "window button");
    wb.autoTextSize(&dummydrawer, 4);
    tabs.getTabContent(0).pushTopAt(&wb, 20, 50);
    
    testinput.debugSetMousePos(0, 0);
    c.handle(testinput);
    
    testinput.debugSetMousePos(wb.getCenterX(), wb.getCenterY());
    c.handle(testinput);
    LUT_SUB_ASSERT_FALSE(wb.mouseOver(testinput)); //it should NOT be over, because the big "window 1" is over this - but some bug once made this be "true" here anyway (elements in tabs were active when another window was over it, due to tabs calling "totallyEnable" on each tab constantly, and totallyEnable had a setElementOver(false) call in it...)
    
    w1.move(wb.getX1(), wb.getY1());
    c.handle(testinput);
    LUT_SUB_ASSERT_TRUE(wb.mouseOver(testinput)); //window moved away, now mouse should be over the button

  LUT_CASE_END
  
  LUT_CASE("mouseOver on different tabs")
    Container c(dummydrawer);

    Window w2;
    w2.make(50, 50, 300, 300);
    w2.addTop(dummydrawer);
    w2.addTitle("Window 2");
    w2.addCloseButton(dummydrawer);
    w2.setColor(lpi::RGBA_White(192));
    w2.addResizer(dummydrawer);
    c.pushTop(&w2);
    
    Tabs tabs;
    tabs.resize(0, 0, w2.getSizeX(), w2.getSizeY() - 32);
    tabs.addTab("tab 1");
    tabs.addTab("tab 2");
    tabs.addTab("tab 3");
    tabs.addTab("tab 4");
    w2.pushTopAt(&tabs, 0, 16, lpi::gui::Sticky(0.1, 0, 0.1, 0, 0.9, 0, 0.5, 0));
    
    
    Button wb;
    wb.makeTextPanel(0, 0, "window button");
    wb.autoTextSize(&dummydrawer, 4);
    tabs.getTabContent(0).pushTopAt(&wb, 20, 50);
    
    testinput.debugSetMousePos(0, 0);
    c.handle(testinput);
    
    testinput.debugSetMousePos(wb.getCenterX(), wb.getCenterY());
    c.handle(testinput);
    LUT_SUB_ASSERT_TRUE(wb.mouseOver(testinput));
    
    tabs.selectTab(1);
    c.handle(testinput);
    LUT_SUB_ASSERT_FALSE(wb.mouseOver(testinput));

    tabs.selectTab(0);
    c.handle(testinput);
    LUT_SUB_ASSERT_TRUE(wb.mouseOver(testinput));

  LUT_CASE_END

  LUT_END_UNIT_TEST
}


////////////////////////////////////////////////////////////////////////////////

} //namespace gui
} //namespace lpi
