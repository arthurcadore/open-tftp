#include "main.h"
#include <sstream>

enum Comando {
    CMD_EXIT,
    CMD_GET,
    CMD_PUT,
    CMD_INVALIDO,
    CMD_LIST,
    CMD_MOVE,
    CMD_MKDIR,
    CMD_REMOVE
};

// Função para converter a string do comando para o enum correspondente
Comando converterComando(const std::string& comando) {
    if (comando == "exit") return CMD_EXIT;
    if (comando == "get") return CMD_GET;
    if (comando == "put") return CMD_PUT;
    if (comando == "list") return CMD_LIST;
    if (comando == "move") return CMD_MOVE;
    if (comando == "mkdir") return CMD_MKDIR;
    if (comando == "remove") return CMD_REMOVE;
    return CMD_INVALIDO;
}
/*
  Laço do prompt de comando
*/
void cli(const std::string& ip, const std::string& port) {
    while (true) {
        std::cout << "tftp > ";
        std::string linha;
        std::getline(std::cin, linha);

        std::istringstream iss(linha);
        std::vector<std::string> partes;
        std::string palavra;
        while (iss >> palavra) {
            partes.push_back(palavra);
        }

        if (partes.empty()) {
            continue;
        }

        std::string comando = partes[0];
        Comando cmd = converterComando(comando);

        try {
            switch (cmd) {
                case CMD_EXIT:
                    return;

                case CMD_GET:
                case CMD_PUT:
                case CMD_LIST:
                case CMD_MKDIR:
                case CMD_REMOVE:
                    if (partes.size() < 2) {
                        std::cout << "Uso: " << comando << " <arquivo/diretório>" << std::endl;
                        continue;
                    }
                    {
                        tftpclient client(ip, port, partes[1], 1000);
                        if (cmd == CMD_GET) client.download();
                        if (cmd == CMD_PUT) client.upload();
                        if (cmd == CMD_LIST) client.list();
                        if (cmd == CMD_MKDIR) client.mkdir();
                        if (cmd == CMD_REMOVE) client.remove();
                    }
                    break;

                case CMD_MOVE:
                    if (partes.size() < 3) {
                        std::cout << "Uso: move <origem> <destino>" << std::endl;
                        continue;
                    }
                    {
                        tftpclient client(ip, port, partes[1], 1000);
                        client.move(partes[2]);
                    }
                    break;

                case CMD_INVALIDO:
                default:
                    std::cout << "Comando inválido" << std::endl;
                    break;
            }
        } catch (const std::exception& e) {
            std::cerr << "Erro: " << e.what() << std::endl;
        }
    }
}


/*
  Função principal
  Parâmetros:
    - argc: número de argumentos na linha de comando
    - argv: array de argumentos da linha de comando
*/
int main(int argc, char* argv[]) {

  // verifica se há 3 argumentos na linha de comando (nome do programa, IP e porta)
  if (argc != 3) {
    std::cout << "Uso correto: " << argv[0] << " <IP> <Porta>" << std::endl;
    return 1;
  }

  // Inicializa a cli (prompt de comando)
  cli(argv[1], argv[2]);

}