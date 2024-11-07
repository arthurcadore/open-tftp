#include "messaging.h"


std::vector<uint8_t> requestMessage::serialize() const {

    // Define o delimitador
    char delimiter = 0x00;
    
    // define um vetor em formato de dados octeto
    std::vector<uint8_t> data;

    // Converte o opcode para um inteiro de 16 bits
    uint16_t opcodeInt = static_cast<uint16_t>(opcode);

    // Primeiro e segundo bytes do opcode
    data.push_back(opcodeInt >> 8);      
    data.push_back(opcodeInt & 0xFF);    

    // Insere o nome do arquivo ao final do vetor
    data.insert(data.end(), filename.begin(), filename.end()); 

    // Adiciona o delimitador
    data.push_back(delimiter);

    // Insere o tipo de dado ao final do vetor
    data.insert(data.end(), mode.begin(), mode.end());    

    data.push_back(delimiter);
    return data;
}

std::vector<uint8_t> dataMessage::serialize() const {

    // define um vetor em formato de dados octeto
    std::vector<uint8_t> data;

    // Converte o opcode para um inteiro de 16 bits
    uint16_t opcodeInt = static_cast<uint16_t>(opcode);

    // Primeiro e segundo bytes do opcode
    data.push_back(opcodeInt >> 8);      
    data.push_back(opcodeInt & 0xFF);    

    // Primeiro e segundo bytes do número do bloco
    data.push_back(blockNumber >> 8);
    data.push_back(blockNumber & 0xFF);

    // Insere os dados ao final do vetor
    data.insert(data.end(), data.begin(), data.end());
    return data;
}

std::vector<uint8_t> ackMessage::serialize() const {

    // define um vetor em formato de dados octeto
    std::vector<uint8_t> data;

    // Converte o opcode para um inteiro de 16 bits
    uint16_t opcodeInt = static_cast<uint16_t>(opcode);

    // Primeiro e segundo bytes do opcode
    data.push_back(opcodeInt >> 8);      
    data.push_back(opcodeInt & 0xFF);    

    // Primeiro e segundo bytes do número do bloco
    data.push_back(blockNumber >> 8);
    data.push_back(blockNumber & 0xFF);

    return data;
}

std::vector<uint8_t> errorMessage::serialize() const {

    // Define o delimitador
    char delimiter = 0x00;
    
    // define um vetor em formato de dados octeto
    std::vector<uint8_t> data;

    // Converte o opcode para um inteiro de 16 bits
    uint16_t opcodeInt = static_cast<uint16_t>(opcode);

    // Primeiro e segundo bytes do opcode
    data.push_back(opcodeInt >> 8);      
    data.push_back(opcodeInt & 0xFF);    

    // Primeiro e segundo bytes do código de erro
    data.push_back(errorCode >> 8);
    data.push_back(errorCode & 0xFF);

    // Insere a mensagem de erro ao final do vetor
    data.insert(data.end(), errMsg.begin(), errMsg.end()); 

    // Adiciona o delimitador
    data.push_back(delimiter);

    return data;
}