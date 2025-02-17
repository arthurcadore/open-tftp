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
#include "../proto/msg.pb.h"


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

    void handle() { 
    char bufferRX[1024];
    socklen_t addrLen = sizeof(serverAddr);
    ssize_t recvBytes = recvfrom(fd, bufferRX, sizeof(bufferRX), 0, (sockaddr*)&serverAddr, &addrLen);

    if (recvBytes < 0) {
        throw std::runtime_error("Erro ao receber a mensagem");
    }

    try {
        // Desserializa a mensagem recebida
        tftp2::Mensagem msg;
        if (!msg.ParseFromArray(bufferRX, recvBytes)) {
            throw std::runtime_error("Falha ao desserializar a mensagem");
        }

        if (msg.has_ack()) {
            auto ack = msg.ack();
            blockNumber = ack.block_n() + 1;

            if (blockNumber <= totalBlocks) {
                // Lê o bloco do arquivo
                std::vector<uint8_t> bufferTX = readBlock(this->filename, blockNumber - 1, blocksize, lastblock ? lastBlocksize : blocksize);

                // Cria a mensagem DATA
                tftp2::Mensagem dataMsg;
                auto* data = dataMsg.mutable_data();
                data->set_message(bufferTX.data(), bufferTX.size());
                data->set_block_n(blockNumber);

                std::cout << "Enviando bloco " << blockNumber << " de " << totalBlocks << std::endl;

                std::cout << "Tamanho do bloco: " << bufferTX.size() << std::endl;  

                // Serializa e envia a mensagem
                std::string serializedData;
                dataMsg.SerializeToString(&serializedData);
                sendto(fd, serializedData.data(), serializedData.size(), 0, (sockaddr*)&serverAddr, sizeof(serverAddr));
            } else {
                std::cout << "Upload concluído" << std::endl;
                finish();
            }
        } else if (msg.has_error()) {
            auto error = msg.error();
            std::cout << "Erro recebido do servidor: " << error.errorcode() << std::endl;
            finish();
        }
    } catch (std::exception& e) {
        std::cerr << "Erro: " << e.what() << std::endl;
        finish();
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
    int blockNumber = 0;
    int blocksize = 512;
    int sockfd;
    bool error = false;
    enum State { WAITING_FOR_DATA, RECEIVING_DATA, COMPLETED, ERROR };
    State currentState;

public:
    downloadCallback(sockaddr_in &serverAddr, int sockfd, const std::string& filename, long timeout) 
        : Callback(sockfd, timeout), serverAddr(serverAddr), filename(filename), sockfd(sockfd), currentState(WAITING_FOR_DATA) {
        this->fd = sockfd;

        try {
            if(fileCheck(filename)){
                deleteFile(filename);
            } 
        } catch(std::runtime_error e) {
            // Tratar exceção, se necessário
        }
    }
    
    void handle() {
        char buffer[1024];
        socklen_t addrLen = sizeof(serverAddr);
        ssize_t recvBytes = recvfrom(fd, buffer, sizeof(buffer), 0, (sockaddr*)&serverAddr, &addrLen);

        std::cout << "bloco comprimento: " << recvBytes << std::endl;

        if (recvBytes < 0) {
            throw std::runtime_error("Erro ao receber a mensagem");
        }

        std::string bufferStr(buffer, recvBytes);

        try {
            tftp2::Mensagem msg;
            if (!msg.ParseFromString(bufferStr)) {
                throw std::runtime_error("Falha ao desserializar a mensagem");
            }

            switch (currentState) {
                case WAITING_FOR_DATA:
                    if (msg.has_data()) {
                        currentState = RECEIVING_DATA;
                        processData(msg);
                    } else if (msg.has_error()) {
                        currentState = ERROR;
                        processError(msg);
                    }
                    break;

                case RECEIVING_DATA:
                    if (msg.has_data()) {
                        processData(msg);
                        if (msg.data().message().size() < blocksize) {
                            currentState = COMPLETED;
                            std::cout << "Download concluído" << std::endl;
                            finish();
                        }
                    } else if (msg.has_error()) {
                        currentState = ERROR;
                        processError(msg);
                    }
                    break;

                case COMPLETED:
                case ERROR:
                    // Nada a fazer, a transferência já foi concluída ou terminou com erro
                    break;
            }
        } catch (std::exception& e) {
            std::cerr << "Erro: " << e.what() << std::endl;
            currentState = ERROR;
            finish();
        }
    }

    void processData(const tftp2::Mensagem& msg) {

        auto data = msg.data();
        blockNumber = data.block_n();
        
        std::cout << "Recebendo bloco " << blockNumber << std::endl;
        std::cout << "Estado atual: " << currentState << std::endl;
        
        writeBlock(this->filename, data.message());

        tftp2::Mensagem ackMsg;
        auto* ack = ackMsg.mutable_ack();
        ack->set_block_n(blockNumber);

        std::string serializedAck;
        ackMsg.SerializeToString(&serializedAck);
        sendto(fd, serializedAck.data(), serializedAck.size(), 0, (sockaddr*)&serverAddr, sizeof(serverAddr));
    }

    void processError(const tftp2::Mensagem& msg) {
        auto error = msg.error();
        std::cout << "Erro recebido do servidor: " << error.errorcode() << std::endl;
        finish();
    }

    void handle_timeout() {
        std::cout << "Timeout na sessão com o servidor: " << getIP(this->serverAddr) << std::endl;  
        currentState = ERROR;
        try {
            deleteFile(this->filename);
        } catch(std::runtime_error e) {
            std::cout << e.what() << std::endl;
        }
        finish();
    }
};


#endif