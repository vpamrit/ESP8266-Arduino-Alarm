/***************************************************
  This is an example sketch for our optical Fingerprint sensor

  Designed specifically to work with the Adafruit BMP085 Breakout
  ----> http://www.adafruit.com/products/751

  These displays use TTL Serial to communicate, 2 pins are required to
  interface
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
 ****************************************************/

#include <Adafruit_Fingerprint.h>
#include <LoggingHelpers.h>
#include <NotifierClient.h>
#include <pt.h>
#include <pt-sleep.h>

#if (defined(__AVR__) || defined(ESP8266)) && !defined(__AVR_ATmega2560__)
// For UNO and others without hardware serial, we must use software serial...
// pin #2 is IN from sensor (GREEN wire)
// pin #3 is OUT from arduino  (WHITE wire)
// Set up the serial port to use softwareserial..
SoftwareSerial mySerial(4, 5);

#else
// On Leonardo/M0/etc, others with hardware serial, use hardware serial!
// #0 is green wire, #1 is white
#define mySerial Serial1

#endif

enum ScannerState
{
  INIT = 0,
  ENROLL = 1,
  SCAN = 2,
  CLOUD = 3,
  IDLE = 4,
  BOOT = 5
};

//pin states
const int stateButton = 2;

//forward declare
static uint8_t EnrollMode();
static uint8_t ScanMode();
static uint8_t InitMode();
static void changeStateButtonPress();
static int buttonFunction(struct pt *pt);
static int stateFunction(struct pt *pt);

//static variables and globals
static ScannerState state = ENROLL;
static ScannerState prevState = BOOT;
volatile unsigned long lastTimeUpdated = 0;
volatile int pendingStateSwitches = 0;
static unsigned long timer1;
static unsigned long timer2;
NotifierClient client;
static Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);
uint8_t id;

char buff [100];
volatile byte index;
volatile bool receivedone;  /* use reception complete flag */

//protothread init
static struct pt buttonPT, statePT;

// create SPI Client
static SPIMaster spiClient;

void setupPins()
{
  pinMode(stateButton, INPUT);
}

void setupSPI() {
  SPCR |= bit(SPE);         /* Enable SPI */
  pinMode(MISO, OUTPUT);    /* Make MISO pin as OUTPUT */
  index = 0;
  receivedone = false;
  SPI.attachInterrupt();    /* Attach SPI interrupt */
}

void setup()
{
  Serial.begin(9600);
  while (!Serial)
    ;
  delay(100);
  Serial.println("\n\nNotification system on");

  while (1)
  {
    // set the data rate for the sensor serial port
    finger.begin(57600);

    if (finger.verifyPassword())
    {
      Serial.println("Found fingerprint sensor!");
      break;
    }
    else
    {
      Serial.println("Did not find fingerprint sensor :(");
    }
    delay(1);
  };

  setupSPI();


  Serial.println(F("Reading sensor parameters"));
  finger.getParameters();
  Serial.print(F("Status: 0x"));
  Serial.println(finger.status_reg, HEX);
  Serial.print(F("Sys ID: 0x"));
  Serial.println(finger.system_id, HEX);
  Serial.print(F("Capacity: "));
  Serial.println(finger.capacity);
  Serial.print(F("Security level: "));
  Serial.println(finger.security_level);
  Serial.print(F("Device address: "));
  Serial.println(finger.device_addr, HEX);
  Serial.print(F("Packet len: "));
  Serial.println(finger.packet_len);
  Serial.print(F("Baud rate: "));
  Serial.println(finger.baud_rate);

  attachInterrupt(digitalPinToInterrupt(stateButton), changeStateButtonPress, RISING);

  //init protothreads
  PT_INIT(&buttonPT);
  PT_INIT(&statePT);
}

void loop() // run over and over again
{
 if (receivedone)          /* Check and print received buffer if any */
  {
    buff[index] = 0;
    Serial.println(index);
    Serial.println(buff);
    index = 0;
    receivedone = false;
  }

  PT_SCHEDULE(stateFunction(&statePT));
  PT_SCHEDULE(buttonFunction(&buttonPT));
};

ISR (SPI_STC_vect)
{
  uint8_t oldsrg = SREG;
  cli();
  char c = SPDR;
  if (index < sizeof buff)
  {
    buff[index++] = c;
    Serial.println(c);
    if (c == '\n'){     /* Check for newline character as end of msg */
      Serial.println("Success");
     receivedone = true;
    }
  }
  SREG = oldsrg;
}

static void changeStateButtonPress()
{
  if (millis() - lastTimeUpdated > 500)
  {
    pendingStateSwitches = pendingStateSwitches + 1;
    lastTimeUpdated = millis();
  }
}

static int buttonFunction(struct pt *pt)
{
  PT_BEGIN(pt);
  if (pendingStateSwitches != 0)
  {
    PT_INIT(&statePT); //reset

    while (pendingStateSwitches >= 1)
    {
      pendingStateSwitches = pendingStateSwitches - 1;
      switch (state)
      {
      case IDLE:
        state = SCAN;
        Serial.println("Mode scan");
        break;
      case SCAN:
        state = ENROLL;
        Serial.println("Mode enroll");
        break;
      case ENROLL:
        state = CLOUD;
        Serial.println("Mode cloud");
        break;
      case CLOUD:
        state = SCAN;
        Serial.println("Mode scan");
        break;
      default:
        state = IDLE;
        Serial.println("Mode idle");
        break;
      }
    }
  }
  PT_END(pt);
};

static int stateFunction(struct pt *pt)
{
  static int res = PT_ENDED;
  PT_BEGIN(pt);

  switch (state)
  {
  case INIT:
    res = InitMode(); // establish server connection etc.
  case ENROLL:
    res = EnrollMode();
    break;
  case SCAN:
    res = ScanMode();
    break;
  case CLOUD:
  case IDLE:
  default:
    break;
  }

  prevState = state;
  if (res != PT_ENDED)
  {
    return res;
  }
  else
  {
    state = IDLE;
  }

  PT_END(pt);
}

static uint8_t InitMode()
{
  client.init();
  return PT_ENDED;
}

static uint8_t ScanMode()
{
  static uint8_t p;
  p = -1;

  PT_WAIT_UNTIL_WITH_DELAY(&statePT, p == FINGERPRINT_OK, p = finger.getImage(), 50, 5000);

  do
  {
    if (logError(p, "Image taken"))
      break;

    p = finger.image2Tz();
    if (logError(p, "Image converted"))
      break;

    p = finger.fingerSearch();
    if (logError(p, "Found a match"))
      break;

    //match found
    Serial.print("Found ID #");
    Serial.print(finger.fingerID);
    Serial.print(" with confidence of ");
    Serial.println(finger.confidence);

    static bool res = false;

    PT_WAIT_UNTIL(&statePT,
                  (res = client.sendFingerConfirmation(finger.fingerID, finger.confidence)));

    // wait confirmation?
    if (res)
    {
      Serial.println("Issued request successfully to server");
      break;
    }

    Serial.println("Failed to connect to server...");
  } while (0);

  return PT_ENDED;
}

static uint8_t EnrollMode()
{
  static int p;
  p = -1;

  if (id == 128)
  {
    Serial.println("Sensor filled. Manually clear.");
    return PT_ENDED;
  }

  id++; //increment to new id
  Serial.println("Ready to enroll a fingerprint!");

  do
  {
    Serial.print("Waiting for valid finger to enroll as #");
    Serial.println(id);

    PT_WAIT_UNTIL_WITH_DELAY(&statePT, p == FINGERPRINT_OK, p = finger.getImage(), 50, 5000);
    if (logError(p, "Image taken"))
      break;

    // OK success!
    p = finger.image2Tz(1);
    if (logError(p, "Image converted"))
      break;

    Serial.println("Remove finger");
    PT_SLEEP(&statePT, 2000);

    //TODO: replace with loop
    p = 0;
    while (p != FINGERPRINT_NOFINGER)
    {
      p = finger.getImage();
    }
    Serial.print("ID ");
    Serial.println(id);
    p = -1;
    Serial.println("Place same finger again");

    PT_WAIT_UNTIL_WITH_DELAY(&statePT, p == FINGERPRINT_OK, p = finger.getImage(), 50, 5000);
    if (logError(p, "Image taken"))
      break;

    // OK success!
    p = finger.image2Tz(2);
    if (logError(p, "Image converted"))
      break;

    // OK converted!
    Serial.print("Creating model for #");
    Serial.println(id);

    p = finger.createModel();
    if (logError(p, "Prints matched!"))
      break;

    Serial.print("ID ");
    Serial.println(id);
    p = finger.storeModel(id);

    if (logError(p, "Stored!"))
      break;
  } while (0);

  if (p != FINGERPRINT_OK)
  {
    id--; //decrement id
  }

  return PT_ENDED;
};