#ifndef INET_H
#define INET_H

#include <string>
#include <stdexcept>
#include <cstring>
#include <arpa/inet.h> 
#include <fstream>
#include <iostream>
#include <string>
#include <cmath>

#include "messaging.h"
#include "io.h"
#include "../libs/poller.h"


/*
    Converte uma string contendo um endereço IPv4 para uma estrutura sockaddr_in
    Parametros:
        ipAddress: string contendo o endereço IPv4
    Retorno:
        Estrutura sockaddr_in com o endereço IPv4
*/
sockaddr_in stringToIPv4(const std::string& ipAddress);

/*
    Converte uma string contendo uma porta para um inteiro
    Parametros:
        p: string contendo a porta
    Retorno:
        Número da porta
*/
int stringToPort(const std::string& port);

/*
    Classe para implementar um cliente TFTP
*/
struct tftpclient {

    std::string ip;
    std::string port;
    std::string filename;
    sockaddr_in serverAddr;

    /*
        Construtor
        Parametros:
            ip: endereço IP do servidor
            p: porta do servidor
    */
    tftpclient(const std::string& ip, const std::string& p,  const std::string& filename) : ip(ip), port(p), filename(filename) {

        // Converte o endereço IP para uma estrutura sockaddr_in
        serverAddr = stringToIPv4(ip);

        // Converte a porta para um inteiro e armazena na estrutura sockaddr_in
        int port = stringToPort(p);

        // utiliza a função htons para converter a porta para o formato de rede
        serverAddr.sin_port = htons(port);
    };

    /*
        Envia um arquivo para o servidor
        Parametros:
            filename: nome do arquivo a ser enviado
    */
    void upload();

    void download();
};

class uploadCallback : public Callback {
    sockaddr_in serverAddr;
    std::string filename;
    int blockNumber = 0;
    int totalBlocks;    
    int blocksize = 512;
    int fileSize;
    bool lastblock = false;

    public:
    uploadCallback(sockaddr_in &serverAddr, const std::string& filename) : Callback(0, 0), serverAddr(serverAddr), filename(filename) {

        if(fileCheck(filename)){
            int fileSize = fileLenght(filename);

            // calcula o número total de blocos e arredonda para cima
            totalBlocks = ceil(fileSize / blocksize);
            std::cout << "File size: " << totalBlocks << " Blocks" << std::endl;
        }

        disable_timeout();
    }
    
    void handle(){ 

            // cria a mensagem de WRQ e envia para o servidor
            requestMessage msg(OpcodeRM::WRITE, this->filename);

            std::cout << "Sending WRQ" << std::endl;

            std::cout << "msg: " << std::string(msg.serialize().begin(), msg.serialize().end()) << std::endl;

            // envia a mensagem para o servidor
            sendto(fd, msg.serialize().data(), msg.serialize().size(), 0, (sockaddr*)&serverAddr, sizeof(serverAddr));
    }

    void handle_timeout(){
            std::cout << "Timeout" << std::endl;
    }

    
};


class downloadCallback : public Callback {
    sockaddr_in serverAddr;
    std::string filename;
    int blockNumber = 0;
    int totalBlocks;    
    int blocksize = 512;
    int fileSize;
    bool lastblock = false;

    public:
    downloadCallback(sockaddr_in &serverAddr, const std::string& filename) : Callback(0, 0), serverAddr(serverAddr), filename(filename) {

        fileCheck(filename);
        int fileSize = fileLenght(filename);

        // calcula o número total de blocos e arredonda para cima
        totalBlocks = ceil(fileSize / blocksize);

        disable_timeout();

    }
    
    void handle(){ 

            // cria a mensagem de WRQ e envia para o servidor
            requestMessage msg(OpcodeRM::WRITE, this->filename);

            // envia a mensagem para o servidor
            sendto(fd, msg.serialize().data(), msg.serialize().size(), 0, (sockaddr*)&serverAddr, sizeof(serverAddr));
    }

    void handle_timeout(){
            std::cout << "Timeout" << std::endl;
    }

    
};


#endif