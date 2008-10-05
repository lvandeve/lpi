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

#define LUT_MY_RESET \
{\
  debugSetLMB(0);\
  debugSetRMB(0);\
  debugSetMousePos(0, 0);\
}

GUIInputSDL* testinput = &lpi::gGUIInput;
  
void unitTest()
{
  lpi::screen(1024, 768, 0, "Unit Testing...");
  
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
    debugSetMousePos(100, 100);
    LUT_SUB_ASSERT_TRUE(!dummy.mouseOver(testinput))
    debugSetMousePos(1, 1);
    LUT_SUB_ASSERT_TRUE(dummy.mouseOver(testinput))
    LUT_SUB_ASSERT_TRUE(!dummy.mouseDown(testinput))
    debugSetLMB(1);
    LUT_SUB_ASSERT_TRUE(dummy.mouseDown(testinput))
    debugSetMousePos(100, 100);
    LUT_SUB_ASSERT_TRUE(!dummy.mouseDown(testinput))
  LUT_CASE_END
  
  LUT_CASE("mouseJustDown")
    Dummy dummy;
    dummy.resize(0, 0, 50, 50);
    LUT_SUB_ASSERT_TRUE(!dummy.mouseJustDown(testinput))
    debugSetMousePos(100, 100);
    debugSetLMB(1);
    LUT_SUB_ASSERT_TRUE(!dummy.mouseJustDown(testinput))
    debugSetMousePos(20, 20);
    LUT_SUB_ASSERT_TRUE(dummy.mouseJustDown(testinput)) //the mouse moved over the element, the mouse button was already down since being outside the element, so it's now down over the element for the first time
    LUT_SUB_ASSERT_TRUE(!dummy.mouseJustDown(testinput)) //now it may not return true anymore!
    debugSetLMB(0);
    LUT_SUB_ASSERT_TRUE(!dummy.mouseJustDown(testinput))
    debugSetLMB(1);
    LUT_SUB_ASSERT_TRUE(dummy.mouseJustDown(testinput))
    LUT_SUB_ASSERT_TRUE(!dummy.mouseJustDown(testinput)) //now it may not return true anymore!
  LUT_CASE_END
  
  LUT_CASE("mouseJustDownHere")
    Dummy dummy;
    dummy.resize(0, 0, 50, 50);
    LUT_SUB_ASSERT_TRUE(!dummy.mouseJustDownHere(testinput))
    debugSetMousePos(100, 100);
    debugSetLMB(1);
    LUT_SUB_ASSERT_TRUE(!dummy.mouseJustDownHere(testinput))
    debugSetMousePos(20, 20);
    LUT_SUB_ASSERT_TRUE(!dummy.mouseJustDownHere(testinput))
    debugSetLMB(0);
    LUT_SUB_ASSERT_TRUE(!dummy.mouseJustDownHere(testinput))
    debugSetLMB(1);
    LUT_SUB_ASSERT_TRUE(dummy.mouseJustDownHere(testinput))
    LUT_SUB_ASSERT_TRUE(!dummy.mouseJustDownHere(testinput)) //now it may not return true anymore!
  LUT_CASE_END
  
  LUT_CASE("mouseJustDownHere on top of window")
    Container c;
    Window w;
    w.make(0, 0, 100, 100);
    c.pushTopAt(&w, 0, 0);
    
    w.addTop();
    c.handle(testinput);c.handle(testinput);
    
    int mx = w.top.getCenterX();
    int my = w.top.getCenterY();
    c.handle(testinput);c.handle(testinput);
    LUT_SUB_ASSERT_TRUE(!w.top.mouseJustDownHere(testinput))
    debugSetMousePos(100, 100);
    debugSetLMB(1);
    c.handle(testinput);c.handle(testinput);
    LUT_SUB_ASSERT_TRUE(!w.top.mouseJustDownHere(testinput))
    debugSetMousePos(mx, my);
    LUT_SUB_ASSERT_TRUE(!w.top.mouseJustDownHere(testinput))
    debugSetLMB(0);
    c.handle(testinput);c.handle(testinput);
    LUT_SUB_ASSERT_TRUE(!w.top.mouseJustDownHere(testinput))
    debugSetLMB(1);
    c.handle(testinput);c.handle(testinput);
    LUT_SUB_ASSERT_TRUE(w.top.mouseJustDownHere(testinput))
    LUT_SUB_ASSERT_TRUE(!w.top.mouseJustDownHere(testinput)) //now it may not return true anymore!
  LUT_CASE_END
  
  LUT_CASE("mouseGrabbed")
    Dummy dummy;
    dummy.resize(0, 0, 50, 50);
    LUT_SUB_ASSERT_TRUE(!dummy.mouseGrabbed(testinput)) //mouse not down yet
    debugSetMousePos(10, 10);
    debugSetLMB(1);
    LUT_SUB_ASSERT_TRUE(dummy.mouseGrabbed(testinput)) //mouse down on element so it's grabbed
    debugSetMousePos(100, 100);
    LUT_SUB_ASSERT_TRUE(dummy.mouseGrabbed(testinput)) //mouse moved away but still down ==> still grabbed
    LUT_SUB_ASSERT_TRUE(dummy.mouseGetGrabX() == 10 && dummy.mouseGetGrabY() == 10)
    debugSetLMB(0);
    LUT_SUB_ASSERT_TRUE(!dummy.mouseGrabbed(testinput)) //mouse button up ==> not grabbed anymore
    debugSetLMB(1);
    LUT_SUB_ASSERT_TRUE(!dummy.mouseGrabbed(testinput)) //mouse down somewhere else not on element ==> not grabbed
    debugSetMousePos(15, 15);
    LUT_SUB_ASSERT_TRUE(!dummy.mouseGrabbed(testinput)) //mouse still down and moving over element, but it wasn't down HERE, so not grabbed
    /*//now repeat the test to see if states can properly be reused
    debugSetLMB(0);
    LUT_SUB_ASSERT_TRUE(!dummy.mouseGrabbed(testinput))
    debugSetLMB(1);
    LUT_SUB_ASSERT_TRUE(dummy.mouseGrabbed(testinput)) //mouse down on element so it's grabbed
    debugSetMousePos(100, 100);
    LUT_SUB_ASSERT_TRUE(dummy.mouseGrabbed(testinput)) //mouse moved away but still down ==> still grabbed
    LUT_SUB_ASSERT_TRUE(dummy.mouseGetGrabX() == 15 && dummy.mouseGetGrabY() == 15)
    debugSetLMB(0);
    LUT_SUB_ASSERT_TRUE(!dummy.mouseGrabbed(testinput)) //mouse button up ==> not grabbed anymore
    debugSetLMB(1);
    LUT_SUB_ASSERT_TRUE(!dummy.mouseGrabbed(testinput)) //mouse down somewhere else not on element ==> not grabbed
    debugSetMousePos(11, 11);
    LUT_SUB_ASSERT_TRUE(!dummy.mouseGrabbed(testinput)) //mouse still down and moving over element, but it wasn't down HERE, so not grabbed*/
  LUT_CASE_END
  
  //the fact that the container and the window handles the elements, influences the result
  LUT_CASE("mouseGrabbed on top of window")
    Container c;
    Window w;
    w.make(0, 0, 100, 100);
    c.pushTopAt(&w, 0, 0);
    
    w.addTop();
    c.handle(testinput);c.handle(testinput);
    
    int mx = w.top.getCenterX();
    int my = w.top.getCenterY();
 
    c.handle(testinput);c.handle(testinput);
    LUT_SUB_ASSERT_TRUE(!w.top.mouseGrabbed(testinput)) //mouse not down yet
    debugSetMousePos(mx, my);
    debugSetLMB(1);
    c.handle(testinput);c.handle(testinput);
    LUT_SUB_ASSERT_TRUE(w.top.mouseGrabbed(testinput)) //mouse down on element so it's grabbed
    debugSetMousePos(200, 200);
    c.handle(testinput);c.handle(testinput);
    LUT_SUB_ASSERT_TRUE(w.top.mouseGrabbed(testinput)) //mouse moved away but still down ==> still grabbed
    LUT_SUB_ASSERT_TRUE(w.top.mouseGetGrabX() == mx && w.top.mouseGetGrabY() == my)
    debugSetLMB(0);
    c.handle(testinput);c.handle(testinput);
    LUT_SUB_ASSERT_TRUE(!w.top.mouseGrabbed(testinput)) //mouse button up ==> not grabbed anymore
    debugSetLMB(1);
    c.handle(testinput);c.handle(testinput);
    LUT_SUB_ASSERT_TRUE(w.top.mouseGrabbed(testinput)) //mouse down somewhere else not on element ==> normally not grabbed, BUT, the window moves!! ==> under mouse and grabbed again
  LUT_CASE_END
  
  LUT_CASE("dragging a window")
    Container c;
    Window w;
    w.make(0, 0, 100, 100);
    c.pushTopAt(&w, 0, 0);
    
    w.addTop();
    c.handle(testinput);c.handle(testinput);
    
    int mx = w.top.getCenterX();
    int my = w.top.getCenterY();
    
    int wx1 = w.getX0();
    int wy1 = w.getY0();
    
    debugSetMousePos(mx, my); //mouse above top bar of the window
    debugSetLMB(1);
    c.handle(testinput);c.handle(testinput);
    
    int wx2 = w.getX0();
    int wy2 = w.getY0();
    
    LUT_SUB_ASSERT_TRUE(wx1 == wx2 && wy1 == wy2)
    
    debugSetMousePos(mx + 100, my + 50);
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
    debugSetMousePos(10, 10); //mouse above
    LUT_SUB_ASSERT_TRUE(!dummy.mouseJustDown(testinput))
    LUT_SUB_ASSERT_TRUE(!dummy.mouseJustDownHere(testinput))
    debugSetLMB(1); //press button
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
    debugSetMousePos(1, 1);
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
    LUT_SUB_ASSERT_TRUE(!line.isActive())
    
    //move mouse above it and click
    debugSetMousePos(1, 1);
    debugSetLMB(1);
    line.handle(testinput);
    //it must be active now!
    LUT_SUB_ASSERT_TRUE(line.isActive())
    
    //now release the mouse button
    debugSetLMB(0);
    line.handle(testinput);
    //it should still be active!
    LUT_SUB_ASSERT_TRUE(line.isActive())
    
    //move mouse away
    debugSetMousePos(100, 100);
    line.handle(testinput);
    //it should still be active!
    LUT_SUB_ASSERT_TRUE(line.isActive())
    
    //move mouse away from it and click
    debugSetMousePos(100, 100);
    debugSetLMB(1);
    line.handle(testinput);
    //it may not be active anymore after clicking on a location outside (???????)
    LUT_SUB_ASSERT_TRUE(!line.isActive())
  LUT_CASE_END
  LUT_CASE("selfActivate of InputLine in Window")
    Container c;
    Window w;
    w.make(0, 0, 100, 100);
    c.pushTopAt(&w, 0, 0);
    
    InputLine line;
    line.make(0, 0, 10);
    w.pushTopAt(&line, 0, 0);
    
    //initially it's not active
    c.handle(testinput); //handle line all the time to let the selfActivate system work
    LUT_SUB_ASSERT_TRUE(!line.isActive())
    
    //now a little test to see if it's at the correct position for these tests below
    debugSetMousePos(line.getCenterX(), line.getCenterY());
    line.setActive(true);
    line.setElementOver(false);
    LUT_SUB_ASSERT_TRUE(line.mouseOver(testinput))
    line.setElementOver(true);
    line.setActive(false);
    
    //move mouse above it and click
    debugSetMousePos(line.getCenterX(), line.getCenterY());
    debugSetLMB(1);
    c.handle(testinput);
    //it must be active now!
    LUT_SUB_ASSERT_TRUE(line.isActive())
    
    //now release the mouse button
    debugSetLMB(0);
    c.handle(testinput);
    //it should still be active!
    LUT_SUB_ASSERT_TRUE(line.isActive())
    
    //move mouse away
    debugSetMousePos(100, 100);
    c.handle(testinput);
    //it should still be active!
    LUT_SUB_ASSERT_TRUE(line.isActive())
    
    //move mouse away from it and click
    debugSetMousePos(100, 100);
    debugSetLMB(1);
    c.handle(testinput);
    //it may not be active anymore after clicking on a location outside (???????)
    LUT_SUB_ASSERT_TRUE(!line.isActive())
  LUT_CASE_END
  
  //two things on a container, both at the same location, first the one is totallyEnabled and the other totallyDisabled, then vica versa, then each time only the enabled one may return true to mouseOver
  LUT_CASE("enabled and disabled on each other")
    Container c;
    Window w;
    w.make(0, 0, 100, 100);
    c.pushTopAt(&w, 0, 0);
    
    Dummy dummy1;
    dummy1.resize(0, 0, 50, 50);
    w.pushTopAt(&dummy1, 0, 0);
    
    Dummy dummy2;
    dummy2.resize(0, 0, 50, 50);
    w.pushTopAt(&dummy2, 0, 0);
    
    debugSetMousePos(dummy1.getCenterX(), dummy1.getCenterY()); //doesn't matter if you take center of dummy1 or dummy2, both are exactly at same position
    
    dummy1.totallyEnable();
    dummy2.totallyDisable();
    
    LUT_SUB_ASSERT_TRUE(dummy1.mouseOver(testinput))
    LUT_SUB_ASSERT_TRUE(!dummy2.mouseOver(testinput))
    
    dummy1.totallyDisable();
    dummy2.totallyEnable();
    
    LUT_SUB_ASSERT_TRUE(!dummy1.mouseOver(testinput))
    LUT_SUB_ASSERT_TRUE(dummy2.mouseOver(testinput))
  LUT_CASE_END
  
  //dragging a window that has a top
  LUT_CASE("dragging of a window")
  
    Container c;
    Window w;
    w.make(0, 0, 100, 100);
    w.addTop();
    c.pushTopAt(&w, 0, 0);
    
    int mx = w.top.getCenterX();
    int my = w.top.getCenterY();
    
    //grab the window
    debugSetMousePos(mx, my);
    debugSetLMB(1);
    
    c.handle(testinput);
    
    //move
    debugSetMousePos(200, 200);
    
    c.handle(testinput);
    
    //now the window must still be below the mouse
    LUT_SUB_ASSERT_TRUE(w.mouseOver(testinput))
  LUT_CASE_END
  
  //when there are two windows, A and B, and you're dragging one over the other, but you drag the mouse fast and the mousepointer goes to a location not over the dragged window (the dragged window will be under the mouse again soon), then the other window may not swap to top
  LUT_CASE("fast dragging of one window over another")
  
    Container c;
    Window A;
    A.make(0, 0, 100, 100);
    A.addTop();
    c.pushTopAt(&A, 0, 0);
    
    Window B;
    B.make(0, 0, 100, 100);
    B.addTop();
    c.pushTopAt(&B, 200, 0);
    
    int ax = A.top.getCenterX();
    int ay = A.top.getCenterY();
    int bx = B.top.getCenterX();
    int by = B.top.getCenterY();
    
    //grab the window A
    debugSetMousePos(ax, ay);
    debugSetLMB(1);
    
    c.handle(testinput);
    
    //move it over window B
    debugSetMousePos(bx, by);
    
    c.handle(testinput);
    
    //now window A must be over window B, so window A must have mouseOver, window B not
    
    LUT_SUB_ASSERT_TRUE(A.mouseOver(testinput))
    LUT_SUB_ASSERT_TRUE(!B.mouseOver(testinput))
  
  LUT_CASE_END
  
  //dragging a window that has a top that is shifted: when starting to drag, the window may not "jump" (but I once had)
  LUT_CASE("dragging of a window with shifted top")
  
    Container c;
    Window w;
    w.make(0, 0, 100, 100);
    w.addTop(&lpi::gui::builtInTexture[47], 2, 2, 2); //could cause jump of two pixels when the bug was there
    c.pushTopAt(&w, 0, 0);
    
    int mx = w.top.getCenterX();
    int my = w.top.getCenterY();
    
    //grab the window
    debugSetMousePos(mx, my);
    debugSetLMB(1);
    
    c.handle(testinput);
    
    LUT_SUB_ASSERT_TRUE(w.getX0() == 0 && w.getY0() == 0); //NOT shifted over two pixels
  LUT_CASE_END

  LUT_END_UNIT_TEST
}


////////////////////////////////////////////////////////////////////////////////

} //namespace gui
} //namespace lpi