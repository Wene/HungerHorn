#include "player.h"
#include <DFRobotDFPlayerMini.h>

struct player_impl
{
  DFRobotDFPlayerMini player;
  void printDetail(uint8_t type, uint16_t value);
  void check_state();
};

Player::Player(): impl(new player_impl)
{

}

Player::~Player()
{
  delete impl;
}

void Player::setup()
{
  Serial2.begin(9600);

  while(!impl->player.begin(Serial2))
  {
    Serial.println(F("connecting to DF-Player..."));
    delay(1000);
  }
  while(true)
  {
    Serial.println(F("waiting for SD card..."));
    if(impl->player.available())
    {
      if(DFPlayerCardOnline == impl->player.readType())
      {
        break;
      }
    }
    delay(1000);
  }

  impl->player.setTimeOut(500);
  impl->player.volume(20);  // 0-30
  impl->player.EQ(DFPLAYER_EQ_NORMAL);
  impl->player.outputDevice(DFPLAYER_DEVICE_SD);

}

void Player::play_track(int number)
{
  impl->player.playMp3Folder(number);
}

void Player::print_state()
{
  impl->check_state();
}

void player_impl::check_state()
{
  if (player.available()) {
    printDetail(player.readType(), player.read());
  }
}

void player_impl::printDetail(uint8_t type, uint16_t value)
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

