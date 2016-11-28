#include <string>
#include "ListItem.h"
#include "BuildListItem.h"

/*******************************************************************************

Constructor

*******************************************************************************/
BuildListItem::BuildListItem() 
{ 
  _name = ""; 
  addProp("category", "");
  addProp("installed_version", "");
  addProp("available_version", "");
  addBoolProp("tagged", false);
  addBoolProp("installed", false);
}
