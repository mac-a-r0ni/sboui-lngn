#pragma once

#include <string>
#include <curses.h>
#include "ListBox.h"
#include "MouseEvent.h"

/*******************************************************************************

List displayed by a ComboBox

*******************************************************************************/
class ComboBoxList: public ListBox {
  
  protected:

    void redrawFrame();
    void redrawSingleItem(unsigned int idx);

  public:

    /* Constructors */

    ComboBoxList();
    ComboBoxList(WINDOW *win);

    /* Edit list */

    void clearList();

    /* Accessing properties */

    void preferredSize(int & height, int & width) const;

    /* Drawing */

    void draw(bool force=false);

    /* User interaction loop */

    std::string exec(MouseEvent * mevent=NULL);
};
