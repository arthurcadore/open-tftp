#include "io.h"

/*
    Verifica se o arquivo existe, se é possivel abrir. 

    Parâmetros:
        - nomeArquivo: nome do arquivo a ser lido
*/
bool fileCheck(const std::string& nomeArquivo){

    // Abre o arquivo em modo binário
    std::ifstream arquivo(nomeArquivo, std::ios::in | std::ios::binary);

    // Verifica se o arquivo foi aberto com sucesso
    if (!arquivo) {
        std::cerr << "Erro ao abrir o arquivo para leitura: " << nomeArquivo << std::endl;
        return false;
    }

    // Fecha o arquivo
    arquivo.close();

    return true;
}

/*
    Função para retornar o tamanho de um arquivo
    Parâmetros:
        - nomeArquivo: nome do arquivo a ser lido
*/
int fileLenght(const std::string& nomeArquivo){

    // Abre o arquivo em modo binário
    std::ifstream arquivo(nomeArquivo, std::ios::in | std::ios::binary);

    // Verifica se o arquivo foi aberto com sucesso
    if (!arquivo) {
        std::cerr << "Erro ao abrir o arquivo para leitura: " << nomeArquivo << std::endl;
        return -1;
    }

    // Move o ponteiro do arquivo para o final
    arquivo.seekg(0, std::ios::end);

    // Pega o tamanho do arquivo
    int tamanho = arquivo.tellg();

    // Fecha o arquivo
    arquivo.close();

    return tamanho;
}


/*
    Função para ler blocos de um arquivo e retornar o conteúdo em um char*
    Parâmetros:
        - nomeArquivo: nome do arquivo a ser lido
        - n: número do bloco a ser lido
        - blockSize: tamanho do bloco a ser lido
*/
char* readBlock(const std::string& nomeArquivo, int n, int blockSize){

    // Abre o arquivo em modo binário
    std::ifstream arquivo(nomeArquivo, std::ios::in | std::ios::binary);

    // Verifica se o arquivo foi aberto com sucesso
    if (!arquivo) {
        std::cerr << "Erro ao abrir o arquivo para leitura: " << nomeArquivo << std::endl;
        return nullptr;
    }

    // Move o ponteiro do arquivo para o bloco n
    arquivo.seekg(n * blockSize);

    // Aloca memória para o bloco
    char* bloco = new char[blockSize];

    // Lê o bloco
    arquivo.read(bloco, blockSize);

    // Fecha o arquivo
    arquivo.close();

    return bloco;
}

/*
    Função para escrever blocos de um arquivo
    Parâmetros:
        - nomeArquivo: nome do arquivo a ser escrito
        - n: número do bloco a ser escrito
        - blockSize: tamanho do bloco a ser escrito
        - bloco: conteúdo do bloco a ser escrito
*/
void writeBlock(const std::string& nomeArquivo, int n, int blockSize, char* bloco){

    // Abre o arquivo em modo binário
    std::ofstream arquivo(nomeArquivo, std::ios::out | std::ios::binary);

    // Verifica se o arquivo foi aberto com sucesso
    if (!arquivo) {
        std::cerr << "Erro ao abrir o arquivo para escrita: " << nomeArquivo << std::endl;
        return;
    }

    // Move o ponteiro do arquivo para o bloco n
    arquivo.seekp(n * blockSize);

    // Escreve o bloco
    arquivo.write(bloco, blockSize);

    // Fecha o arquivo
    arquivo.close();
}



