#pragma once

#include <vector>
#include "BuildListItem.h"

int compute_reqs_order(const BuildListItem & build,
                       std::vector<BuildListItem *> & reqlist,
                       std::vector<BuildListItem> & slackbuilds);
