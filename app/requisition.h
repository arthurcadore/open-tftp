#ifndef REQUISITION_H
#define REQUISITION_H


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
#define DATA 3
#define ACK 4
#define ERROR 5

using namespace std;


/*
 2 bytes     string    1 byte     string   1 byte
 ------------------------------------------------
| Opcode |  Filename  |   0  |   "octet"   |   0  |
 ------------------------------------------------
*/

struct requestMessage {
    uint16_t opcode;      // Opcode de 2 bytes
    std::string filename; // Nome do arquivo
    std::string mode;     // Modo "octet"

    // Construtor
    requestMessage(uint16_t op, const std::string& filename): 
        opcode(op), 
        filename(filename), 
        mode("octet") {}

    // Função para serializar o pacote em um vetor de bytes
    std::vector<uint8_t> serialize() const;    
};

/*
  2 bytes     2 bytes      512 bytes
 ----------------------------------
| Opcode |   Block #  |   Data     |
 ----------------------------------
*/
struct dataMessage {
    uint16_t opcode;      // Opcode de 2 bytes
    uint16_t blockNumber; // Número do bloco
    std::string data;     // Dados

    // Construtor
    dataMessage(uint16_t op, uint16_t block, const std::string& data): 
        opcode(op), 
        blockNumber(block), 
        data(data) {}

    // Função para serializar o pacote em um vetor de bytes
    std::vector<uint8_t> serialize() const;
};


/*
    Function to create the request message to be sent to 
    the server WRR (Write Request) and WRQ (Read Request)
*/

string requestMessage(int opcode, string filename, string mode);

/* '
    Function to create the data block to be sent to the server        
*/
string datablock(int blockNumber, char* data);

/* 
    Function to create the ACK message to be sent to the server
*/
string ackMessage(int blockNumber);

/* 
    Function to create the error message to be sent to the server
*/
string errorMessage(int errorCode, string errMsg);

#endif