// WinSocketRaw.cpp : 定义控制台应用程序的入口点。
//

#include <iostream>
#include "winsock2.h"


#pragma comment(lib, "ws2_32.lib")

using namespace std;

struct IP //定义IP首部

{
	unsigned char h_lenver; //4位首部长度+4位IP版本号
	unsigned char tos; //8位服务类型TOS
	unsigned short total_len; //16位总长度（字节）
	unsigned short ident; //16位标识
	unsigned short frag_and_flags; //3位标志位
	unsigned char ttl; //8位生存时间 TTL
	unsigned char proto; //8位协议 (TCP, UDP 或其他)
	unsigned short checksum; //16位IP首部校验和
	unsigned int sourceIP; //32位源IP地址
	unsigned int destIP; //32位目的IP地址
};

struct TCP//定义TCP首部
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

struct UDP//定义UDP首部
{
	unsigned short sport;//16位源端口
	unsigned short dport;//16位目的端口
	unsigned short len;//UDP 长度
	unsigned short cksum;//检查和
};

/*不处理ICMP数据
struct ICMP//定义ICMP首部
{
	unsigned short sport;
	unsigned short dport;
	unsigned char type;
	unsigned char code;
	unsigned short cksum;
	unsigned short id;
	unsigned short seq;
	unsigned long timestamp;
};*/

int main()
{
	SOCKET sock;
	WSADATA wsd;
	char recvInf[65535] = { 0 }; //收到的信息
	DWORD dwBytesRet;

	int pCount = 0; //收到的包数量
	unsigned int optval = 1; //参数值
	char * ip = NULL; //对ip数据的指针
	char * tcp = NULL; //对tcp数据的指针
	char * udp = NULL; //对udp数据的指针
	int lentcp, lenudp;

	WSAStartup(MAKEWORD(2, 1), &wsd);

	if ((sock = socket(AF_INET, SOCK_RAW, IPPROTO_IP)) == SOCKET_ERROR)
	{
		cout << "启动原始嵌套字出错!" << endl;
		exit(-1);
	}


    //获取本机地址
	char  name[MAXBYTE];
    gethostname(name, MAXBYTE);
    struct hostent FAR * pHostent;
    pHostent = (struct hostent *)malloc(sizeof(struct hostent));
    pHostent = gethostbyname(name);

    //设置
    SOCKADDR_IN sa;
    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = INADDR_ANY;
    memcpy(&sa.sin_addr, pHostent->h_addr_list[0], pHostent->h_length); //设置本机地址*/

	if (SOCKET_ERROR == bind(sock, (SOCKADDR *)&sa, sizeof(sa)))
	{
		cout << "绑定出错！" << endl;
		exit(-2);
	}

	//将网卡设置为混杂模式
	if(SOCKET_ERROR == WSAIoctl(sock, (IOC_IN|IOC_VENDOR|1), &optval, sizeof(optval), nullptr, 0, &dwBytesRet, nullptr, nullptr))
    {
        cout << "混杂失败！" << endl;
    }

	IP * pip = NULL;//IP数据块指针
	TCP * ptcp = NULL;//TCP数据块指针
	UDP * pudp = NULL;//UDP数据块指针
	char szSourceIP[32];//源IP
	char szDestIP[32];//目的IP
	SOCKADDR_IN saSource;//源地址结构体
	SOCKADDR_IN saDest;//目的地址结构体
	int lenfrom = sizeof(saSource);
	pip = (IP*)recvInf;

	while (true)
	{
		recv(sock, recvInf, sizeof(recvInf), 0);//接收包
		//将32位IP地址转为数字 .形式

		//只处理TCP UDP信息包
		if (!(pip->proto == IPPROTO_TCP || pip->proto == IPPROTO_UDP))
			continue;

		pCount++;
		saSource.sin_addr.s_addr = pip->sourceIP;
		strncpy(szSourceIP, inet_ntoa(saSource.sin_addr), 32);
		saDest.sin_addr.s_addr = pip->destIP;
		strncpy(szDestIP, inet_ntoa(saDest.sin_addr), 32);

		lentcp = ntohs(pip->total_len) - (sizeof(IP) + sizeof(TCP));
		lenudp = ntohs(pip->total_len) - (sizeof(IP) + sizeof(UDP));

		system("cls");
		cout << "################第"<< pCount <<  " 号包#######################" << endl;
		cout << "******************  IP头部信息  *********************" << endl;
		cout << "源IP: " << szSourceIP << endl << "目的IP:" << szDestIP << endl;

		//处理TCP包
		if (pip->proto == IPPROTO_TCP && lentcp != 0)
		{
			ptcp = (TCP *)(recvInf + sizeof(IP));
			char * data = recvInf + sizeof(IP) + sizeof(TCP); //TCP数据指针
			cout << "**************  TCP头部信息  ********************" << endl;
			cout << "源端口号:" << ptcp->source_port << endl << "目的端口号：" << ptcp->dest_port << endl;
			cout << "数据：" << data << endl;
		}

		//处理UDP包
		if (pip->proto == IPPROTO_UDP && lenudp != 0)
		{
			pudp = (UDP *)(recvInf + sizeof(IP));
			char * data = recvInf + sizeof(IP) + sizeof(UDP); //UDP数据指针
			cout << "**************  UDP头部信息  ********************" << endl;
			cout << "源端口号:" << pudp->sport << endl << "目的端口号：" << pudp->dport << endl;
			cout << "数据：" << data << endl;
		}

		memset(recvInf, 0, sizeof(recvInf));//清空缓冲区
	}
}
