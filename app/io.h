#ifndef IO_H
#define IO_H

#include <iostream>
#include <fstream>

/*
    Função para ler um arquivo e retornar o conteúdo em um buffer

    Parâmetros:
        - nomeArquivo: nome do arquivo a ser lido
        - buffer: ponteiro para o buffer que irá armazenar o conteúdo do arquivo
        - tamanho: tamanho do buffer
*/
void writefile(const char* buffer, std::size_t tamanho, const std::string& nomeArquivo);

#endif
