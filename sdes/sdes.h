#pragma once
#include <bitset>
#include <cstddef>
#include <random>
#include <vector>
#define KEY_LEN 10
#define BLOCK_LEN 8
#define ROUND_COUNT 2

class sdes {
  std::bitset<KEY_LEN> cypher_key; //Ключи шифрования
  unsigned int key_straight_P_block[KEY_LEN]{
      2, 4, 1, 6, 3, 9, 0, 8, 7, 5}; //Маска для прямого P-блока ключа
  unsigned int key_compressed_P_block[BLOCK_LEN]{
      5, 2, 6, 3, 7, 4, 9, 8}; //Маска для P-блока сжатия ключа
  unsigned int round;          //Количество раундов

  std::bitset<KEY_LEN> get_random_key(); //Сгенерировать случайный ключ
  std::bitset<BLOCK_LEN> key_gen(); //Генерация ключа для каждого раунда
  std::bitset<BLOCK_LEN>
      key_compress(std::bitset<KEY_LEN>); //Сжатие ключа через P-блок

  template <unsigned long bit_count = BLOCK_LEN>
  std::bitset<bit_count>
      reverse(std::bitset<bit_count>); //Инвертировать порядок битов в bitset

  template <unsigned long bit_count = 8, unsigned long split_count = 2>
  std::vector<std::bitset<bit_count / split_count>>
      split(std::bitset<bit_count>); //Разделить блок на части

  template <unsigned long bit_count = 8, unsigned long split_count = 2>
  std::bitset<bit_count>
      unite(std::vector<std::bitset<bit_count / split_count>>); //Соеднить части
                                                                //блока вместе

public:
  sdes();
  sdes(std::bitset<KEY_LEN>);

  template <size_t N>
  friend std::bitset<N>
  operator>>(const std::bitset<N> &bits,
             size_t pos); //Перегруженный оператор кольцевого побитового сдвига

  void print();
};
