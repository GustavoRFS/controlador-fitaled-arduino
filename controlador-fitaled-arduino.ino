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
  delay(100);
  char charAtual=Serial.read();
  String numero="";
  while (charAtual!=' '){
    delay(100);
    numero+=charAtual;
    charAtual=Serial.read();
  }

  Serial.println(numero);
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
  char byteRecebido=Serial.read();
  Serial.println(byteRecebido);
  if (byteRecebido=='C'){
    Serial.read();
    r=receberInt();
    g=receberInt();
    b=receberInt();
  }
  
  //LIXO: CASO UM COMANDO SEJA ADICIONADO, ADICIONAR AQUI, CASO CONTRARIO O COMANDO NÃO FUNCIONARÁ
  else if (byteRecebido!='v' and byteRecebido!='V' and byteRecebido!='b' and byteRecebido!='B' and byteRecebido!='P' and byteRecebido!='s' and byteRecebido!='f' and byteRecebido!='S' and byteRecebido!='F' and byteRecebido!='e'){
    byteRecebido=processoAtual;
  }
  
  while(Serial.available()){
    char lixo=Serial.read();
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
  if (Serial.available()){
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
  atraso=checarAtrasoSmooth();
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
  // controle.enableIRIn();
  Serial.begin(2000000);
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
