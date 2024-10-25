#ifndef IO_H
#define IO_H

#include <iostream>
#include <fstream>
#include <string>  // Make sure string is included
#include <cstddef> // For std::size_t

/*
    Verifica se o arquivo existe, se é possivel abrir e retorna seu tamanho em bytes

    Parâmetros:
        - nomeArquivo: nome do arquivo a ser lido
*/
int checkfile(const std::string& nomeArquivo);

/*
    Função para ler um arquivo e retornar o conteúdo em um buffer
    Parâmetros:
        - nomeArquivo: nome do arquivo a ser lido
        - buffer: ponteiro para o buffer que irá armazenar o conteúdo do arquivo
        - tamanho: tamanho do buffer
*/
void writefile(const char* buffer, std::size_t tamanho, const std::string& nomeArquivo);

/*
    Função para ler de um arquivo de texto uma quantidade especifica de bytes e armazenar em um buffer
    Parâmetros:
        - nomeArquivo: nome do arquivo a ser lido
        - buffer: ponteiro para o buffer que irá armazenar o conteúdo do arquivo
        - tamanho: tamanho do buffer
        - offset: offset para leitura do arquivo
*/
void readfile(const std::string& nomeArquivo, char* buffer, std::size_t tamanho, std::size_t offset);

#endif
