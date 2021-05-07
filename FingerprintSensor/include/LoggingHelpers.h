#include "Arduino.h"
#include <Adafruit_Fingerprint.h>

#define PT_WAIT_UNTIL_WITH_DELAY(pt, COND, EXEC, DELAY, TIMEOUT) \
    timer1 = millis();                                     \
    do                                                     \
    {                                                      \
        if (!(((COND) || millis() - timer1 > TIMEOUT)))    \
        {                                                  \
            EXEC;                                          \
            timer2 = millis();                             \
            Serial.print("");                              \
            LC_SET((pt)->lc);                              \
            if (!(millis() - timer2 > DELAY))              \
            {                                              \
                return PT_WAITING;                         \
            }                                              \
        }                                                  \
        else                                               \
        {                                                  \
            break;                                         \
        }                                                  \
    } while (1);

bool logError(uint8_t errCode, const char successMsg[])
{
    bool res = true;
    switch (errCode)
    {
    case FINGERPRINT_OK:
        Serial.println(successMsg);
        res = false;
        break;
    case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Error when receiving data package");
        break;
    case FINGERPRINT_NOFINGER:
        Serial.println("No finger on the sensor");
        break;
    case FINGERPRINT_IMAGEFAIL:
        Serial.println("Failed to enroll the finger");
        break;
    case FINGERPRINT_IMAGEMESS:
        Serial.println("Failed to generate character file due to overly disorderly");
        break;
    case FINGERPRINT_FEATUREFAIL:
        Serial.println("Failed to generate character file due to the lack of character point");
        break;
    case FINGERPRINT_NOMATCH:
        Serial.println("Finger doesn't match");
        break;
    case FINGERPRINT_NOTFOUND:
        Serial.println("Failed to find matching finger");
        break;
    case FINGERPRINT_ENROLLMISMATCH:
        Serial.println("Failed to combine the character files");
        break;
    case FINGERPRINT_BADLOCATION:
        Serial.println("Addressed PageID is beyond the finger library");
        break;
    case FINGERPRINT_DBRANGEFAIL:
        Serial.println("Error when reading template from library or invalid template");
        break;
    case FINGERPRINT_UPLOADFEATUREFAIL:
        Serial.println("Error when uploading template");
        break;
    case FINGERPRINT_PACKETRESPONSEFAIL:
        Serial.println("Module failed to receive the following data packages");
        break;
    case FINGERPRINT_UPLOADFAIL:
        Serial.println("Error when uploading image");
        break;
    case FINGERPRINT_DELETEFAIL:
        Serial.println("Failed to delete the template");
        break;
    case FINGERPRINT_DBCLEARFAIL:
        Serial.println("Failed to clear finger library");
        break;
    case FINGERPRINT_PASSFAIL:
        Serial.println("Find whether the fingerprint passed or failed");
        break;
    case FINGERPRINT_INVALIDIMAGE:
        Serial.println("Failed to generate image because of lac of valid primary image");
        break;
    case FINGERPRINT_FLASHERR:
        Serial.println("Error when writing flash");
        break;
    case FINGERPRINT_INVALIDREG:
        Serial.println("Invalid register number");
        break;
    case FINGERPRINT_ADDRCODE:
        Serial.println("Address code");
        break;
    case FINGERPRINT_PASSVERIFY:
        Serial.println("Verify the fingerprint passed");
        break;
    // case FINGERPRINT_STARTCODE:
    //     Serial.println("Fixed falue of EF01H; High byte transferred first");
    //     break;
    default:
        Serial.println("Unknown error");
        break;
    }

    // if(res)
    //     Serial.println("If printed message makes no sense; corrupted reception");
    return res;
}