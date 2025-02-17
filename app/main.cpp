#include "main.h"

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
    if (comando == "rename") return CMD_MOVE;
    if (comando == "mdkir") return CMD_MKDIR;
    if (comando == "remove") return CMD_REMOVE;
    return CMD_INVALIDO;
}
/*
  Laço do prompt de comando
*/
void cli(const std::string& ip, const std::string& port) {
    while (true) {
        // Exibe o prompt de comando
        std::cout << "tftp > ";
        std::string comando;

        // Lê a linha de comando
        std::getline(std::cin, comando);

        // Separa a string comando em duas partes: comando e argumento
        std::string argumento;
        std::size_t pos = comando.find(" ");
        if (pos != std::string::npos) {
            argumento = comando.substr(pos + 1);
            comando = comando.substr(0, pos);
        }

        // Ignora comandos vazios
        if (comando.empty()) {
            continue;
        }

        // Converte o comando para o enum correspondente
        Comando cmd = converterComando(comando);

        // Switch-case para tratar os comandos
        switch (cmd) {
            case CMD_EXIT:
                // Sai do laço
                return;

            case CMD_GET:
                // Ignora argumentos vazios
                if (argumento.empty()) {
                    std::cout << "Argumento inválido" << std::endl;
                    continue;
                }

                try {
                    // Faz o download do arquivo
                    tftpclient client(ip, port, argumento, 100);
                    client.download();
                } catch (std::runtime_error& e) {
                    std::cout << e.what() << std::endl;
                }
                break;

            case CMD_PUT:
                // Ignora argumentos vazios
                if (argumento.empty()) {
                    std::cout << "Argumento inválido" << std::endl;
                    continue;
                }

                try {
                    // Instancia um cliente TFTP
                    tftpclient client(ip, port, argumento, 1000);
                    client.upload();
                } catch (std::runtime_error& e) {
                    std::cout << e.what() << std::endl;
                }
                break;

            case CMD_LIST:

                if (argumento.empty()) {
                    std::cout << "Argumento inválido" << std::endl;
                    continue;
                }
              
                try {
                    // Instancia um cliente TFTP
                    tftpclient client(ip, port, argumento, 1000);
                    client.list();
                } catch (std::runtime_error& e) {
                    std::cout << e.what() << std::endl;
                }
              
              case CMD_MOVE:
                if (argumento.empty()) {
                    std::cout << "Argumento inválido" << std::endl;
                    continue;
                }
              
                try {
                    // Instancia um cliente TFTP
                    tftpclient client(ip, port, argumento, 1000);
                    client.move();
                } catch (std::runtime_error& e) {
                    std::cout << e.what() << std::endl;
                }

              case CMD_MKDIR:
                if (argumento.empty()) {
                    std::cout << "Argumento inválido" << std::endl;
                    continue;
                }
              
                try {
                    // Instancia um cliente TFTP
                    tftpclient client(ip, port, argumento, 1000);
                    client.mkdir();
                } catch (std::runtime_error& e) {
                    std::cout << e.what() << std::endl;
                }
                break;

              case CMD_REMOVE:
                if (argumento.empty()) {
                    std::cout << "Argumento inválido" << std::endl;
                    continue;
                }
              
                try {
                    // Instancia um cliente TFTP
                    tftpclient client(ip, port, argumento, 1000);
                    client.remove();
                } catch (std::runtime_error& e) {
                    std::cout << e.what() << std::endl;
                }
                break;


            case CMD_INVALIDO:
            default:
                std::cout << "Comando inválido" << std::endl;
                break;
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