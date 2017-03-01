//
// Created by phil on 2/25/17.
//

#include "mkfs_dir.h"

mkfs_dir::mkfs_dir(std::string dir_name_in, mkfs_dir *parent_dir_in) : base_name(dir_name_in),
                                                                       parent(parent_dir_in) {}

mkfs_dir::~mkfs_dir() {

}

