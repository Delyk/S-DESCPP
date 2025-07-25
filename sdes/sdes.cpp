#include "sdes.h"
#include <bitset>
#include <cstddef>
#include <iostream>
#include <random>

sdes::sdes() : round(1) { get_random_key(); }

sdes::sdes(std::bitset<10> key) : round(1) {
  for (size_t i = 0; i < KEY_LEN; i++) {
    cypher_key[i] = key[key_straight_P_block[i]];
  }
}

//Получаем случайный ключ из генератора случайных чисел
void sdes::get_random_key() {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dist(0, 1);
  std::bitset<KEY_LEN> key;

  for (size_t i = 0; i < KEY_LEN; i++) {
    key[i] = dist(gen);
  }

  for (size_t i = 0; i < KEY_LEN; i++) {
    cypher_key[i] = key[key_straight_P_block[i]];
  }
}

//Генерируем ключ для нового раунда
// std::bitset<BLOCK_LEN> sdes::key_gen() {}

void sdes::print() {
  std::cout << "Key: " << cypher_key.to_string() << std::endl;
  // std::cout << "First Key: " << key_gen().to_string() << std::endl;
  // std::cout << "Second Key: " << key_gen().to_string() << std::endl;
}
