#include "main.h"

/*
  Laço do prompt de comando
*/
void cli(const std::string& ip, const std::string& port) {
  while (true) {

      // exibe o prompt de comando
      std::cout << "ftp > ";
      std::string comando;

      // lê a linha de comando
      std::getline(std::cin, comando);

      // separa a string comando em duas partes: comando e argumento
      std::string argumento;
      std::size_t pos = comando.find(" ");
      if (pos != std::string::npos) {
        argumento = comando.substr(pos + 1);
        comando = comando.substr(0, pos);
      }

      // ignora comandos vazios
      if (comando.empty()) {
        continue;
      }

      // verifica se o comando é "exit" e sai do laço
      if (comando == "exit") {
        break;

      // verifica se o comando é "get" e chama a função download
      } else if (comando == "get") {

        // ignora argumentos vazios
        if (argumento.empty()) {
          std::cout << "Argumento inválido" << std::endl;
          continue;
        }

        // faz o download do arquivo
        tftpclient client(ip, port, argumento);


      // verifica se o comando é "put" e chama a função upload
      } else if (comando == "put") {

        // ignora argumentos vazios
        if (argumento.empty()) {
          std::cout << "Argumento inválido" << std::endl;
          continue;
        } 
        // instancia um cliente TFTP
        tftpclient client(ip, port, argumento);

      // comando inválido
      } else {
        std::cout << "Comando inválido" << std::endl;
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