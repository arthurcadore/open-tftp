#include "main.h"

/*
  Converte uma string contendo um endereço IPv4 para uma estrutura sockaddr_in
  Parâmetros:
    - ipAddress: string contendo o endereço IPv4
  Retorno:
    - estrutura sockaddr_in preenchida com o endereço IPv4
  Exceção:
    - std::invalid_argument caso o endereço IPv4 seja inválido
*/
sockaddr_in stringToIPv4(const std::string& ipAddress) {

    // Cria uma estrutura sockaddr_in para armazenar o endereço IPv4
    struct sockaddr_in ipv4Addr;

    // Zera a estrutura sockaddr_in
    memset(&ipv4Addr, 0, sizeof(ipv4Addr));

    // Define a família do endereço para IPv4
    ipv4Addr.sin_family = AF_INET;

    // Converte a string para endereço binário. Se falhar, lança uma exceção.
    if (inet_pton(AF_INET, ipAddress.c_str(), &(ipv4Addr.sin_addr)) != 1) {
        throw std::invalid_argument("Endereço IPv4 inválido: " + ipAddress);
    }

    return ipv4Addr;
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
    cout << "Uso correto: " << argv[0] << " <IP> <Porta>" << endl;
    return 1;
  }

  // tenta converter o IP string para um IPv4, caso não consiga exibe o erro "Endereço IPv4 inválido"
  struct sockaddr_in ipv4Addr;
  try {
    ipv4Addr = stringToIPv4(argv[1]);
  } catch (invalid_argument e) {
    cout << "Endereço IPv4 inválido" << endl;
    return 1;
  }

  // tenta converter a porta para número usando stoi, e exibe o erro "Porta inválida" caso não seja possível
  // Também verifica se a porta é um número entre 1 e 65535.
  int port;
  try {
    port = stoi(argv[2]);
    if (port < 1 || port > 65535) {
      cout << "Porta inválida, utilize um número entre 1 e 65535" << endl;
      return 1;
    }
  } catch (invalid_argument e) {
    cout << "Argumento de porta inválido, utilize um número" << endl;
    return 1;
  }

  /*
    Laço do prompt de comando
  */
  while (true) {
    cout << "ftp > ";
    string comando;
    getline(cin, comando);

    // separa a string comando em duas partes: comando e argumento
    string argumento;
    size_t pos = comando.find(" ");
    if (pos != string::npos) {
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
        cout << "Argumento inválido" << endl;
        continue;
      }

      cout << "Fazendo download do arquivo..." << endl;


    // verifica se o comando é "put" e chama a função upload
    } else if (comando == "put") {

      // ignora argumentos vazios
      if (argumento.empty()) {
        cout << "Argumento inválido" << endl;
        continue;
      }

      cout << "Fazendo upload do arquivo..." << endl;

    // comando inválido
    } else {
      cout << "Comando inválido" << endl;
    }
  }
}