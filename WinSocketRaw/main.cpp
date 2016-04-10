#include <iostream>
#include "winsock2.h"

#pragma comment(lib, "ws2_32.lib")

using namespace std;

struct IP
{

};

struct TCP
{
    unsigned short source_port; //16
    unsigned short dest_port;
    unsigned sequence;   //32
    unsigned ack;
    unsigned char lenres;   //8
    unsigned char flag;
    unsigned short win;   //16
    unsigned short sum;
    unsigned short urp;
};




int main()
{
    int recivedlen;
    SOCKET getinfor;
    char BUF[65535];
    SOCKADDR_IN from;
    getinfor = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);

    while(true)
    {
        int fromlen = sizeof(from);
        recivedlen = recvfrom(getinfor, BUF, sizeof(BUF), 0, (SOCKADDR*)&from, &fromlen);

    }
}
