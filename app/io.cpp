#include "io.h"

/*
    Verifica se o arquivo existe, se é possivel abrir e retorna seu tamanho em bytes

    Parâmetros:
        - nomeArquivo: nome do arquivo a ser lido
*/
int checkfile(const std::string& nomeArquivo){

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
    Função para escrever em um arquivo de texto o conteúdo de um buffer

    Parâmetros:
        - nomeArquivo: nome do arquivo a ser lido
        - buffer: ponteiro para o buffer que irá armazenar o conteúdo do arquivo
        - tamanho: tamanho do buffer
*/
void writefile(const char* buffer, std::size_t tamanho, const std::string& nomeArquivo) {
    // Abre o arquivo em modo binário
    std::ofstream arquivo(nomeArquivo, std::ios::out | std::ios::binary | std::ios::app);

    // Verifica se o arquivo foi aberto com sucesso
    if (!arquivo) {
        std::cerr << "Erro ao abrir o arquivo para escrita: " << nomeArquivo << std::endl;
        return;
    }

    // Escreve o buffer no arquivo
    arquivo.write(buffer, tamanho);

    // Verifica se a escrita foi bem-sucedida
    if (!arquivo) {
        std::cerr << "Erro ao escrever no arquivo: " << nomeArquivo << std::endl;
    }

    // Fecha o arquivo
    arquivo.close();
}

/*

    Função para ler de um arquivo de texto uma quantidade especifica de bytes e armazenar em um buffer

    A função também faz a leitura conforme um offset especificado, assim podendo ler de 512 em 512 bytes

    Parâmetros:
        - nomeArquivo: nome do arquivo a ser lido
        - buffer: ponteiro para o buffer que irá armazenar o conteúdo do arquivo
        - tamanho: tamanho do buffer
        - offset: offset para leitura do arquivo
        - lenght: tamanho maximo do arquivo
*/

bool readfile(const std::string& nomeArquivo, char* buffer, std::size_t data_lenght, std::size_t offset, std::size_t file_lenght) {
    // Abre o arquivo em modo binário
    std::ifstream arquivo(nomeArquivo, std::ios::in | std::ios::binary);

    // Verifica se o arquivo foi aberto com sucesso
    if (!arquivo) {
        std::cerr << "Erro ao abrir o arquivo para leitura: " << nomeArquivo << std::endl;
        return false;
    }

    // Verifca se o offset é maior que o tamanho do arquivo
    if (file_lenght >= offset + data_lenght) {

        // Move o ponteiro do arquivo para o offset
        // Pega um bloco de "data_lenght" bytes do arquivo e armazena no buffer
        arquivo.seekg(offset);
        arquivo.read(buffer, data_lenght);

        // Verifica se a leitura foi bem-sucedida
        if (!arquivo) {
            std::cerr << "Erro ao ler o arquivo: " << nomeArquivo << std::endl;
        }

        arquivo.close();
        return true;
        
    } else {

        // Leva o ponteiro até o offset e lê o restante do arquivo
        arquivo.seekg(offset);
        arquivo.read(buffer, file_lenght - offset);

        // Verifica se a leitura foi bem-sucedida
        if (!arquivo) {
            std::cerr << "Erro ao ler o arquivo: " << nomeArquivo << std::endl;
        }
    
        arquivo.close();
        return false;
    }
    
}

