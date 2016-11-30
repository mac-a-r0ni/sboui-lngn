#pragma once

#include <string>
#include "ListBox.h"

/*******************************************************************************

ListBox for making a selection

*******************************************************************************/
class SelectionBox: public ListBox {
  
  protected:

    std::string _info;

    void redrawFrame() const;
    void redrawSingleItem(unsigned int idx);

  public:

    /* Constructors */

    SelectionBox();
    SelectionBox(WINDOW *win, const std::string & name);

    /* Set attributes */

    void setInfo(const std::string & info);

    /* Get attributes */

    void minimumSize(int & height, int & width) const;
    void preferredSize(int & height, int & width) const;

    /* User interaction loop */

    std::string exec();
};