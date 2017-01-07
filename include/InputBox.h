#pragma once

#include <string>
#include <vector>
#include <curses.h>
#include "InputItem.h"

/*******************************************************************************

Box containing a number of InputItems

*******************************************************************************/
class InputBox {

  protected:

    WINDOW *_win;
    std::string _msg, _info, _redraw_type;
    std::vector<InputItem *> _items;    
    unsigned int _highlight, _prevhighlight;

    /* Prints to end of line, padding with spaces and avoiding borders */

    void printToEol(const std::string & msg) const;
    void printSpaces(unsigned int nspaces) const;

    /* Setting item to be highlighted */

    void highlightFirst();
    void highlightLast();
    void highlightPrevious();
    void highlightNext();

    /* Drawing */
    
    virtual void redrawFrame() const;

  public:

    /* Constructors */

    InputBox();
    InputBox(WINDOW *win, const std::string & msg);

    /* Add items */

    void addItem(InputItem *item);

    /* Set attributes */

    void setWindow(WINDOW *win);
    void setMessage(const std::string & msg);
    void setInfo(const std::string & info);

    /* Get attributes */

    virtual void minimumSize(int & height, int & width) const;
    virtual void preferredSize(int & height, int & width) const;

    /* Draws frame, entry, etc. as needed */

    virtual void draw(bool force=false);

    /* User interaction loop */

    virtual std::string exec();
};