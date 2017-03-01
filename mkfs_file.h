//
// Created by phil on 2/25/17.
//

#ifndef MKFS_MKFS_FILE_H
#define MKFS_MKFS_FILE_H

#include "utils.h"

enum mkfs_file_status {
    OPEN_WRITE,
    OPEN_READ,
    CLOSED
};

class mkfs_file {
public:

    mkfs_file(int fd_in, std::string fname_in, long long offset_in, mkfs_file_status status_in);

    virtual ~mkfs_file();

    mkfs_file_status getStatus() const;

    /* set the status */
    void setStatus(mkfs_file_status status);

    /* retrieve offset into the file */
    long long getOffset() { return this->offset; }

    /* set offset in file */
    void setLoc(long long loc_in) { this->loc = loc_in; }

    /* set offset in file */
    void setSize(long long size_in) { this->size= size_in; }

    /* retrieve offset into the file */
    long long getLoc() { return this->loc; }

    /* get file name */
    std::string getFname() { return this->fname; }

    /* get fd */
    void setFd(int fd_in) { this->fd = fd_in; }

    /* get fd */
    int getFd() { return this->fd; }

    /* get size */
    long long getSize() { return this->size; }

    /* reset fd and seek location in the file */
    void close() { this->fd = -1; this->loc = 0; this->setStatus(mkfs_file_status::CLOSED); }

private:

    /* file name */
    std::string fname;

    /* fd for when it is open */
    int fd=-1;

    /* file descriptor */

    /* size of file in bytes */
    long long size=0;

    /* current location in file */
    long long loc=0;

    /* offset to start of file in the file system */
    long long offset=0;

    /* hold status of file */
    mkfs_file_status status;

};


#endif //MKFS_MKFS_FILE_H
