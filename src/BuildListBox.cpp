#include <vector>
#include <string>
#include <cmath>     // floor
#include <algorithm> // min
#include "Color.h"
#include "color_settings.h"
#include "BuildListItem.h"
#include "ListBox.h"
#include "BuildListBox.h"

using namespace color;

/*******************************************************************************

Draws window border, title, and header

*******************************************************************************/
void BuildListBox::redrawFrame() const
{
  unsigned int rows, cols, namelen, i, nspaces, vlineloc;
  double mid, left, right;
  int pair_header;

  getmaxyx(_win, rows, cols);

  // Title

  namelen = _name.size();
  mid = double(cols)/2.0;
  left = std::floor(int(mid) - double(namelen)/2);
  right = left + namelen;
  wmove(_win, 0, left);
  wattron(_win, A_BOLD);
  wprintw(_win, _name.c_str());
  wattroff(_win, A_BOLD);

  // Corners

  wmove(_win, 0, 0);
  waddch(_win, ACS_ULCORNER);
  wmove(_win, rows-1, 0);
  waddch(_win, ACS_LLCORNER);
  wmove(_win, rows-1, cols-1);
  waddch(_win, ACS_LRCORNER);
  wmove(_win, 0, cols-1);
  waddch(_win, ACS_URCORNER);

  // Top border

  wmove(_win, 0, 1);
  for ( i = 1; int(i) < left-1; i++ ) { waddch(_win, ACS_HLINE); }
  wmove(_win, 0, right+1);
  for ( i = right+1; i < cols-1; i++ ) { waddch(_win, ACS_HLINE); }
  
  // Left border

  for ( i = 1; i < rows-1; i++ ) { mvwaddch(_win, i, 0, ACS_VLINE); }

  // Right border

  for ( i = 1; i < rows-1; i++ ) { mvwaddch(_win, i, cols-1, ACS_VLINE); }

  // Bottom border

  wmove(_win, rows-1, 1);
  for ( i = 1; i < cols-1; i++ ) { waddch(_win, ACS_HLINE); }

  // Symbols on right border to indicate scrolling

  if (_firstprint != 0) { mvwaddch(_win, 3, cols-1, ACS_DIAMOND); }
  if (_items.size() > _firstprint + rows-4)
  {
    mvwaddch(_win, rows-2, cols-1, ACS_DIAMOND);
  }

  // Draw header

  pair_header = colors.pair(header, bg_normal);

  wmove(_win, 1, 1);
  if (pair_header != -1) { wattron(_win, COLOR_PAIR(pair_header)); }
  wattron(_win, A_BOLD);
  wprintw(_win, "Name");
  if (pair_header != -1) { wattroff(_win, COLOR_PAIR(pair_header)); }
  wattroff(_win, A_BOLD);

  vlineloc = cols-2 - std::string("Installed").size();
  nspaces = vlineloc - std::string("Name").size();
  for ( i = 0; i < nspaces; i++ ) { waddch(_win, ' '); }

  if (pair_header != -1) { wattron(_win, COLOR_PAIR(pair_header)); }
  wattron(_win, A_BOLD);
  printToEol("Installed");
  wmove(_win, 2, 1);
  if (pair_header != -1) { wattroff(_win, COLOR_PAIR(pair_header)); }
  wattroff(_win, A_BOLD);

  // Draw horizontal and then vertical line

  for ( i = 1; i < cols-1; i++ ) { waddch(_win, ACS_HLINE); }
  for ( i = 1; i < rows-1; i++ ) { mvwaddch(_win, i, vlineloc, ACS_VLINE); }

  // Draw connections between horizontal and vertical lines

  mvwaddch(_win, 2, 0, ACS_LTEE);
  mvwaddch(_win, 2, cols-1, ACS_RTEE);
  mvwaddch(_win, 0, vlineloc, ACS_TTEE);
  mvwaddch(_win, 2, vlineloc, ACS_PLUS);
  mvwaddch(_win, rows-1, vlineloc, ACS_BTEE);
}

/*******************************************************************************
 
Redraws a single item. Note: doesn't check if the item is actually on the
screen or not.

*******************************************************************************/
void BuildListBox::redrawSingleItem(unsigned int idx)
{
  std::string fg, bg;
  int color_pair, nspaces, vlineloc, printlen;
  unsigned int rows, cols, i; 

  getmaxyx(_win, rows, cols);

  // Go to item location, optionally highlight, and print item

  wmove(_win, idx-_firstprint+3, 1);

  // Turn on highlight color

  if (int(idx) == _highlight)
  {
    if (_activated) 
    { 
      if (_items[idx]->tagged()) { fg = tagged; }
      else { fg = fg_highlight_active; }
      bg = bg_highlight_active; 
    }
    else
    {
      if (_items[idx]->tagged()) { fg = tagged; }
      else { fg = fg_highlight_inactive; }
      bg = bg_highlight_inactive; 
    }
    color_pair = colors.pair(fg, bg);
    if (color_pair != -1) { wattron(_win, COLOR_PAIR(color_pair)); }
    else { wattron(_win, A_REVERSE); }
  } 
  else
  {
    if (_items[idx]->tagged()) { fg = tagged; }
    else { fg = fg_normal; }
    bg = bg_normal;
    color_pair = colors.pair(fg, bg);
    if (color_pair != -1) { wattron(_win, COLOR_PAIR(color_pair)); }
  }

  if (_items[idx]->tagged()) { wattron(_win, A_BOLD); }

  // Save highlight idx for redrawing later.
  // Note: prevents this method from being const.
  
  if (int(idx) == _highlight) { _prevhighlight = _highlight; }

  // Print item and turn off tag color

  vlineloc = cols-2 - std::string("Installed").size() - 1;
  printlen = std::min(int(_items[idx]->name().size()), vlineloc);
  wprintw(_win, _items[idx]->name().substr(0,printlen).c_str());

  if (_items[idx]->tagged()) 
  { 
    wattroff(_win, A_BOLD); 
    if (color_pair != -1) { wattroff(_win, COLOR_PAIR(color_pair)); }
    if (int(idx) == _highlight)
    {
      if (_activated) 
      {
        fg = fg_highlight_active;
        bg = bg_highlight_active;
      }
      else
      {
        fg = fg_highlight_inactive;
        bg = bg_highlight_inactive;
      }
    }
    else
    {
      fg = fg_normal;
      bg = bg_normal;
    }
    color_pair = colors.pair(fg, bg);
    if (color_pair != -1) { wattron(_win, COLOR_PAIR(color_pair)); }
    else { wattron(_win, A_REVERSE); }
  }

  // Print spaces, divider, install status

  nspaces = vlineloc - _items[idx]->name().size();
  for ( i = 0; int(i) < nspaces; i++ ) { waddch(_win, ' '); }
  waddch(_win, ACS_VLINE);
  if (_items[idx]->installed()) { printToEol("    x    "); }
  else { printToEol("         "); }

  // Turn off color

  if (color_pair != -1) { wattroff(_win, COLOR_PAIR(color_pair)); }
  else
  {
    if (int(idx) == _highlight) { wattroff(_win, A_REVERSE); }
  }
}

/*******************************************************************************

Constructors

*******************************************************************************/
BuildListBox::BuildListBox()
{
  _win = NULL;
  _name = "";
  _items.resize(0);
  _highlight = 0;
  _firstprint = 0;
  _prevhighlight = 0;
  _activated = true;
  _reserved_rows = 4;
}

BuildListBox::BuildListBox(WINDOW *win, const std::string & name)
{
  _win = win;
  _name = name;
  _items.resize(0);
  _highlight = 0;
  _firstprint = 0;
  _prevhighlight = 0;
  _activated = true;
  _reserved_rows = 4;
}

/*******************************************************************************

Checks if all items are tagged

*******************************************************************************/
bool BuildListBox::allTagged() const
{
  unsigned int i, nitems;
  bool all_tagged;

  nitems = _items.size();
  all_tagged = true;
  for ( i = 0; i < nitems; i++ )
  {
    if (! _items[i]->tagged()) 
    {
      all_tagged = false;
      break;
    }
  }

  return all_tagged;
}

/*******************************************************************************

Tags or untags (if everything is already tagged) items. Returns 0 if the
operation tags everything, and 1 if it untags everything.

*******************************************************************************/
unsigned int BuildListBox::tagAll()
{
  unsigned int i, nitems, retval;

  // First check if everything is tagged

  nitems = _items.size();
  if (! allTagged())
  {
    for ( i = 0; i < nitems; i++ ) 
    { 
      if (! _items[i]->tagged()) { _items[i]->setTagged(true); }
    }
    retval = 0;
  }
  else
  {
    for ( i = 0; i < nitems; i++ ) 
    { 
      if (_items[i]->tagged()) { _items[i]->setTagged(false); }
    }
    retval = 1;
  }

  // Set all items to be redrawn

  _redraw_type = "items";

  return retval;
}

/*******************************************************************************

User interaction: returns key stroke or selected item name

*******************************************************************************/
std::string BuildListBox::exec()
{
  int ch, check_redraw;
  std::string retval;

  const int MY_ESC = 27;
  const int MY_TAB = 9;

  // Highlight first entry on first display

  if (_highlight == 0) { highlightFirst(); }

  // Draw list elements

  draw();

  // Get user input

  switch (ch = getch()) {

    // Enter key: return name of highlighted item

    case '\n':
    case '\r':
    case KEY_ENTER:
      retval = _items[_highlight]->name();
      _redraw_type = "none";
      break;

    // Tab key: return keyTabSignal

    case MY_TAB:
      retval = keyTabSignal;
      _redraw_type = "changed";
      break;

    // Arrows/Home/End/PgUp/Dn: change highlighted value

    case KEY_UP:
      retval = highlightSignal;
      check_redraw = highlightPrevious();
      if (check_redraw == 1) { _redraw_type = "all"; }
      else { _redraw_type = "changed"; }
      break;
    case KEY_DOWN:
      retval = highlightSignal;
      check_redraw = highlightNext();
      if (check_redraw == 1) { _redraw_type = "all"; }
      else { _redraw_type = "changed"; }
      break;
    case KEY_PPAGE:
      retval = highlightSignal;
      check_redraw = highlightPreviousPage();
      if (check_redraw == 1) { _redraw_type = "all"; }
      else { _redraw_type = "changed"; }
      break;
    case KEY_NPAGE:
      retval = highlightSignal;
      check_redraw = highlightNextPage();
      if (check_redraw == 1) { _redraw_type = "all"; }
      else { _redraw_type = "changed"; }
      break;
    case KEY_HOME:
      retval = highlightSignal;
      check_redraw = highlightFirst();
      if (check_redraw == 1) { _redraw_type = "all"; }
      else { _redraw_type = "changed"; }
      break;
    case KEY_END:
      retval = highlightSignal;
      check_redraw = highlightLast();
      if (check_redraw == 1) { _redraw_type = "all"; }
      else { _redraw_type = "changed"; }
      break;

    // Resize signal: redraw (may not work with some curses implementations)

    case KEY_RESIZE:
      retval = resizeSignal;
      break;

    // Quit key

    case MY_ESC:
      retval = quitSignal;
      _redraw_type = "none";
      break;

    // t: tag item

    case 't':
      retval = tagSignal;
      _items[_highlight]->setTagged(! _items[_highlight]->tagged());
      _redraw_type = "changed";
      break;

    default:
      _redraw_type = "none";
      break;
  }
  return retval;
}