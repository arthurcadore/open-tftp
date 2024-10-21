#include "requisition.h"

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

string datablock(int blockNumber, string data) {

    string msg;
    msg += (char) (DATA >> 8);
    msg += (char) (DATA & 0xFF);
    msg += (char) (blockNumber >> 8);
    msg += (char) (blockNumber & 0xFF);
    msg += data;

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