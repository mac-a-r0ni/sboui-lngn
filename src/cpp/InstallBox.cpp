#include <iostream>
#include <vector>
#include <string>
#include <curses.h>
#include <cmath>     // floor
#include <algorithm> // max, min
#include "Color.h"
#include "color_settings.h"
#include "string_util.h"
#include "signals.h"
#include "requirements.h"
#include "backend.h"
#include "BuildListItem.h"
#include "InstallBox.h"

using namespace color;

/*******************************************************************************

Draws window border and title

*******************************************************************************/
void InstallBox::redrawFrame() const
{
  unsigned int rows, cols, namelen, i, nspaces, vlineloc;
  double mid, left, right;
  int pair_title, pair_info, pair_header;

  getmaxyx(_win, rows, cols);

  // Info on bottom of window

  namelen = _info.size();
  mid = double(cols)/2.0;
  left = std::floor(mid - double(namelen)/2.0);
  wmove(_win, rows-2, 1);
  wclrtoeol(_win);
  pair_info = colors.pair(fg_info, bg_info);
  if (pair_info != -1) { wattron(_win, COLOR_PAIR(pair_info)); }
  wattron(_win, A_BOLD);
  printSpaces(left-1);
  printToEol(_info);
  if (pair_info != -1) { wattroff(_win, COLOR_PAIR(pair_info)); }
  wattroff(_win, A_BOLD);

  // Title

  namelen = _name.size();
  left = std::floor(mid - double(namelen)/2.0);
  right = left + namelen;
  wmove(_win, 0, left);
  pair_title = colors.pair(fg_title, bg_title);
  if (pair_title != -1) { wattron(_win, COLOR_PAIR(pair_title)); }
  wattron(_win, A_BOLD);
  wprintw(_win, _name.c_str());
  if (pair_title != -1) { wattroff(_win, COLOR_PAIR(pair_title)); }
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

  if (_firstprint != 0) { mvwaddch(_win, 3, cols-1, ACS_UARROW); }
  if (_items.size() > _firstprint + rows-4)
  {
    mvwaddch(_win, rows-4, cols-1, ACS_DARROW);
  }

  // Draw header

  wmove(_win, 1, 1);
  pair_header = colors.pair(header_popup, bg_popup);
  if (pair_header != -1) { wattron(_win, COLOR_PAIR(pair_header)); }
  wattron(_win, A_BOLD);
  wprintw(_win, "Name");

  vlineloc = cols-2 - std::string(" Reinstall ").size();
  nspaces = vlineloc - std::string("Name").size();
  for ( i = 0; i < nspaces; i++ ) { waddch(_win, ' '); }

  printToEol(" Action");
  if (pair_header != -1) { wattroff(_win, COLOR_PAIR(pair_header)); }
  wattroff(_win, A_BOLD);

  // Draw horizontal and then vertical lines

  wmove(_win, 2, 1);
  for ( i = 1; i < cols-1; i++ ) { waddch(_win, ACS_HLINE); }
  wmove(_win, rows-3, 1);
  for ( i = 1; i < cols-1; i++ ) { waddch(_win, ACS_HLINE); }
  for ( i = 3; i < rows-4; i++ ) { mvwaddch(_win, i, vlineloc, ACS_VLINE); }

  // Draw connections between horizontal and vertical lines

  mvwaddch(_win, 2, 0, ACS_LTEE);
  mvwaddch(_win, 2, cols-1, ACS_RTEE);
  mvwaddch(_win, rows-3, 0, ACS_LTEE);
  mvwaddch(_win, rows-3, cols-1, ACS_RTEE);
  mvwaddch(_win, 2, vlineloc, ACS_TTEE);
  mvwaddch(_win, rows-3, vlineloc, ACS_BTEE);
}

/*******************************************************************************
 
Redraws a single item. Note: doesn't check if the item is actually on the
screen or not.

*******************************************************************************/
void InstallBox::redrawSingleItem(unsigned int idx)
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
      fg = fg_highlight_active;
      bg = bg_highlight_active; 
    }
    else
    {
      fg = fg_highlight_inactive;
      bg = bg_highlight_inactive; 
    }
    color_pair = colors.pair(fg, bg);
    if (color_pair != -1) { wattron(_win, COLOR_PAIR(color_pair)); }
    else 
    { 
      if (_activated) { wattron(_win, A_REVERSE); }
    }
  } 
  else
  {
    fg = fg_popup;
    bg = bg_popup;
    color_pair = colors.pair(fg, bg);
    if (color_pair != -1) { wattron(_win, COLOR_PAIR(color_pair)); }
  }

  // Save highlight idx for redrawing later.
  // Note: prevents this method from being const.
  
  if (int(idx) == _highlight) { _prevhighlight = _highlight; }

  // Print item with selection, spaces, divider, action

  vlineloc = cols-2 - std::string(" Reinstall ").size() - 1;
  printlen = std::min(int(_items[idx]->name().size()), vlineloc-4);

  nspaces = vlineloc - 4 - (_items[idx]->name().size());
  if (_items[idx]->getBoolProp("tagged")) { wprintw(_win, "[X] "); }
  else { wprintw(_win, "[ ] "); }

  wprintw(_win, _items[idx]->name().substr(0,printlen).c_str());

  for ( i = 0; int(i) < nspaces; i++ ) { waddch(_win, ' '); }

  waddch(_win, ACS_VLINE);

  waddch(_win, ' ');
  printToEol(_items[idx]->getProp("action"));

  // Move to area between brackets to show cursor

  wmove(_win, _highlight-_firstprint+3, 2);

  // Turn off color

  if (color_pair != -1) { wattroff(_win, COLOR_PAIR(color_pair)); }
  else
  {
    if ( (int(idx) == _highlight) && _activated ) { wattroff(_win, A_REVERSE); }
  }
}

/*******************************************************************************

Constructors

*******************************************************************************/
InstallBox::InstallBox()
{ 
  _reserved_rows = 6;
  _info = "Enter: Ok | Esc: Cancel"; 
  _builds.resize(0);
}

InstallBox::InstallBox(WINDOW *win, const std::string & name)
{
  _reserved_rows = 6;
  _info = "Enter: Ok | Space: Toggle | Esc: Cancel"; 
  _builds.resize(0);
  _win = win;
  _name = name;
}

/*******************************************************************************

Get attributes

*******************************************************************************/
void InstallBox::minimumSize(int & height, int & width) const
{
  int namelen, reserved_cols, action_cols;
  unsigned int i, nitems;

  // Minimum usable height

  nitems = _items.size();
  height = _reserved_rows + 2;

  // Minimum usable width

  action_cols = std::string(" Reinstall ").size() + 1; // Room for divider
  reserved_cols = 2;
  width = _name.size() + action_cols;
  if (int(_info.size()) > width) { width = _info.size(); }
  for ( i = 0; i < nitems; i++ )
  {
    namelen = _items[i]->name().size() + 4 + action_cols;
    if (namelen > width) { width = namelen; }
  }
  width += reserved_cols;
}

void InstallBox::preferredSize(int & height, int & width) const
{
  int namelen, reserved_cols, widthpadding, action_cols;
  unsigned int i, nitems;

  // Preferred height: no scrolling

  nitems = _items.size();
  height = _reserved_rows + nitems;

  // Preferred width: minimum usable + some padding

  action_cols = std::string(" Reinstall ").size() + 1; // Room for divider
  widthpadding = 6;
  reserved_cols = 2;
  width = _name.size() + action_cols;
  if (int(_info.size()) > width) { width = _info.size(); }
  for ( i = 0; i < nitems; i++ )
  {
    namelen = _items[i]->name().size() + 4 + action_cols;
    if (namelen > width) { width = namelen; }
  }
  width += reserved_cols + widthpadding;
}

bool InstallBox::installingAllDeps() const
{
  int nreqs, i;

  nreqs = _items.size() - 1;   // Only consider dependencies, not the requested
                               // SlackBuild

  for ( i = 0; i < nreqs; i++ )
  {
    if ( (_items[i]->getProp("action") != "Reinstall") &&   
         (! _items[i]->getBoolProp("tagged")) ) { return false; }
  }

  return true;
}

/*******************************************************************************

Creates list based on SlackBuild selected. Returns 0 if dependency resolution
succeeded or 1 if some could not be found in the repository.

*******************************************************************************/
int InstallBox::create(BuildListItem & build,
                            std::vector<BuildListItem> & slackbuilds,
                            const std::string & action) 
{
  int check; 
  unsigned int nreqs, i, nbuilds;
  std::vector<BuildListItem *> reqlist;

  check = compute_reqs_order(build, reqlist, slackbuilds);
  if (check != 0) { return check; }

  // Create copy of reqlist and determine action

  nreqs = reqlist.size();
  nbuilds = 0;
  for ( i = 0; i < nreqs; i++ ) 
  { 
    if (action == "Remove")
    {
      if (reqlist[i]->getBoolProp("installed"))
      {
        _builds.push_back(*reqlist[i]);
        _builds[nbuilds].setBoolProp("tagged", false);
        _builds[nbuilds].addProp("action", "Remove");
        nbuilds++;
      }
    }
    else
    {
      _builds.push_back(*reqlist[i]); 
      if (! reqlist[i]->getBoolProp("installed"))
      {
        _builds[nbuilds].setBoolProp("tagged", true);
        _builds[nbuilds].addProp("action", "Install");
      }
      else
      {
        if (reqlist[i]->upgradable())
        {
          _builds[nbuilds].setBoolProp("tagged", true);
          _builds[nbuilds].addProp("action", "Upgrade");
        }
        else
        {
          _builds[nbuilds].setBoolProp("tagged", false);
          _builds[nbuilds].addProp("action", "Reinstall");
        }
      }
      nbuilds++;
    }
  }

  // Set action for requested SlackBuild and add it at the end

  _builds.push_back(build);
  _builds[nreqs].setBoolProp("tagged", true);
  _builds[nreqs].addProp("action", action);

  // Add to list (note have to do this separately because _builds changes
  // throughout the above loop)

  for ( i = 0; i <= nreqs; i++ ) { addItem(&_builds[i]); }

  // Set window title

  if (nreqs == 1)
  {
    if (action == "Remove")
      setName(build.name() + " (1 installed dep)");
    else
      setName(build.name() + " (1 dep)");
  }
  else
  {
    if (action == "Remove")
      setName(build.name() + " (" + int2string(nreqs) + " installed deps)");
    else
      setName(build.name() + " (" + int2string(nreqs) + " deps)");
  }

  return check;
}

/*******************************************************************************

User interaction: returns key stroke or other signal

*******************************************************************************/
std::string InstallBox::exec()
{
  int ch, check_redraw;
  std::string retval;

  const int MY_ESC = 27;

  curs_set(1);

  // Highlight first entry on first display

  if (_highlight == 0) { highlightFirst(); }

  // Draw list elements

  draw();

  // Get user input

  switch (ch = getch()) {

    // Enter key: accept selection

    case '\n':
    case '\r':
    case KEY_ENTER:
      retval = signals::keyEnter;
      _redraw_type = "all";
      break;

    // Arrows/Home/End/PgUp/Dn: change highlighted value

    case KEY_UP:
      retval = signals::highlight;
      check_redraw = highlightPrevious();
      if (check_redraw == 1) { _redraw_type = "all"; }
      else { _redraw_type = "changed"; }
      break;
    case KEY_DOWN:
      retval = signals::highlight;
      check_redraw = highlightNext();
      if (check_redraw == 1) { _redraw_type = "all"; }
      else { _redraw_type = "changed"; }
      break;
    case KEY_PPAGE:
      retval = signals::highlight;
      check_redraw = highlightPreviousPage();
      if (check_redraw == 1) { _redraw_type = "all"; }
      else { _redraw_type = "changed"; }
      break;
    case KEY_NPAGE:
      retval = signals::highlight;
      check_redraw = highlightNextPage();
      if (check_redraw == 1) { _redraw_type = "all"; }
      else { _redraw_type = "changed"; }
      break;
    case KEY_HOME:
      retval = signals::highlight;
      check_redraw = highlightFirst();
      if (check_redraw == 1) { _redraw_type = "all"; }
      else { _redraw_type = "changed"; }
      break;
    case KEY_END:
      retval = signals::highlight;
      check_redraw = highlightLast();
      if (check_redraw == 1) { _redraw_type = "all"; }
      else { _redraw_type = "changed"; }
      break;

    // Resize signal: redraw (may not work with some curses implementations)

    case KEY_RESIZE:
      retval = signals::resize;
      _redraw_type = "all";
      break;

    // Quit key

    case MY_ESC:
      retval = signals::quit;
      _redraw_type = "all";
      break;

    // Space: toggle item

    case ' ':
      retval = " ";
      _items[_highlight]->setBoolProp("tagged", 
                                 (! _items[_highlight]->getBoolProp("tagged")));
      check_redraw = highlightNext();
      if (check_redraw == 1) { _redraw_type = "all"; }
      else { _redraw_type = "changed"; }
      break;

    default:
      retval = char(ch);
      _redraw_type = "none";
      break;
  }
  curs_set(0);

  return retval;
}

/*******************************************************************************

Install, upgrade, reinstall, or remove SlackBuild and dependencies. Returns 0 on
success.

*******************************************************************************/
int InstallBox::applyChanges() const
{
  unsigned int nbuilds, i;
  int check, retval;
  std::string response, msg;

  // Install/upgrade/reinstall/remove tagged SlackBuilds

  nbuilds = _builds.size();
  retval = 0;
  for ( i = 0; i < nbuilds; i++ )
  {
    if (_builds[i].getBoolProp("tagged"))
    {
      if (_builds[i].getProp("action") == "Upgrade") { check = 
                                               upgrade_slackbuild(_builds[i]); }
      else if (_builds[i].getProp("action") == "Remove") { check = 
                                                remove_slackbuild(_builds[i]); }
      else { check = install_slackbuild(_builds[i]); }

      // Handle errors

      if (check == 127)
      {
        retval = check;
        std::cout << "Error: package manager not found. "
                  << "Press Enter to return...";
        std::getline(std::cin, response);
        break;
      }
      else if (check != 0)
      {
        retval = check;
        if (i != nbuilds-1)
        {
          std::cout << "An error occurred. Continue anyway [y/N]?";
          std::getline(std::cin, response);
          if ( (response != "y") && (response != "Y") ) { break; }
        }
        else
        {
          std::cout << " An error occurred. Press Enter to return...";
          std::getline(std::cin, response);
        }
      }
    }
  }

  return retval;
} 