
// LÍBRERIAS
#include <SoftwareSerial.h> //Libreria Software Serial para Bluetooth (Se descarga), en esta caso usamos el HC-05 que sirve para usarse de Esclavo ó Maestro
#include <LiquidCrystal_I2C.h> //Librería de la pantalla LCD
#include <Wire.h>  //Librería para permitir comunicarse con el I2C
#include <Servo.h>  // librería para poder controlar el servo
#include <DHT.h> //Librería paa usar sensores DHT (Se descarga)
#include <DHT_U.h> //Complemento de la librería DHT


//PINES ESPECIALES
#define DHTPIN 6 //Se define el pin del DHT
#define trig 7 // Emisor de pulso o señal
#define echo 8 // Receptor "del eco" del pulso o señal
#define DHTTYPE DHT11 //Se define el tipo de sensor que se usara para medir la temperatura y humedad, en este caso sera el DHT-11

//OBJETOS DE LAS LIBRERÍAS
DHT dht(DHTPIN, DHTTYPE); // Crea un objeto de la librería DHT, indicando así el sensor
SoftwareSerial BT(10,11); //Definir los pines RX y TX del Arduino conectados al Bluetooth
Servo garage;   // Crea un objeto servo llamado servoMotor
Servo ventana;  //Crea un objeto servo llamado ventana
Servo puerta; //Crea un objeto Servo llamado puerta
LiquidCrystal_I2C lcd(0x3,16,2);  //Crear el objeto lcd con dirección (0x3,16,2) si es una pantalla de 16x2

//OBJETOS
char Mensaje; //Variable que mandara los mensajes a los Switches y por voz

// Bocina
const int buzzer = 13; //Bocina conectada al Pin 13

//Sistema de IR
const int sensorPin = 12; //Sensor de IR conectado al Pin 12 
const int LEDIV = 31; //Led Interior Verde
const int LEDIR = 30; //Led Interior Rojo
int luz = 0; //Valor para leer el dato de la luz       
int valor_sensor = 0; //Este sera el valor que andara leyendo la fotoresistencia       
int valor_limite = 475;  //Este valor hara que el LED cambie de estado a una determinada luminosidad.
int valor_luz;



//Sistema de luces interiores
int LEDSA = 43; //Led Sala
int LEDCO = 44; //Led cuarto
int LEDCA = 45; //Led baño
int LEDGA = 46; //Led Garage

//Luces para el Garaje
const int LEDVG = 47; //Led Verde Garage
const int LEDRG = 48; //Led Rojo Garage

//Variables para almacenar las lecturas del sensor DHT11
float celsius;
float fahrenheit;
float humedad;

void setup() {
  Serial.begin(9600); 
  BT.begin(9600); //Inicializamos el puerto serie BT (Para Modo AT 2)
  dht.begin(); //Comunicacion con el DHT11
  Serial.println("Iniciando Listo"); //Mensaje para indicar que esta el sistema listo
  lcd.init(); //Inicializamos la pantalla LCD  
  lcd.backlight(); //Encender la luz de fondo
  lcd.setCursor(1,1);
  lcd.print("BIENVENIDO(A)"); //Mensaje de Bienvenida
  lcd.setCursor(1,2);
  lcd.print("A DOMOTIBOT"); //Mensaje de Bienvenida
  delay(5000);
  
  garage.attach(2); //Se conecta la variable del Servo Garage al pin 2
  puerta.attach(3); //Se conecta la variable del Servo Puerta al pin 3
  ventana.attach(4); //Se conecta la variable del Servo Ventana al pin 4
  
  //Se establecen los pines como entradas o salidas
  //Todos los LED's se establecen como Salida
  pinMode(sensorPin, INPUT); //Se establece como Entrada
  pinMode(buzzer, OUTPUT);
  pinMode(LEDIV,OUTPUT);
  pinMode(LEDIR,OUTPUT);
  
  //pinMode(LEDR1, OUTPUT); 
  //pinMode(LEDR2, OUTPUT);
  //pinMode(LEDR3, OUTPUT); 
  //pinMode(LEDP1, OUTPUT);
  //pinMode(LEDP2,OUTPUT); 
  //pinMode(LEDP3,OUTPUT); 
  //pinMode(LEDBO,OUTPUT); 
  pinMode(LEDSA,OUTPUT); 
  pinMode(LEDCO,OUTPUT); 
  pinMode(LEDCA,OUTPUT); 
  pinMode(LEDGA,OUTPUT); 

  pinMode(LEDVG,OUTPUT); 
  pinMode(LEDRG,OUTPUT); 

  pinMode(trig, OUTPUT); //Se configura el Emisor como Salida
  pinMode(echo, INPUT); //Se configura el Receptor como Entrada
}

void loop() 
{ 
  long duration, distance; //Establecemos duration y distance como variables numéricas extensas

  digitalWrite(trig, LOW); //Para tener un pulso limpio empezamos con 2 microsegundos en apagado
  delayMicroseconds(2);
  digitalWrite(trig, HIGH); //Mandamos un pulso de 5 microsegundos
  delayMicroseconds(5);
  digitalWrite(trig, LOW); //Apagamos
  duration = pulseIn(echo, HIGH); //Medimos el tiempo que la señal tarda en volver al sensor en microsegundos
  distance = (duration/2)*0.0343; //La distancia es el tiempo por la velocidad del sonido (343 m/s = 0.0343 cm/microseg)
    
  digitalWrite(LEDVG,LOW); //Led verde indica que la puerta esta abierta
  digitalWrite(LEDRG,HIGH); //Led Rojo indicara que la puerta esta cerrada

  if (distance < 10)
  { //Si la distancia es menor a un metro y medio, se abrira la puerta del garage 
    Serial.println("Puerta de Garage abierta");
    digitalWrite(LEDVG,HIGH);
    digitalWrite(LEDRG,LOW);
    lcd.clear();
    lcd.setCursor(1,1);
    lcd.print("AUTO LLEGANDO");
    lcd.setCursor(1,2);
    lcd.print("ABRIR GARAJE");
    delay(1000);
    tone(buzzer,600);
    delay(1000);
    noTone(buzzer);
    garage.write(150);
    delay(8000);
    tone(buzzer,500);
    delay(2000);
    noTone(buzzer);
    garage.write(0);
    delay(1000);
  }
  else 
  {
    Serial.println("Garage cerradO");
    digitalWrite(LEDVG,LOW);
    digitalWrite(LEDRG,HIGH);
    lcd.clear();
    lcd.setCursor(1,1);
    lcd.print("GARAJE");
    lcd.setCursor(1,2);
    lcd.print("CERRADO");
    delay(1000);
    garage.write(0);                          
  }
  
  //SISTEMA DE TEMPERATURA Y HUMEDAD
  celsius = dht.readTemperature();  //Lee y almacena la temperatura del sensor DHT11
  fahrenheit = dht.readTemperature(true);  //Lee y almacena la temperatura del sensor DHT11
  humedad = dht.readHumidity();         //Lee y almacena la humedad del sensor DHT11
  
  
  //Envía los datos del DHT11 como la temperatura, humedad e intensidad a la aplicación móvil por medio del Bluetooth
  BT.print(celsius);    
  BT.print(" °C");
  BT.print("|");
  BT.print(fahrenheit);
  BT.print(" °F");
  BT.print("|");
  BT.print(humedad);
  BT.print(" %");
  BT.print("|");

  delay(1000);

  //Se envían los datos del DHT11 a la pantalla LCD
  lcd.clear();
  lcd.setCursor(1,1);
  lcd.print("CENTIGRADOS: ");
  lcd.print(celsius);
  lcd.print(" C");
  lcd.setCursor(1,2);
  lcd.print("HUMEDAD: ");
  lcd.print(humedad);
  lcd.print(" %");
  delay(1000);

  //Se envían los datos del DHT11 al Monitor Serie
  Serial.print(celsius);
  Serial.print(" °C");
  Serial.print("|");
  Serial.print(fahrenheit);
  Serial.print(" °F");
  Serial.print("|");
  Serial.print(humedad);
  Serial.print(" %");
  Serial.print("|");
  Serial.print(valor_luz);
  Serial.print(" %");
  delay(1000);                
  
  //NOTA: El DHT11 manda datos cada segundo. Si se usa un DHT22, cambiarlo por delay(2000)

//SISTEMA DE ALARMA AL ENTRAR EN LA PUERTA
  int value = 0; //Declaramos una variable llamada value donde guardara el valor de la luz leida por el sensor
  value = digitalRead(sensorPin); //el valor va a ser leido por el sensor
  
  digitalWrite(LEDIV,HIGH); //Led verde indica que no hay movimiento y todo esta seguro
  digitalWrite(LEDIR,LOW); //Led Rojo indicara que hay movimiento inesperado
  
  if (value == HIGH) //Aquí indicamos que si el valor esta encendido, en el Serial mostrara que no hay obstaculo y por lo tanto en el noTOne(buzzer) no emitirá un sonido
  {
    Serial.println("Seguro: No hay movimiento");
    noTone(buzzer);
    lcd.clear();
    lcd.setCursor(1,1);
    lcd.print("SEGURO: SIN");
    lcd.setCursor(1,2);
    lcd.print("MOVIMIENTO");
    delay(1000);
  }
  //En caso contrario 
  else
  {
    digitalWrite(LEDIR,HIGH); //El Led Rojo se enciende, indicando que hay movimiento
    digitalWrite(LEDIV,LOW); //El Led Verde se apaga
    lcd.clear();
    lcd.setCursor(1,1);
    lcd.print("!ADVERTENCIA!");
    lcd.setCursor(1,2);
    lcd.print("HAY MOVIMIENTO");
    delay(1000);
    tone(buzzer,2500); //Suena un tono
    delay(1000); //Durante 150 ms
    tone(buzzer,1500); //Suena otro tono
    delay(1000); //Durante 150 ms
    noTone(buzzer);
    Serial.println("Advertencia: Detectó movimiento"); //Manda un mensaje al Serial donde indica que hay movimiento
  }
  delay(100); //Se actualizara cada 100 ms  


//SISTEMA DE LUCES INTERIORES Y SISTEMA DE ABRIR-CERRAR PUERTAS POR SWITCH Y VOZ
if(BT.available()) 
{  //Si llega un dato por el puerto BT se envía al monitor Serial
  Mensaje = BT.read(); //Guarda en la variable “Mensaje” el dato recibido por Bluetooth
  
  /*Aqui la condicion con la app, indica que las letras Mayúculas son enviadas tanto por el control manual de la app, como con el control por voz, haciendo que se enciendan y/o 
  abra el objeto, de igual forma las Minúsculas indican que se apagan tanto por manual como por voz.*/
  
  if(Mensaje == 'A') 
  {
    Serial.println("Se encendió la Sala");
    digitalWrite(LEDSA,HIGH);
    lcd.clear();
    lcd.setCursor(1,1);
    lcd.print("SE ENCENDIO LA");
    lcd.setCursor(5,2);
    lcd.print("SALA");
    delay(1000);
  }
  else if(Mensaje == 'a') 
  {
     Serial.println("Se apagó la Sala");
    digitalWrite(LEDSA,LOW);
    lcd.clear();
    lcd.setCursor(1,1);
    lcd.print("SE APAGO LA");
    lcd.setCursor(5,2);
    lcd.print("SALA");
    delay(1000);
  }
  else if(Mensaje == 'B') 
  {
    Serial.println("Se encendió el cuarto");
    digitalWrite(LEDCO,HIGH);
    lcd.clear();
    lcd.setCursor(1,1);
    lcd.print("SE ENCENDIO EL");
    lcd.setCursor(5,2);
    lcd.print("CUARTO");
    delay(1000);
  }
  else if(Mensaje == 'b') 
  {
    Serial.println("Se apagó el cuarto");
    digitalWrite(LEDCO,LOW);
    lcd.clear();
    lcd.setCursor(1,1);
    lcd.print("SE APAGO EL");
    lcd.setCursor(5,2);
    lcd.print("CUARTO");
    delay(1000);
  }    
  else if(Mensaje == 'C') 
  {
    Serial.println("Se encendió el baño");
    digitalWrite(LEDCA,HIGH);
    lcd.clear();
    lcd.setCursor(1,1);
    lcd.print("SE ENCENDIO EL");
    lcd.setCursor(5,2);
    lcd.print("BAÑO");
    delay(1000);
  }
  else if(Mensaje == 'c') 
  {
    Serial.println("Se apagó el baño");
    digitalWrite(LEDCA,LOW);
    lcd.clear();
    lcd.setCursor(1,1);
    lcd.print("SE APAGO EL");
    lcd.setCursor(5,2);
    lcd.print("BAÑO");
    delay(1000);
  }
  else if(Mensaje == 'D') 
  {
    Serial.println("Se encendió el Garaje");
    digitalWrite(LEDGA,HIGH);
    lcd.clear();
    lcd.setCursor(1,1);
    lcd.print("SE ENCENDIO EL");
    lcd.setCursor(5,2);
    lcd.print("GARAJE");
    delay(1000);
  }
  else if(Mensaje == 'd') 
  {
    Serial.println("Se apagó el Garaje");
    digitalWrite(LEDGA,LOW);
    lcd.clear();
    lcd.setCursor(1,1);
    lcd.print("SE APAGO EL");
    lcd.setCursor(5,2);
    lcd.print("GARAJE");
    delay(1000);
  }    
  else if(Mensaje == 'E') 
  {
    Serial.println("Se encendió toda la Casa");
    digitalWrite(LEDSA,HIGH);
    digitalWrite(LEDCO,HIGH);
    digitalWrite(LEDCA,HIGH);
    digitalWrite(LEDGA,HIGH);
    lcd.clear();
    lcd.setCursor(1,1);
    lcd.print("SE ENCENDIO");
    lcd.setCursor(1,2);
    lcd.print("TODA LA CASA");
    delay(1000);
  }
  else if(Mensaje == 'e') 
  {
    Serial.println("Se apagó toda la Casa");
    digitalWrite(LEDSA,LOW);
    digitalWrite(LEDCO,LOW);
    digitalWrite(LEDCA,LOW);
    digitalWrite(LEDGA,LOW);
    lcd.clear();
    lcd.setCursor(1,1);
    lcd.print("SE APAGO");
    lcd.setCursor(1,2);
    lcd.print("TODA LA CASA");
    delay(1000);
  }    
  
 
  //ABRIR O CERRAR PUERTAS POR VOZ O SWITCHES
  //Garaje 
  else if(Mensaje == 'O') 
  {
    Serial.println("Se abrio Garaje por Switch o Voz");
    digitalWrite(LEDVG,HIGH);
    digitalWrite(LEDRG,LOW);
    lcd.clear();
    lcd.setCursor(1,1);
    lcd.print("SE ABRIO EL");
    lcd.setCursor(5,2);
    lcd.print("GARAJE");
    delay(1000);    
    tone(buzzer,600);
    delay(1000);
    noTone(buzzer);
    garage.write(150);
    delay(8000);
    tone(buzzer,500);
    delay(2000);
    noTone(buzzer);
    garage.write(0);
    delay(1000);
  }
    /*No hay necesidad de poner un apartado de que se cierre la puerta manualmente, ya que el loop en la parte donde lee la distancia del ultrasónico, la puerta del garaje
  se cierra automaticamente, pero lo pondre por si en dado caso, el dato que se envia al Bluetooth no llega, con la voz se puede volver activar.*/
  else if(Mensaje == 'o') 
  {
    Serial.println("Se cerró Garaje por Switch o Voz");
    lcd.clear();
    lcd.setCursor(1,1);
    lcd.print("SE CERRO EL");
    lcd.setCursor(5,2);
    lcd.print("GARAJE");
    delay(1000);
    noTone(buzzer);
    digitalWrite(LEDVG,LOW);
    digitalWrite(LEDRG,HIGH);
    garage.write(0);   
  }
  else if(Mensaje == 'p') 
  {
    Serial.println("Se cerró la Puerta");
    lcd.clear();
    lcd.setCursor(1,1);
    lcd.print("SE CERRO LA");
    lcd.setCursor(5,2);
    lcd.print("PUERTA");
    puerta.write(0);
    //delay(2000);
  }
  else if(Mensaje == 'P') 
  {
    Serial.println("Se abrió la Puerta 120 grados");
    lcd.clear();
    lcd.setCursor(1,1);
    lcd.print("SE ABRIO LA");
    lcd.setCursor(5,2);
    lcd.print("PUERTA");
    delay(1000);
    puerta.write(120);
  }
  else if(Mensaje == 'Q') 
  {
    Serial.println("Se abrió la Ventana");
    lcd.clear();
    lcd.setCursor(1,1);
    lcd.print("SE ABRIO LA");
    lcd.setCursor(5,2);
    lcd.print("VENTANA");
    delay(1000);
    ventana.write(0);
    //delay(5000);
  }
  else if(Mensaje == 'q') 
  {
    Serial.println("Se cerró la Ventana");
    lcd.clear();
    lcd.setCursor(1,1);
    lcd.print("SE CERRO LA");
    lcd.setCursor(5,2);
    lcd.print("VENTANA");
    delay(1000);
    ventana.write(90);
    //delay(5000);
  }
  delay(100);
 }
}
