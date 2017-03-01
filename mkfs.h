//
// Created by phil on 2/10/17.
//

#ifndef MKFS_MKFS_H
#define MKFS_MKFS_H

#include <map>
#include <string>
#include <deque>

#ifndef MKFS_MKFS_FILE_H
    #include "mkfs_file.h"
#endif

#ifndef MKFS_MKFS_DIR_H
    #include "mkfs_dir.h"
#endif

#include "mkfs_net.h"

class mkfs {
public:

    /* constructor(s) */
    mkfs();
    mkfs(long size_bytes);

    /* destructor */
    virtual ~mkfs();

    /* launch interactive shell */
    void launch_shell();

    void processCmd(string line);

    void start_server(int port);

    /* open file and return file descriptor */
    int mkfs_open(std::string file_name_in, mkfs_file_status status_in);

    /* open file and return file descriptor */
    int mkfs_close(int fd_in);

    /* write data to file */
    int mkfs_write(int fd_in, std::string data_in);

    /* read from file */
    std::string mkfs_read(int fd_in, int size_in);

    /* seek in file at fd */
    int mkfs_seek(int fd_in, int offset_in);

    /* make sub directory in current directory */
    int mkfs_mkdir(std::string dir_name_in);

    /* make sub directory in current directory */
    int mkfs_mkdir(std::vector<std::string> path_in);

    /* change to new directory */
    int mkfs_cd(std::vector<std::string> path_in);

    /* cat a file */
    int mkfs_cat(std::vector<std::string> path_in);

    /* cat a file */
    int mkfs_cat(std::string file_name_in);

    /* import into file system */
    int mkfs_import(std::string src, std::string dest);

    /* export file system */
    int mkfs_export(std::string src, std::string dest);

    /* show contents of current direcory */
    void mkfs_ls();

    /* show contents of current direcory */
    void mkfs_tree();

    /* remove dir from current directory */
    int mkfs_rmdir(std::string dir_name_in);

    /* get the next file descriptor id */
    int get_next_fd();

    /* return path to current directory */
    std::string get_cd_path() { return this->cd->get_full_path(); }

    /* see if file is in the file system */
    int find_file(std::string);

    /* see if file is in the file system */
    mkfs_file *find_file(std::string, int i=0);

private:

    /* initialize file system */
    void init();

    /* clean memory */
    void clean_up();

    /* size of memory chuck (default is 100MB) */
    long size_bytes = 0;

    /* keeps track of number of files */
    int high_fd = 1;

    /* keep track of retired fds */
    std::deque<int> retired_fds;

    /* map open file descriptors to file structures */
    std::map<int, mkfs_file *> fd_map;

    /* file ptr to actual file of the file system */
    FILE *file_system = 0;

    /* pointer to start of file system on the real file system */
    void *file_system_start = 0;

    /* current location starting from zero in the file system */
    long long offset_in_file_system = 0;

    /* current directory */
    mkfs_dir *cd = 0;

    /* root directory */
    mkfs_dir *root_dir;

    mkfs_net *net;

};


#endif //MKFS_MKFS_H
