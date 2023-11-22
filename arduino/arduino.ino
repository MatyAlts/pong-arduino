#include <LiquidCrystal.h>
LiquidCrystal lcd (2,3,4,5,6,7);

const int MIN_PALETA_IA_Y = 0;   // Posición mínima de la paleta de la IA
const int MAX_PALETA_IA_Y = 420;  // Posición máxima de la paleta de la IA

class Player{
  private:
  int Y_POS = 210;
  int SCORE = 0;
  public:
  void limits(){
    Y_POS = constrain(Y_POS, MIN_PALETA_IA_Y, MAX_PALETA_IA_Y);
  }
  void changePos(int y){
    Y_POS = y;
  }
  void changeScore(){
    SCORE++;
  }
  void resetScore(){
    SCORE = 0;
  }
  void sumPos(int y){
    Y_POS += y;
  }
  void equalsPos(int y){
    Y_POS = y;
  }
  int PosValue(){
    return Y_POS;
  }
  int ScoreValue(){
    return SCORE;
  }
};
Player Player_1, Player_2;

char* diff = "hard";
bool start = false;
int potPin = A0;       // El pin analógico donde está conectado el potenciómetro
/*Player_1.changePos(210);    // Posición Y inicial de la paleta 1
Player_2.changePos(210);    // Posición Y inicial de la paleta 2*/
int pelotaX = 480;     // Posición X inicial de la pelota
int pelotaY = 240;     // Posición Y inicial de la pelota
int direccionPelotaX = random(2) == 0 ? -5 : -5; // Dirección X inicial de la pelota
int direccionPelotaY = random(2) == 0 ? -5 : 5; // Dirección Y inicial de la pelota
int puntaje_1 = 0;    //Puntaje inicial del jugador 1
int puntaje_2 = 0;    //Puntaje inicial del jugador 2
int recording = false;
int replaying = false;

const int RANGO_PALETA_IA_MIN = 150; // Posición mínima del rango de movimiento de la paleta de la IA
const int RANGO_PALETA_IA_MAX = 330; // Posición máxima del rango de movimiento de la paleta de la IA

const int VELOCIDAD_PALETA_IA = 8; // Velocidad de movimiento de la paleta de la IA

unsigned long ultimoCambioDireccion = 0; // Almacena el tiempo del último cambio de dirección
unsigned long intervaloCambioDireccion = 800; // Intervalo de tiempo para cambiar la dirección (2 segundos)

int direccionActual = 1; // Dirección de movimiento actual (1 para arriba, -1 para abajo)

int dificultad = 3;   //Dificultad de la IA


void seguirPelota(){
  // Calcula la diferencia entre la posición de la paleta de la IA y la posición de la pelota
  int diferencia = pelotaY - Player_2.PosValue();

  // Ajusta la posición de la paleta de la IA en función de la diferencia
  if (abs(diferencia) > VELOCIDAD_PALETA_IA) {
    if (diferencia > 0) {
      Player_2.sumPos(VELOCIDAD_PALETA_IA);
    } else if (diferencia < 20) {
      Player_2.sumPos(-VELOCIDAD_PALETA_IA);
    }
  } else {
    // Si la diferencia es menor que la velocidad de la paleta, mueve la paleta directamente a la posición de la pelota
    Player_2.equalsPos(pelotaY);
  }
}

void movimientoAleatorio() {
  unsigned long tiempoActual = millis();

  if (tiempoActual - ultimoCambioDireccion >= intervaloCambioDireccion) {
    // Cambia la dirección de forma cíclica cada intervaloCambioDireccion
    direccionActual = -direccionActual; // Cambia la dirección (de arriba a abajo o viceversa)

    // Actualiza el tiempo del último cambio de dirección
    ultimoCambioDireccion = tiempoActual;
  }

  // Mueve la paleta de la IA en la dirección actual
  Player_2.sumPos(direccionActual * VELOCIDAD_PALETA_IA);
}

void setup() {
  analogWrite(11,120);    //Establece el brillo de la pantalla LCD
  lcd.begin(16,2);    //Inicializa el LCD
  pinMode(13, OUTPUT);    //Pin para el LED
  digitalWrite(13,HIGH);
  Serial.begin(19200);    //Velocidad del serial
  randomSeed(analogRead(0));    // Inicializa la semilla aleatoria
  Serial.println("Done!");    //Primer mensaje al iniciar Arduino
}

void loop() {
  Player_2.limits();
  if(!replaying){
    lcd.clear();
    lcd.setCursor(0,0);     //Especifica al LCD donde debe escribir
    lcd.print("Player 1:");
    lcd.setCursor(14,0);
    lcd.print(Player_1.ScoreValue());
    lcd.setCursor(0,1);
    lcd.print("CPU:");
    lcd.setCursor(5,1);
    lcd.print(diff);      //Imprime la dificultad de la CPU
    lcd.setCursor(14,1);
    lcd.print(Player_2.ScoreValue());
    if(recording){
      delay(1);
      lcd.setCursor(10,0);
      lcd.print("REC");
    }
  }
  else{
    dificultad = 5;
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Reproduciendo");
    lcd.setCursor(0,1);
    lcd.print("Partida");
    lcd.setCursor(7,1);
    lcd.print(".");
  }

  /******************************************************************************************/
  //Leer los mensajes recibidos por Serial

  String mensaje = ""; // Inicializa una cadena vacía

    // Lee caracteres hasta que se reciba un salto de línea ('\n') o hasta que se alcance un límite máximo
    char caracter;
    int maxLongitud = 64; // Establece un límite máximo de longitud del mensaje
    int tiempoMaximoEspera = 5; // Establece un tiempo máximo de espera en milisegundos

    unsigned long inicioTiempo = millis();
    while (millis() - inicioTiempo < tiempoMaximoEspera) {
      if (Serial.available() > 0) {
        caracter = Serial.read();
        mensaje += caracter;
        if (caracter == '\n' || mensaje.length() >= maxLongitud) {
          // Se encontró un salto de línea o se alcanzó el límite máximo
          break;
        }
      }
    }

    // Procesa el mensaje si no está vacío
    if (mensaje.length() > 0) {
      // Verifica si el mensaje recibido es igual a "reinicio"
      if (mensaje.equals("reinicio\n")) {
        // Si el mensaje es igual a "reinicio", reinicia el puntaje de los jugadores
        Player_1.resetScore();
        Player_2.resetScore();
      }
      if (mensaje.equals("recON\n")) {
        recording = true;
      }
      if (mensaje.equals("recOFF\n")) {
        recording = false;
      }
      if (mensaje.equals("replayON\n")) {
        replaying = true;
      }
      if (mensaje.equals("replayOFF\n")) {
        replaying = false;
      }
      // Verifica si el mensaje recibido es igual a "start"
      if (mensaje.equals("start\n")) {
        // Si el mensaje es igual a "start", empieza a funcionar el juego
    start = true;
      }
      // Verifica si el mensaje recibido es igual a "dificil"
      if (mensaje.equals("dificil\n")) {
        // Si el mensaje es igual a "dificil", cambia la dificultad del juego
        diff = "hard";
        dificultad = 3;
      }
      // Verifica si el mensaje recibido es igual a "facil"
      if (mensaje.equals("facil\n")) {
        // Si el mensaje es igual a "facil", cambia la dificultad del juego
        diff = "easy";
        dificultad = 15;
      }
    }
    /**************************************************************************************************/
  //El juego solo se ejecuta si el usuario lo pide
  if(start){
    if(!replaying){

  // Lee el valor del potenciómetro  
  int valorPot = analogRead(potPin);

  // Mapea el valor del potenciómetro al rango de la pantalla (10 a 420)
  Player_1.changePos(map(valorPot, 0, 1023, 0, 420));

  if (pelotaX > 480) {
    // La pelota está más allá del límite, sigue la configuración de seguimiento
    seguirPelota();
  } else {
    // La pelota está antes del límite, permite el movimiento aleatorio
    movimientoAleatorio();
  }

  // Actualizar la posición de la pelota
  pelotaX += direccionPelotaX;
  pelotaY += direccionPelotaY;

  // Detectar colisiones con las paletas (simulada)
  if (pelotaX < 40 && pelotaY > Player_1.PosValue() && pelotaY < Player_1.PosValue() + 60) {
    direccionPelotaX = 5;
  } else if (pelotaX > 920 && pelotaY > Player_2.PosValue() && pelotaY < Player_2.PosValue() + 60) {
    direccionPelotaX = -5;
  }

  // Detectar colisiones con los bordes superior e inferior
  if (pelotaY < 10 || pelotaY > 470) {
    direccionPelotaY *= -1;
  }

  // Detectar puntuación
  if (pelotaX < 0) {
    // Puntuación para jugador 2
    pelotaX = 480; // Reiniciar la posición de la pelota
    pelotaY = 240;
    direccionPelotaX = random(2) == 0 ? -5 : 5; // Cambiar la dirección X de la pelota
    direccionPelotaY = random(2) == 0 ? -5 : 5; // Cambiar la dirección Y de la pelota
    Player_2.changeScore();
    digitalWrite(13,LOW);
    delay(100);
    digitalWrite(13,HIGH);
  } else if (pelotaX > 960) {
    // Puntuación para jugador 1
    pelotaX = 480; // Reiniciar la posición de la pelota
    pelotaY = 240;
    direccionPelotaX = random(2) == 0 ? -5 : 5; // Cambiar la dirección X de la pelota
    direccionPelotaY = random(2) == 0 ? -5 : 5; // Cambiar la dirección Y de la pelota
    Player_1.changeScore();
    digitalWrite(13,LOW);
    delay(100);
    digitalWrite(13,HIGH);
  }
  // Envía la posición de la pelota y las paletas por serial a Processing
  Serial.print(Player_1.PosValue());
  Serial.print(",");
  Serial.print(Player_2.PosValue());
  Serial.print(",");
  Serial.print(pelotaX);
  Serial.print(",");
  Serial.print(pelotaY);
  Serial.print(",");
  Serial.print(direccionPelotaX);
  Serial.print(",");
  Serial.print(direccionPelotaY);
  Serial.print(",");
  Serial.print(Player_1.ScoreValue());
  Serial.print(",");
  Serial.println(Player_2.ScoreValue());
  }
}
  delay(dificultad);
}
