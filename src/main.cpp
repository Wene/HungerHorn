#include <Arduino.h>
#include <DFRobotDFPlayerMini.h>

DFRobotDFPlayerMini player;

void printDetail(uint8_t type, uint16_t value)
{
  switch (type)
  {
    case TimeOut:
      Serial.println(F("Time Out!"));
      break;
    case WrongStack:
      Serial.println(F("Stack Wrong!"));
      break;
    case DFPlayerCardInserted:
      Serial.println(F("Card Inserted!"));
      break;
    case DFPlayerCardRemoved:
      Serial.println(F("Card Removed!"));
      break;
    case DFPlayerCardOnline:
      Serial.println(F("Card Online!"));
      break;
    case DFPlayerUSBInserted:
      Serial.println("USB Inserted!");
      break;
    case DFPlayerUSBRemoved:
      Serial.println("USB Removed!");
      break;
    case DFPlayerPlayFinished:
      Serial.print(F("Number:"));
      Serial.print(value);
      Serial.println(F(" Play Finished!"));
      break;
    case DFPlayerError:
      Serial.print(F("DFPlayerError:"));
      switch (value)
      {
        case Busy:
          Serial.println(F("Card not found"));
          break;
        case Sleeping:
          Serial.println(F("Sleeping"));
          break;
        case SerialWrongStack:
          Serial.println(F("Get Wrong Stack"));
          break;
        case CheckSumNotMatch:
          Serial.println(F("Check Sum Not Match"));
          break;
        case FileIndexOut:
          Serial.println(F("File Index Out of Bound"));
          break;
        case FileMismatch:
          Serial.println(F("Cannot Find File"));
          break;
        case Advertise:
          Serial.println(F("In Advertise"));
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
}

static void check_state(void)
{
  if (player.available()) {
    printDetail(player.readType(), player.read());
  }
}

void setup()
{
  Serial.begin(115200);
  Serial.println(F("Starting..."));

  Serial2.begin(9600);

  while(!player.begin(Serial2))
  {
    Serial.println(F("connecting to DF-Player..."));
    delay(1000);
  }
  while(true)
  {
    Serial.println(F("waiting for SD card..."));
    if(player.available())
    {
      if(DFPlayerCardOnline == player.readType())
      {
        break;
      }
    }
    delay(1000);
  }

  player.setTimeOut(500);
  player.volume(20);  // 0-30
  player.EQ(DFPLAYER_EQ_NORMAL);
  player.outputDevice(DFPLAYER_DEVICE_SD);

  Serial.println(F("Setup done."));
}

void loop()
{
  if(Serial.available())
  {
    int input = Serial.read();
    if(input >= '1' && input <= '9')
    {
      int track = input - '0';
      player.playMp3Folder(track);
    }
  }

  check_state();
}
