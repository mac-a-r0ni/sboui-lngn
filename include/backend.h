#pragma once

#include <string>
#include <vector>
#include "BuildListItem.h"

/* Backend settings */

extern std::string repo_dir;
extern std::string package_manager;
extern std::string sync_cmd, install_cmd, upgrade_cmd;

/* Backend operations */

int read_repo(std::vector<BuildListItem> & slackbuilds);
void install_slackbuilds(std::vector<BuildListItem> & slackbuilds, 
                  const std::string & opts = "", const std::string & vars = "");
void upgrade_slackbuilds(std::vector<BuildListItem> & slackbuilds, 
                  const std::string & opts = "", const std::string & vars = "");
void remove_slackbuilds(std::vector<BuildListItem> & slackbuilds);