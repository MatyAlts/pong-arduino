import processing.serial.*;
import controlP5.*;

ControlP5 cp5;
Textfield miCampoDeTexto;
boolean borrarRectangulo = false;
boolean easyMode = false;
boolean hardMode = true;
ArrayList<String> datosGuardados = new ArrayList<String>();
boolean grabando = false;
boolean reproduciendo = false;
boolean mostrarTexto = false;
PImage icono;
PImage fondo;

String[] datosReplay;
int indiceReplay = 0;
PrintWriter writer; // Declarar un objeto PrintWriter

//Completar con el nombre del puerto serial
String Puerto = "COM4";
boolean Start = false;

Serial puertoSerial;  // Objeto Serial para la comunicación con Arduino
int paleta1Y, paleta2Y, pelotaX, pelotaY, direccionPelotaX, direccionPelotaY, puntaje_1, puntaje_2;

void setup() {
  fondo = loadImage("background.png");
  icono = loadImage("icon.png");
  surface.setIcon(icono);
  println(Serial.list()); // Imprime los puertos disponibles
  size(960, 480);  // Tamaño de la ventana
  background(fondo);   // Fondo negro
  textAlign(CENTER, CENTER);
  cp5 = new ControlP5(this);
  PFont fuente = createFont("Arial", 16);
  // Crea un campo de texto
  miCampoDeTexto = cp5.addTextfield("dificultad")
                     .setPosition(width/2-100, 400)
                     .setSize(200, 40)
                     .setColorBackground(color(255, 255, 255))
                     .setColorValue(color(0, 0, 0))
                     .setFont(fuente);
  
  // Configuración del puerto serial
  try {
    // Intenta abrir el puerto serial
    puertoSerial = new Serial(this, Puerto, 19200);
    println("Puerto serial abierto con éxito.");
  } catch (RuntimeException e) {
    // Maneja la excepción si no se puede abrir el puerto serial
    println("Error al abrir el puerto serial: " + e.getMessage());
  }
  // Inicializa las variables
  paleta1Y = 210;
  paleta2Y = 210;
  pelotaX = 480;
  pelotaY = 240;
  direccionPelotaX = 1;
  direccionPelotaY = 1;
  puntaje_1 = 0;
  puntaje_2 = 0;
  
  frameRate(60);  // Establece la velocidad de fotogramas a 60 FPS
}
void draw() {
  background(fondo);
  
  cp5.draw();
  if (borrarRectangulo) {
    fill(240); // Cambia el color del fondo al color de fondo actual
    rect(20, 20, 200, 40); // Dibuja un rectángulo sobre el campo de texto
    borrarRectangulo = false; // Reinicia la bandera para evitar borrar continuamente
  }
  if(mostrarTexto){
    textSize(32);
    text("Pulsa 4 para seguir jugando.",width/2, height/2);
  }
  // Lee datos del puerto serial si están disponibles
  if(!reproduciendo){
    while (puertoSerial.available() > 0) {
      String datos = puertoSerial.readStringUntil('\n');
        if (datos != null) {
          if(grabando){
            datosGuardados.add(datos); // Agrega datos a la lista
          }
        String[] valores = datos.trim().split(",");
          if (valores.length == 8) {
            paleta1Y = int(valores[0]);
            paleta2Y = int(valores[1]);
            pelotaX = int(valores[2]);
            pelotaY = int(valores[3]);
            direccionPelotaX = int(valores[4]);
            direccionPelotaY = int(valores[5]);
            puntaje_1 = int(valores[6]);
            puntaje_2 = int(valores[7]);
          }
       }
    }
  }
  else{ // Lee datos directamente desde el archivo guardado
    frameRate(120);
    if (indiceReplay < datosReplay.length) {
    String datos = datosReplay[indiceReplay];
    String[] valores = datos.split(",");
    if (datos != null) {
    if (valores.length == 8) {
      paleta1Y = int(valores[0]);
      paleta2Y = int(valores[1]);
      pelotaX = int(valores[2]);
      pelotaY = int(valores[3]);
      direccionPelotaX = int(valores[4]);
      direccionPelotaY = int(valores[5]);
      puntaje_1 = int(valores[6]);
      puntaje_2 = int(valores[7]);
    }
  }
  }
  else{
    mostrarTexto = true; // Muestra la pantalla cuando finalice la repeticion
    return;
  }
  indiceReplay++;
  }
  // Limpia la pantalla
  background(fondo);
  textSize(64);
  if(Start == true){
    textSize(12);
    text("dificultad",width/2,30);
    text("facil",width/2,50);
    if(easyMode){
      text("x",width/2 + 20,50);
    }
    text("dificil",width/2,70);
    if(hardMode){
      text("x",width/2 + 20,70);
    }
    
    textSize(64);
  // Dibuja las paletas
  fill(255,0,0);
  rect(20, paleta1Y, 20, 60); // Paleta 1
  fill(0,0,255);
  rect(920, paleta2Y, 20, 60); // Paleta 2
  
  // Dibuja la pelota
  fill(0,255,0);
  ellipse(pelotaX, pelotaY, 20, 20);
  
  fill(255,0,0);
  text(puntaje_1, 380, 50);
  fill(0,0,255);
  text(puntaje_2, 580, 50);
  fill(255);
  if(reproduciendo){
      textSize(20);
      text("reproduciendo grabacion..",150,30);
      textSize(64);
    }
  if(grabando){
      textSize(20);
      text("grabando repeticion..",150,30);
      textSize(64);
    }
  }
  else{
    text("Pulsa ENTER para empezar.",width/2, height/2-40);
    textSize(32);
    text("Pulsa 1 para grabar, 2 para parar",width/2, height/2+40);
    text("Pulsa 3 para cargar la repeticion",width/2, height/2+80);
  }
}
void keyPressed(){
  String resetGame = "reinicio";
  byte[] resetGameAction = resetGame.getBytes();
  if (key == '3' && Start == true) {
    easyMode = false;
    hardMode = true;
    cargarReplay("replay.txt");
    reproduciendo = true;
    String replayON = "replayON";
    byte[] replayONSer = replayON.getBytes();
    puertoSerial.write('\n');
    puertoSerial.write(replayONSer);
    puertoSerial.write('\n');
  }
  if (key == '4' && Start == true) {
    reproduciendo = false;
    puertoSerial.write('\n');
    puertoSerial.write(resetGameAction);
    puertoSerial.write('\n');
    String replayOFF = "replayOFF";
    byte[] replayOFFSer = replayOFF.getBytes();
    puertoSerial.write('\n');
    puertoSerial.write(replayOFFSer);
    puertoSerial.write('\n');
    String hard = "dificil";
    byte[] hardDif = hard.getBytes();
    println("Dificultad cambiada a: DIFICIL");
    easyMode = false;
    hardMode = true;
    puertoSerial.write('\n');
    puertoSerial.write(hardDif);
    puertoSerial.write('\n');
    indiceReplay = 0;
  }
  if (key == '1' && Start == true) {
    // Comienza a grabar
    grabando = true;
    String recON = "recON";
    byte[] recONSer = recON.getBytes();
    puertoSerial.write('\n');
    puertoSerial.write(recONSer);
    puertoSerial.write('\n');
  } else if (key == '2' && Start == true) {
    String recOFF = "recOFF";
    byte[] recOFFSer = recOFF.getBytes();
    puertoSerial.write('\n');
    puertoSerial.write(recOFFSer);
    puertoSerial.write('\n');
    // Detiene la grabación y guarda los datos en un archivo
    grabando = false;
    guardarReplay("replay.txt");
  }
  if(key == 'r' || key == 'R' && Start == true) {
    puertoSerial.write('\n');
    puertoSerial.write(resetGameAction);
    puertoSerial.write('\n');
  }
  String startGame = "start";
  byte[] startGameAction = startGame.getBytes();
  if(key==10){
    String textoIngresado = miCampoDeTexto.getText();
    
    // Haz algo con el texto ingresado, por ejemplo, imprímelo
    if(textoIngresado.toUpperCase().equals("FACIL")){
      String easy = "facil";
      byte[] easyDif = easy.getBytes();
      println("Dificultad cambiada a: FACIL");
      hardMode = false;
      easyMode = true;
      puertoSerial.write('\n');
      puertoSerial.write(easyDif);
      puertoSerial.write('\n');
    }
    if(textoIngresado.toUpperCase().equals("DIFICIL")){
      String hard = "dificil";
      byte[] hardDif = hard.getBytes();
      println("Dificultad cambiada a: DIFICIL");
      easyMode = false;
      hardMode = true;
      puertoSerial.write('\n');
      puertoSerial.write(hardDif);
      puertoSerial.write('\n');
    }
    // Borra el contenido del campo de texto
    miCampoDeTexto.clear();
    
    // Marca la bandera para borrar el rectángulo
    borrarRectangulo = true;
    
    puertoSerial.write('\n');
    puertoSerial.write(startGameAction);
    puertoSerial.write('\n');
    Start = true;
    }
}
void guardarReplay(String nombreArchivo) {
  writer = createWriter(nombreArchivo); // Crear un objeto PrintWriter
  for (String dato : datosGuardados) {
    if (!dato.trim().isEmpty()) { // Verifica que el dato no esté vacío
      writer.println(dato); // Escribe el dato en una línea
    }
  }
  writer.flush(); // Vacía el búfer del escritor
  writer.close(); // Cierra el archivo
  println("Replay guardado en " + nombreArchivo);
  datosGuardados.clear(); // Limpia la lista de datos grabados
}
void cargarReplay(String nombreArchivo) {
  datosReplay = loadStrings(nombreArchivo);
  println("Replay cargado desde " + nombreArchivo);
}
