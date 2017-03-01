#include <iostream>

#include "mkfs.h"
#include "mkfs_net.h"

int main(int argc, char* argv[]) {

    if(argc == 4 && strcmp(argv[1],"-c") == 0)
    {
        mkfs_net net;
        int port = atoi(argv[3]);
        net.clientConnect(string(argv[2]),port);
        string line;
        while(getline(cin,line))
        {
            net.clientSendCmdLine(line);
            net.clientWaitRespond();
        }
    }
    else if(argc == 3 && strcmp(argv[1],"-s") == 0)
    {
        int port = atoi(argv[2]);
        mkfs *fs = new mkfs();
        fs->start_server(port);
        delete fs;
    }
    else
    {
        mkfs *fs = new mkfs();

        fs->launch_shell();

        delete(fs);
    }

    return 0;
}
