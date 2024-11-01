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