//
// Created by phil on 2/25/17.
//

#ifndef MKFS_MKFS_DIR_H
#define MKFS_MKFS_DIR_H

#include "utils.h"

#ifndef MKFS_MKFS_FILE_H
    #include "mkfs_file.h"
#endif

class mkfs_dir {
public:

    mkfs_dir(std::string dir_name_in, mkfs_dir *parent_dir_in);

    virtual ~mkfs_dir();

    /* add file to directory */
    void add_file(mkfs_file *file_in) { this->files.push_back(file_in); }

    /* add directory to directory */
    void add_dir(mkfs_dir *dir_in) { this->directories.push_back(dir_in); }

    /* get full path string */
    std::string get_full_path() { return this->full_path; }

    /* get full path string */
    std::string get_base_name() { return this->base_name; }

    /* get full path string */
    mkfs_dir *get_parent() { return this->parent; }

    /* set full path */
    void set_full_path(std::string path_in) { this->full_path = path_in; }

    /* get files in directory */
    std::vector<mkfs_file *> &getFiles() { return this->files; }

    /* get files in directory */
    std::vector<mkfs_dir *> &getDirs() { return this->directories; }

private:

    /* path to file in mkfs */
    std::string full_path;

    /* base name of directory */
    std::string base_name;

    /* list of files in this directory */
    std::vector<mkfs_file *> files;

    /* list of directories in this directory */
    std::vector<mkfs_dir *> directories;

    /* ptr to parent directory */
    mkfs_dir *parent=0;
};


#endif //MKFS_MKFS_DIR_H
