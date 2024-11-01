
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

#include "requisition.h"


std::vector<uint8_t> requestMessage::serialize() const {

    // Define o delimitador
    char delimiter = 0x00;
    
    // define um vetor em formato de dados octeto
    std::vector<uint8_t> data;

    // Primeiro e segundo bytes do opcode
    data.push_back(opcode >> 8);      
    data.push_back(opcode & 0xFF);    

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

    // Primeiro e segundo bytes do opcode
    data.push_back(opcode >> 8);      
    data.push_back(opcode & 0xFF);    

    // Primeiro e segundo bytes do número do bloco
    data.push_back(blockNumber >> 8);
    data.push_back(blockNumber & 0xFF);

    // Insere os dados ao final do vetor
    data.insert(data.end(), mode.begin(), mode.end());
    return data;
}

//

/*
 2 bytes     string    1 byte     string   1 byte
 ------------------------------------------------
| Opcode |  Filename  |   0  |   "octet"   |   0  |
 ------------------------------------------------

- Opcode: 1 ou 2 
- String1: Nome do arquivo
- Delimitador: 0x00 (1 byte)
- String2: octet
- Delimitador: 0x00 (1 byte)
*/

string requestMessage(int opcode, string filename, string mode) {

    string msg;
    msg += (char) (opcode >> 8);
    msg += (char) (opcode & 0xFF);
    msg += filename;
    msg += '\0';
    msg += mode;
    msg += '\0';

    // imprime a mensagem completa no terminal em hexa decimal para verificação:

    cout << "Mensagem: ";

    if (opcode == RRQ) {
        cout << "RRQ ";
    } else {
        cout << "WRQ ";
    }

    cout << endl;

    return msg;
}

//   2 bytes     2 bytes      512 bytes
//  ----------------------------------
// | Opcode |   Block #  |   Data     |
//  ----------------------------------
// 
// - Opcode: 3 
// - Block: número do bloco (2 bytes)
// - Data: 512 Bytes

string datablock(int blockNumber, char* data) {
    
        string msg;
        msg += (char) (DATA >> 8);
        msg += (char) (DATA & 0xFF);
        msg += (char) (blockNumber >> 8);
        msg += (char) (blockNumber & 0xFF);
    
        for (int i = 0; i < 512; i++) {
            msg += data[i];
        }
    
        cout << "Data: " << blockNumber << endl;
    
        return msg;
}

//   2 bytes     2 bytes
//   ---------------------
//  | Opcode |   Block #  |
//   ---------------------
// 
// - Opcode: 4
// - Block: número do bloco (2 bytes)

string ackMessage(int blockNumber) {

    string msg;
    msg += (char) (ACK >> 8);
    msg += (char) (ACK & 0xFF);
    msg += (char) (blockNumber >> 8);
    msg += (char) (blockNumber & 0xFF);

    cout << "ACK: " << blockNumber << endl;
    
    return msg;
}

//  2 bytes     2 bytes      string    1 byte
//  -----------------------------------------
// | Opcode |  ErrorCode |   ErrMsg   |   0  |
//  -----------------------------------------
//
//- Opcode: 5
//- ErrorCode: 0 á 7 (abaixo)
//- ErrMsg: String Identificando o erro
//- Delimitador: 0x00 (1 byte)

string errorMessage(int errorCode, string errMsg) {

    string msg;
    msg += (char) (ERROR >> 8);
    msg += (char) (ERROR & 0xFF);
    msg += (char) (errorCode >> 8);
    msg += (char) (errorCode & 0xFF);
    msg += errMsg;
    msg += '\0';

    return msg;
}
