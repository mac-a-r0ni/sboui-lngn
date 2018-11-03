#include <string>
#include <vector>
#include <curses.h>
#include <algorithm>    // max
#include "Color.h"
#include "settings.h"
#include "signals.h"
#include "MenubarListItem.h"
#include "MenubarList.h"
#include "Menubar.h"
#include "MouseEvent.h"

/*******************************************************************************

Determines column of first character in MenubarList lidx

*******************************************************************************/
unsigned int Menubar::menuColumn(unsigned int lidx) const
{
  unsigned int i, col;

  if (lidx >= _lists.size())
    return 0;

  col = 0;
  for ( i = 0; i < lidx; i++ )
  {
    col += _lists[i].name().size() + 2*_pad;
  }

  return col;
}

/*******************************************************************************

Places and sizes list box

*******************************************************************************/
void Menubar::placeListBox()
{
  int width, height;
  unsigned int left;

  // Get position and size

  left = menuColumn(_highlight);
  _lists[_highlight].preferredSize(height, width);

  // Set position and size

  mvwin(_listwins[_highlight], 1, left);
  wresize(_listwins[_highlight], height, width);
}

/*******************************************************************************

Highlights first menu label

*******************************************************************************/
void Menubar::highlightFirst()
{
  if (_lists.size() == 0)
    return;

  _prevhighlight = _highlight;
  _highlight = 0;
}

/*******************************************************************************

Highlights last menu label

*******************************************************************************/
void Menubar::highlightLast()
{
  if (_lists.size() == 0)
    return;

  _prevhighlight = _highlight;
  _highlight = std::max(int(_lists.size()) - 1, 0);
}

/*******************************************************************************

Highlights previous menu label

*******************************************************************************/
void Menubar::highlightPrevious()
{
  if (_lists.size() == 0)
    return;

  if (_highlight == 0)
    return;
  else
  {
    _prevhighlight = _highlight;
    _highlight -= 1;
  }
}

/*******************************************************************************

Highlights next menu label

*******************************************************************************/
void Menubar::highlightNext()
{
  if (_lists.size() == 0)
    return;

  if (_highlight == int(_lists.size())-1)
    return;
  else
  {
    _prevhighlight = _highlight;
    _highlight += 1;
  }
}

/*******************************************************************************

Redraws a single menu label

*******************************************************************************/
void Menubar::redrawSingleItem(unsigned int idx)
{
  unsigned int nlists, pos, i;
  int color_pair;

  nlists = _lists.size();
  if (idx >= nlists)
    return;

  // Get drawing position and color

  pos = menuColumn(idx);
  if ( (int(idx) == _highlight) && (_activated) )
    color_pair = colors.getPair("fg_highlight_active", "bg_highlight_active");
  else
    color_pair = colors.getPair(_fg_color, _bg_color);

  // Save highlight idx for redrawing later.
  // Note: prevents this method from being const.

  if (int(idx) == _highlight)
    _prevhighlight = _highlight;

  // Print item

  wmove(_win, 0, pos);
  if (colors.turnOn(_win, color_pair) != 0)
  {
    if ( (int(idx) == _highlight) && (_activated) )
      wattron(_win, A_REVERSE);
  }
  for ( i = 0; i < _pad; i++ )
  {
    waddch(_win, ' ');
  }
  wprintw(_win, _lists[idx].name().c_str());
  for ( i = 0; i < _pad; i++ )
  {
    waddch(_win, ' ');
  }
  if (colors.turnOff(_win) != 0)
  {
    if ( (int(idx) == _highlight) && (_activated) )
      wattroff(_win, A_REVERSE);
  }
}

/*******************************************************************************

Redraws the previously and currently highlighted menu labels

*******************************************************************************/
void Menubar::redrawChangedItems()
{
  if (_prevhighlight < int(_lists.size()))
    redrawSingleItem(_prevhighlight);

  if (_highlight < int(_lists.size()))
    redrawSingleItem(_highlight);
}

/*******************************************************************************

Redraws all menu labels

*******************************************************************************/
void Menubar::redrawAllItems()
{
  unsigned int i, nlists;

  nlists = _lists.size();
  for ( i = 0; i < nlists; i++ ) { redrawSingleItem(i); }
}

/*******************************************************************************

User interaction with highlighted list

*******************************************************************************/
std::string Menubar::execList(MouseEvent * mevent)
{
  std::string retval;

  placeListBox();
  retval = _lists[_highlight].exec();

  wresize(_listwins[_highlight], 0, 0);
  wrefresh(_listwins[_highlight]);

  if (_parent)
    _parent->draw(true);

  return retval;
}

/*******************************************************************************

Constructors and destructor

*******************************************************************************/
Menubar::Menubar()
{
  _parent = NULL;
  _highlight = 0;
  _prevhighlight = 0;
  _activated = false;
  _lists.resize(0);
  _listwins.resize(0);
  _fg_color = "fg_title";
  _bg_color = "bg_title";
  _redraw_type = "all";
  _pad = 0;
  _outerpad = 0;
  _innerpad = 0;
}
Menubar::Menubar(WINDOW *win)
{
  _parent = NULL;
  _win = win;
  _highlight = 0;
  _prevhighlight = 0;
  _activated = false;
  _lists.resize(0);
  _listwins.resize(0);
  _fg_color = "fg_title";
  _bg_color = "bg_title";
  _redraw_type = "all";
  _pad = 0;
  _outerpad = 0;
  _innerpad = 0;
}
Menubar::~Menubar() { clearLists(); }

/*******************************************************************************

Set attributes

*******************************************************************************/
void Menubar::setActivated(bool activated) { _activated = activated; }
void Menubar::setParent(CursesWidget * parent) { _parent = parent; }
void Menubar::setPad(unsigned int pad) { _pad = pad; }
void Menubar::setListPad(unsigned int outerpad, unsigned int innerpad)
{
  unsigned int i, nlists;

  _outerpad = outerpad;
  _innerpad = innerpad;
  nlists = _lists.size();
  for ( i = 0; i < nlists; i++ )
  {
    _lists[i].setPad(outerpad, innerpad);
  }
}

/*******************************************************************************

Adds a list and returns 0, or does nothing and returns 1 if a list already
exists by that name.

*******************************************************************************/
int Menubar::addList(const std::string & lname)
{
  unsigned int i, nlists;

  nlists = _lists.size();
  for ( i = 0; i < nlists; i++ )
  {
    if (lname == _lists[i].name())
      return 1;
  }

  MenubarList list;
  _listwins.push_back(newwin(1, 1, 0, 0));
  list.setName(lname);
  list.setWindow(_listwins[nlists]);
  list.setPad(_outerpad, _innerpad);
  _lists.push_back(list);

  return 0;
}

/*******************************************************************************

Adds an item to a list by list idx. Returns 0 if successful. Returns 1 if list
doesn't exist, or returns 2 if list exists but already has an item by that name.

*******************************************************************************/
int Menubar::addListItem(unsigned int lidx, const std::string & text,
                         const std::string & shortcut, int hotkey)
{
  unsigned int nlists, j, nitems;

  nlists = _lists.size();
  if (lidx >= nlists)
    return 1;

  nitems = _lists[lidx].numItems();
  for ( j = 0; j < nitems; j++ )
  {
    if (_lists[lidx].itemByIdx(j)->name() == text)
      return 2;
  }
  _lists[lidx].addItem(new MenubarListItem(text, shortcut, hotkey));

  return 0;
}

/*******************************************************************************

Adds an item to a list by list name. Returns 0 if successful. Returns 1 if list
doesn't exist, or returns 2 if list exists but already has an item by that name.

*******************************************************************************/
int Menubar::addListItem(const std::string & lname, const std::string & text,
                         const std::string & shortcut, int hotkey)
{
  unsigned int i, nlists;

  nlists = _lists.size();
  for ( i = 0; i < nlists; i++ )
  {
    if (_lists[i].name() == lname)
      return addListItem(i, text, shortcut, hotkey);
  }

  return 1;
}

/*******************************************************************************

Removes all lists

*******************************************************************************/
void Menubar::clearLists()
{
  unsigned int i, nlists;

  nlists = _lists.size();
  for ( i = 0; i < nlists; i++ )
  {
    _lists[i].clearList();
    delwin(_listwins[i]);
  }

  _lists.resize(0);
  _listwins.resize(0);
  _highlight = 0;
  _prevhighlight = 0;
}

/*******************************************************************************

Get attributes

*******************************************************************************/
unsigned int Menubar::numLists() const { return _lists.size(); }
void Menubar::minimumSize(int & height, int & width) const
{
  int rows, cols;

  getmaxyx(_win, rows, cols);
  height = 1;
  width = cols;
}

void Menubar::preferredSize(int & height, int & width) const
{
  minimumSize(height, width);
}

/*******************************************************************************

Handles mouse event

*******************************************************************************/
std::string Menubar::handleMouseEvent(MouseEvent * mevent)
{
  return "";
}

/*******************************************************************************

Draws menubar

*******************************************************************************/
void Menubar::draw(bool force)
{
  if (force) { _redraw_type = "all"; }

  if (_redraw_type == "all")
  {
    int rows, cols;

    getmaxyx(_win, rows, cols);
    wmove(_win, 0, 0);
    colors.turnOn(_win, _fg_color, _bg_color);
    printToEol("", cols);
    colors.turnOff(_win);
    redrawAllItems();
  }
  else
    redrawChangedItems();
  wrefresh(_win);
}

/*******************************************************************************

User interaction

*******************************************************************************/
std::string Menubar::exec(MouseEvent * mevent)
{
  std::string retval;
  bool getting_input;

  // Highlight first entry on first display

  if ( (_highlight == 0) && (_prevhighlight == 0) ) { highlightFirst(); }

  getting_input = true;
  while (getting_input)
  {
    // Draw list elements

    draw();
    _redraw_type = "all";

    // Get user input

    retval = execList(mevent);

    // Enter/Esc/F9: leave menu

    if (retval == signals::keyF9)
      retval = signals::quit;

    if ( (retval == signals::keyEnter) || (retval == signals::quit) )
      getting_input = false;

    // Right/left: change highlighted menu label

    else if (retval == signals::keyRight)
    {
      _redraw_type = "changed";
      highlightNext();
    }

    else if (retval == signals::keyLeft)
    {
      _redraw_type = "changed";
      highlightPrevious();
    }

    // Mouse
    //FIXME: handle this

    else if (retval == signals::mouseEvent)
      getting_input = false;
  }

  return retval;
}