#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

//Configuração de rede e senha
const char *rede = "AMR";
const char *senha = "12345678";

WiFiServer server(80);

// Variável de estado
char estado = 'r';  // r = vermelho, y = amarelo, g = verde
int tempo = 1000;
unsigned long ultimaTroca = 0;
int tempoCor = 5000; // 5 segundos por cor

void setup() {
  Serial.begin(9600);

  WiFi.begin(rede, senha);
  Serial.println("Conectando-se ao Wi-Fi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("Wi-Fi conectado.");
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());

  server.begin();
}

void loop() {
  // ---- Controle automático do semáforo ----
  if (millis() - ultimaTroca >= tempoCor) {
    ultimaTroca = millis();
    if (estado == 'r') estado = 'g';   // vermelho -> verde
    else if (estado == 'g') estado = 'y'; // verde -> amarelo
    else estado = 'r';                 // amarelo -> vermelho
    Serial.print("Mudou estado para: ");
    Serial.println(estado);

    
  }

  // ---- Servidor Web ----
  WiFiClient client = server.available();

  if (client) {
    String request = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        request += c;
        if (request.endsWith("\r\n\r\n")) break;
      }
    }

    // Endpoint para AJAX -> retorna só a cor atual
    if (request.indexOf("/estado") != -1) {
      client.println("HTTP/1.1 200 OK");
      client.println("Content-type:text/plain");
      client.println();
      client.print(estado);
      client.stop();
      return;
    }

    // Endpoint para mudar estado manualmente (botão da ambulância)
    if (request.indexOf("/setEstado?valor=") != -1) {
      int pos = request.indexOf("valor=") + 6;
      if (pos < request.length()) {
        estado = request.charAt(pos);
        Serial.print("Novo estado recebido do navegador: ");
        Serial.println(estado);
      }
      client.println("HTTP/1.1 200 OK");
      client.println("Content-type:text/plain");
      client.println();
      client.print("OK - Estado atualizado para ");
      client.print(estado);
      client.println();
      client.stop();
      return;
    }

    // Página HTML
    client.println("HTTP/1.1 200 OK");
    client.println("Content-type:text/html");
    client.println();
    client.println(updateWebpage());
    client.stop();
  }
}

// --- Página HTML ---
String updateWebpage() {
  String ptr = "<!DOCTYPE html><html><head><title>Semáforo ESP8266</title><meta charset='UTF-8'>";
  ptr += "<style>body{text-align:center;font-family:Arial;background:#f0f0f0;}";
  ptr += ".light{width:100px;height:100px;margin:20px auto;border-radius:50%;background:gray;}";
  ptr += "</style></head><body>";
  ptr += "<h1>Semáforo ESP8266</h1>";
  ptr += "<div id='red' class='light'></div>";
  ptr += "<div id='yellow' class='light'></div>";
  ptr += "<div id='green' class='light'></div>";
  ptr += "<button onclick=\"enviarEstado('g')\">🚑 Ambulância</button>";

  // JS: busca estado a cada 1s
  ptr += "<script>";
  ptr += "function atualizar(){";
  ptr += "fetch('/estado').then(r=>r.text()).then(cor=>{";
  ptr += "document.getElementById('red').style.backgroundColor=(cor==='r')?'red':'gray';";
  ptr += "document.getElementById('yellow').style.backgroundColor=(cor==='y')?'yellow':'gray';";
  ptr += "document.getElementById('green').style.backgroundColor=(cor==='g')?'green':'gray';";
  ptr += "});}";
  ptr += "setInterval(atualizar,";
  ptr += tempo;
  ptr +=");atualizar();";

  ptr += "function enviarEstado(cor){";
  ptr += "fetch('/setEstado?valor='+cor).then(r=>r.text()).then(t=>console.log(t));";
  ptr += "}";
  ptr += "</script></body></html>";
  return ptr;
}
