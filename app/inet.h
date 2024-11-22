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
    Cria um socket
    Retorno:
        Descritor do socket
*/  
int createSocket();

/*
    Classe para implementar um cliente TFTP
*/
struct tftpclient {

    std::string ip;
    std::string port;
    std::string filename;
    sockaddr_in serverAddr;
    int sockfd;

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

        // cria o socket
        sockfd = createSocket();
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
    int sockfd;

    public:
    uploadCallback(sockaddr_in &serverAddr, int sockfd, const std::string& filename) : Callback(0, 0), serverAddr(serverAddr), filename(filename), sockfd(sockfd) {

        if(fileCheck(filename)){
            int fileSize = fileLenght(filename);

            // calcula o número total de blocos e arredonda para cima
            totalBlocks = ceil(fileSize / blocksize);
            std::cout << "File size: " << totalBlocks << " Blocks" << std::endl;
        }

        disable_timeout();
    }
    
    void handle(){ 
        char buffer[516];


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
    int sockfd;

    public:
    downloadCallback(sockaddr_in &serverAddr, int sockfd, const std::string& filename) : Callback(0, 0), serverAddr(serverAddr), filename(filename), sockfd(sockfd) {

        disable_timeout();

        this->fd = sockfd;
    }
    
    void handle(){ 

     char buffer[516];
     
          socklen_t addrLen = sizeof(serverAddr);
          ssize_t recvBytes = recvfrom(fd, buffer, sizeof(buffer), 0, (sockaddr*)&serverAddr, &addrLen);

          std::cout << "Recebendo " << recvBytes << " bytes" << std::endl;

          if (recvBytes < 0) {
              throw std::runtime_error("Erro ao receber a mensagem");
          }

          // imprime a mensagem recebida no terminal para visualização: 

            std::cout << "Mensagem recebida: " << std::string(buffer, recvBytes) << std::endl;
    }

    void handle_timeout(){
            std::cout << "Timeout" << std::endl;
    }
};


#endif