#pragma once

#include <string>
#include <vector>
#include "BuildListItem.h"

/* Backend settings */

extern std::string repo_dir;
extern std::string package_manager;
extern std::string sync_cmd, install_cmd, upgrade_cmd;
extern std::string editor;

/* Backend operations */

int read_repo(std::vector<BuildListItem> & slackbuilds);
std::vector<std::string> split(const std::string & instr, char delim=' ');
void list_installed(std::vector<BuildListItem> & slackbuilds,
                    std::vector<BuildListItem *> & installedlist);
void list_nondeps(const std::vector<BuildListItem *> & installedlist,
                        std::vector<BuildListItem *> & nondeplist);
void install_slackbuilds(std::vector<BuildListItem> & slackbuilds, 
                  const std::string & opts = "", const std::string & vars = "");
void upgrade_slackbuilds(std::vector<BuildListItem> & slackbuilds, 
                  const std::string & opts = "", const std::string & vars = "");
void remove_slackbuilds(std::vector<BuildListItem> & slackbuilds);
void view_readme(BuildListItem & build);
void compute_build_order(BuildListItem & build,
                         std::vector<BuildListItem *> & reqs,
                         std::vector<BuildListItem> & slackbuilds);
