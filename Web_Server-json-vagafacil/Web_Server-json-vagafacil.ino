

//Declarando as bibliotecas...
#include <Ultrasonic.h>
#include <SPI.h>
#include <Ethernet.h>

//Declarando as constantes para o sensor no pino 4 e 5.
#define trigger 4
#define echo 5


//Declarando as constantes dos leds.
#define livre 8
#define ocupado 9

//Inicializando o Sensor
Ultrasonic ultrasonic(trigger, echo);

//Mac do shield de Ethernet do Arduino
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
//Inicializando as informaçoes da rede
IPAddress ip(192, 168, 1, 109);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

//Porta do servidor
EthernetServer server(8080);

void setup() {

  pinMode(livre, OUTPUT);
  pinMode(ocupado, OUTPUT);
  
  Serial.begin(9600);

  //Inicializando do servidor
  Ethernet.begin(mac, ip, gateway, subnet);
  server.begin();
}

void loop() {

  EthernetClient client = server.available();
  if (client) {
    Serial.println("new client");
    // Uma requisição http terminal em linha branca
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        /*
        Se você chegou ao final da linha (recebeu um caractere de nova linha) 
        e a linha está em branco, a solicitação http terminou, 
        para que você possa enviar uma resposta
        */
        if (c == '\n' && currentLineIsBlank) {

          float cm;
          //distancia em tempo
          long microsec = ultrasonic.timing();
          //variavel cm e conversao para centimetro
          cm = ultrasonic.convert(microsec, Ultrasonic::CM);

          // Envia uma resposta no cabeçalho http
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: application/json;charset=utf-8");
          client.println("Access-Control-Allow-Origin: *"); // Dá o total acesso para qualquer um que requisite
          client.println("Server: Arduino");
          client.println("Connection: close");  // A conexão será fechada após a conclusão da resposta
          client.println("Refresh: 3");
          client.println();
          
          client.print("{\"mac\":\"");
          for(int i = 0; i <= 5; i++){
          client.print((mac[i]),HEX);
          }
          client.print("\",");

          if(cm>4){

            digitalWrite(livre, HIGH);
            digitalWrite(ocupado, LOW);
            client.print("\"disponivel\":\"1\"}");
            Serial.println("VAGA LIVRE");
            delay(1000);
            break;
            
          }

          else{

            
            digitalWrite(ocupado, HIGH);
            digitalWrite(livre, LOW);
            client.print("\"disponivel\":\"0\"}");
            Serial.println("VAGA OCUPADA");
            delay(1000);
            break;
            
          }
          
          
        }
        if (c == '\n') {
          // Começa uma nova linha
          currentLineIsBlank = true;
        } else if (c != '\r') {
          // Você recebeu um caractere na linha atual
          currentLineIsBlank = false;
        }
      }
    }
    // Tempo do navegador web para receber os dados
    delay(1);
    // Fecha a conexão
    client.stop();
    Serial.println("client disconnected");
  }
  
}

