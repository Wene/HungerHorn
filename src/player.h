#pragma once

struct player_impl;

class Player
{
public:
  Player();
  ~Player();
  void setup();
  void play_track(int number);
  void print_state();

private:
  player_impl *impl;
};

extern Player player;
