#include "messaging.h"

#include <fstream>
#include <iostream>


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

    // Insere os dados da mensagem (vetor de char) ao final do vetor
    data.insert(data.end(), this->data.begin(), this->data.end());

    return data;
}

// função para deserializar o pacote
dataMessage dataMessage::deserialize(char buffer[], int comprimento) {
    
        // Verifica se o comprimento do buffer é menor que 4
        if (comprimento < 4) {
            throw std::runtime_error("Tamanho do buffer menor que 4");
        }
    
        // Converte os dois primeiros bytes do buffer para um inteiro de 16 bits
        uint16_t opcodeInt = (buffer[0] << 8) | buffer[1];

        if(static_cast<OpcodeDM>(opcodeInt) != OpcodeDM::DATA) {
            throw std::runtime_error("Opcode inválido");
        }
    
        // Converte os dois bytes seguintes para um inteiro de 16 bits
        uint16_t blockNumber = (buffer[2] << 8) | buffer[3];
    
        // Cria um vetor de char com os dados da mensagem
        std::vector<char> data(buffer + 4, buffer + comprimento);
    
        // Retorna um objeto dataMessage com os dados deserializados
        return dataMessage(OpcodeDM::DATA, blockNumber, std::string(data.begin(), data.end()));
}

// função para imprimir o conteúdo da mensagem
std::string dataMessage::printData() {
    // converte para string e a retorna
    return std::string(data.begin(), data.end());
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

// função para deserializar o pacote
errorMessage errorMessage::deserialize(char buffer[], int comprimento) {
        // Verifica se o comprimento do buffer é menor que 4
        if (comprimento < 4) {
            throw std::runtime_error("Tamanho do buffer menor que 4");
        }
    
        // Converte os dois primeiros bytes do buffer para um inteiro de 16 bits
        uint16_t opcodeInt = (buffer[0] << 8) | buffer[1];
    
        // Converte os dois bytes seguintes para um inteiro de 16 bits
        uint16_t errorCode = (buffer[2] << 8) | buffer[3];
    
        // Cria um vetor de char com os dados da mensagem
        std::vector<char> errMsg(buffer + 4, buffer + comprimento);
    
        // Retorna um objeto errorMessage com os dados deserializados
        return errorMessage(OpcodeEM::ERROR, errorCode, std::string(errMsg.begin(), errMsg.end()));
}

// função para imprimir o conteúdo da mensagem
std::string errorMessage::printData() {
    // converte para string e a retorna
    return std::string(errMsg.begin(), errMsg.end());
}