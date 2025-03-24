#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Fuzzy.h>

// Constants
#include <DallasTemperature.h>
#include <OneWire.h>

#define ONE_WIRE_BUS 3  

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
#define GAS_SENSOR A0   // Pin analog yang terhubung ke sensor gas MQ-2
const int pinBuzzer = 5;
const int relayPin = 6;
const int pinLed = 2;


// Inisialisasi LCD I2C dengan alamat 0x27
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Variabel global
float hum;    // Menyimpan nilai kelembaban
float temp;   // Menyimpan nilai suhu

Fuzzy *fuzzy = new Fuzzy();

// Membuat Fuzzy Set

// Fuzzy Set Input

// Inisialisai Fuzzy Set Kadar gas
FuzzySet *rendah = new FuzzySet(0, 0, 300, 500);
FuzzySet *medium = new FuzzySet(300, 500, 700, 800);
FuzzySet *tinggi = new FuzzySet(700, 800, 1000, 1000);

// Inisialisai Fuzzy Set Suhu
FuzzySet *normal = new FuzzySet(0, 20, 25, 30);
FuzzySet *hangat = new FuzzySet(25, 30, 30, 35);
FuzzySet *panas = new FuzzySet(30, 35, 100, 100);

// Fuzzy Set Output
FuzzySet *aman = new FuzzySet(0, 0, 25, 50);
FuzzySet *siaga = new FuzzySet(25, 50, 75, 100);
FuzzySet *waspada = new FuzzySet(75, 100, 125, 150);
FuzzySet *bahaya = new FuzzySet(125, 150, 175, 200);



void setup() {

  pinMode(relayPin, OUTPUT);
  pinMode(pinLed, OUTPUT);
  pinMode(pinBuzzer, OUTPUT);
  lcd.begin(16, 2);
  Serial.begin(9600);
  
  // Inisialisasi LCD
  lcd.init();
  lcd.backlight();

  FuzzyInput *kadargas = new FuzzyInput(1);
// Definisi fungsi keanggotaan kadar gas
  kadargas->addFuzzySet(rendah);
  kadargas->addFuzzySet(medium);
  kadargas->addFuzzySet(tinggi);
fuzzy->addFuzzyInput(kadargas);

// definisi fungsi keanggotaan suhu
 FuzzyInput *suhu = new FuzzyInput(2);
   suhu->addFuzzySet(normal);
   suhu->addFuzzySet(hangat);
   suhu->addFuzzySet(panas);
   fuzzy->addFuzzyInput(suhu);

  // Output
    FuzzyOutput *airpump = new FuzzyOutput(1);
  airpump->addFuzzySet(aman);
  airpump->addFuzzySet(siaga);
  airpump->addFuzzySet(waspada);
  airpump->addFuzzySet(bahaya);
   fuzzy->addFuzzyOutput(airpump);

   // Building FuzzyRule 1
  FuzzyRuleAntecedent *ifkadargasRendahAndsuhuNormal = new FuzzyRuleAntecedent();
  ifkadargasRendahAndsuhuNormal->joinWithAND(rendah, normal); 
  FuzzyRuleConsequent *thenAirpumpAman= new FuzzyRuleConsequent();
  thenAirpumpAman->addOutput(aman);
  FuzzyRule *fuzzyRule1 = new FuzzyRule(1, ifkadargasRendahAndsuhuNormal , thenAirpumpAman);
  fuzzy->addFuzzyRule(fuzzyRule1);

  // Building FuzzyRule 2
  FuzzyRuleAntecedent *ifkadargasRendahAndsuhuHangat = new FuzzyRuleAntecedent();
  ifkadargasRendahAndsuhuHangat->joinWithAND(rendah, hangat); 
  //FuzzyRuleConsequent *thenAirpumpAman= new FuzzyRuleConsequent();
  thenAirpumpAman->addOutput(aman);
  FuzzyRule *fuzzyRule2 = new FuzzyRule(2, ifkadargasRendahAndsuhuHangat, thenAirpumpAman);
  fuzzy->addFuzzyRule(fuzzyRule2);

  // Building FuzzyRule 3
  FuzzyRuleAntecedent *ifkadargasRendahAndsuhuPanas = new FuzzyRuleAntecedent();
  ifkadargasRendahAndsuhuPanas->joinWithAND(rendah, panas); 
  FuzzyRuleConsequent *thenAirpumpWaspada= new FuzzyRuleConsequent();
  thenAirpumpWaspada->addOutput(waspada);
  FuzzyRule *fuzzyRule3 = new FuzzyRule(3, ifkadargasRendahAndsuhuPanas, thenAirpumpWaspada);
  fuzzy->addFuzzyRule(fuzzyRule3);

  // Building FuzzyRule 4
  FuzzyRuleAntecedent *ifkadargasMediumAndsuhuNormal = new FuzzyRuleAntecedent();
  ifkadargasMediumAndsuhuNormal ->joinWithAND(medium, normal); 
  FuzzyRuleConsequent *thenAirpumpSiaga= new FuzzyRuleConsequent();
  thenAirpumpSiaga->addOutput(siaga);
  FuzzyRule *fuzzyRule4 = new FuzzyRule(4, ifkadargasMediumAndsuhuNormal , thenAirpumpSiaga);
  fuzzy->addFuzzyRule(fuzzyRule4);

  // Building FuzzyRule 5
  FuzzyRuleAntecedent *ifkadargasMediumAndsuhuHangat = new FuzzyRuleAntecedent();
  ifkadargasMediumAndsuhuHangat ->joinWithAND(medium, hangat); 
  //FuzzyRuleConsequent *thenAirpumpSiaga= new FuzzyRuleConsequent();
  thenAirpumpSiaga->addOutput(siaga);
  FuzzyRule *fuzzyRule5 = new FuzzyRule(5, ifkadargasMediumAndsuhuHangat , thenAirpumpSiaga);
  fuzzy->addFuzzyRule(fuzzyRule5);

  // Building FuzzyRule 6
  FuzzyRuleAntecedent *ifkadargasMediumAndsuhuPanas = new FuzzyRuleAntecedent();
  ifkadargasMediumAndsuhuPanas->joinWithAND(medium, panas); 
  //FuzzyRuleConsequent *thenAirpumpWaspada= new FuzzyRuleConsequent();
  thenAirpumpWaspada->addOutput(waspada);
  FuzzyRule *fuzzyRule6 = new FuzzyRule(6, ifkadargasMediumAndsuhuPanas, thenAirpumpWaspada);
  fuzzy->addFuzzyRule(fuzzyRule6);

  //Building FuzzyRule 7
  FuzzyRuleAntecedent *ifkadargasTinggiAndsuhuNormal = new FuzzyRuleAntecedent();
  ifkadargasTinggiAndsuhuNormal->joinWithAND(tinggi, normal); 
  //FuzzyRuleConsequent *thenAirpumpWaspada= new FuzzyRuleConsequent();
  thenAirpumpWaspada->addOutput(waspada);
  FuzzyRule *fuzzyRule7 = new FuzzyRule(7, ifkadargasTinggiAndsuhuNormal, thenAirpumpWaspada);
  fuzzy->addFuzzyRule(fuzzyRule7);

  //Building FuzzyRule 8
  FuzzyRuleAntecedent *ifkadargasTinggiAndsuhuHangat = new FuzzyRuleAntecedent();
  ifkadargasTinggiAndsuhuHangat->joinWithAND(tinggi, hangat); 
  //FuzzyRuleConsequent *thenAirpumpWaspada= new FuzzyRuleConsequent();
  thenAirpumpWaspada->addOutput(waspada);
  FuzzyRule *fuzzyRule8 = new FuzzyRule(8, ifkadargasTinggiAndsuhuHangat, thenAirpumpWaspada);
  fuzzy->addFuzzyRule(fuzzyRule8);

//Building FuzzyRule 9
  FuzzyRuleAntecedent *ifkadargasTinggiAndsuhuPanas = new FuzzyRuleAntecedent();
  ifkadargasTinggiAndsuhuPanas->joinWithAND(tinggi, panas); 
  FuzzyRuleConsequent *thenAirpumpBahaya= new FuzzyRuleConsequent();
  thenAirpumpBahaya->addOutput(bahaya);
  FuzzyRule *fuzzyRule9 = new FuzzyRule(9, ifkadargasTinggiAndsuhuPanas, thenAirpumpBahaya);
  fuzzy->addFuzzyRule(fuzzyRule9);  
}

void loop(){

  sensors.requestTemperatures();
  temp = sensors.getTempCByIndex(0);
  int gasValue = analogRead(GAS_SENSOR);

  Serial.print("Kadar Gas: ");
  Serial.print(gasValue);
  Serial.print("|| Suhu: ");
  Serial.println(temp);

  fuzzy->setInput(1, gasValue);
  fuzzy->setInput(2, temp);

  fuzzy->fuzzify();

  Serial.print("Kadar Gas: Rendah-> ");
  Serial.print(rendah->getPertinence());
  Serial.print(", Medium-> ");
  Serial.print(medium->getPertinence());
  Serial.print(", Tinggi-> ");
  Serial.println(tinggi->getPertinence());

  Serial.print("Suhu: Normal-> ");
  Serial.print(normal->getPertinence());
  Serial.print(",  Hangat-> ");
  Serial.print(hangat->getPertinence());
  Serial.print(",  Panas-> ");
  Serial.print(panas->getPertinence());

  float output1 = fuzzy->defuzzify(1);

  Serial.println("Output: ");
  Serial.print("Status: Aman-> ");
  Serial.print(aman->getPertinence());
  Serial.print(", Siaga-> ");
  Serial.print(siaga->getPertinence());
  Serial.print(", Waspada-> ");
  Serial.print(waspada->getPertinence());
  Serial.print(", Bahaya-> ");
  Serial.println(bahaya->getPertinence());


   Serial.println("Output: ");
   lcd.clear();
   Serial.print("Status: ");
   Serial.print(output1);
   lcd.setCursor(0, 0);
   lcd.print("Status:");
   lcd.println(output1);
   
  // Mengecek rentang nilai output dan menampilkan indeks sesuai dengan rentangnya
 // Inisialisasi variabel status
String status = "";

if (output1 >= 0 && output1 < 25) {
    status = "Aman";
    
  } else if (output1 >= 25 && output1 < 50) {
    status = "Aman";
    
  } else if (output1 >= 50 && output1 < 75) {
    status = "Siaga";
    
  } else if (output1 >= 75 && output1 < 100) {
    status = "Siaga";
    
  } else if (output1 >= 100 && output1 < 125) {
    status = "Waspada";
    
  } else if (output1 >= 125 && output1 < 150) {
    status = "Waspada";
    
  } else if (output1 >= 150 && output1 < 175) {
    status = "Bahaya";
    
  } else if (output1 >= 175 && output1 <= 200) {
    status = "Bahaya";
    
}

 // Mengendalikan relay, LED, dan buzzer sesuai dengan status
  if (status == "Aman") {
    // Pastikan relay, LED, dan buzzer dimatikan
    digitalWrite(relayPin, HIGH);
    digitalWrite(pinLed, LOW);
    digitalWrite(pinBuzzer, LOW);
  } else if (status == "Siaga") {
    // Aktifkan relay, mungkin aktifkan LED, pastikan buzzer dimatikan
    digitalWrite(relayPin, LOW);
    digitalWrite(pinLed, LOW);
    digitalWrite(pinBuzzer, LOW);
  } else if (status == "Waspada") {
    // Aktifkan relay, aktifkan LED, pastikan buzzer dimatikan
    digitalWrite(relayPin, LOW);
    digitalWrite(pinLed, HIGH);
    digitalWrite(pinBuzzer, LOW);
  } else if (status == "Bahaya") {
    // Aktifkan relay, aktifkan LED, aktifkan buzzer
    digitalWrite(relayPin, LOW);
    digitalWrite(pinLed, HIGH);
    digitalWrite(pinBuzzer, HIGH);
  }

// Tampilkan status pada Serial Monitor dan LCD
Serial.print("Status: ");
Serial.println(status);
lcd.setCursor(0, 1);
lcd.print("Status: ");
lcd.print(status);

delay(1000);


}
