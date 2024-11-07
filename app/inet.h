#ifndef INET_H
#define INET_H

#include <string>
#include <stdexcept>
#include <cstring>
#include <arpa/inet.h> 
#include <fstream>
#include <iostream>
#include <string>

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
    Classe para implementar um cliente TFTP
*/
struct tftpclient {
    std::string ip;
    std::string port;
    sockaddr_in serverAddr;

    /*
        Construtor
        Parametros:
            ip: endereço IP do servidor
            p: porta do servidor
    */
    tftpclient(const std::string& ip, const std::string& p) : ip(ip), port(p) {

        // Converte o endereço IP para uma estrutura sockaddr_in
        serverAddr = stringToIPv4(ip);

        // Converte a porta para um inteiro e armazena na estrutura sockaddr_in
        int port = stringToPort(p);
        serverAddr.sin_port = htons(port);
    };

    /*
        Função para fazer download de um arquivo
        Parametros:
            filename: nome do arquivo a ser baixado
    */
    void download(const std::string& filename);

    /*
        Função para fazer upload de um arquivo
        Parametros:
            filename: nome do arquivo a ser enviado
    */
    void upload(const std::string& filename);

};



#endif