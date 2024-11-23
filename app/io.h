#ifndef IO_H
#define IO_H

#include <iostream>
#include <fstream>
#include <string>  
#include <cstddef> 

/*
    Verifica se o arquivo existe, se é possivel abrir. 

    Parâmetros:
        - nomeArquivo: nome do arquivo a ser lido
*/
bool fileCheck(const std::string& nomeArquivo);

/*
    Função para retornar o tamanho de um arquivo
    Parâmetros:
        - nomeArquivo: nome do arquivo a ser lido
*/
int fileLenght(const std::string& nomeArquivo);

/*
    Função para ler blocos de um arquivo e retornar o conteúdo em um char*
    Parâmetros:
        - nomeArquivo: nome do arquivo a ser lido
        - n: número do bloco a ser lido
        - blockSize: tamanho do bloco a ser lido
*/
char* readBlock(const std::string& nomeArquivo, int n, int blockSize);

/*
    Função para escrever blocos em um arquivo
    Parâmetros:
        - nomeArquivo: nome do arquivo a ser escrito
        - block: bloco recebido para ser escrito (concatenado ao arquivo), do tipo string
*/
void writeBlock(const std::string& nomeArquivo, const std::string& block);

#endif
