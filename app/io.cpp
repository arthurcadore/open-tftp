#include "io.h"

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