
#include "esp_camera.h"
#include <WiFi.h>
#include<WiFiClient.h>
#include<BlynkSimpleEsp32.h>
//
// WARNING!!! PSRAM IC required for UXGA resolution and high JPEG quality
//            Ensure ESP32 Wrover Module or other board with PSRAM is selected
//            Partial images will be transmitted if image exceeds buffer size
//

// Select camera model
//#define CAMERA_MODEL_WROVER_KIT
//#define CAMERA_MODEL_ESP_EYE
//#define CAMERA_MODEL_M5STACK_PSRAM
//#define CAMERA_MODEL_M5STACK_WIDE
#define CAMERA_MODEL_AI_THINKER

#include "camera_pins.h"
#include "HTTPClient.h"
#define LED 13
#define PHOTO 14
#define LOCK 12
#define IR 2
#define IN_IR 15 

const char* ssid = "POCO PHONE"; //WiFi SSID
const char* password = "kkrocks2"; //WiFi Password
char auth[] = "VxB6VScoYUUQ8ehqQ5ezZWjjQr2lF0TH";


const char *root_ca =
"-----BEGIN CERTIFICATE-----\n" \
"MIIDujCCAqKgAwIBAgILBAAAAAABD4Ym5g0wDQYJKoZIhvcNAQEFBQAwTDEgMB4G\n" \
"A1UECxMXR2xvYmFsU2lnbiBSb290IENBIC0gUjIxEzARBgNVBAoTCkdsb2JhbFNp\n" \
"Z24xEzARBgNVBAMTCkdsb2JhbFNpZ24wHhcNMDYxMjE1MDgwMDAwWhcNMjExMjE1\n" \
"MDgwMDAwWjBMMSAwHgYDVQQLExdHbG9iYWxTaWduIFJvb3QgQ0EgLSBSMjETMBEG\n" \
"A1UEChMKR2xvYmFsU2lnbjETMBEGA1UEAxMKR2xvYmFsU2lnbjCCASIwDQYJKoZI\n" \
"hvcNAQEBBQADggEPADCCAQoCggEBAKbPJA6+Lm8omUVCxKs+IVSbC9N/hHD6ErPL\n" \
"v4dfxn+G07IwXNb9rfF73OX4YJYJkhD10FPe+3t+c4isUoh7SqbKSaZeqKeMWhG8\n" \
"eoLrvozps6yWJQeXSpkqBy+0Hne/ig+1AnwblrjFuTosvNYSuetZfeLQBoZfXklq\n" \
"tTleiDTsvHgMCJiEbKjNS7SgfQx5TfC4LcshytVsW33hoCmEofnTlEnLJGKRILzd\n" \
"C9XZzPnqJworc5HGnRusyMvo4KD0L5CLTfuwNhv2GXqF4G3yYROIXJ/gkwpRl4pa\n" \
"zq+r1feqCapgvdzZX99yqWATXgAByUr6P6TqBwMhAo6CygPCm48CAwEAAaOBnDCB\n" \
"mTAOBgNVHQ8BAf8EBAMCAQYwDwYDVR0TAQH/BAUwAwEB/zAdBgNVHQ4EFgQUm+IH\n" \
"V2ccHsBqBt5ZtJot39wZhi4wNgYDVR0fBC8wLTAroCmgJ4YlaHR0cDovL2NybC5n\n" \
"bG9iYWxzaWduLm5ldC9yb290LXIyLmNybDAfBgNVHSMEGDAWgBSb4gdXZxwewGoG\n" \
"3lm0mi3f3BmGLjANBgkqhkiG9w0BAQUFAAOCAQEAmYFThxxol4aR7OBKuEQLq4Gs\n" \
"J0/WwbgcQ3izDJr86iw8bmEbTUsp9Z8FHSbBuOmDAGJFtqkIk7mpM0sYmsL4h4hO\n" \
"291xNBrBVNpGP+DTKqttVCL1OmLNIG+6KYnX3ZHu01yiPqFbQfXf5WRDLenVOavS\n" \
"ot+3i9DAgBkcRcAtjOj4LaR0VknFBbVPFd5uRHg5h6h+u/N5GJG79G+dwfCMNYxd\n" \
"AfvDbbnvRG15RjF+Cv6pgsH/76tuIMRQyV+dTZsXjAzlAcmgQWpzU/qlULRuJQ/7\n" \
"TBj0/VLZjmmx6BEP3ojY+x1J96relc8geMJgEtslQIxq/H5COEBkEveegeGTLg==\n" \
"-----END CERTIFICATE-----\n";

const char *googleScriptLink = ("https://script.google.com/macros/s/AKfycbyiouuhAzYcD3rbgJmfhf0x1vk40xPCi1l6PwpkJ4yTg9caLb4/exec");

String local_IP;
int count = 0;

void startCameraServer();
void setFaceIds(float *floats_list,uint8_t len);
int getFaceToSend();
void lockStream();
void unlockStream();
void setLengthOfFaceListNames(int);
String *getFaceNamesArray();

void startCameraServer();
void takePhoto(){
  digitalWrite(LED,LOW);
  delay(200);
  uint32_t randomNum= random(50000);
  Serial.println("http://"+local_IP+"/capture?_cb="+(String)randomNum);
  Blynk.setProperty(V1,"urls","http://"+local_IP+"/capture?_cb="+(String)randomNum);
  digitalWrite(LED,HIGH);
}

void sendVisit(int personId)
{ 
   HTTPClient http;
  Serial.println(F("Sending visit"));
  uint64_t time = esp_timer_get_time();
   Serial.println(F("s1"));
  char url[110];
  sprintf(url,"%s?faceid=%d",googleScriptLink,personId);
Serial.println(url);
  http.begin(url, root_ca);
  Serial.println(F("s3"));
  int code = http.GET();
  Serial.println(F("s4"));
  Serial.printf("code %d\n", code);
  
}

void loadFaceIds()
{
  HTTPClient http;
  Serial.println(F("Getting new Ids"));
  uint64_t time = esp_timer_get_time();
  char url[150];
  sprintf(url,"%s?getnames",googleScriptLink);

  http.begin(url, root_ca);
  

  const char *loca = "Location";

  const char *headerKeys[] = {loca};
  http.collectHeaders(headerKeys, 1);
  int code = http.GET();
  Serial.printf("code %d\n", code);
  if (code == 302)
  {

    String newUrl = http.header(loca);

    http.end();

    Serial.println(newUrl);
    http.begin(newUrl, root_ca);
    code = http.GET();
    Serial.printf("status code %d\n", code);

    

    Serial.println(http.getStream().readStringUntil('\n'));
      
    int row = 0;
    int poi = 0;
    String *names=getFaceNamesArray();
    while (http.getStream().available())
    {
      String val_str=http.getStream().readStringUntil(',');
      //sprintf(nanames[poi++],"")
      names[poi++]=val_str;
      
  
    }
     setLengthOfFaceListNames(poi-1);
    //names[poi++]="";

    

    
  }



}

boolean matchFace = false;
boolean openLock = false;
long prevMillis = 0;
int interval = 6000;  //DELAY

void setup() {
  pinMode(LOCK, OUTPUT);
  pinMode(LED, OUTPUT);
  pinMode(IN_IR,INPUT);
  pinMode(IR,INPUT);
  
  digitalWrite(LOCK, LOW);
  digitalWrite(LED, HIGH);

  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();

  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  //init with high specs to pre-allocate larger buffers
  if (psramFound()) {
    config.frame_size = FRAMESIZE_UXGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }

#if defined(CAMERA_MODEL_ESP_EYE)
  pinMode(13, INPUT_PULLUP);
  pinMode(14, INPUT_PULLUP);
#endif

  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  sensor_t * s = esp_camera_sensor_get();
  //initial sensors are flipped vertically and colors are a bit saturated
  if (s->id.PID == OV3660_PID) {
    s->set_vflip(s, 1);//flip it back
    s->set_brightness(s, 1);//up the blightness just a bit
    s->set_saturation(s, -2);//lower the saturation
  }
  //drop down frame size for higher initial frame rate
  s->set_framesize(s, FRAMESIZE_QVGA);

#if defined(CAMERA_MODEL_M5STACK_WIDE)
  s->set_vflip(s, 1);
  s->set_hmirror(s, 1);
#endif

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  loadFaceIds();
  startCameraServer();

  Serial.print("Camera Ready! Use 'http://");
  Serial.print(WiFi.localIP());
  local_IP = WiFi.localIP().toString();
  Serial.println("' to connect");
  Blynk.begin(auth, ssid, password);
}

void loop() {
  
    int temp1=getFaceToSend();

  
  Blynk.run();
  if(digitalRead(IR)==LOW && count==0){
    count=1;
    Blynk.notify("Someone is at the door!");
    takePhoto();
    delay(3000);
    count=0;
    }
  /*if(digitalRead(PHOTO)==HIGH){
    Serial.println("Capture Photo");
    takePhoto();
    delay(1000);
    } */
  if(digitalRead(LOCK)==HIGH){
    Serial.println("Unlock Door");
  }
  
  if (matchFace == true && openLock == false)
  {
     
    openLock = true;
    digitalWrite(LOCK, HIGH);
    digitalWrite(LED, LOW);
    prevMillis = millis();
    Serial.print("UNLOCK DOOR");
  }
  if(digitalRead(IN_IR)==LOW && openLock==false){
    openLock=true;
    digitalWrite(LOCK,HIGH);
    digitalWrite(LED,LOW);
    prevMillis=millis();
    Serial.print("Unlock Door");
  }
  
  if (openLock == true && millis() - prevMillis > interval)
  {
    openLock = false;
    matchFace = false;
    digitalWrite(LOCK, LOW);
    digitalWrite(LED, HIGH);
    Serial.print("LOCK DOOR");
  }
   if(temp1>-1)
  {
    lockStream();
   sendVisit(temp1);
    unlockStream();
  }
}
