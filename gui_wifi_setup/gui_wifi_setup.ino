/*gui_wifi_setup.ino - Code for graphical user interface to setup WiFi on Wio Terminal.
  Created by Yuta Kurota, September 13, 2021.
  Released into the public domain.*/

#include "rpcWiFi.h" //include wifi library
#include "TFT_eSPI.h" //include LCD library
#include "Free_Keybord.h" //include Keybord library

TFT_eSPI tft;
TFT_eSprite spr = TFT_eSprite(&tft);

#define CornflowerBlue 0x64bd // Defining the color of icons
Keybord mykey; // Cleate a keybord


void setup() {
    // Set LCD
    tft.begin();
    tft.setRotation(3);
    tft.fillScreen(TFT_BLACK);

    // Set WiFi to station mode and disconnect from an AP if it was previously connected
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);

    // Set Bottons
    pinMode(WIO_KEY_A, INPUT_PULLUP);
    pinMode(WIO_KEY_B, INPUT_PULLUP);
    pinMode(WIO_KEY_C, INPUT_PULLUP);

    pinMode(WIO_5S_UP, INPUT_PULLUP);
    pinMode(WIO_5S_DOWN, INPUT_PULLUP);
    pinMode(WIO_5S_LEFT, INPUT_PULLUP);
    pinMode(WIO_5S_RIGHT, INPUT_PULLUP);
    pinMode(WIO_5S_PRESS, INPUT_PULLUP);
}
 
void loop() {
    settingMenu("WiFi Setup",24);

    // finding wifi
    int n = scanWiFi();
    String mySSID = getSSIDs(n);
    int len = mySSID.length();
    char charBuf[len+1];
    mySSID.toCharArray(charBuf, len+1);
    const char* ssid = charBuf;
    
    // get passward
    settingMenu("Enter the password",12);
    String myPASSWORD = text_input_5waySwitch(mykey);
    int len2 = myPASSWORD.length();
    char charBuf2[len2+1];
    myPASSWORD.toCharArray(charBuf2, len2+1);
    const char* password = charBuf2;

    // Connecting to WiFi
    settingMenu("WiFi Setup",24);
    tft.setFreeFont(FMB18);
    tft.setTextDatum(MC_DATUM);
    tft.drawString("Connecting to",160,90);
    tft.drawString("WiFi.",160,150);
    WiFi.begin(ssid, password);
    int i = 0;
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        settingMenu("WiFi Setup",24);
        tft.setFreeFont(FMB18);
        tft.setTextDatum(MC_DATUM);
        tft.drawString("Connecting to",160,90);
        if (i<=0){
          tft.drawString("WiFi..",160,150);
        }else if (i==1){
          tft.drawString("WiFi...",160,150);
        }else if (i==2){
          tft.drawString("WiFi....",160,150);
        }else{
          tft.drawString("WiFi.....",160,150);
        }
        WiFi.begin(ssid, password);
        i += 1;
        if (i>=2) {
          settingMenu("WiFi Setup",24);
          tft.setFreeFont(FMB18);
          tft.setTextDatum(MC_DATUM);
          tft.drawString("Unable to connect",160,90);
          tft.drawString("to WiFi.",160,150);
          tft.drawString("Try Again!!",160,150);
          delay(1500);
          errorMenu();
        }
    }

    // Display IP Address on the LCD
    char ip[16];
    sprintf(ip, "IP: %d.%d.%d.%d", WiFi.localIP()[0], WiFi.localIP()[1], WiFi.localIP()[2], WiFi.localIP()[3]);
    settingMenu("WiFi Setup",24);
    tft.setTextDatum(MC_DATUM);
    tft.drawString("Connected!!",160,90);
    tft.setFreeFont(FMB12);
    tft.drawString(String(ip),160,150);
    tft.setTextDatum(TL_DATUM);

    // Wait a bit before scanning again
    delay(10000);
}

// Function for displaying the menu screen
void settingMenu(String title, int sf) {
  tft.fillScreen(TFT_BLACK);
  tft.fillRect(0,0,320,50,TFT_BLUE);
  tft.setTextColor(TFT_WHITE);
  if (sf <= 9){
    tft.setFreeFont(FMB9);
  } else if (sf <= 12){
    tft.setFreeFont(FMB12);
  } else if (sf <= 18){
    tft.setFreeFont(FMB18);
  } else if (sf <= 24){
    tft.setFreeFont(FMB24);
  } else {
    tft.setFreeFont(FMB12);
  }
  tft.setTextDatum(MC_DATUM);
  tft.drawString(title,160,25);
  tft.setTextDatum(TL_DATUM);
}

// Function for displaying the error menu screen
void errorMenu() {
  tft.setRotation(3);
  tft.fillScreen(TFT_BLACK);

  tft.fillRect(0,0,320,50,TFT_RED);

  tft.setTextColor(TFT_WHITE);
  tft.setFreeFont(FMB24);
  tft.setTextDatum(MC_DATUM);
  tft.drawString("ERROR",160,25);

  tft.setFreeFont(FMB18);
  tft.setTextDatum(MC_DATUM);
  tft.drawString("Restart the",160,90);
  tft.drawString("Wio Terminal.",160,150);

  while(1);
}

// Function for scanning available wifi spots
int scanWiFi() {
    tft.setFreeFont(FMB18);
    tft.drawString("Searching",60,90);
    tft.drawString("WiFi...",90,150);
 
    // WiFi.scanNetworks will return the number of networks found
    int n = WiFi.scanNetworks();
    settingMenu("Select WiFi",24);
    if (n == 0) {
        tft.drawString("not found",60,120);
        delay(1000);
        errorMenu();
    } else {
        tft.setCursor(0,80);
        tft.setFreeFont(FSB12); 
        tft.print(n);
        tft.println(" networks found");
    }
    return n;
}

// Function for selecting the wifi spot you want to use.
String getSSIDs(int n){
  if (n == 0) {
    errorMenu();
  } else {
    int i = 1;
    showWiFiInfo(i);
    char page[11];
    sprintf(page, "%03d/%03d", i, n);
    tft.setFreeFont(FSB12);
    tft.setTextColor(TFT_WHITE,TFT_BLACK);
    tft.setTextDatum(BR_DATUM);
    tft.drawString(page,320,240);
    tft.setTextDatum(TL_DATUM);
    while (!(digitalRead(WIO_5S_PRESS) == LOW)) {
      if ((digitalRead(WIO_5S_UP) == LOW)||(digitalRead(WIO_5S_RIGHT) == LOW)||(digitalRead(WIO_KEY_B) == LOW)){
        i = i + 1;
        if (i > n) {
          i = 1;
        }
        showWiFiInfo(i);
        sprintf(page, "%03d/%03d", i, n);
        tft.setFreeFont(FSB12);
        tft.setTextColor(TFT_WHITE,TFT_BLACK);
        tft.setTextDatum(BR_DATUM);
        tft.drawString(page,320,240);
        tft.setTextDatum(TL_DATUM);
        delay(500);
      } else if ((digitalRead(WIO_5S_DOWN) == LOW)||(digitalRead(WIO_5S_LEFT) == LOW)||(digitalRead(WIO_KEY_C) == LOW)){
        i = i - 1;
        if (i <= 0) {
          i = n; 
        }
        showWiFiInfo(i);
        sprintf(page, "%03d/%03d", i, n);
        tft.setFreeFont(FSB12);
        tft.setTextColor(TFT_WHITE,TFT_BLACK);
        tft.setTextDatum(BR_DATUM);
        tft.drawString(page,320,240);
        tft.setTextDatum(TL_DATUM);
        delay(500);
      } else if (digitalRead(WIO_KEY_A) == LOW){
        settingMenu("Select WiFi",24);
        n = scanWiFi();
        i = 1;
        showWiFiInfo(i);
        sprintf(page, "%03d/%03d", i, n);
        tft.setFreeFont(FSB12);
        tft.setTextColor(TFT_WHITE,TFT_BLACK);
        tft.setTextDatum(BR_DATUM);
        tft.drawString(page,320,240);
        tft.setTextDatum(TL_DATUM);
      }
    }
    return WiFi.SSID(i);
  }
}

// Function for displaying information on wifi spots
void showWiFiInfo(int i) {
  spr.createSprite(310, 120); //create buffer
  spr.fillSprite(TFT_BLACK); //fill background color of buffer
  spr.setFreeFont(FSB12); //set font type 
  spr.setTextColor(TFT_WHITE); //set text color
  spr.setCursor(0,0); //set cursor
  // display SSID
  spr.println("");
  spr.print("SSID: ");
  spr.println(WiFi.SSID(i));
  // display RSSI
  spr.print("RSSI: ");
  spr.print(WiFi.RSSI(i));
  spr.println(" dBm");
  // display WiFi Type
  spr.print("Type: ");
  byte encryption = WiFi.encryptionType(i);
  if (encryption == 2){
    spr.println("TKIP (WPA)");
  } else if (encryption == 4){
    spr.println("CCMP (WPA)");
  } else if (encryption == 5){
    spr.println("WEP");
  } else if (encryption == 7){
    spr.println("NONE");
  } else if (encryption == 8){
    spr.println("AUTO");
  } else {
    spr.println(WiFi.encryptionType(i));
  }
  spr.pushSprite(10, 80); //push to LCD 
  spr.deleteSprite(); //clear buffer
}
