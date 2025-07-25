#pragma once
#include <bitset>
#define KEY_LEN 10
#define BLOCK_LEN 8

class sdes {
  std::bitset<KEY_LEN> cypher_key;
  unsigned int key_straight_P_block[KEY_LEN]{2, 4, 1, 6, 3, 9, 0, 8, 7, 5};
  unsigned int key_compressed_P_block[BLOCK_LEN]{5, 2, 6, 3, 7, 4, 9, 8};
  unsigned int round;

  void get_random_key();
  // std::bitset<BLOCK_LEN> key_gen();

public:
  sdes();
  sdes(std::bitset<10>);

  void print();
};
