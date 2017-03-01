//
// Created by phil on 2/10/17.
//

#include <sstream>
#include <iostream>

/* local headers */
#include "mkfs.h"

mkfs::mkfs(long size_bytes) : size_bytes(size_bytes) {
    this->init();
}

mkfs::mkfs() {

    /* default size if none is specified */
    this->size_bytes = 1000000;
}

mkfs::~mkfs() {
}

void mkfs::init() {

    /* allocate memory buffer and set offset to be 0 */
    this->file_system_start     = malloc((size_t)this->size_bytes);
    this->offset_in_file_system = 0;

    //fs.open("data", std::ifstream::)

    /* open file and write all zeroes to initialize */
    this->file_system = fopen("./data", "w");
    fwrite(this->file_system_start, (size_t)this->size_bytes, 1, this->file_system);

    /* create root directory and set current dir to root and make parent for root be null */
    this->root_dir = new mkfs_dir("/", NULL);
    this->root_dir->set_full_path("/");
    this->cd       = this->root_dir;
}

void mkfs::start_server(int port)
{
    net = new mkfs_net;
    net->serverStart(port);
    while(1)
    {
        string line = net->serverRead();
        net->serverGrabCout();
        processCmd(line);
        net->serverReleaseCoutAndRespond();
    }
}

void mkfs::launch_shell() {

    std::string line;
    std::cout << ">> ";

    while (std::getline(std::cin, line)) {
        processCmd(line);
    }
}

void mkfs::processCmd(string line){
    /* split parameters */
    std::vector<std::string> parse = split(line, ' ');

    /* check if we just hit enter */
    if(parse.empty()) {
        std::cout << ">> ";
        return;
    }

    /* parse arguments */
    if(parse[0].compare("exit") == 0) { /* EXIT */
        /* clean up memory */
        this->clean_up();
        return;
    }
    else if(parse[0].compare("mkfs") == 0) { /* MKFS */
        /* initialize file system */
        this->init();
    }
    else if(parse[0].compare("open") == 0) { /* OPEN */

        int fd = -1;

        /* check number of arguments */
        if (parse.size() != 3)
            std::cout << "Error: Expected file name and flag as parameters.\n";
        else if(parse[2].compare("r") != 0 && parse[2].compare("w") != 0)
            std::cout << "Error: Invalid flag.\n";
        else if(parse[2].compare("r") == 0)
            fd = this->mkfs_open(parse[1], OPEN_READ);
        else
            fd = this->mkfs_open(parse[1], OPEN_WRITE);

        /* check for success */
        if(fd != -1)
            std::cout << "SUCCESS, fd=" << fd << std::endl;
        else
            std::cout << "Error: failed to open file: " <<  parse[2] << std::endl;
    }
    else if(parse[0].compare("write") == 0) { /* WRITE */

        /* check number of arguments */
        if (parse.size() < 3)
            std::cout << "Error: Expected fd and string as parameters.\n";
        else if(this->fd_map.find(atoi(parse[1].c_str())) == this->fd_map.end()) {
            std::cout << "Error: fd=" << parse[1] << " is not open.\n";
        }
        else {

            /* construct input to be written */
            std::string input("");
            int i = 2;
            for(i = 2; i < parse.size() - 1; i++)
                input += parse[i] + " ";

            input += parse[i];

            /* write to file system */
            this->mkfs_write(atoi(parse[1].c_str()), input);
        }
    }
    else if(parse[0].compare("read") == 0) { /* READ */

        /* check number of arguments */
        if (parse.size() != 3)
            std::cout << "Error: Expected fd and offset as parameters.\n";
        else{
            std::string data = this->mkfs_read(atoi(parse[1].c_str()), atoi(parse[2].c_str()));
            std::cout << "OUTPUT: " << data << std::endl;
        }
    }
    else if(parse[0].compare("close") == 0) { /* CLOSE */

        /* check number of arguments */
        if (parse.size() != 2)
            std::cout << "Error: Expected file descriptor as a parameter.\n";
        else {
            int ret = this->mkfs_close(atoi(parse[1].c_str()));

            /* check for success */
            if (ret != -1)
                std::cout << "SUCCESS, closed fd=" << parse[1] << std::endl;
            else
                std::cout << "Error: fd=" << parse[2] << " does not exist.\n";
        }
    }
    else if (parse[0].compare("seek") == 0) { /* SEEK */
        /* check number of arguments */
        if (parse.size() != 3)
            std::cout << "Error: Expected fd and offset as parameters.\n";
        else if(this->fd_map.find(atoi(parse[1].c_str())) == this->fd_map.end()) {
            std::cout << "Error: fd=" << parse[1] << " is not open.\n";
        }
        else {
            this->mkfs_seek(atoi(parse[1].c_str()), atoi(parse[2].c_str()));
        }
    }
    else if(parse[0].compare("mkdir") == 0) { /* MKDIR */

        /* check number of arguments */
        if (parse.size() != 2)
            std::cout << "Error: Expected directory name as a parameter.\n";
        else {
            std::vector<std::string> path = split(parse[1], '/');
            if(this->mkfs_mkdir(path) != -1)
                std::cout << "SUCCESS: created director\n";
            else
                std::cout << "Error: failed to create directory\n";
        }
    }
    else if(parse[0].compare("rmdir") == 0) { /* RMDIR */

        /* check number of arguments */
        if (parse.size() != 2)
            std::cout << "Error: Expected dir name as a parameter.\n";
        else if(this->mkfs_rmdir(parse[1]) != -1)
            std::cout << "SUCCESS: removed directory\n";
        else
            std::cout << "Error: no such directory'" << parse[1] << std::endl;
    }
    else if(parse[0].compare("cd") == 0) { /* CD */

        /* check number of arguments */
        if (parse.size() != 2)
            std::cout << "Error: Expected directory name as a parameter.\n";
        else {
            std::vector<std::string> path = split(parse[1], '/');
            if(this->mkfs_cd(path) != -1)
                std::cout << "SUCCESS\n";
            else
                std::cout << "Error: not in current directory!\n";
        }
    }
    else if(parse[0].compare("ls") == 0) { /* LS */
        this->mkfs_ls();
    }
    else if(parse[0].compare("tree") == 0) { /* tree */
        this->mkfs_tree();
    }
    else if(parse[0].compare("cat") == 0) { /* CAT */

        /* check number of arguments */
        if (parse.size() != 2)
            std::cout << "Error: Expected file name as a parameter.\n";
        else if(this->mkfs_cat(parse[1]) == -1)
            std::cout << "Error: " << parse[1] << " not in current directory\n";
    }
    else if (parse[0].compare("import") == 0) { /* IMPORT */
        /* check number of arguments */
        if (parse.size() != 3)
            std::cout << "Error: Expected src and dest as parameters.\n";
        else
            this->mkfs_import(parse[1], parse[2]);
    }
    else if (parse[0].compare("export") == 0) { /* IMPORT */
        /* check number of arguments */
        if (parse.size() != 3)
            std::cout << "Error: Expected src and dest as parameters.\n";
        else
            this->mkfs_export(parse[1], parse[2]);
    }
    else
        std::cout << "Error: invalid command '" << parse[0] << "'\n";
    std::cout << ">> ";
}

int mkfs::mkfs_open(std::string file_name_in, mkfs_file_status status) {

    //int find = this->find_file(file_name_in);

    mkfs_file *find = this->find_file(file_name_in, 0);

    /* already in file system but closed */
    if(find != NULL) {

        find->setStatus(status);
        find->setFd(this->get_next_fd());

        /* insert into open file map */
        this->fd_map.insert(std::pair<int, mkfs_file *>(find->getFd(), find));

        return find->getFd();
    }
    else {

        /* get next available file descriptor that is the lowest */
        int fd = this->get_next_fd();

        /* create file object */
        mkfs_file *f = new mkfs_file(fd, file_name_in, this->offset_in_file_system, status);

        /* insert into open file map */
        this->fd_map.insert(std::pair<int, mkfs_file *>(fd, f));

        /* add file ptr to the current directory */
        this->cd->add_file(f);

        return fd;
    }
}


int mkfs::get_next_fd() {

    /* if we have no retired fds and high_fd == 0 then we know this is the first open file */
    /*if(this->retired_fds.empty() && this->high_fd == 0) {
        this->high_fd = 1;
        return this->high_fd;
    }
    else {
        int fd = this->retired_fds.front();
        this->retired_fds.pop_front();
        return fd;
    }*/
    return this->high_fd++;
}

void mkfs::clean_up() {

    /* close file descriptor */
    fclose(this->file_system);

    /* reset offset */
    this->offset_in_file_system = -1;

}

int mkfs::mkfs_close(int fd) {

    /* check if fd is in our structure */
    if(this->fd_map.find(fd) == this->fd_map.end()) {
        return -1;
    }
    else {

        /* close fd in file structure */
        this->fd_map.at(fd)->close();

        /* remove fd from fd map */
        this->fd_map.erase(fd);

        /* decrement high fd if we are removing this and it is highest */
        if(fd == this->high_fd && fd > 1)
            this->high_fd--;

        return 1;
    }
}

int mkfs::mkfs_write(int fd_in, std::string data_in) {

    /* get ptr to file structure */
    mkfs_file *current = this->fd_map[fd_in];

    if(current->getStatus() == OPEN_READ) {
        std::cout << "Error: file is open for read only. Cannot write.\n";
        return -1;
    }

    /* reopen file for writing */
    freopen("./data", "w", this->file_system);

    /* move to location in the file system */
    if(current->getSize() == 0)
        fseek(this->file_system, current->getOffset() , SEEK_SET);
    else
        fseek(this->file_system, current->getOffset() + current->getSize(), SEEK_SET);

    /* size of string */
    long long sz = (long long)data_in.size();

    /* set the size of file */
    current->setSize(current->getSize() + sz);

    //std::cout << "getSize(): " << current->getSize() << "\n";
    //std::cout << "getOffset(): " << current->getOffset() << "\n";
    //std::cout << "sz: " << sz << "\n";

    /* write data to file system */
    fwrite(&data_in[0], sizeof(char), sz, this->file_system);

    /* increment offset in big file */
    this->offset_in_file_system =  this->offset_in_file_system  + sz;

    /* set seek ptr in file structure */
    //current->setLoc(current->getLoc() + sz);

    return 0;
}

int mkfs::find_file(std::string fname) {

    for(int i = 0; i < this->cd->getFiles().size(); i++) {
        if(this->cd->getFiles()[i]->getFname().compare(fname) == 0) {
            return this->cd->getFiles()[i]->getFd();
        }
    }

    return -1;
}

mkfs_file *mkfs::find_file(std::string fname, int i) {

    for(int i = 0; i < this->cd->getFiles().size(); i++) {
        if(this->cd->getFiles()[i]->getFname().compare(fname) == 0) {
            return this->cd->getFiles()[i];
        }
    }

    return NULL;
}

std::string mkfs::mkfs_read(int fd_in, int size_in) {


    /* get ptr to file structure */
    mkfs_file *current = this->fd_map[fd_in];

    /* if file is empty return error string */
    if(current->getSize() == 0)
        return std::string("File is empty!\n");

    /* only read until end of file */
    if(current->getLoc() + size_in > current->getSize())
        size_in = current->getSize() - current->getLoc();

    /* string buffer to hold data */
    char buff[size_in];

    /* reopen file for reading */
    freopen("./data", "r", this->file_system);

    /* move to start of file in the file system */
    fseek(this->file_system, current->getOffset() + current->getLoc(), SEEK_SET);

    /* read data from file system into buffer */
    fread(buff, sizeof(char), size_in, this->file_system);

    /* set null char */
    //buff[size_in] = '\0';

    /* set seek ptr in file structure */
    current->setLoc(current->getLoc() + size_in);

    return std::string(buff);
}
int mkfs::mkfs_seek(int fd_in, int offset_in) {

    /* get ptr to file structure */
    mkfs_file *current = this->fd_map[fd_in];

    /* if file is empty return error string */
    if(current->getSize() == 0)
        return -1;

    /* only seek until end of file */
    if(offset_in > current->getSize()) {
        offset_in = current->getSize();
        std::cout << "Offset goes passed end of file so we just seek to the end\n";
    }

    /* move to start of file in the file system */
    fseek(this->file_system, current->getOffset() + offset_in, SEEK_SET);

    /* set seek ptr in file structure */
    current->setLoc(offset_in);

    return fd_in;
}

int mkfs::mkfs_mkdir(std::string dir_name_in) {

    /* set tmp dir structure ptr to be cd */
    mkfs_dir *tmp = this->cd;

    /* search for name in current directory and bail if it already exists */
    for(int i = 0; i < tmp->getDirs().size(); i++)
        if(tmp->getDirs()[i]->get_base_name().compare(dir_name_in) == 0)
            return -1;

    /* make new dir structure */
    mkfs_dir *new_dir = new mkfs_dir(dir_name_in, this->cd);

    /* add to cd children dirs */
    this->cd->add_dir(new_dir);

    return 0;
}

int mkfs::mkfs_cd(std::vector<std::string> path_in ) {


    /* search through directory structure starting at cd */
    for(int i = 0; i < path_in.size(); i++) {

        /* if cd has a parent then go to parent */
        if(path_in[i].compare("..") == 0 && this->cd->get_parent() != NULL) {
            this->cd = this->cd->get_parent();
            return 1;
        }
        else if(path_in[i].compare(".") != 0) {
            /* search through current directory for the base directory name */
            for(int j = 0; j < this->cd->getDirs().size(); j++)
                if(this->cd->getDirs()[j]->get_base_name().compare(path_in[i]) == 0) {
                    this->cd = this->cd->getDirs()[j];
                    return 1;
                }
        }
    }

    /* couldn't find directory */
    return -1;
}

void mkfs::mkfs_ls() {

    /* print directories */
    std::cout << "\nDirectories: \n";
    for(int i = 0; i < this->cd->getDirs().size(); i++)
        std::cout << this->cd->getDirs()[i]->get_base_name() << std::endl;

    /* print directories */
    std::cout << "\n\nFiles: \n";
    for(int i = 0; i < this->cd->getFiles().size(); i++)
        std::cout << this->cd->getFiles()[i]->getFname() << std::endl;
    std::cout << std::endl;
}

int mkfs::mkfs_cat(std::vector<std::string> path_in) {

    /* search through directory structure starting at cd */
    for(int i = 0; i < path_in.size(); i++) {

        std::cout << "i: " << path_in[i] << std::endl;
        /* if cd has a parent then go to parent */
        if(path_in[i].compare("..") == 0 && this->cd->get_parent() != NULL)
            this->cd = this->cd->get_parent();
        else if(path_in[i].compare(".") != 0) {
            /* search through current directory for the base directory name */
            for(int j = 0; j < this->cd->getFiles().size(); j++)
                if(this->cd->getFiles()[j]->getFname().compare(path_in[i]) == 0) {

                    return 1;
                }
        }
    }

    /* couldn't find file */
    return -1;
}

int mkfs::mkfs_cat(std::string file_name_in) {

    for(int i = 0; i < this->cd->getFiles().size(); i++) {
        if(this->cd->getFiles()[i]->getFname().compare(file_name_in) == 0) {
            int fd = this->mkfs_open(file_name_in, OPEN_READ);
            std::cout << this->mkfs_read(fd, this->cd->getFiles()[i]->getSize()) << std::endl;
            this->mkfs_close(fd);
            return 1;
        }
    }

    /* couldn't find file */
    return -1;
}

int mkfs::mkfs_import(std::string src, std::string dest) {

    /* open external file */
    FILE *src_f = fopen(src.c_str(), "r");

    /* get size */
    fseek (src_f, 0, SEEK_END);
    long long size=ftell (src_f);
    fseek (src_f, 0, SEEK_SET);

    /* read into buffer */
    char buff[size];
    fread(buff, sizeof(char), size, src_f);
    fclose(src_f);

    /* make new file structure */
    int fd = this->mkfs_open(dest, OPEN_WRITE);

    /* write it to file system */
    this->mkfs_write(fd, std::string(buff));

    /* close file */
    this->mkfs_close(fd);

    return 0;
}

int mkfs::mkfs_export(std::string src, std::string dest) {

    /* open local file and read in data */
    int fd = this->mkfs_open(src, OPEN_READ);
    std::string src_data = this->mkfs_read(fd, this->fd_map[fd]->getSize());
    this->mkfs_close(fd);

    /* open external file */
    FILE *dest_f = fopen(dest.c_str(), "w");

    /* write data to external loc */
    fwrite(src_data.c_str(), sizeof(char), src_data.size(), dest_f);
    fclose(dest_f);

    return 0;
}

void mkfs::mkfs_tree() {

    /* print directories */
    std::cout << "\nDirectories: \n";
    for(int i = 0; i < this->cd->getDirs().size(); i++)
        std::cout << this->cd->getDirs()[i]->get_base_name() << std::endl;

    /* print directories */
    std::cout << "\n\nFiles: \n";
    for(int i = 0; i < this->cd->getFiles().size(); i++)
        std::cout << this->cd->getFiles()[i]->getFname() << "\tsize: " << this->cd->getFiles()[i]->getSize() << " bytes\n";
    std::cout << std::endl;
}

int mkfs::mkfs_rmdir(std::string dir_name_in) {

    /* search for directory in current directory */
    for(int i = 0; i < this->cd->getDirs().size(); i++) {
        if(this->cd->getDirs()[i]->get_base_name().compare(dir_name_in) == 0) {
            this->cd->getDirs().erase(this->cd->getDirs().begin() + i);
            return 1;
        }
    }

    return -1;
}

int mkfs::mkfs_mkdir(std::vector<std::string> path_in) {

    /* search through directory structure starting at cd */
    int i = 0;
    for(i = 0; i < path_in.size() - 1; i++) {

        /* if cd has a parent then go to parent */
        if(path_in[i].compare("..") == 0 && this->cd->get_parent() != NULL) {
            this->cd = this->cd->get_parent();
            return 1;
        }
        else if(path_in[i].compare(".") != 0) {

            /* flag indicating if all directories exist */
            bool exist = false;

            /* search through current directory for the base directory name */
            for(int j = 0; j < this->cd->getDirs().size(); j++) {

                /* check if we find the directory */
                if (this->cd->getDirs()[j]->get_base_name().compare(path_in[i]) == 0) {
                    this->cd = this->cd->getDirs()[j];
                    exist = true;
                    break;
                }
            }
            /* incorrect path so bail */
            if(exist == false)
                return -1;
        }
    }

    this->mkfs_mkdir(path_in[i]);

    /* successfully created directory */
    return 1;
}