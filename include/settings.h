#pragma once

#include <string>
#include "Color.h"

namespace settings
{
  extern std::string repo_dir;
  extern std::string package_manager;
  extern std::string sync_cmd, install_cmd, upgrade_cmd;
  extern std::string install_clos, install_vars;
  extern std::string upgrade_clos, upgrade_vars;
  extern std::string editor; 
  extern std::string color_theme_file;
  extern std::string layout;
  extern std::string env;
  extern bool resolve_deps, confirm_changes, enable_color;
}

namespace color 
{
  extern Color colors;
  extern std::string fg_normal, bg_normal;
  extern std::string fg_title, bg_title;
  extern std::string fg_info, bg_info;
  extern std::string fg_highlight_active, bg_highlight_active;
  extern std::string fg_highlight_inactive, bg_highlight_inactive;
  extern std::string header, header_popup;
  extern std::string tagged;
  extern std::string fg_popup, bg_popup;
  extern std::string fg_warning, bg_warning;
  extern std::string hotkey;
  extern std::string fg_combobox, bg_combobox;
}

void apply_color_settings();
int read_config();
int activate_color();
void deactivate_color();
