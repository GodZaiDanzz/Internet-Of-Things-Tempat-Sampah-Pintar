#define BLYNK_TEMPLATE_ID "TMPL6H3fScB2D"
#define BLYNK_TEMPLATE_NAME "TRASHIOT"
#define BLYNK_AUTH_TOKEN "9xzmKUuIZ9aXAAL0tWTH9nXMeoaJTRel"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ESP32Servo.h>

//================ KREDENSIAL WIFI =================
// !!! PASTIKAN WI-FI / HOTSPOT INI BERFREKUENSI 2.4 GHz !!!
char ssid[] = "punyague"; 
char pass[] = "punyagua"; 

//================ PIN =================
#define TRIG1 18
#define ECHO1 19
#define TRIG2 25
#define ECHO2 26
#define SERVO_PIN 27

LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo tutupTong;

const int tinggiTong = 30;
const int tutup = 0;
const int buka = 90;
bool sedangTerbuka = false;

float bacaJarak(int trigPin, int echoPin) {
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    long durasi = pulseIn(echoPin, HIGH, 30000);
    if(durasi == 0) {
        return tinggiTong;
    }
    return durasi * 0.0343 / 2;
}

BLYNK_WRITE(V2) {
   int tombol = param.asInt();
   if(tombol == 1 && !sedangTerbuka) {
      sedangTerbuka = true;
      tutupTong.write(buka);
      delay(3000);
      tutupTong.write(tutup);
      delay(500);
      sedangTerbuka = false;
   }
}

void setup() {
    Serial.begin(115200);
    delay(1000); // Jeda stabilisasi serial

    pinMode(TRIG1, OUTPUT);
    pinMode(ECHO1, INPUT);
    pinMode(TRIG2, OUTPUT);
    pinMode(ECHO2, INPUT);

    lcd.begin();
    lcd.backlight();

    tutupTong.attach(SERVO_PIN);
    tutupTong.write(tutup);

    lcd.setCursor(0,0);
    lcd.print("Connect Wi-Fi...");
    Serial.println("Memulai koneksi Wi-Fi...");

    // Memulai koneksi ke Blynk dan WiFi
    Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

    // Kode di bawah ini hanya akan jalan JIKA Blynk.begin() BERHASIL Konek
    Serial.println("Wi-Fi & Blynk Terhubung Sukses!");
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Smart Trash");
    lcd.setCursor(0,1);
    lcd.print("Starting...");
    delay(2000);
    lcd.clear();
}

void loop() {
    Blynk.run(); 

    float jarakOrang = bacaJarak(TRIG1,ECHO1);
    float jarakSampah = bacaJarak(TRIG2,ECHO2);

    Serial.print("Orang: ");
    Serial.print(jarakOrang);
    Serial.print(" cm | Sampah: ");
    Serial.print(jarakSampah);
    Serial.println(" cm");

    if(jarakOrang < 20 && !sedangTerbuka) {
        sedangTerbuka=true;
        tutupTong.write(buka);
        delay(3000);
        tutupTong.write(tutup);
        delay(500);
        sedangTerbuka=false;
    }

    int persen = ((tinggiTong - jarakSampah) * 100) / tinggiTong;
    persen = constrain(persen,0,100);

    lcd.setCursor(0,0);
    lcd.print("Sampah:      ");
    lcd.setCursor(9,0);
    lcd.print(persen);
    lcd.print("% ");

    lcd.setCursor(0,1);
    lcd.print("                ");
    lcd.setCursor(0,1);

    String statusBlynk = "";
    if(persen < 50) {
        lcd.print("Kosong");
        statusBlynk = "Kosong";
    } else if(persen < 80) {
        lcd.print("Hampir Full");
        statusBlynk = "Hampir Full";
    } else {
        lcd.print("FULL");
        statusBlynk = "FULL";
    }

    Blynk.virtualWrite(V0, persen);
    Blynk.virtualWrite(V1, statusBlynk);

    delay(500);
}