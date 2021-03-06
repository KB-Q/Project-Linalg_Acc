
// DO NOT EDIT THIS FILE - it is machine generated -*- c++ -*-

#ifndef __javax_swing_plaf_basic_BasicToolBarSeparatorUI__
#define __javax_swing_plaf_basic_BasicToolBarSeparatorUI__

#pragma interface

#include <javax/swing/plaf/basic/BasicSeparatorUI.h>
extern "Java"
{
  namespace java
  {
    namespace awt
    {
        class Dimension;
        class Graphics;
    }
  }
  namespace javax
  {
    namespace swing
    {
        class JComponent;
        class JSeparator;
      namespace plaf
      {
          class ComponentUI;
        namespace basic
        {
            class BasicToolBarSeparatorUI;
        }
      }
    }
  }
}

class javax::swing::plaf::basic::BasicToolBarSeparatorUI : public ::javax::swing::plaf::basic::BasicSeparatorUI
{

public:
  BasicToolBarSeparatorUI();
  static ::javax::swing::plaf::ComponentUI * createUI(::javax::swing::JComponent *);
public: // actually protected
  virtual void installDefaults(::javax::swing::JSeparator *);
public:
  virtual void paint(::java::awt::Graphics *, ::javax::swing::JComponent *);
  virtual ::java::awt::Dimension * getPreferredSize(::javax::swing::JComponent *);
  virtual ::java::awt::Dimension * getMinimumSize(::javax::swing::JComponent *);
  virtual ::java::awt::Dimension * getMaximumSize(::javax::swing::JComponent *);
private:
  ::java::awt::Dimension * __attribute__((aligned(__alignof__( ::javax::swing::plaf::basic::BasicSeparatorUI)))) size;
public:
  static ::java::lang::Class class$;
};

#endif // __javax_swing_plaf_basic_BasicToolBarSeparatorUI__
