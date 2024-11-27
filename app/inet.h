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
    Retorna o endereço IP de um socket
    Parametros:
        sockfd: descritor do socket
    Retorno:
        Endereço IP do socket
*/
std::string getIP(sockaddr_in sockfd);

/*
    Classe para implementar um cliente TFTP
*/
struct tftpclient {

    std::string ip;
    std::string port;
    std::string filename;
    sockaddr_in serverAddr;
    int sockfd;
    long timeout;

    /*
        Construtor
        Parametros:
            ip: endereço IP do servidor
            p: porta do servidor
    */
    tftpclient(const std::string& ip, const std::string& p,  const std::string& filename, const long& timeout) : ip(ip), port(p), filename(filename), timeout(timeout){

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
    int blockNumber = 1;
    int totalBlocks;    
    int blocksize = 512;
    int fileSize;
    bool lastblock = false;
    int sockfd;
    bool error = false;
    int lastBlocksize;

    public:
    uploadCallback(sockaddr_in &serverAddr, int sockfd, const std::string& filename, long timeout) : Callback(sockfd, timeout), serverAddr(serverAddr), filename(filename), sockfd(sockfd) {

        this->fd = sockfd;

        try{
            // verifica se o arquivo existe e se é possível abri-lo
            if(fileCheck(filename)){

                // obtém o tamanho do arquivo
                int fileSize = fileLenght(filename);

                // calcula o número total de blocos e arredonda para cima
                totalBlocks = ceil(fileSize / blocksize) + 1;
                // std::cout << "File size: " << totalBlocks << " Blocks" << std::endl;

                // calcula o tamanho do último segmento em bytes
                lastBlocksize = fileSize % blocksize;
                // std::cout << "Last segment length: " << lastBlocksize << " Bytes" << std::endl;
            } 
        } catch(std::runtime_error e){

            // cria uma mensagem de erro
            std::cout << e.what() << std::endl;

            // finaliza a transferência
            finish();
        }
    }
    
    void handle(){ 
        char * bufferRX = new char[516];
         
        socklen_t addrLen = sizeof(serverAddr);
        ssize_t recvBytes = recvfrom(fd, bufferRX, sizeof(bufferRX), 0, (sockaddr*)&serverAddr, &addrLen);
    
        if (recvBytes < 0) {
            throw std::runtime_error("Erro ao receber a mensagem");
        }
    
        try{
            // converte o buffer para um ackMessage
            ackMessage msg = ackMessage::deserialize(bufferRX, recvBytes);
            // std::cout << "ACK Number: " << msg.printBN() << std::endl;
            blockNumber = msg.blockNumber + 1;

            if(blockNumber <= totalBlocks){

            // cria um buffer para armazenar o bloco
            std::vector<uint8_t> bufferTX(516);

            if (blockNumber == totalBlocks) {
                lastblock = true;
                 bufferTX = readBlock(this->filename, (blockNumber-1), blocksize, lastBlocksize);
                //  std::cout << "Last Block: " << block << std::endl;
            } else {
                 bufferTX = readBlock(this->filename, (blockNumber-1), blocksize, blocksize);
                //  std::cout << "Block: " << block << std::endl;
            }

            // cria um dataMessage
            dataMessage data(OpcodeDM::DATA, blockNumber, bufferTX);

            // envia o bloco para o servidor
            sendto(fd, data.serialize().data(), data.serialize().size(), 0, (sockaddr*)&serverAddr, sizeof(serverAddr));
            }else{
                std::cout << "Upload concluído" << std::endl;
                finish();
            }
        } catch(std::runtime_error e){
            std::cout << e.what() << std::endl;
            // cria uma mensagem de erro
            errorMessage msg = errorMessage::deserialize(bufferRX, recvBytes);
            std::cout << "Erro recebido do servidor: " << msg.printData() << std::endl;
            finish();
            error = true;
        }
    }

    void handle_timeout(){
            std::cout << "Timeout na sessão com o servidor: " << getIP(this->serverAddr) << std::endl;   
            finish();
    }

    
};


class downloadCallback : public Callback {
    sockaddr_in serverAddr;
    std::string filename;
    int blockNumber = 1;
    int totalBlocks;    
    int blocksize = 516;
    int fileSize;
    bool lastblock = false;
    int sockfd;
    bool error = false;

    public:
    downloadCallback(sockaddr_in &serverAddr, int sockfd, const std::string& filename, long timeout) : Callback(sockfd, timeout), serverAddr(serverAddr), filename(filename), sockfd(sockfd) {
        this->fd = sockfd;

        try{
            // verifica se um arquivo com o mesmo nome ja existe
            if(fileCheck(filename)){
                // deleta o arquivo
                deleteFile(filename);
            } 
        } catch(std::runtime_error e){
        }
    }
    
    void handle(){ 

    char buffer[516];
     
    socklen_t addrLen = sizeof(serverAddr);
    ssize_t recvBytes = recvfrom(fd, buffer, sizeof(buffer), 0, (sockaddr*)&serverAddr, &addrLen);

    if (recvBytes < 0) {
        throw std::runtime_error("Erro ao receber a mensagem");
    }

    if (recvBytes < blocksize) {
        lastblock = true;
    }

    try{
        // converte o buffer para um dataMessage   
        dataMessage msg = dataMessage::deserialize(buffer, recvBytes);

        blockNumber = msg.printBN();

        // escreve o bloco no arquivo
        writeBlock(this->filename, msg.printData());

    } catch (std::runtime_error e) {
        std::cout << e.what() << std::endl;
        // cria uma mensagem de erro
        errorMessage msg = errorMessage::deserialize(buffer, recvBytes);
        std::cout << "Erro recebido do servidor: " << msg.printData() << std::endl;
        finish();
        error = true;
    }

    // cria um ackMessage
    ackMessage ack(OpcodeAM::ACK, blockNumber);

    // envia o ack para o servidor
    sendto(fd, ack.serialize().data(), ack.serialize().size(), 0, (sockaddr*)&serverAddr, sizeof(serverAddr));

    // incrementa o número do bloco
    blockNumber++;

    if(lastblock && !error){
        std::cout << "Download concluído" << std::endl;
        finish();
    };

    }

    void handle_timeout(){
            std::cout << "Timeout na sessão com o servidor: " << getIP(this->serverAddr) << std::endl;  

            // deleta o arquivo em caso de timeout
            try{
                deleteFile(this->filename);
            } catch(std::runtime_error e){
                std::cout << e.what() << std::endl;
            }

            finish();
    }
};


#endif