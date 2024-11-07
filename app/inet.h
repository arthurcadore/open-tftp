#ifndef INET_H
#define INET_H

#include <string>
#include <stdexcept>
#include <cstring>
#include <arpa/inet.h> 

struct tftpclient {
    std::string ip;
    std::string port;
    std::sockaddr_in serverAddr;

    tftpclient(const std::string& ip, const std::string& p) : ip(ip), port(p) {
        serverAddr = stringToIPv4(ip);

        std::int port = stringToPort(p);

        address.sin_port = htons(port);
    }
}

std::sockaddr_in stringToIPv4(const std::string& ipAddress);

std::int stringToPort(const std::string& port);


#endif