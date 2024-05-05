#include <ESP8266WiFi.h>

const char* ssid = "robotWifi";
const char* password = "password";
const int port = 12345;

const int motorIn1 = 5; // D1
const int motorIn2 = 4; // D2

const int motor1EncoderPin1 = 14; // D5
const int motor1EncoderPin2 = 12; // D6

const bool reverseMotor1 = false;
const bool reverseMotor2 = false;

int motor1Count = 0;

WiFiServer server(port);


void IRAM_ATTR readEncoder();

void setup() {
  delay(10);
  Serial.begin(115200);

  pinMode(motorIn1, OUTPUT);
  pinMode(motorIn2, OUTPUT);

  pinMode(motor1EncoderPin1, INPUT);
  pinMode(motor1EncoderPin2, INPUT);

	digitalWrite(motorIn1, HIGH);
	digitalWrite(motorIn2, HIGH);

  attachInterrupt(digitalPinToInterrupt(motor1EncoderPin1), readEncoder, CHANGE);

  // Configure the ESP8266 as an access point
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);

  // Start the server
  server.begin();

  Serial.println("Access Point created");
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
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


volatile bool encoderALast = LOW;
volatile bool encoderBLast = LOW;

void IRAM_ATTR readEncoder(){
  bool encoderAState = digitalRead(motor1EncoderPin1);
  bool encoderBState = digitalRead(motor1EncoderPin2);
  
  if (encoderBState != encoderAState) {
    motor1Count++;
  } else {
    motor1Count--;
  }

  encoderALast = encoderAState;

  Serial.println(motor1Count);
}

void controllerUpdate();
void setMotor1(int speed);


void loop() {
  delay(10);
  
  // Check if a client has connected
  WiFiClient client = server.available();
  if (client) {
    Serial.println("New client connected");

    // Read data from the client
    // while (client.connected()) {
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

        // printController();
        controllerUpdate();
      }
    // }

    // Close the connection
    client.stop();
    Serial.println("Client disconnected");
  }

  static bool direction = false;
  static int val = 0;
  if(direction){
    val++;
  }else{
    val--;
  }


  if(val >= 1023 || val <= -1024){
    direction = !direction;
  }

  setMotor1(val);
  Serial.println(val);

}


// String repeatString(String str, int count) {
//   String result = "";
//   for (int i = 0; i < count; i++) {
//     result += str;
//   }
//   return result;
// }

// String leftPad(String str, int targetWidth){
//   return repeatString(" ", (targetWidth - str.length())) + str;
// }

// String printButton(String str, bool isPressed){
//   if(isPressed){
//     return str;
//   }else{
//     return repeatString("-", str.length());
//   }
// }

// void printController(){
//   Serial.println("LS:[" + 
//     leftPad(String(axis_ls_x), 6) + ", " + leftPad(String(axis_ls_y),6) + "] RS:[" + 
//     leftPad(String(axis_rs_x), 6) + ", " + leftPad(String(axis_rs_y),6) + "] LT:" +
//     leftPad(String(axis_lt), 4) + ", RT:" + 
//     leftPad(String(axis_rt), 4) + " [" +
//     printButton("A", btn_A) + 
//     printButton("B", btn_B) + 
//     printButton("X", btn_X) + 
//     printButton("Y", btn_Y) + 
//     printButton("LB", btn_LB) + 
//     printButton("RB", btn_RB) + 
//     printButton("LS", btn_LS) + 
//     printButton("RS", btn_RS) + 
//     printButton("@", btn_xbox) + 
//     printButton("H", btn_home) + setMotor1
//     printButton("M", btn_menu) + 
//     printButton("^", btn_dpad_up) + 
//     printButton("v", btn_dpad_down) + 
//     printButton("<", btn_dpad_left) + 
//     printButton(">", btn_dpad_right) + "]");
// }

void setMotor1(int speed){
  Serial.println(speed);
  if(speed > 0){
    analogWrite(motorIn1, !reverseMotor1 ? speed : 0);
    analogWrite(motorIn2,  reverseMotor1 ? speed : 0);
  }else{
    analogWrite(motorIn1, !reverseMotor1 ? abs(speed) : 0);
    analogWrite(motorIn2,  reverseMotor1 ? abs(speed) : 0);
  }
}

void controllerUpdate() {
  // setMotor1((int)((double)axis_ls_y/32));
}