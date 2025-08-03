#pragma once
#include <bitset>
#include <csetjmp>
#include <cstddef>
#include <random>
#include <vector>
#define KEY_LEN 10
#define BLOCK_LEN 8
#define ROUND_COUNT 2

namespace sdes_cypher {

class sdes {
  template <size_t N = 8> class bits {
    bool storage[N];

  public:
    bits();
    bits(bool[]);
    bits(unsigned);
    bits(char);
    bool split(unsigned, unsigned);
    void unite(bool[], bool[]);
    void mixing(unsigned[]);
    unsigned to_unsigned() const;
    bool &operator[](size_t);
    bits &operator<<=(size_t) noexcept;
    bool &operator^=(const bits &) noexcept;
  };

  std::bitset<KEY_LEN> cypher_key; //Ключ шифрования
  unsigned key_straight_P_block[KEY_LEN]{
      2, 4, 1, 6, 3, 9, 0, 8, 7, 5}; //Маска для прямого P-блока ключа
  unsigned int key_compressed_P_block[BLOCK_LEN]{
      5, 2, 6, 3, 7, 4, 9, 8}; //Маска для P-блока сжатия ключа
  unsigned initial_P_block[BLOCK_LEN]{1, 5, 2, 0, 3,
                                      7, 4, 6}; //Начальная перестановка текста
  unsigned int final_P_block[BLOCK_LEN]{3, 0, 2, 4, 6,
                                        1, 7, 5}; //Конечная перестановка текста
  unsigned P_block_expansion[BLOCK_LEN]{3, 0, 1, 2,
                                        1, 2, 3, 0}; //Маска P-блока расширения
  unsigned P_block_straight[BLOCK_LEN / 2]{1, 3, 2, 0}; //Маска прямого P-блока
  unsigned S_blocks_tables[2][BLOCK_LEN / 2][BLOCK_LEN / 2]{
      {{1, 0, 3, 2}, {3, 2, 1, 0}, {0, 2, 1, 3}, {3, 1, 3, 2}},
      {{0, 1, 2, 3}, {2, 0, 1, 3}, {3, 0, 1, 0}, {2, 1, 0, 3}}};
  // S-блоки
  unsigned current_round; //Количество прошедших раундов

  std::bitset<KEY_LEN> get_random_key(); //Сгенерировать случайный ключ
  std::bitset<BLOCK_LEN> key_gen(); //Генерация ключа раунда
  std::bitset<BLOCK_LEN>
      key_compress(std::bitset<KEY_LEN>); //Сжатие ключа через P-блок
  std::bitset<BLOCK_LEN>
      initial_permutation(std::bitset<BLOCK_LEN>); //Начальная перестановка
  std::bitset<BLOCK_LEN>
      final_permutation(std::bitset<BLOCK_LEN>); //Конечная перестановка
  std::bitset<BLOCK_LEN> round(std::bitset<BLOCK_LEN>,
                               std::bitset<BLOCK_LEN>); //Раунд шифрования
  std::bitset<BLOCK_LEN / 2>
      sdes_function(std::bitset<BLOCK_LEN / 2>,
                    std::bitset<BLOCK_LEN>); //Функция смешивания S-DES
  std::bitset<BLOCK_LEN>
      expansion_P_block(std::bitset<BLOCK_LEN / 2>); // P-блок расширения
  std::bitset<BLOCK_LEN / 2>
      S_blocks(std::vector<std::bitset<BLOCK_LEN / 2>>); // S-блоки
  std::bitset<BLOCK_LEN / 2>
      straight_P_block(std::bitset<BLOCK_LEN / 2>); //Прямой P-блок

  inline unsigned progression(unsigned num) { return (num * (num + 1)) / 2; }
  template <unsigned long bit_count = BLOCK_LEN>
  std::bitset<bit_count>
      reverse(std::bitset<bit_count>); //Инвертировать порядок битов в bitset

  template <unsigned long bit_count = 8, unsigned long split_count = 2>
  std::vector<std::bitset<bit_count / split_count>>
      split(std::bitset<bit_count>); //Разделить блок на части

  template <unsigned long bit_count = 8, unsigned long split_count = 2>
  std::bitset<bit_count>
      unite(std::vector<std::bitset<bit_count / split_count>>); //Соеднить части
  template <size_t N = 4>
  unsigned long cut(std::bitset<N>, unsigned,
                    unsigned); //Вырезать два бита из
                               //блока и преобразовать их в число

public:
  sdes();
  sdes(std::bitset<KEY_LEN>);

  void print(std::bitset<BLOCK_LEN>);
};

} // namespace sdes_cypher
