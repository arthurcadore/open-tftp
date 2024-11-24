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
        throw std::runtime_error("Erro ao abrir o arquivo para leitura: " + nomeArquivo);
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
        - blockSize: tamanho do bloco (referencia)
        - lenght: quantidade de bytes a serem lidos
*/
char* readBlock(const std::string& nomeArquivo, int n, int blockSize, int lenght){

    if(lenght == 0){
        // cria uma string vazia
        char* bloco = new char[1];
        bloco[0] = '\0';
        return bloco;
    }

    // Abre o arquivo em modo binário
    std::ifstream arquivo(nomeArquivo, std::ios::in | std::ios::binary);

    // Move o ponteiro do arquivo para o bloco n
    arquivo.seekg(n * blockSize);

    // Aloca memória para o bloco
    char* bloco = new char[lenght];

    // Lê o bloco (tenta pegar blockSize bytes, caso não consiga, pega o que conseguir)
    arquivo.read(bloco, lenght);
    
    // Fecha o arquivo
    arquivo.close();

    return bloco;
}


/*
    Função para escrever blocos em um arquivo
    Parâmetros:
        - nomeArquivo: nome do arquivo a ser escrito
        - block: bloco recebido para ser escrito (concatenado ao arquivo), do tipo string
*/
void writeBlock(const std::string& nomeArquivo, const std::string& block){

    // Abre o arquivo em modo binário
    std::ofstream arquivo(nomeArquivo, std::ios::out | std::ios::binary | std::ios::app);

    // Verifica se o arquivo foi aberto com sucesso
    if (!arquivo) {
        std::cerr << "Erro ao abrir o arquivo para escrita: " << nomeArquivo << std::endl;
        return;
    }

    // Escreve o bloco no arquivo
    arquivo.write(block.c_str(), block.size());

    // Fecha o arquivo
    arquivo.close();
}

/*
    Função para deletar arquivo em caso de timeout
    Parâmetros:
        - nomeArquivo: nome do arquivo a ser deletado
*/
void deleteFile(const std::string& nomeArquivo){
    // Deleta o arquivo
    if (remove(nomeArquivo.c_str()) != 0) {
        std::cerr << "Erro ao deletar o arquivo: " << nomeArquivo << std::endl;
    }
}

