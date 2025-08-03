#pragma once
#include <bitset>
#include <csetjmp>
#include <cstddef>
#include <initializer_list>
#include <random>
#include <vector>
#define KEY_LEN 10
#define BLOCK_LEN 8
#define ROUND_COUNT 2

namespace sdes_cypher {

class sdes {
  class bits {
    std::vector<bool> storage;

  public:
    bits(size_t = 8);
    bits(std::vector<bool>);
    bits(unsigned);
    bits(char);
    bits cut(unsigned, unsigned);
    void unite(bits &&) noexcept;
    void mixing(unsigned[]);
    unsigned to_unsigned() const;
    void push(bool);
    std::string to_string() const;
    size_t size() const;
    std::vector<bool>::reference operator[](size_t);
    bits &operator<<=(size_t) noexcept;
    bits &operator^=(const bits &) noexcept;
  };

  bits cypher_key; // Ключ шифрования
  unsigned key_straight_P_block[KEY_LEN]{
      2, 4, 1, 6, 3, 9, 0, 8, 7, 5}; // Маска для прямого P-блока ключа
  unsigned int key_compressed_P_block[BLOCK_LEN]{
      5, 2, 6, 3, 7, 4, 9, 8}; // Маска для P-блока сжатия ключа
  unsigned initial_P_block[BLOCK_LEN]{1, 5, 2, 0, 3,
                                      7, 4, 6}; // Начальная перестановка текста
  unsigned int final_P_block[BLOCK_LEN]{
      3, 0, 2, 4, 6, 1, 7, 5}; // Конечная перестановка текста
  unsigned P_block_expansion[BLOCK_LEN]{3, 0, 1, 2,
                                        1, 2, 3, 0}; // Маска P-блока расширения
  unsigned P_block_straight[BLOCK_LEN / 2]{1, 3, 2, 0}; // Маска прямого P-блока
  unsigned S_blocks_tables[2][BLOCK_LEN / 2][BLOCK_LEN / 2]{
      {{1, 0, 3, 2}, {3, 2, 1, 0}, {0, 2, 1, 3}, {3, 1, 3, 2}},
      {{0, 1, 2, 3}, {2, 0, 1, 3}, {3, 0, 1, 0}, {2, 1, 0, 3}}};
  // S-блоки
  unsigned current_round; // Количество прошедших раундов

  bits get_random_key();   // Сгенерировать случайный ключ
  bits key_gen();          // Генерация ключа раунда
  bits key_compress(bits); // Сжатие ключа через P-блок
  bits round(bits,
             bits); // Раунд шифрования
  bits sdes_function(bits,
                     bits);         // Функция смешивания S-DES
  bits expansion_P_block(bits);     // P-блок расширения
  bits S_blocks(std::vector<bits>); // S-блоки

  inline unsigned progression(unsigned num) { return (num * (num + 1)) / 2; }

public:
  sdes();
  sdes(std::initializer_list<bool>);

  void print(std::initializer_list<bool>);
};

} // namespace sdes_cypher
