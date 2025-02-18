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

/**
 * @brief Converte uma string contendo um endereço IPv4 para uma estrutura sockaddr_in.
 * 
 * @param ipAddress String contendo o endereço IPv4 (ex.: "192.168.0.1").
 * @return sockaddr_in Estrutura contendo o endereço convertido.
 */
sockaddr_in stringToIPv4(const std::string& ipAddress);

/**
 * @brief Converte uma string contendo uma porta para um inteiro.
 * 
 * @param port String contendo o número da porta.
 * @return int Número da porta convertido.
 */
int stringToPort(const std::string& port);

/**
 * @brief Cria um socket para comunicação.
 * 
 * @return int Descritor do socket criado.
 */
int createSocket();

/**
 * @brief Retorna o endereço IP de uma estrutura sockaddr_in.
 * 
 * @param sockfd Estrutura sockaddr_in contendo os dados do socket.
 * @return std::string Endereço IP extraído.
 */
std::string getIP(sockaddr_in sockfd);

/**
 * @brief Estrutura que implementa um cliente TFTP.
 * 
 * Essa estrutura gerencia a comunicação com o servidor TFTP,
 * armazenando o endereço, porta, nome do arquivo, socket e timeout.
 */
struct tftpclient {
    std::string ip;          // Endereço IP do servidor
    std::string port;        // Porta do servidor (em formato string)
    std::string filename;    // Nome do arquivo para transferência
    sockaddr_in serverAddr;  // Estrutura com o endereço do servidor
    int sockfd;              // Descritor do socket utilizado na comunicação
    long timeout;            // Tempo limite para operações

    /**
     * @brief Construtor do tftpclient.
     * 
     * Inicializa o cliente TFTP realizando:
     * - Conversão do IP para sockaddr_in.
     * - Conversão da porta para inteiro e configuração do campo sin_port.
     * - Criação do socket para a comunicação.
     * 
     * @param ip Endereço IP do servidor.
     * @param p Porta do servidor (em formato string).
     * @param filename Nome do arquivo a ser transferido.
     * @param timeout Tempo limite para as operações.
     */
    tftpclient(const std::string& ip, const std::string& p,  const std::string& filename, const long& timeout) 
        : ip(ip), port(p), filename(filename), timeout(timeout) {

        // Converte o endereço IP para a estrutura sockaddr_in
        serverAddr = stringToIPv4(ip);

        // Converte a porta para inteiro
        int port = stringToPort(p);

        // Converte a porta para o formato de rede e armazena na estrutura
        serverAddr.sin_port = htons(port);

        // Cria o socket para comunicação com o servidor
        sockfd = createSocket();
    };

    // Métodos para operações TFTP (implementação provavelmente em outro arquivo .cpp)
    void upload();
    void download();
    void move(const std::string& newname);
    void list();
    void mkdir();
    void remove();
};

/**
 * @brief Classe para gerenciar o envio (upload) de arquivos via TFTP.
 * 
 * Herda de Callback e implementa a lógica para:
 * - Enviar blocos de dados do arquivo.
 * - Aguardar e processar ACKs.
 * - Tratar erros e timeouts.
 */
class uploadCallback : public Callback {
    sockaddr_in serverAddr;    // Endereço do servidor
    std::string filename;      // Nome do arquivo a ser enviado
    int blockNumber = 1;       // Número do bloco atual (inicia em 1)
    int totalBlocks;           // Número total de blocos a serem enviados
    int blocksize = 512;       // Tamanho padrão de cada bloco (em bytes)
    int fileSize;              // Tamanho total do arquivo
    bool lastblock = false;    // Flag para identificar o último bloco
    int sockfd;                // Descritor do socket
    bool error = false;        // Flag para indicar ocorrência de erro
    int lastBlocksize;         // Tamanho do último bloco (caso seja menor que o padrão)
    
    // Estados possíveis durante o upload
    enum State { SENDING_DATA, WAITING_FOR_ACK, COMPLETED, ERROR };
    State currentState;

public:
    /**
     * @brief Construtor do uploadCallback.
     * 
     * Inicializa o callback para upload, configurando os parâmetros de
     * transferência e verificando o arquivo (tamanho, existência, etc.).
     * 
     * @param serverAddr Endereço do servidor.
     * @param sockfd Descritor do socket.
     * @param filename Nome do arquivo a ser enviado.
     * @param timeout Tempo limite para a operação.
     */
    uploadCallback(sockaddr_in &serverAddr, int sockfd, const std::string& filename, long timeout) 
        : Callback(sockfd, timeout), serverAddr(serverAddr), filename(filename), sockfd(sockfd), currentState(SENDING_DATA) {
        this->fd = sockfd;

        try {
            // Verifica se o arquivo existe e pode ser lido
            if(fileCheck(filename)){
                fileSize = fileLenght(filename); // Obtém o tamanho do arquivo
                totalBlocks = ceil(fileSize / blocksize) + 1; // Calcula o número total de blocos
                lastBlocksize = fileSize % blocksize; // Tamanho do último bloco, se necessário

                // Imprime informações para depuração
                std::cout << "Tamanho do arquivo: " << fileSize << std::endl;
                std::cout << "Número de blocos: " << totalBlocks << std::endl;
                std::cout << "Tamanho do último bloco: " << lastBlocksize << std::endl;
                std::cout << "block number: " << blockNumber << std::endl;
            } 
        } catch(std::runtime_error e) {
            std::cout << e.what() << std::endl;
            finish(); // Finaliza o callback em caso de erro
        }
    }

    /**
     * @brief Processa as mensagens recebidas do servidor.
     * 
     * Dependendo do estado atual, o método:
     * - Envia o próximo bloco de dados.
     * - Aguarda a confirmação (ACK).
     * - Trata mensagens de erro.
     */
    void handle() {
        char bufferRX[1024]; // Buffer para armazenar dados recebidos
        socklen_t addrLen = sizeof(serverAddr);
        ssize_t recvBytes = recvfrom(fd, bufferRX, sizeof(bufferRX), 0, (sockaddr*)&serverAddr, &addrLen);

        std::cout << "bloco recebido: " << recvBytes << std::endl;

        if (recvBytes < 0) {
            throw std::runtime_error("Erro ao receber a mensagem");
        }

        try {
            tftp2::Mensagem msg;
            // Desserializa a mensagem recebida usando Protocol Buffers
            if (!msg.ParseFromArray(bufferRX, recvBytes)) {
                throw std::runtime_error("Falha ao desserializar a mensagem");
            }

            // Processa a mensagem de acordo com o estado atual
            switch (currentState) {
                case SENDING_DATA:
                    std::cout << "Enviando DATA" << std::endl;
                    sendData(); // Envia o próximo bloco de dados
                    currentState = WAITING_FOR_ACK;
                    break;

                case WAITING_FOR_ACK:
                    std::cout << "Esperando ACK" << std::endl;
                    if (msg.has_ack()) {
                        auto ack = msg.ack();
                        if (ack.block_n() == blockNumber) {
                            std::cout << "ACK number: " << blockNumber << " confirmado" << std::endl;
                            blockNumber++;

                            if (blockNumber <= totalBlocks) {
                                sendData(); // Continua enviando blocos se necessário
                                break;
                            } else {
                                currentState = COMPLETED;
                                std::cout << "Upload concluído" << std::endl;
                                finish(); // Finaliza a transferência
                            }
                        }
                    } else if (msg.has_error()) {
                        currentState = ERROR;
                        processError(msg); // Trata erros enviados pelo servidor
                        break;
                    }
                case COMPLETED:
                case ERROR:
                    // Se a transferência estiver concluída ou ocorrer um erro, não há ações adicionais
                    break;
            }
        } catch (std::exception& e) {
            std::cerr << "Erro: " << e.what() << std::endl;
            currentState = ERROR;
            finish();
        }
    }

    /**
     * @brief Envia um bloco de dados do arquivo para o servidor.
     * 
     * Prepara o bloco de dados (realizando leitura do arquivo) e envia-o
     * após a serialização via Protocol Buffers.
     */
    void sendData() {
        // Verifica se o bloco atual é o último
        if (blockNumber == totalBlocks) {
            lastblock = true;
        }

        // Lê o bloco de dados do arquivo
        std::vector<uint8_t> bufferTX = readBlock(this->filename, blockNumber - 1, blocksize, lastblock ? lastBlocksize : blocksize);

        // Prepara a mensagem de dados
        tftp2::Mensagem dataMsg;
        auto* data = dataMsg.mutable_data();
        data->set_message(bufferTX.data(), bufferTX.size());
        data->set_block_n(blockNumber);

        // Serializa a mensagem para uma string
        std::string serializedData;
        dataMsg.SerializeToString(&serializedData);
        // Envia o bloco para o servidor
        sendto(fd, serializedData.data(), serializedData.size(), 0, (sockaddr*)&serverAddr, sizeof(serverAddr));

        std::cout << "Enviando bloco " << blockNumber << " de " << totalBlocks << std::endl;
    }

    /**
     * @brief Processa mensagens de erro recebidas do servidor.
     * 
     * @param msg Mensagem de erro recebida.
     */
    void processError(const tftp2::Mensagem& msg) {
        auto error = msg.error();
        std::cout << "Erro recebido do servidor: " << error.errorcode() << std::endl;
        finish(); // Finaliza a operação em caso de erro
    }

    /**
     * @brief Trata a ocorrência de timeout durante a comunicação.
     */
    void handle_timeout() {
        std::cout << "Timeout na sessão com o servidor: " << getIP(this->serverAddr) << std::endl;   
        currentState = ERROR;
        finish(); // Finaliza o callback em caso de timeout
    }
};

/**
 * @brief Classe para gerenciar o download de arquivos via TFTP.
 * 
 * Gerencia a recepção de dados, gravação dos blocos no arquivo e envio de ACKs.
 */
class downloadCallback : public Callback {
    sockaddr_in serverAddr;    // Endereço do servidor
    std::string filename;      // Nome do arquivo a ser salvo
    int blockNumber = 0;       // Número do bloco atual recebido
    int blocksize = 512;       // Tamanho do bloco padrão
    int sockfd;                // Descritor do socket
    bool error = false;        // Flag de erro

    // Estados possíveis para o download
    enum State { WAITING_FOR_DATA, RECEIVING_DATA, COMPLETED, ERROR };
    State currentState;

public:
    /**
     * @brief Construtor do downloadCallback.
     * 
     * Prepara a operação de download e remove qualquer arquivo existente com o mesmo nome.
     * 
     * @param serverAddr Endereço do servidor.
     * @param sockfd Descritor do socket.
     * @param filename Nome do arquivo a ser baixado.
     * @param timeout Tempo limite para a operação.
     */
    downloadCallback(sockaddr_in &serverAddr, int sockfd, const std::string& filename, long timeout) 
        : Callback(sockfd, timeout), serverAddr(serverAddr), filename(filename), sockfd(sockfd), currentState(WAITING_FOR_DATA) {
        this->fd = sockfd;

        try {
            // Se o arquivo já existe, remove-o para garantir a integridade do download
            if(fileCheck(filename)){
                deleteFile(filename);
            } 
        } catch(std::runtime_error e) {
            // Tratar exceção, se necessário
        }
    }
    
    /**
     * @brief Processa as mensagens recebidas durante o download.
     * 
     * Lê os dados do servidor e, conforme o estado, processa ou finaliza o download.
     */
    void handle() {
        char buffer[1024]; // Buffer para armazenar os dados recebidos
        socklen_t addrLen = sizeof(serverAddr);
        ssize_t recvBytes = recvfrom(fd, buffer, sizeof(buffer), 0, (sockaddr*)&serverAddr, &addrLen);

        std::cout << "bloco comprimento: " << recvBytes << std::endl;

        if (recvBytes < 0) {
            throw std::runtime_error("Erro ao receber a mensagem");
        }

        // Converte o buffer para uma string para desserialização
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
                        // Se o tamanho do bloco recebido for menor que o padrão, é o último bloco
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
                    // Nenhuma ação se a transferência já foi concluída ou ocorreu erro
                    break;
            }
        } catch (std::exception& e) {
            std::cerr << "Erro: " << e.what() << std::endl;
            currentState = ERROR;
            finish();
        }
    }

    /**
     * @brief Processa os dados recebidos e envia um ACK para o servidor.
     * 
     * @param msg Mensagem contendo os dados do bloco.
     */
    void processData(const tftp2::Mensagem& msg) {
        auto data = msg.data();
        blockNumber = data.block_n();
        
        std::cout << "Recebendo bloco " << blockNumber << std::endl;
        std::cout << "Estado atual: " << currentState << std::endl;
        
        // Grava o bloco de dados no arquivo
        writeBlock(this->filename, data.message());

        // Prepara e envia o ACK confirmando o recebimento do bloco
        tftp2::Mensagem ackMsg;
        auto* ack = ackMsg.mutable_ack();
        ack->set_block_n(blockNumber);

        std::string serializedAck;
        ackMsg.SerializeToString(&serializedAck);
        sendto(fd, serializedAck.data(), serializedAck.size(), 0, (sockaddr*)&serverAddr, sizeof(serverAddr));
    }

    /**
     * @brief Processa mensagens de erro durante o download.
     * 
     * @param msg Mensagem de erro recebida.
     */
    void processError(const tftp2::Mensagem& msg) {
        auto error = msg.error();
        std::cout << "Erro recebido do servidor: " << error.errorcode() << std::endl;
        finish();
    }

    /**
     * @brief Trata o timeout na sessão de download.
     */
    void handle_timeout() {
        std::cout << "Timeout na sessão com o servidor: " << getIP(this->serverAddr) << std::endl;  
        currentState = ERROR;
        try {
            deleteFile(this->filename); // Remove o arquivo parcialmente baixado
        } catch(std::runtime_error e) {
            std::cout << e.what() << std::endl;
        }
        finish();
    }
};

/**
 * @brief Classe para gerenciar a renomeação/movimentação de arquivos no servidor.
 */
class moveCallback : public Callback {
    sockaddr_in serverAddr;    // Endereço do servidor
    std::string oldName;       // Nome original do arquivo
    std::string newName;       // Novo nome para o arquivo
    int sockfd;                // Descritor do socket
    bool error = false;
    
    // Estados possíveis para a operação de renomeação
    enum State { SENDING_MOVE, WAITING_FOR_ACK, COMPLETED, ERROR };
    State currentState;

public:
    /**
     * @brief Construtor do moveCallback.
     * 
     * @param serverAddr Endereço do servidor.
     * @param sockfd Descritor do socket.
     * @param oldName Nome original do arquivo.
     * @param newName Novo nome a ser aplicado.
     * @param timeout Tempo limite para a operação.
     */
    moveCallback(sockaddr_in &serverAddr, int sockfd, const std::string& oldName, const std::string& newName, long timeout) 
        : Callback(sockfd, timeout), serverAddr(serverAddr), oldName(oldName), newName(newName), sockfd(sockfd), currentState(WAITING_FOR_ACK) {
        this->fd = sockfd;
        std::cout << "moveCallback: " << oldName << " -> " << newName << std::endl;
    }

    /**
     * @brief Processa as respostas do servidor para a operação de renomeação.
     */
    void handle() {
        char bufferRX[1024]; // Buffer para armazenar os dados recebidos
        socklen_t addrLen = sizeof(serverAddr);
        ssize_t recvBytes = recvfrom(fd, bufferRX, sizeof(bufferRX), 0, (sockaddr*)&serverAddr, &addrLen);

        if (recvBytes < 0) {
            throw std::runtime_error("Erro ao receber a mensagem");
        }

        try {
            tftp2::Mensagem msg;
            if (!msg.ParseFromArray(bufferRX, recvBytes)) {
                throw std::runtime_error("Falha ao desserializar a mensagem");
            }

            switch (currentState) {
                case WAITING_FOR_ACK:
                    std::cout << "Esperando ACK" << std::endl;
                    if (msg.has_ack()) {
                        auto ack = msg.ack();
                        if (ack.block_n() == 0) {
                            std::cout << "ACK recebido. Renomeação concluída." << std::endl;
                            currentState = COMPLETED;
                            finish();
                        }
                    } else if (msg.has_error()) {
                        currentState = ERROR;
                        processError(msg);
                    }
                    break;

                case COMPLETED:
                case ERROR:
                    // Nenhuma ação necessária se a operação já estiver concluída ou com erro
                    break;
            }
        } catch (std::exception& e) {
            std::cerr << "Erro: " << e.what() << std::endl;
            currentState = ERROR;
            finish();
        }
    }

    /**
     * @brief Processa mensagens de erro para a operação de renomeação.
     * 
     * @param msg Mensagem de erro recebida.
     */
    void processError(const tftp2::Mensagem& msg) {
        auto error = msg.error();
        std::cout << "Erro recebido do servidor: " << error.errorcode() << std::endl;
        finish();
    }

    /**
     * @brief Trata o timeout da operação de renomeação.
     */
    void handle_timeout() {
        std::cout << "Timeout na sessão com o servidor: " << getIP(this->serverAddr) << std::endl;   
        currentState = ERROR;
        finish();
    }
};

/**
 * @brief Classe para gerenciar a remoção de arquivos no servidor.
 */
class removeCallback : public Callback {
    sockaddr_in serverAddr;    // Endereço do servidor
    std::string oldName;       // Nome do arquivo a ser removido
    int sockfd;                // Descritor do socket
    bool error = false;
    
    // Estados possíveis para a remoção do arquivo
    enum State { SENDING_MOVE, WAITING_FOR_ACK, COMPLETED, ERROR };
    State currentState;

public:
    /**
     * @brief Construtor do removeCallback.
     * 
     * @param serverAddr Endereço do servidor.
     * @param sockfd Descritor do socket.
     * @param oldName Nome do arquivo a ser removido.
     * @param timeout Tempo limite para a operação.
     */
    removeCallback(sockaddr_in &serverAddr, int sockfd, const std::string& oldName, long timeout) 
        : Callback(sockfd, timeout), serverAddr(serverAddr), oldName(oldName), sockfd(sockfd), currentState(WAITING_FOR_ACK) {
        this->fd = sockfd;
        std::cout << "moveCallback: " << oldName << std::endl;
    }

    /**
     * @brief Processa a resposta do servidor para a operação de remoção.
     */
    void handle() {
        char bufferRX[1024];
        socklen_t addrLen = sizeof(serverAddr);
        ssize_t recvBytes = recvfrom(fd, bufferRX, sizeof(bufferRX), 0, (sockaddr*)&serverAddr, &addrLen);

        if (recvBytes < 0) {
            throw std::runtime_error("Erro ao receber a mensagem");
        }

        try {
            tftp2::Mensagem msg;
            if (!msg.ParseFromArray(bufferRX, recvBytes)) {
                throw std::runtime_error("Falha ao desserializar a mensagem");
            }

            switch (currentState) {
                case WAITING_FOR_ACK:
                    std::cout << "Esperando ACK" << std::endl;
                    if (msg.has_ack()) {
                        auto ack = msg.ack();
                        if (ack.block_n() == 0) {
                            std::cout << "ACK recebido. Remoção concluída." << std::endl;
                            currentState = COMPLETED;
                            finish();
                        }
                    } else if (msg.has_error()) {
                        currentState = ERROR;
                        processError(msg);
                    }
                    break;

                case COMPLETED:
                case ERROR:
                    break;
            }
        } catch (std::exception& e) {
            std::cerr << "Erro: " << e.what() << std::endl;
            currentState = ERROR;
            finish();
        }
    }

    /**
     * @brief Processa mensagens de erro para a operação de remoção.
     * 
     * @param msg Mensagem de erro recebida.
     */
    void processError(const tftp2::Mensagem& msg) {
        auto error = msg.error();
        std::cout << "Erro recebido do servidor: " << error.errorcode() << std::endl;
        finish();
    }

    /**
     * @brief Trata o timeout da operação de remoção.
     */
    void handle_timeout() {
        std::cout << "Timeout na sessão com o servidor: " << getIP(this->serverAddr) << std::endl;   
        currentState = ERROR;
        finish();
    }
};

/**
 * @brief Classe para gerenciar a criação de diretórios no servidor.
 */
class mkdirCallback : public Callback {
    sockaddr_in serverAddr;    // Endereço do servidor
    std::string dirname;       // Nome do diretório a ser criado
    int sockfd;                // Descritor do socket
    bool error = false;
    
    // Estados possíveis para a criação de diretórios
    enum State { SENDING_MKDIR, WAITING_FOR_ACK, COMPLETED, ERROR };
    State currentState;

public:
    /**
     * @brief Construtor do mkdirCallback.
     * 
     * @param serverAddr Endereço do servidor.
     * @param sockfd Descritor do socket.
     * @param dirname Nome do diretório a ser criado.
     * @param timeout Tempo limite para a operação.
     */
    mkdirCallback(sockaddr_in &serverAddr, int sockfd, const std::string& dirname, long timeout) 
        : Callback(sockfd, timeout), serverAddr(serverAddr), dirname(dirname), sockfd(sockfd), currentState(WAITING_FOR_ACK) {
        this->fd = sockfd;
    }

    /**
     * @brief Processa a resposta do servidor para a criação do diretório.
     */
    void handle() {
        char bufferRX[1024];
        socklen_t addrLen = sizeof(serverAddr);
        ssize_t recvBytes = recvfrom(fd, bufferRX, sizeof(bufferRX), 0, (sockaddr*)&serverAddr, &addrLen);

        if (recvBytes < 0) {
            throw std::runtime_error("Erro ao receber a mensagem");
        }

        try {
            tftp2::Mensagem msg;
            if (!msg.ParseFromArray(bufferRX, recvBytes)) {
                throw std::runtime_error("Falha ao desserializar a mensagem");
            }

            switch (currentState) {
                case WAITING_FOR_ACK:
                    std::cout << "Esperando ACK" << std::endl;
                    if (msg.has_ack()) {
                        auto ack = msg.ack();
                        if (ack.block_n() == 0) {
                            std::cout << "ACK recebido. Diretório criado." << std::endl;
                            currentState = COMPLETED;
                            finish();
                        }
                    } else if (msg.has_error()) {
                        currentState = ERROR;
                        processError(msg);
                    }
                    break;

                case COMPLETED:
                case ERROR:
                    break;
            }
        } catch (std::exception& e) {
            std::cerr << "Erro: " << e.what() << std::endl;
            currentState = ERROR;
            finish();
        }
    }

    /**
     * @brief Processa mensagens de erro para a criação do diretório.
     * 
     * @param msg Mensagem de erro recebida.
     */
    void processError(const tftp2::Mensagem& msg) {
        auto error = msg.error();
        std::cout << "Erro recebido do servidor: " << error.errorcode() << std::endl;
        finish();
    }

    /**
     * @brief Trata o timeout da operação de criação do diretório.
     */
    void handle_timeout() {
        std::cout << "Timeout na sessão com o servidor: " << getIP(this->serverAddr) << std::endl;   
        currentState = ERROR;
        finish();
    }
};

/**
 * @brief Classe para gerenciar a listagem de arquivos e diretórios no servidor.
 */
class listCallback : public Callback {
    sockaddr_in serverAddr;    // Endereço do servidor
    std::string filename;      // Diretório ou referência para listagem
    int sockfd;                // Descritor do socket
    bool error = false;
    
    // Estados possíveis para a listagem
    enum State { WAITING_FOR_LIST, RECEIVING_LIST, ERROR };
    State currentState;

public:
    /**
     * @brief Construtor do listCallback.
     * 
     * @param serverAddr Endereço do servidor.
     * @param sockfd Descritor do socket.
     * @param filename Diretório ou nome de referência para listagem.
     * @param timeout Tempo limite para a operação.
     */
    listCallback(sockaddr_in &serverAddr, int sockfd, const std::string& filename, long timeout) 
        : Callback(sockfd, timeout), serverAddr(serverAddr), filename(filename), sockfd(sockfd), currentState(WAITING_FOR_LIST) {
        this->fd = sockfd;
    }

    /**
     * @brief Processa a resposta do servidor contendo a lista de arquivos/diretórios.
     */
    void handle() {
        char bufferRX[1024];
        socklen_t addrLen = sizeof(serverAddr);
        ssize_t recvBytes = recvfrom(fd, bufferRX, sizeof(bufferRX), 0, (sockaddr*)&serverAddr, &addrLen);

        if (recvBytes < 0) {
            throw std::runtime_error("Erro ao receber a mensagem");
        }

        try {
            tftp2::Mensagem msg;
            if (!msg.ParseFromArray(bufferRX, recvBytes)) {
                throw std::runtime_error("Falha ao desserializar a mensagem");
            }

            switch (currentState) {
                case WAITING_FOR_LIST:
                    if (msg.has_list_resp()) {
                        processList(msg); // Processa e exibe a lista recebida
                        finish();
                    } else if (msg.has_error()) {
                        currentState = ERROR;
                        processError(msg);
                    }
                    break;
                case ERROR:
                    break;
            }
        } catch (std::exception& e) {
            std::cerr << "Erro: " << e.what() << std::endl;
            currentState = ERROR;
            finish();
        }
    }

    /**
     * @brief Processa a mensagem contendo a lista de itens do diretório.
     * 
     * @param msg Mensagem contendo a resposta da listagem.
     */
    void processList(const tftp2::Mensagem& msg) {
        auto list = msg.list_resp();
        std::cout << "Arquivos no diretório " << filename << ":" << std::endl;
        // Itera sobre os itens listados e exibe se são diretórios ou arquivos
        for (int i = 0; i < list.items_size(); i++) {
            if(list.items(i).has_dir()) {
                std::cout << "DIR: " << list.items(i).dir().path() << std::endl;
            } else {
                std::cout << "FILE: " << list.items(i).file().nome() << std::endl;
            }
        }
    }

    /**
     * @brief Processa mensagens de erro durante a listagem.
     * 
     * @param msg Mensagem de erro recebida.
     */
    void processError(const tftp2::Mensagem& msg) {
        auto error = msg.error();
        std::cout << "Erro recebido do servidor: " << error.errorcode() << std::endl;
        finish();
    }

    /**
     * @brief Trata o timeout durante a operação de listagem.
     */
    void handle_timeout() {
        std::cout << "Timeout na sessão com o servidor: " << getIP(this->serverAddr) << std::endl;
        currentState = ERROR;
        finish();
    }
};

#endif
