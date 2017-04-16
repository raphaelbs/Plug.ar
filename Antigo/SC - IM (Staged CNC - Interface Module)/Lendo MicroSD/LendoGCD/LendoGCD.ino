/*
  SD card test

 This example shows how use the utility libraries on which the'
 SD library is based in order to get info about your SD card.
 Very useful for testing a card when you're not sure whether its working or not.

 The circuit:
  * SD card attached to SPI bus as follows:
 ** MOSI - pin 11 on Arduino Uno/Duemilanove/Diecimila
 ** MISO - pin 12 on Arduino Uno/Duemilanove/Diecimila
 ** CLK - pin 13 on Arduino Uno/Duemilanove/Diecimila
 ** CS - depends on your SD card shield or module.
     Pin 4 used here for consistency with other Arduino examples
*/

// include the SD library:
#include <SPI.h>
#include <SD.h>

// CS do SD
const int chipSelect = 10;
// Arquivo aberto no cartao
File file;
// Estado do cartão de memória (carregado ou não)
bool isSDCard = false;
// Estado do arquivo (aberto ou não)
bool fileOpen;
// Próximo comando será o nome do arquivo
bool isFileName = false;
// Posição do começo da linha anterior no arquivo
unsigned long prevLine;

String readStringFromSerial(char scapedChar);

void setup() {
  // Inicia o Serial
  Serial.begin(9600);
  // Aguarda o serial iniciar..
  while (!Serial) { }

  Serial.println(F("BEM VINDO AO SC-IM (Staged CNC - Interface Module)"));
  Serial.println(F("Use o comando 'help.' para ajuda.\nOBS: Todos comandos devem terminar com '.'\n"));
}

void loop() {
  if(Serial.available()){
    // lê uma string do serial e pára no parâmetro
    String serialString = readStringFromSerial('.');

    // Envia o comando para o interpretador
    interpretator(serialString);
  }
}

/**
 * Interpreta os comandos vindos do serial.
 */
void interpretator(String& command){

  // Monta o cartao SD
  if(command.equals("mountSD")){
    // Inicia a conexão com cartão SD
    if (SD.begin(chipSelect)) {
      Serial.println(F("[c:mountSD] Cartao SD inicializado com sucesso!"));
    } else {
      Serial.println(F("[c:mountSD] Falha ao abrir o cartao SD, confira as portas, o numero do chipSelect, a formatacao.."));
    }
    isSDCard = true;
    return;
  }

  if(command.equals("unmountSD")){
    isSDCard = false;
    return;
  }
  
  // Abre o menu de ajuda
  if(command.equals("help")){
    help();
    return;
  }
  
  // Lista todos arquivos do cartao com data e tamanho
  if(command.equals("list")){
    Sd2Card card;
    SdVolume volume;
    SdFile root;
    Serial.println(F("[c:list] Listagem do diretorio:\nARQUIVO / DATA / TAMANHO "));
    if(card.init(SPI_HALF_SPEED, chipSelect)){
      if(volume.init(card)){
        if(root.openRoot(volume)){
          root.ls(LS_R | LS_DATE | LS_SIZE);
        }else{
          Serial.println(F("[erro] nao foi possivel abrir o diretorio!"));
        }
      }else{
        Serial.println(F("[erro] nao foi possivel abrir o volume dos dados!"));
      }
    }else{
      Serial.println(F("[erro] nao foi possivel carregar o cartao!"));
    }
    root.close();
    return;
  }

  

  if(isSDCard){
    if(isFileName){
      // Abre o arquivo
      String fileName = command + ".GCD";
      Serial.print(F("[log] tentando abrir o arquivo "));
      Serial.println(fileName);
      file = SD.open(fileName);
      fileOpen = file.available();
      isFileName = false;
    
      if(fileOpen)
        Serial.println(F("[sucesso] Arquivo aberto!\n"));
      else
        Serial.println(F("[erro] O nome do arquivo esta correto?\nUse o comando 'list' para listar o diretorio.\n"));
      return;
    }
    
    // Abre arquivo
    if(command.equals("open")){
      isFileName = true;
      Serial.println(F("[c:open] Informe agora o nome do arquivo (extensao .GCD):"));
      return;
    }
  
    // Fecha arquivo
    if(command.equals("close")){
      isFileName = false;
      fileOpen = false;
      file.close();
      return;
    }

    // Se o arquivo estiver aberto..
    if(fileOpen){
      // Lê a próxima letra do arquivo SEM mover o ponteiro.
      if(command.equals("peek")){
        Serial.print(F("[c:peek] '"));
        Serial.write(file.peek());
        Serial.print(F("' -> "));
        Serial.println(file.peek());
        return;
      }
      
      // Lê a próxima linha e move o ponteiro.
      if(command.equals("nextLine")){
        Serial.print(F("[c:nextLine] "));
        String line = nextLine();
        Serial.println(line);
        return;
      }
      
      // Lê a linha anterior. O ponteiro fica igual.
      if(command.equals("repeatLine")){
        Serial.print(F("[c:repeatLine] "));
        String line = repeatLine();
        Serial.println(line);
        return;
      }
      
      // Lê o próximo GCODE e move o ponteiro.
      // Ignora comentarios e linhas vazias.
      if(command.equals("nextGCODE")){
        Serial.print(F("[c:nextGCODE] "));
        String line = nextGCODE();
        Serial.println(line);
        return;
      }
    }
  }
  
  Serial.print(F("[erro] Comando '"));
  Serial.print(command);
  Serial.println(F("' nao existe ou nao esta disponivel.\n       Consulte a hiearquia pelo comando 'list'"));
}



/**
 * Lê o próximo GCODE do arquivo e move o ponteiro para a próxima linha.
 */
String nextGCODE(){
  prevLine = file.position();
  String line = "";
  char chr;
  bool findComment = false;
  while((chr = file.read()) != '\n'){
    if(!findComment){
      if(chr == ';')
          findComment = true;
      else{
          if(chr != 13)
            line += chr;
      }
    }    
  }
  if(line.equals(""))
    return nextGCODE();
  return line;
}

/**
 * Lê a linha e move o ponteiro para a próxima.
 */
String nextLine(){
  prevLine = file.position();
  String line = "";
  char chr;
  while((chr = file.read()) != '\n'){
    line += chr;
  }
  return line;
}

/**
 * Lê a linha anterior e mantêm o ponteiro na mesma posição.
 */
String repeatLine(){
  file.seek(prevLine);
  String line = "";
  char chr;
  while((chr = file.read()) != '\n'){
    line += chr;
  }
  return line;
}

/**
 * Cria um buffer para o Serial e retorna em String.
 */
String readStringFromSerial(char scapedChar){
  String serialString = "";
  char serialChar;
  while((serialChar = Serial.read()) != '.'){
    if((int)serialChar != -1)
      serialString += serialChar;
  }
  return serialString;
}

void StrToCharArray(){
  
}

/**
 * Imprime uma string via Serial sem soltar linha.
 */
void printString(String& str){
  if(str != ""){
    char* buff = (char*) malloc(sizeof(char)*str.length()+1);
    str.toCharArray(buff, str.length()+1);
    Serial.print(str);
    free(buff);
  }
}

/**
 * Imprime uma string via Serial soltando uma linha.
 */
void printlnString(String& str){
  printString(str);
  Serial.println("");
}


/**
 * Cria menu de ajuda
 */
void help(){
  Serial.println(F("\n[c:help] Bem vindo ao menu de ajuda!"));
  Serial.println(F("| ~ Todos os comandos devem ser seguidos por '.'"));
  Serial.println(F("| ~ Exemplo: para chamar este menu use 'help.'"));
  Serial.println(F("|- Hierarquia de comando:"));
  Serial.println(F("|-[list] -> Lista todos arquivos do cartao com data e tamanho (auto-monta)"));
  Serial.println(F("|-[help] -> Abre o menu de ajuda"));
  Serial.println(F("|-[mountSD] -> Monta o cartao SD"));
  Serial.println(F("|   |-[open] -> Abre arquivo"));
  Serial.println(F("|   |   |-[peek] -> Le a proxima letra do arquivo SEM mover o ponteiro."));
  Serial.println(F("|   |   |-[nextLine] -> Le a proxima linha e move o ponteiro."));
  Serial.println(F("|   |   |-[repeatLine] -> Le a linha anterior. O ponteiro permanece no mesmo local."));
  Serial.println(F("|   |   |-[nextGCODE] -> Le o proximo GCODE e move o ponteiro. Ignora comentarios e linhas vazias."));
  Serial.println(F("|   |-[close] -> Fecha arquivo"));
  
  
  Serial.println(F("\\[]\n"));
}

