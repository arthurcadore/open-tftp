#ifndef MESSAGING_H
#define MESSAGING_H

#include <string>
#include <vector>
#include <cstdint>


/*
Definição dos opcodes para cada tipo de mensagem. 
*/
enum class OpcodeRM : uint16_t {
    READ = 1,
    WRITE = 2
};

enum class OpcodeDM : uint16_t {
    DATA = 3
};

enum class OpcodeAM : uint16_t {
    ACK = 4
};

enum class OpcodeEM : uint16_t {
    ERROR = 5
};

/*
 2 bytes     string    1 byte     string   1 byte
 ------------------------------------------------
| Opcode |  Filename  |   0  |   "octet"   |   0  |
 ------------------------------------------------
*/
struct requestMessage {
    OpcodeRM opcode;        // Opcodes restritos ao enum
    std::string filename;   // Nome do arquivo
    std::string mode;       // Modo "octet"

    // Construtor
    requestMessage(OpcodeRM op, const std::string& filename): 
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
    OpcodeDM opcode;      // Opcode de 2 bytes
    uint16_t blockNumber; // Número do bloco
    std::string data;    

    // Construtor
    dataMessage(OpcodeDM op, uint16_t block, const std::string& data): 
        opcode(op), 
        blockNumber(block), 
        data(data) {}

    // Função para serializar o pacote em um vetor de bytes
    std::vector<uint8_t> serialize() const;

    // Função para deserializar o pacote
    static dataMessage deserialize(char buffer[], int comprimento);

    // Função para imprimir o conteúdo de data;
    std::string printData();
};

/*
  2 bytes     2 bytes
 ---------------------
| Opcode |   Block #  |
 ---------------------
*/
struct ackMessage {
    OpcodeAM opcode;      // Opcode de 2 bytes
    uint16_t blockNumber; // Número do bloco

    // Construtor
    ackMessage(OpcodeAM op, uint16_t block): 
        opcode(op), 
        blockNumber(block) {}

    // Função para serializar o pacote em um vetor de bytes
    std::vector<uint8_t> serialize() const;

    // Função para deserializar o pacote
    static ackMessage deserialize(char buffer[], int comprimento);

    // Função para imprimir o conteúdo de data;
    std::string printBN();
};

/*
  2 bytes     2 bytes      string    1 byte
 -----------------------------------------
| Opcode |  ErrorCode |   ErrMsg   |   0  |
 -----------------------------------------
*/
struct errorMessage {
    OpcodeEM opcode;      // Opcode de 2 bytes
    uint16_t errorCode;   // Código de erro
    std::string errMsg;   // Mensagem de erro

    // Construtor
    errorMessage(OpcodeEM op, uint16_t code, const std::string& msg): 
        opcode(op), 
        errorCode(code), 
        errMsg(msg) {}

    // Função para serializar o pacote em um vetor de bytes
    std::vector<uint8_t> serialize() const;

    // Função para deserializar o pacote
    static errorMessage deserialize(char buffer[], int comprimento);

    // Função para imprimir o conteúdo de data;
    std::string printData();
};

#endif