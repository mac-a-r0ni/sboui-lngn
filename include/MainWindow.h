#pragma once

#include <curses.h>
#include <vector>
#include <string>
#include "CursesWidget.h"
#include "AbstractListBox.h"
#include "CategoryListItem.h"
#include "CategoryListBox.h"
#include "BuildListItem.h"
#include "BuildListBox.h"
#include "FilterBox.h"
#include "SearchBox.h"
#include "InputBox.h"

/*******************************************************************************

Main window

*******************************************************************************/
class MainWindow {

  private:

    WINDOW *_win1, *_win2;
    CategoryListBox _clistbox;
    std::vector<BuildListBox> _blistboxes;
    std::vector<BuildListItem> _slackbuilds;
    std::vector<BuildListItem *> _installedlist, _nondeplist;
    std::vector<CategoryListItem> _categories;
    FilterBox _fbox;
    SearchBox _searchbox;
    std::string _title, _filter, _info, _status;
    unsigned int _category_idx, _activated_listbox, _layout;

    void printToEol(const std::string & msg) const;
    void printStatus(const std::string & msg);
    void clearStatus();
    void refreshStatus();

    void redrawHeaderFooter() const;
    void redrawWindowsHorz();
    void redrawWindowsVert();
    void redrawWindows(bool force=false);
    void redrawAll(bool force=false);
    void toggleLayout();

    /* Clears windows, lists, etc. */

    void clearData();

    /* Filters lists */

    void filterAll();
    void filterInstalled();
    void filterUpgradable();
    void filterTagged();
    void filterNonDeps();
    void filterSearch(const std::string & searchterm, bool case_sensitive=false,
                      bool whole_whord=false);

    /* Actions for a selected SlackBuild */

    bool modifyPackage(BuildListItem & build, const std::string & action);
    void showBuildOrder(BuildListItem & build);
    void showInverseReqs(BuildListItem & build);
    void browseFiles(const BuildListItem & build);

    /* Sync/update */

    int syncRepo();

    /* Sets size of popup boxes */

    void popupSize(int & height, int & width, CursesWidget *popup) const;
    void placePopup(CursesWidget *popup, WINDOW *win) const;

    /* Temporarily hides a window */

    void hideWindow(WINDOW *win) const;

  public:

    /* Constructor and destructor */

    MainWindow();
    ~MainWindow();

    /* Window setup */

    int initialize();
    int readLists();

    /* Set properties */

    void setTitle(const std::string & title);
    void setInfo(const std::string & info);

    /* Dialogs */

    void selectFilter();
    void search();
    void showBuildActions(BuildListItem & build);

    /* Shows the main window */

    void show();
};
