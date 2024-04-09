#include <ESP8266WiFi.h>
// #include ""

const char* ssid = "test";
const char* password = "test";
const int port = 12345;

WiFiServer server(port);

void setup() {
  Serial.begin(115200);
  delay(10);

  // Configure the ESP8266 as an access point
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);

  // Start the server
  server.begin();

  Serial.println("Access Point created");
  Serial.print("SSID: ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.softAPIP());
  Serial.print("Listening on port: ");
  Serial.println(port);
}

short bytesToShort(byte highByte, byte lowByte) {
  return (short)((highByte << 8) | lowByte);
}

void bytesToBoolArray(byte byte1, byte byte2, bool boolArray[]) {
  for (int i = 0; i < 8; i++) {
    boolArray[i] = bitRead(byte1, i);
    boolArray[i + 8] = bitRead(byte2, i);
  }
}

short axis_ls_x = 0;
short axis_ls_y = 0;
short axis_rs_x = 0;
short axis_rs_y = 0;
short axis_lt = 0;
short axis_rt = 0;

bool btn_A = false;
bool btn_B = false;
bool btn_X = false;
bool btn_Y = false;
bool btn_LB = false;
bool btn_RB = false;
bool btn_LS = false;
bool btn_RS = false;
bool btn_xbox = false;
bool btn_home = false;
bool btn_menu = false;
bool btn_dpad_up = false;
bool btn_dpad_down = false;
bool btn_dpad_left = false;
bool btn_dpad_right = false;

void loop() {
  // Check if a client has connected
  WiFiClient client = server.available();
  if (client) {
    Serial.println("New client connected");

    // Read data from the client
    while (client.connected()) {
      if (client.available()) {
        byte buffer[14];
        client.readBytes(buffer, 14);

        axis_ls_x = bytesToShort(buffer[0], buffer[1]);
        axis_ls_y = bytesToShort(buffer[2], buffer[3]);
        axis_rs_x = bytesToShort(buffer[4], buffer[5]);
        axis_rs_y = bytesToShort(buffer[6], buffer[7]);
        axis_lt = bytesToShort(buffer[8], buffer[9]);
        axis_rt = bytesToShort(buffer[10], buffer[11]);

        bool buttons[16];
        bytesToBoolArray(buffer[12], buffer[13], buttons);


        // if(buttons[0] != btn_A && !btn_A){
        //   Serial.println("A pressed!");
        // }else if (buttons[0] != btn_A && btn_A){
        //   Serial.println("A Unpressed!");
        // }

        btn_A = buttons[0];
        btn_B = buttons[1];
        btn_X = buttons[2];
        btn_Y = buttons[3];
        btn_LB = buttons[4];
        btn_RB = buttons[5];
        btn_LS = buttons[6];
        btn_RS = buttons[7];
        btn_xbox = buttons[8];
        btn_home = buttons[9];
        btn_menu = buttons[10];
        btn_dpad_up = buttons[11];
        btn_dpad_down = buttons[12];
        btn_dpad_left = buttons[13];
        btn_dpad_right = buttons[14];

        printController();
      }
    }

    // Close the connection
    client.stop();
    Serial.println("Client disconnected");
  }
}


String repeatString(String str, int count) {
  String result = "";
  for (int i = 0; i < count; i++) {
    result += str;
  }
  return result;
}

String leftPad(String str, int targetWidth){
  return repeatString(" ", (targetWidth - str.length())) + str;
}

String printButton(String str, bool isPressed){
  if(isPressed){
    return str;
  }else{
    return repeatString("-", str.length());
  }
}

void printController(){
  Serial.println("LS:[" + 
    leftPad(String(axis_ls_x), 6) + ", " + leftPad(String(axis_ls_y),6) + "] RS:[" + 
    leftPad(String(axis_rs_x), 6) + ", " + leftPad(String(axis_rs_y),6) + "] LT:" +
    leftPad(String(axis_lt), 4) + ", RT:" + 
    leftPad(String(axis_rt), 4) + " [" +
    printButton("A", btn_A) + 
    printButton("B", btn_B) + 
    printButton("X", btn_X) + 
    printButton("Y", btn_Y) + 
    printButton("LB", btn_LB) + 
    printButton("RB", btn_RB) + 
    printButton("LS", btn_LS) + 
    printButton("RS", btn_RS) + 
    printButton("@", btn_xbox) + 
    printButton("H", btn_home) + 
    printButton("M", btn_menu) + 
    printButton("^", btn_dpad_up) + 
    printButton("v", btn_dpad_down) + 
    printButton("<", btn_dpad_left) + 
    printButton(">", btn_dpad_right) + "]");
}
