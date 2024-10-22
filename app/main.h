#ifndef MAIN_H
#define MAIN_H

#include <fstream>
#include <iostream>
#include <list>
#include <string>
#include <vector>

#include <sys/socket.h> // Para socket
#include <arpa/inet.h>  // Para inet_pton
#include <netinet/in.h> // Para sockaddr_in
#include <cstring>      // Para memset
#include <unistd.h>     // Para close
#include <stdexcept>    // Para std::runtime_error

// definindo as constantes de opcode
#define RRQ 1
#define WRQ 2

#include "requisition.h"
#include "io.h"

using namespace std;

sockaddr_in stringToIPv4(const std::string& ipAddress);

void download(sockaddr_in ip, int porta, string arquivo);

void upload(sockaddr_in ip, int porta, string arquivo);

string errorCheck(char* msg);

int main(int argc, char* argv[]);


#endif
