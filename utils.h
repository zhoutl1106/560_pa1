//
// Created by phil on 2/25/17.
//

#ifndef MKFS_UTILS_H
#define MKFS_UTILS_H

#endif //MKFS_UTILS_H

#include <string>
#include <sstream>
#include <vector>


/* template to split by a delimiter */
template<typename Out>
void split(const std::string &s, char delim, Out result);

/* function called to return vector of components */
std::vector<std::string> split(const std::string &s, char delim);