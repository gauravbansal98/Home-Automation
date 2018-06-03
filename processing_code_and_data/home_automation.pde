import processing.serial.*;
import processing.video.*;
int prev=0;
Serial port;
Capture video;
PImage prevImage;

PImage room;
PImage button;
PImage buzzer;
PImage clock;

PImage camera;
boolean cameraState=false;

PImage[] fan=new PImage[8];
boolean fanState=false;

PImage[] wallBulb=new PImage[2];
boolean wallbulbState=false;

PImage welcome;
boolean welcomeState=true;

PImage door;
boolean doorState=false;

PImage bulb;
boolean rightbulbState=false;
boolean leftbulbState=false;

int i=0;
int Time=0;
String comingDaata;
float r1,g1,b1,r2,g2,b2;

void setup()
{
  size(1200,650);
  println(Serial.list());
  port=new Serial(this,Serial.list()[2],9600);
  port.bufferUntil('\n');
  String[] temp=loadStrings("temperature.txt");
  video=new Capture(this,640,480,30);
  video.start();
  prevImage=createImage(640,480,RGB);
  for(i=0;i<8;i++)
  {
    fan[i]=loadImage("fan"+i+".jpg");
  }
  for(i=0;i<2;i++)
  {
    wallBulb[i]=loadImage("wallbulb"+i+".png");
  }
  room=loadImage("room.jpg");
  clock=loadImage("clock.jpg");
  welcome=loadImage("Welcome.png");
  door=loadImage("door.png");
  buzzer=loadImage("buzzer.png");
  button=loadImage("switch.png");
  bulb=loadImage("bulb.png");
  camera=loadImage("camera.png");
 //while(true)
  //{
    if(video.available())
    {
      video.read();
    //  break;
    }
 // port.write('w');
 // }
}

void draw()
{
  staticThings();
  cameraState();
  fanState();
  wallbulbState();
  welcomeState();
  leftbulbState();
  rightbulbState();
  doorState();
  motionDetector();
}

void staticThings()
{
  background(255);
  image(room,0,0,1200,650);
  image(clock,522,166,70,70);
  image(buzzer,238,130,50,50);
  image(button,235,235,20,20);
  image(button,255,235,20,20);
  image(button,275,235,20,20);
  image(button,295,235,20,20);
  image(camera,1060,90,30,30);
}

void captureEvent(Capture video)
{
  prevImage.loadPixels();
  video.loadPixels();
  for(int i=0;i<video.width;i++)
  {
    for(int j=0;j<video.height;j++)
    prevImage.pixels[i+video.width*j]=video.pixels[i+video.width*j];
  }
  prevImage.updatePixels();
  video.read();
}

void cameraState()
{
  if(cameraState)
  image(video,318,169,148,71);
}

void wallbulbState()
{
  if(wallbulbState)
    {
      image(wallBulb[1],891,100,50,50);
    }
    else
      image(wallBulb[0],900,120,50,50);
}
  

void fanState()
{
  if(fanState)
    {
      image(fan[i],470,20,140,70);
      if(i==7)
      i=0;
      i++;
    }
  else
    image(fan[i],470,20,140,70);
}

void welcomeState()
{
  if(welcomeState)
    {
      image(welcome,686,184,150,60);
    }
}

void rightbulbState()
{
  if(rightbulbState)
    {
       image(bulb,160,143,90,90);
    }
}

void leftbulbState()
{
  if(leftbulbState)
    {
       image(bulb,565,225,70,70);
    }
}

void doorState()
{
  if(doorState)
    {
       image(door,1018,138,135,215);
    }
}

void mousePressed()
{
  if(mouseX > 1018 && mouseX < 1150 && mouseY > 138 && mouseY < 353)
    {
      if(doorState)
        {
          port.write('k');
        }
      else
        port.write('j');
       doorState=!doorState;
    }
   else if(mouseX > 235 && mouseX < 255 && mouseY >235 && mouseY < 255)
     {
       if(leftbulbState)
         {
           port.write('x');
         }
       else
         port.write('z');
       leftbulbState=!leftbulbState;
     }
    else if(mouseX > 255 && mouseX < 275 && mouseY > 235 && mouseY < 255 )
      {
        if(rightbulbState)
          {
            port.write('v');
          }
        else
          port.write('c');
        rightbulbState=!rightbulbState;
      }
    else if(mouseX > 275 && mouseX < 295 && mouseY > 235 && mouseY < 255 )
      {
        if(fanState)
          {
            port.write('n');
          }
        else
          port.write('b');
        fanState=!fanState;
      }
    else if(mouseX > 295 && mouseX < 315 && mouseY > 235 && mouseY < 255 )
      {
        if(wallbulbState)
          {
            port.write('l');
          }
        else
          port.write('m');
        wallbulbState=!wallbulbState;
      }
      else if(mouseX > 1060 && mouseX < 1090 && mouseY >90 && mouseY < 120 )
      {
        cameraState=!cameraState;
      }
}  
    
    
 void keyPressed()
 {
   if(key=='1')
   {
       if(leftbulbState)
         {
           port.write('x');
         }
       else
         port.write('z');
       leftbulbState=!leftbulbState;
   }
   else if(key=='2') 
     {
        if(rightbulbState)
          {
            port.write('v');
          }
        else
          port.write('c');
        rightbulbState=!rightbulbState;
    }
   else if(key=='3')
   {
        if(fanState)
          {
            port.write('n');
          }
        else
          port.write('b');
        fanState=!fanState;
   }
   else if(key=='4')
   {
        if(wallbulbState)
          {
            port.write('l');
          }
        else
          port.write('m');
        wallbulbState=!wallbulbState;
   }
   else if(key=='d')
   {
      if(doorState)
        {
         port.write('k');
        }
      else
         port.write('j');
      doorState=!doorState;
   }
   else if(key=='5')
   cameraState=!cameraState;
   else if(key=='p')
   {
     println("gaurav");
     image(video,0,0,width,height);
     saveFrame("gaurav.jpg");
     port.write('p');
   }
 }

  String[] sss=new String[100000];
 int t=0;
 
 void serialEvent(Serial port)
{
  comingDaata=port.readStringUntil('\n');
   println(comingDaata);
   char comingData=comingDaata.charAt(0);
   if(comingData=='2')
   {
   prev=0;
   }
   if(comingData=='2'||prev<3)
   {
     if(prev==0)
     {
       prev++;
       return;
     }
     int hour=hour(),minute=minute(),sec=second(),day=day(),month=month();
     if(prev==1)
     sss[t++]="month="+month+" day="+day+" time="+hour+"-"+minute+"-"+sec+"  temperature"+"="+comingDaata;
     else
     sss[t++]="month="+month+" day="+day+" time="+hour+"-"+minute+"-"+sec+"  humidity"+"="+comingDaata;
     String[] ssss=sss;
     saveStrings("temperature.txt",ssss);
     prev++;
     return;
   }
  
  delay(5);
  if(comingData=='z')
  {
    leftbulbState=true;
  }
  else if(comingData=='x')
  {
    leftbulbState=false;
  }
  else if(comingData=='c')
  {
    rightbulbState=true;
  }
  else if(comingData=='v')
  {
    rightbulbState=false;
  }
  else if(comingData=='b')
  {
    fanState=true;
  }
  else if(comingData=='n')
  {
    fanState=false;
  }
  else if(comingData=='m')
  {
    wallbulbState=true;
  }
  else if(comingData=='l')
  {
    wallbulbState=false;
  }
  else if(comingData=='k')
  {
    doorState=false;
    welcomeState=true;
  }
  else if(comingData=='j')
  {
    welcomeState=false;
    doorState=true;
  }
  else if (comingData=='0')
  {
    saveFrame("roomState.jpg");
    port.write('o');
  }
  else if(comingData=='1')
  {
     image(video,318,169,148,71);
    saveFrame("Guest.jpg");
    port.write('1');
  }
}

void motionDetector()
{
  int count=0;
  video.loadPixels();
  prevImage.loadPixels();
  for(int i=0;i<video.width;i++)
  {
    for(int j=0;j<video.height;j++)
    {
      int loc=i+video.width*j;
      color currentcolor=video.pixels[loc];
      r1=red(currentcolor);
      g1=green(currentcolor);
      b1=blue(currentcolor);
      color prevcolor=prevImage.pixels[loc];
      r2=red(prevcolor);
      b2=blue(prevcolor);
      g2=green(prevcolor);
      
      float d=dist(r1,g1,b1,r2,g2,b2);
      if(d>50)
      count++;
      if(count>5000)
      {
       // port.write('p');
       // saveFrame("gaurav.jpg");
      //  println("hi");
        return;
      }
    }
  }
}
