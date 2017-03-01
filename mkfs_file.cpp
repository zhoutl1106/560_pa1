//
// Created by phil on 2/25/17.
//

#include "mkfs_file.h"

mkfs_file::~mkfs_file() {

}

mkfs_file::mkfs_file( int fd_in, std::string fname_in, long long offset_in, mkfs_file_status status) :fd(fd_in),
                                                                                                      fname(fname_in),
                                                                                                      loc(0),
                                                                                                      offset(offset_in),
                                                                                                      status(status),
                                                                                                      size(0) {}

mkfs_file_status mkfs_file::getStatus() const {
    return status;
}

void mkfs_file::setStatus(mkfs_file_status status) {
    mkfs_file::status = status;
}
