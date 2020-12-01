/*
 * A velocidade é aumentada ao receber 'V' e diminuida com 'v'
 * Brilho é aumentado com 'B' e diminuido com 'b'
 *
//Comandos:
 * 
 * C = cor fixa
 * S = Strobe 
 * s = Smooth
 * F = Flash
 * f = Fade de uma cor só
 * e = Fade de varias cores
 * 
 * Ao receber 'C', os próximos valores são inteiros para r, outro para g e outro para b
 * Ao receber 'S', 's', 'F', 'f' ou 'e' a única alteração será em processoAtual
 * Ao receber 'V','v','B' ou 'b', a unica alteração será na velocidade ou no brilho
 * Ao receber 'P', o dispositivo entra ou sai do Standby
 * 
 * Pinagem:
 * 
 * Pinos 0,1,10,11,12,13: NADA CONECTADO
 * Pino 2: TX do módulo Bluetooth
 * Pino 3: RX do módulo Bluetooth
 * Pino 4: Controla o Led indicador do StandBy
 * Pino 5: Controla a Intensidade da luz Azul
 * Pino 6: Controla a Intensidade da luz Verde
 * Pino 7: Recebe do módulo Bluetooth o pino STATE. Caso o sinal tenha ruídos, deve-se utilizar um capacitor, caso contrário, o programa vai ficar constantemente piscando azul
 * Pino 8: Pino do receptor InfraVermelho
 * Pino 9: Controla a Intensidade da luz Vermelha
 * 
 */
 
#include <SoftwareSerial.h>
#include <IRremote.h>

IRrecv controle(8);
SoftwareSerial blue(2,3);

//Variaveis globais:
decode_results comandos;

byte const pinR=9;
byte const pinG=6;
byte const pinB=5;

bool standby=false;

//C para cor fixa,S para Strobe,s para smooth,F para Flash e f para fade
char processoAtual;

byte r;
byte g;
byte b;

int velocidade=4;
float brilho=0;
float brilhoAnterior=80;

struct cor{
  byte verm=0;
  byte verd=0;
  byte azul=0;
};

void aumentarBrilho(){
  if (brilho<=95 and not standby){
    brilho+=5;
  }
}
void diminuirBrilho(){
  if (brilho>=10){
    brilho-=5;
  }
}
void diminuirVelocidade(){
  if (velocidade>1){
    velocidade--;
  }
}

void aumentarVelocidade(){  
  if (velocidade<8){
    velocidade++;
  }
}


byte receberInt(){
  char charAtual=blue.read();
  String numero="";
  while (charAtual!=' '){
    numero+=charAtual;
    charAtual=blue.read();
  }
  return numero.toInt();
}


bool statusBTanterior=false;
bool verifConexaoBt(){
  bool statusAtual=digitalRead(7);
  if (statusAtual!=statusBTanterior){
    statusBTanterior=statusAtual;
    return true;
  }
  else{
    return false;
  }
}

void btSwitch(){
  analogWrite(pinR,0);
  analogWrite(pinG,0);
  analogWrite(pinB,0);
  delay(100);
  
  analogWrite(pinB,255*(brilho/100));
  delay(300);
  
  analogWrite(pinB,0);
  delay(100);
  
  analogWrite(pinB,255*(brilho/100));
  delay(300);
  
  analogWrite(pinB,0);
  delay(100);
  
  analogWrite(pinB,255*(brilho/100));
  delay(300);
  
  analogWrite(pinB,0);
  delay(100);
}

char lerBluetooth(){
  char byteRecebido=blue.read();
  if (byteRecebido=='C'){
    blue.read();
    r=receberInt();
    g=receberInt();
    b=receberInt();
  }
  
  //LIXO: CASO UM COMANDO SEJA ADICIONADO, ADICIONAR AQUI, CASO CONTRARIO O COMANDO NÃO FUNCIONARÁ
  else if (byteRecebido!='v' and byteRecebido!='V' and byteRecebido!='b' and byteRecebido!='B' and byteRecebido!='P' and byteRecebido!='s' and byteRecebido!='f' and byteRecebido!='S' and byteRecebido!='F' and byteRecebido!='e'){
    byteRecebido=processoAtual;
  }
  
  while(blue.available()){
    char lixo=blue.read();
  }

  return byteRecebido;
}


void setColor(cor color){
  r=color.verm;
  g=color.verd;
  b=color.azul;
}
  

//Função que fará todas as verificações a todo tempo, para evitar que os comandos no app ou no controle tenham atrasos
bool verificacoes(){
  //Trecho Bluetooth:
  if (blue.available()){
    char comando=lerBluetooth();
    if ((comando != 'b') and (comando != 'B') and (comando != 'v') and ( comando != 'V' )  and (comando != processoAtual)){
      if (comando != 'P'){
        processoAtual=comando;
      }
      else{
        if (standby){
          standby=false;
        }
        else{
          standby=true;
        }
      }
      return true;
    }
    else{
      if (comando == 'b'){
        diminuirBrilho();
      }
      else if (comando == 'B'){
        aumentarBrilho();
      }
      else if (comando=='v'){
        diminuirVelocidade();
      }
      else if (comando=='V'){
        aumentarVelocidade();
      }
      return false;
    }
  }
  
  if (verifConexaoBt()){
    btSwitch();
    delay(300);
  }
  
  //Trecho controle:
  if(controle.decode(&comandos)){
    cor cores[5][3];

    //Coluna 1
    //Botão 1:
    cores[0][0].verm=255;
    //Botão 2:
    cores[1][0].verm=255;
    cores[1][0].verd=25;
    //Botão 3:
    cores[2][0].verm=255;
    cores[2][0].verd=60;
    //Botão 4:
    cores[3][0].verm=255;
    cores[3][0].verd=100;
    //Botão 5:
    cores[4][0].verm=255;
    cores[4][0].verd=200;

    //Coluna 2:
    //Botão 1:
    cores[0][1].verd=255;
    //Botão 2:
    cores[1][1].verd=255;
    cores[1][1].azul=75;
    //Botão 3:
    cores[2][1].verd=255;
    cores[2][1].azul=255;
    //Botão 4:
    cores[3][1].verd=140;
    cores[3][1].azul=255;
    //Botão 5:
    cores[4][1].verd=50;
    cores[4][1].azul=255;

    //Coluna 3:
    //Botão 1:
    cores[0][2].azul=255;
    //Botão 2:
    cores[1][2].azul=255;
    cores[1][2].verm=35;
    //Botão 3:
    cores[2][2].azul=255;
    cores[2][2].verm=90;
    //Botão 4:
    cores[3][2].azul=200;
    cores[3][2].verm=120;
    //Botão 5:
    cores[4][2].azul=100;
    cores[4][2].verm=255;


    //Branco:
    cor branco;
    branco.verm=255;
    branco.verd=255;
    branco.azul=255;
    
    char comando=processoAtual;
    
    //Ifs com os codigos dos botões:
    if(comandos.value==0xE85952E1 or comandos.value==0xF720DF){
      setColor(cores[0][0]);
      comando='C';
    }
    else if (comandos.value==0xD3FD9A81 or comandos.value==0xF710EF){
      setColor(cores[1][0]);
      comando='C';
    }
    else if (comandos.value==0x84044BBD or comandos.value==0xF730CF){
      setColor(cores[2][0]);
      comando='C';
    }
    else if (comandos.value==0xB0F9B3E1 or comandos.value==0xF708F7){
      setColor(cores[3][0]);
      comando='C';
    }
    else if (comandos.value==0x9DE75E1D or comandos.value==0xF728D7){
      setColor(cores[4][0]);
      comando='C';
    }
    else if (comandos.value==0x78CDA4DC or comandos.value==0x78CDA4DD or comandos.value==0xF7A05F){
      setColor(cores[0][1]);
      comando='C';
    }
    else if (comandos.value==0x6471EC7D or comandos.value==0xF7906F){
      setColor(cores[1][1]);
      comando='C';
    }
    else if (comandos.value==0x14789DB9 or comandos.value==0xF7B04F){
      setColor(cores[2][1]);
      comando='C';
    }
    else if (comandos.value==0x416E05DD or comandos.value==0xF78877){
      setColor(cores[3][1]);
      comando='C';
    }
    else if (comandos.value==0xF794B621 or comandos.value==0xF7A857){
      setColor(cores[4][1]);
      comando='C';
    }
    else if (comandos.value==0xA2672345 or comandos.value==0xF7609F){
      setColor(cores[0][2]);
      comando='C';
    }
    else if (comandos.value==0x9D52009D or comandos.value==0xF750AF){
      setColor(cores[1][2]);
      comando='C';
    }
    else if (comandos.value==0x3E121C21 or comandos.value==0xF7708F){
      setColor(cores[2][2]);
      comando='C';
    }
    else if (comandos.value==0x6A844445 or comandos.value==0xF748B7){
      setColor(cores[3][2]);
      comando='C';
    }
    else if (comandos.value==0x57F52E81 or comandos.value==0xF76897){
      setColor(cores[4][2]);
      comando='C';
    }
    else if (comandos.value==0x9BA392C1 or comandos.value==0xF7E01F){
      setColor(branco);
      comando='C';
    }
    
    //Comandos:
    else if (comandos.value==0x8503705D or comandos.value==0xF700FF){
      aumentarBrilho();
    }
    else if(comandos.value==0xDEB0C861 or comandos.value==0xF7807F){
      diminuirBrilho();
    }
    else if(comandos.value==0xD4DD0381 or comandos.value==0xF740BF){
      standby=true;
      delay(60);
      controle.resume();
      return true;
    }
    else if(comandos.value==0xCE1972FD or comandos.value==0xF7C03F){
      if (standby){
        standby=false;
      }
    }
    else if(comandos.value==0xDCC45BE1 or comandos.value==0xF7D02F){//FLASH
      comando='F';
    }
    else if(comandos.value==0x374E8B9D or comandos.value==0xF7F00F){//STROBE
      comando='S';
    }
    else if(comandos.value==0xB9C07541 or comandos.value==0xF7C837){//FADE / ESCURECER switch
      if (processoAtual=='f'){
        comando='e';
      }
      else{
        comando='f';
      }
    }
    else if(comandos.value==0xA7315F7D or comandos.value==0xF7E817){//SMOOTH
      comando='s';
    }    
    delay(60);
    controle.resume();

    if (comando!=processoAtual){
      processoAtual=comando;
      return true;
    }
  }
  return false;
}

void acenderLed(){
  analogWrite(pinR,r*(brilho/100));
  analogWrite(pinG,g*(brilho/100));
  analogWrite(pinB,b*(brilho/100));
}

void acenderLedEscurecer(int intensidade){
  analogWrite(pinR,(r*float(intensidade)/255)*(brilho/100));
  analogWrite(pinG,(g*float(intensidade)/255)*(brilho/100));
  analogWrite(pinB,(b*float(intensidade)/255)*(brilho/100));
}

void apagarLed(){
  analogWrite(pinR,0);
  analogWrite(pinG,0);
  analogWrite(pinB,0);
}

bool piscar(byte atraso){
  if (verificacoes()){
    return true;
  }
  acenderLed();
  delay(atraso);

  if (verificacoes()){
    return true;
  }
  apagarLed();
  delay(atraso/2);
  return false;
}

int checarAtrasoFlash(){
  if (velocidade==8){
    return 540;
  }
  else if (velocidade==7){
    return 550;
  }
  else if (velocidade==6){
    return 570;
  }
  else if (velocidade==5){
    return 590;
  }
  else if (velocidade==4){
    return 600;
  }
  else if (velocidade==3){
    return 620;
  }
  else if (velocidade==2){
    return 640;
  }
  else if (velocidade==1){
    return 660;
  }
  else{
    return 600;
  }
}
  

void flash(){
  byte atraso=checarAtrasoFlash();
  if (piscar(atraso)){
    return;
  }
}


void strobe(){
  byte atraso=checarAtrasoFlash();
  
  //Vermelho
  r=255;
  g=0;
  b=0;
  atraso=checarAtrasoFlash();
  if (piscar(atraso)){
    return;
  }
  
  //Amarelo
  r=255;
  g=200;
  b=0;
  atraso=checarAtrasoFlash();
  if (piscar(atraso)){
    return;
  }
  
  //Verde
  r=0;
  g=255;
  b=0;
  atraso=checarAtrasoFlash();
  if (piscar(atraso)){
    return;
  }
    
  //Azul claro
  r=0;
  g=255;
  b=200;
  atraso=checarAtrasoFlash();
  if (piscar(atraso)){
    return;
  }
  
  //Azul
  r=0;
  g=0;
  b=255;
  atraso=checarAtrasoFlash();
  if (piscar(atraso)){
    return;
  }
  
  //Rosa
  r=255;
  g=0;
  b=200;
  atraso=checarAtrasoFlash();
  if (piscar(atraso)){
    return;
  }
  
  //Branco
  r=255;
  g=255;
  b=255;
  atraso=checarAtrasoFlash();
  if (piscar(atraso)){
    return;
  }
}

byte checarAtrasoSmooth(){ //Para Fade e Smooth
  if (velocidade==8){
    return 1;
  }
  else if (velocidade==7){
    return 2;
  }
  else if (velocidade==6){
    return 3;
  }
  else if (velocidade==5){
    return 4;
  }
  else if (velocidade==4){
    return 5;
  }
  else if (velocidade==3){
    return 6;
  }
  else if (velocidade==2){
    return 7;
  }
  else if (velocidade==1){
    return 8;
  }
  return 4;
}

void fade(){
  byte atraso=checarAtrasoSmooth();
  int i=0;
  for(i=0;i<255;i++){
    acenderLedEscurecer(i);
    if (verificacoes()){
      return;
    }
    atraso=checarAtrasoSmooth();
    delay(atraso);
  }
  
  for(i=255;i>0;i--){
    acenderLedEscurecer(i);
    if (verificacoes()){
      return;
    }
    atraso=checarAtrasoSmooth();
    delay(atraso);
  }  
}

void escurecer(){
  byte atraso=checarAtrasoSmooth();
  
  r=0;
  g=0;
  b=0;
  for(r=0;r<255;r++){
    acenderLed();
    if (verificacoes()){
      return;
    }
    atraso=checarAtrasoSmooth();
    delay(atraso);
  }
  atraso=checarAtrasoSmooth();
  delay(atraso*4);
  
  for(r=255;r>0;r--){
    acenderLed();
    if (verificacoes()){
      return;
    }
    atraso=checarAtrasoSmooth();
    delay(atraso);
  }
  atraso=checarAtrasoSmooth();
  delay(atraso*2);
  
  for (r=0;r<255;r++){
    g++;
    acenderLed();
    if (verificacoes()){
      return;
    }
    atraso=checarAtrasoSmooth();
    delay(atraso);
  }
  atraso=checarAtrasoSmooth();
  delay(atraso*4);
  
  for (r=255;r>0;r--){
    g--;
    acenderLed();
    if (verificacoes()){
      return;
    }
    atraso=checarAtrasoSmooth();
    delay(atraso);
  }
  atraso=checarAtrasoSmooth();
  delay(atraso*2);
  
  for(g=0;g<255;g++){
    acenderLed();
    if (verificacoes()){
      return;
    }
    atraso=checarAtrasoSmooth();
    delay(atraso);
  }
  atraso=checarAtrasoSmooth();
  delay(atraso*4);
  
  for(g=255;g>0;g--){
    acenderLed();
    if (verificacoes()){
      return;
    }
    atraso=checarAtrasoSmooth();
    delay(atraso);
  }
  atraso=checarAtrasoSmooth();
  delay(atraso*2);
  
  for (g=0;g<255;g++){
    b++;
    acenderLed();
    if (verificacoes()){
      return;
    }
    atraso=checarAtrasoSmooth();
    delay(atraso);
  }
  atraso=checarAtrasoSmooth();
  delay(atraso*4);
  
  for (g=255;g>0;g--){
    b--;
    acenderLed();
    if (verificacoes()){
      return;
    }
    atraso=checarAtrasoSmooth();
    delay(atraso);
  }
  atraso=checarAtrasoSmooth();
  delay(atraso*2);
  
  for (b=0;b<255;b++){
    acenderLed();
    if (verificacoes()){
      return;
    }
    atraso=checarAtrasoSmooth();
    delay(atraso);
  }
  atraso=checarAtrasoSmooth();
  delay(atraso*4);
  
  for(b=255;b>0;b--){
    acenderLed();
    if (verificacoes()){
      return;
    }
    atraso=checarAtrasoSmooth();
    delay(atraso);
  }
  atraso=checarAtrasoSmooth();
  delay(atraso*2);
  
  for(b=0;b<255;b++){
    r++;
    acenderLed();
    if (verificacoes()){
      return;
    }
    atraso=checarAtrasoSmooth();
    delay(atraso);
  }
  atraso=checarAtrasoSmooth();
  delay(atraso*4);
  
  for (b=255;b>0;b--){
    r--;
    acenderLed();
    if (verificacoes()){
      return;
    }
    atraso=checarAtrasoSmooth();
    delay(atraso);
  }
  atraso=checarAtrasoSmooth();
  delay(atraso*2);
  
  for(b=0;b<255;b++){
    g++;
    r++;
    acenderLed();
    if (verificacoes()){
      return;
    }
    atraso=checarAtrasoSmooth();
    delay(atraso);
  }
  atraso=checarAtrasoSmooth();
  delay(atraso*4);
  
  for(b=255;b>0;b--){
    g--;
    r--;
    acenderLed();
    if (verificacoes()){
      return;
    }
    atraso=checarAtrasoSmooth();
    delay(atraso);
  }
  atraso=checarAtrasoSmooth();
  delay(atraso*2);
}


bool inicioSmooth;

void smooth(){
  byte atraso=checarAtrasoSmooth();
  
  if (inicioSmooth){
    r=0;
    g=0;
    b=0;
    for (r=0;r<255;r++){
      acenderLed();
      if (verificacoes()){
        return;
      }
      atraso=checarAtrasoSmooth();
      delay(atraso);
    }
    
    inicioSmooth=false;
    
    atraso=checarAtrasoSmooth();
    delay(atraso*4);
  }
  
  //r=255,g=0,b=0
  for (g=0;g<255;g++){
    acenderLed();
    if (verificacoes()){
      return;
    }
    atraso=checarAtrasoSmooth();
    delay(atraso);
  }
  atraso=checarAtrasoSmooth();
  delay(atraso*4);
  
  //r=255,g=255,b=0
  for (r=255;r>0;r--){
    acenderLed();
    if (verificacoes()){
      return;
    }
    atraso=checarAtrasoSmooth();
    delay(atraso);
  }
  atraso=checarAtrasoSmooth();
  delay(atraso*4);
  
  //r=0,g=255,b=0
  for (b=0;b<255;b++){
    acenderLed();
    if (verificacoes()){
      return;
    }
    atraso=checarAtrasoSmooth();
    delay(atraso);
  }
  atraso=checarAtrasoSmooth();
  delay(atraso*4);
  
  //r=0,g=255,b=255
  for(g=255;g>0;g--){
    acenderLed();
    if (verificacoes()){
      return;
    }
    atraso=checarAtrasoSmooth();
    delay(atraso);
  }
  atraso=checarAtrasoSmooth();
  delay(atraso*4);
  
  //r=0,g=0,b=255
  for(r=0;r<255;r++){
    acenderLed();
    if (verificacoes()){
      return;
    }
    atraso=checarAtrasoSmooth();
    delay(atraso);
  }
  atraso=checarAtrasoSmooth();
  delay(atraso*4);
  
  //r=255,g=0,b=255
  for(g=0;g<255;g++){
    acenderLed();
    if (verificacoes()){
      return;
    }
    atraso=checarAtrasoSmooth();
    delay(atraso);    
  }
  atraso=checarAtrasoSmooth;
  delay(atraso*4);
  
  //r=255,g=255,b=255
  for(g=255;g>0;g--){
    b--;
    acenderLed();
    if (verificacoes()){
      return;
    }
    atraso=checarAtrasoSmooth();
    delay(atraso);    
  }
  atraso=checarAtrasoSmooth;
  delay(atraso*4);
}


bool statusStandbyAnterior=false;

void ligar(){
  for (brilho=0;brilho<brilhoAnterior;brilho++){
    acenderLed();
    delay(8);
  }
  acenderLed();
}

void desligar(){
  for(brilho;brilho>0;brilho--){
    acenderLed();
    delay(8);
  }
  acenderLed();
}

void setup(){
  controle.enableIRIn();
  blue.begin(38400);
  processoAtual='C';
  pinMode(7,INPUT);
  pinMode(pinR,OUTPUT);
  pinMode(pinG,OUTPUT);
  pinMode(pinB,OUTPUT);
  pinMode(4,OUTPUT);
  r=0;
  g=255;
  b=75;
  ligar();
}

void loop(){
  
  if (standby){
    if (not statusStandbyAnterior){
      statusStandbyAnterior=true;
      brilhoAnterior=brilho;
      desligar();
      digitalWrite(4,HIGH);
    }
  }
  
  else{
    if (statusStandbyAnterior){
      statusStandbyAnterior=false;
      ligar();
      digitalWrite(4,LOW);
    }
    
    
    if (processoAtual=='C'){
      inicioSmooth=true;
      acenderLed();
    }
    else if (processoAtual=='S'){
      inicioSmooth=true;
      strobe();
    }
    else if (processoAtual=='s'){
      smooth();
    }
    else if (processoAtual=='F'){
      inicioSmooth=true;
      flash();
    }
    else if (processoAtual=='f'){
      inicioSmooth=true;
      fade();
    }
    else if (processoAtual=='e'){
      inicioSmooth=true;
      escurecer();
    }
    else{
      inicioSmooth=true;
      processoAtual='C';
    }
  }
  verificacoes();
}
