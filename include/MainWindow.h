#pragma once

#include <curses.h>
#include <string>
#include "ListBox.h"

/*******************************************************************************

Main window

*******************************************************************************/
class MainWindow {

  private:

    WINDOW *_win1, *_win2;;
    ListBox _allcategories, _allbuilds;
    ListBox *_leftlist, *_rightlist;
    std::string _title, _filter, _info;

    void printToEol(const std::string & msg) const;

    void redrawHeaderFooter() const;
    void redrawWindows() const;
    void redrawAll() const;

  public:

    /* Constructor */

    MainWindow();

    /* First time window setup */

    void initialize();

    /* Set properties */

    void setTitle(const std::string & title);
    void setFilter(const std::string & filter);
    void setInfo(const std::string & info);

    /* Shows the main window */

    void show();
};
