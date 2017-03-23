#pragma once

#include <string>
#include <curses.h>
#include "ComboBoxList.h"
#include "InputItem.h"

class InputBox;

/*******************************************************************************

Combo box input item

*******************************************************************************/
class ComboBox: public InputItem {

  private:

    WINDOW *_listwin;
    ComboBoxList _list;
    InputBox *_parent;

    /* Placing and sizing list box */

    void placeListBox(int y_offset);

    /* User interaction with list */

    void execList(int y_offset);

  public:

    /* Constructor and destructor */

    ComboBox();
    ComboBox(InputBox *parent);
    ~ComboBox();

    /* Set attributes */

    void setParent(InputBox *parent);
    void addChoice(const std::string & choice);
    void setChoice(unsigned int idx);
    void setChoice(const std::string & choice);

    /* User interaction */

    void draw(int y_offset, bool force=false, bool highlight=false); 
    std::string exec(int y_offset);

    /* Accessing properties */

    int getIntProp() const;
    std::string getStringProp() const;
};