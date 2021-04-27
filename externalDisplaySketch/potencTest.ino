#include <SoftwareSerial.h>
#include <LiquidCrystal_I2C.h>

#define PACKEGE_SIZE 8
#define PIN_POT     A0

LiquidCrystal_I2C lcd(0x27,16,2);
byte packege[PACKEGE_SIZE];

bool start = false;

String pages[10] = {"CPU Load: ", "CPU Temp: ", 
                    "GPU Temp: ", "RAM Load: ",
                    "HDD1 Temp: ", "HDD2 Temp: ",
                    "HDD3 Temp: ", "HDD4 Temp: "};

String cpuLoad;
String cpuTemp;

String ramLoad;
String gpuTemp;

String hdd1Temp;
String hdd2Temp;

String hdd3Temp;
String hdd4Temp;

void setup()
{    
    Serial.begin(9600); 
    
    lcd.init();                     
    lcd.backlight();// Включаем подсветку дисплея
    lcd.print("System-monitor");   
    digitalWrite(9, LOW);  
}
void loop()
{
    String msg = "";      

    if (Serial.available() >= PACKEGE_SIZE)
    {
        int inBuff;
        inBuff = Serial.readBytes(packege, PACKEGE_SIZE);        
        start = true;
        lcd.clear();  
         digitalWrite(9, LOW);            
                
         cpuLoad = pages[0] + (int)packege[0];
         cpuTemp = pages[1] + (int)packege[1];

         gpuTemp = pages[2] + (int)packege[2];
         ramLoad= pages[3] + (int)packege[3];

         hdd1Temp = pages[4] + (int)packege[4];
         hdd2Temp = pages[5] + (int)packege[5];

         hdd3Temp = pages[6] + (int)packege[6];
         hdd4Temp = pages[7] + (int)packege[7];


         int rotate{0}, value{0};
        
         rotate = analogRead(PIN_POT);
          
         if(rotate >= 0 && rotate <= 100){
                  lcd.clear(); 
                  lcd.setCursor(0, 0);
                  lcd.print(cpuLoad);    
                  lcd.setCursor(0, 1);
                  lcd.print(cpuTemp);                   
                               
                }
              else if(rotate > 100 && rotate <= 200 ){
                  lcd.clear(); 
                  lcd.setCursor(0, 0);
                  lcd.print(ramLoad);    
                  lcd.setCursor(0, 1);
                  lcd.print(gpuTemp);    
                          
                }
              else if(rotate > 200 && rotate <= 300){
                lcd.clear(); 
                  lcd.setCursor(0, 0);
                  lcd.print(hdd1Temp);    
                  lcd.setCursor(0, 1);
                  lcd.print(hdd2Temp);   
                 digitalWrite(9, HIGH);    
                }
              else if(rotate > 300 && rotate <= 400){
                lcd.clear(); 
                  lcd.setCursor(0, 0);
                  lcd.print(hdd3Temp);    
                  lcd.setCursor(0, 1);
                  lcd.print(hdd4Temp);                
                }
              else if(rotate > 400 && rotate <= 500){
                 lcd.clear(); 
                  lcd.setCursor(0, 0);
                  lcd.print("out");
                }
              else if(rotate > 500 && rotate <= 600){
                lcd.clear(); 
                  lcd.setCursor(0, 0);
                  lcd.print("out");
                }
              else if(rotate > 600 && rotate <= 700){
                lcd.clear(); 
                  lcd.setCursor(0, 0);
                  lcd.print("out");
                }
              else if(rotate > 700 && rotate <= 800){
                lcd.clear(); 
                  lcd.setCursor(0, 0);
                  lcd.print("out");
                }
              else if(rotate > 800 && rotate <= 900){
                lcd.clear(); 
                  lcd.setCursor(0, 0);
                  lcd.print("out");
                }
                else if(rotate > 900 && rotate <= 1023){
                  lcd.clear(); 
                  lcd.setCursor(0, 0);
                  lcd.print("out");
             }    
       
        }  
       
}
