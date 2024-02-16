#include "Wire.h"  // I2C iletişim protokolü kütüphanesi
#include <LiquidCrystal_I2C.h>
#include "MQ131.h"
LiquidCrystal_I2C lcd(0x27, 16, 2);
const int AOUTpin = A3;               // CO Analog çıkış pini
const int informationBuzz = 1500;     // buzz for information pitch
const int warningBuzz = 1000;         // buzz for warning pitch
const float ozonTreshHold = 50.0;     // treshhold for ozon
const float carbonMTreshHold = 50.0;  // treshhold for ozon
const float midalert = 30.0;
const int DOUTpin = 7;                // CO Dijital çıkış pini
int esik;                             // CO Eşik değeri
int coPpm;                            // Ölçülen CO ppm değeri
int buz = 9;                          //buzzer connected to pin 8
bool isBuzzing = false;               // To check if the buzzer is buzzing
void setup() {
  lcd.init();
  lcd.setCursor(0, 0);  //lcd'yi başlatıyoruz
  lcd.backlight();      //lcd arka ışığını açıyoruz.
  lcd.print("CIHAZ BASLIYOR...");
  lcd.setCursor(0, 1);
  lcd.print("BEKLEYINIZ");
  pinMode(DOUTpin, INPUT);  //CO
  pinMode(buz, OUTPUT);     // buzzer is connected as Output from Arduino
  Serial.begin(115200);
  lcd.setCursor(0, 1);
  // TEST FOR BUZZER WHEN INIT LOADING
  // Can make a func for buzzing and stopping TODO
  tone(buz, informationBuzz);  // Send 1KHz sound signal...
  delay(250);                  // ...for 0.5 sec
  noTone(buz);                 // Stop sound...
  delay(250);
  tone(buz, informationBuzz);  // Send 1KHz sound signal...
  delay(250);
  noTone(buz);  // Stop sound...
  delay(250);
  tone(buz, informationBuzz);  // Send 1KHz sound signal...
  delay(250);
  noTone(buz);  // Stop sound...
  delay(250);

  //    OZON SENSÖRÜ
  // Init the sensor
  // - Heater control on pin 2
  // - Sensor analog read on pin A1
  // - Model LOW_CONCENTRATION
  // - Load resistance RL of 1MOhms (1000000 Ohms)
  MQ131.begin(2, A1, LOW_CONCENTRATION, 1000000);

  Serial.println("Calibration in progress...");

  MQ131.calibrate();

  Serial.println("Calibration done!");
  Serial.print("R0 = ");
  Serial.print(MQ131.getR0());
  Serial.println(" Ohms");
  Serial.print("Time to heat = ");
  Serial.print(MQ131.getTimeToRead());
  Serial.println(" s");
}

void loop() {
  
  // Eşik değerinin aşılıp aşılmadığını kontrol et ve seri porta yaz
  // TODO Should delete this if this is unrelevant
  // if (esik == 1) {
  //   Serial.println("Eşik değerinin altında");  //CO
  // } else {
  //   Serial.println("Eşik değerinin üzerinde");  //CO
  // }

  //delay(500);  // CO Yarım saniye bekle
  Serial.println("Sampling...");
  MQ131.sample();
  Serial.print("Concentration O3 : ");
  Serial.print(MQ131.getO3(PPM));
  Serial.println(" ppm");
  Serial.print("Concentration O3 : ");
  Serial.print(MQ131.getO3(PPB));
  Serial.println(" ppb");
  Serial.print("Concentration O3 : ");
  Serial.print(MQ131.getO3(MG_M3));
  Serial.println(" mg/m3");
  Serial.print("Concentration O3 : ");
  Serial.print(MQ131.getO3(UG_M3));
  Serial.println(" ug/m3");

  float coRead = analogRead(AOUTpin);  // CO ppm değerini Analog pinden oku
  float coPpm = 0.0041 * coRead + 0.2798;
  esik = digitalRead(DOUTpin);  // CO eşik değerinin aşılıp aşılmadığını oku
  Serial.print(" CO miktarı: ");
  Serial.print(coPpm);   //CO miktarını yaz
  Serial.print("ppm.");  // CO

  // printing values to screen
  PrintValuesToScreen(coPpm, MQ131.getO3(PPM));
   if (coPpm >= midalert || MQ131.getO3(PPM) >= midalert) {
    tone(buz, warningBuzz);
    Serial.print("Warning CarbonMonoxide or ozon amount is 30 PPm!");

    isBuzzing = true;
  } else if (isBuzzing) {
    noTone(buz);  // Stop sound...
    isBuzzing = false;
  }
  // checks for treshold to buzz if needed
  if (coPpm >= carbonMTreshHold || MQ131.getO3(PPM) >= ozonTreshHold) {
    tone(buz, warningBuzz);
    Serial.print("Warning CarbonMonoxide or ozon amount is above treshhold!");

    isBuzzing = true;
  } else if (isBuzzing) {
    noTone(buz);  // Stop sound...
    isBuzzing = false;
  }
 // delay(5000);
}

void PrintValuesToScreen(float coPpm, float ozon) {
  lcd.clear();
  
  // for carbonM
  lcd.setCursor(0, 0);
  lcd.print("CO : ");
  lcd.print(String(coPpm));
  lcd.print(" ppm");
  // For ozon
  lcd.setCursor(0, 1);
  lcd.print("Ozon : ");
  lcd.print(String(ozon));
  lcd.print(" ppm");
}
