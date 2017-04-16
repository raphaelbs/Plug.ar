import processing.serial.*;

String x, y, stat;
boolean isX, isS;
Cursor cursor;
Serial port;

void setup() {
  size(480, 480);
  frameRate(30);
  cursor = new Cursor();
  x = ""; y = ""; stat = ""; isS = true; isX = true;
  port = new Serial(this, Serial.list()[1], 9600); 
  while(port.available()!=0);
}

void draw() {  
} 

void serialEvent(Serial port) 
{
  int r = port.read();
  if(r == 10){
    x = x.trim(); y = y.trim(); stat = (stat = stat.trim()).substring(stat.length()-3, stat.length());
    cursor.move(stat, parseInt(x), parseInt(y));
    x = ""; y = ""; stat = ""; isS = true; isX = true;
  }else{
    if((char)r == ','){
      if(isS)
        isS = false;
      else
        isX = false;
    }else{
      if(isS){
        stat += (char)r;
      }else{
        if(isX){
          x += (char)r;
        }else{
          y += (char)r;
        }
      }
    }
  }
}

class Cursor
{
  int x, y, size;
  Cursor(){
    this.size = 20;
    this.y = height/2 - 10;
    this.x = width/2 - 10;
  }
  
  void move(String stat, int x, int y)
  {
    interpreter(stat);
    if((this.x <= 0 && x < 0) || (this.x >= width && x > 0))
      this.x = 0;
    else
      this.x += x;
    if((this.y <= 0 && y < 0) || (this.y >= height && y > 0))
      this.y = 0;
    else
      this.y -= y;
    display();
  }
  
  void interpreter(String stat){
    if(stat.charAt(2) == '0'){
      background(255);
    }
  }
  
  void display()
  {
    stroke(255,0,0); 
    fill(255,0,0);
    ellipse(x, y, size, size); 
  }
}
