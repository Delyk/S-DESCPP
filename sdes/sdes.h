#pragma once
#include <bitset>
#include <cstdint>
#include <fstream>
#include <initializer_list>
#include <random>
#include <string>
#include <vector>
#define KEY_LEN 10
#define BLOCK_LEN 8
#define ROUND_COUNT 2

namespace sdes_cypher {

class sdes {
  class bits {
    std::vector<bool> storage;

  public:
    bits();
    bits(const bits &);
    bits(bits &&) noexcept;
    bits(std::initializer_list<bool>);
    bits(std::vector<bool>);
    bits(uint8_t);
    bits cut(unsigned, unsigned);
    void unite(bits &&) noexcept;
    void mixing(std::vector<unsigned>);
    unsigned to_unsigned() const;
    void push(bool);
    std::string to_string() const;
    size_t size() const;
    bool empty() const;
    std::vector<bool>::reference operator[](size_t);
    bits &operator<<=(size_t) noexcept;
    bits &operator^=(const bits &) noexcept;
    bits &operator=(const bits &);
    bits &operator=(bits &&) noexcept;
    ~bits();
  };

  uint16_t key;
  bits cypher_key; // Ключ шифрования
  bits first_key;
  bits second_key;
  std::vector<unsigned> key_straight_P_block{
      2, 4, 1, 6, 3, 9, 0, 8, 7, 5}; // Маска для прямого P-блока ключа
  std::vector<unsigned> key_compressed_P_block{
      5, 2, 6, 3, 7, 4, 9, 8}; // Маска для P-блока сжатия ключа
  std::vector<unsigned> initial_P_block{
      1, 5, 2, 0, 3, 7, 4, 6}; // Начальная перестановка текста
  std::vector<unsigned> final_P_block{3, 0, 2, 4, 6,
                                      1, 7, 5}; // Конечная перестановка текста
  std::vector<unsigned> P_block_expansion{3, 0, 1, 2, 1,
                                          2, 3, 0}; // Маска P-блока расширения
  std::vector<unsigned> P_block_straight{1, 3, 2, 0}; // Маска прямого P-блока
  unsigned S_blocks_tables[2][BLOCK_LEN / 2][BLOCK_LEN / 2]{
      {{1, 0, 3, 2}, {3, 2, 1, 0}, {0, 2, 1, 3}, {3, 1, 3, 2}},
      {{0, 1, 2, 3}, {2, 0, 1, 3}, {3, 0, 1, 0}, {2, 1, 0, 3}}};
  // S-блоки
  unsigned current_round; // Количество прошедших раундов

  bits get_random_key(); // Сгенерировать случайный ключ
  bits key_gen(unsigned); // Генерация ключа раунда
  bits round(bits,
             bits); // Раунд шифрования
  bits sdes_function(bits,
                     bits);         // Функция смешивания S-DES
  bits S_blocks(std::vector<bits>); // S-блоки

  inline unsigned progression(unsigned num) { return (num * (num + 1)) / 2; }

public:
  sdes();
  sdes(std::initializer_list<bool>);
  sdes(uint16_t);
  uint16_t getKey() const;

  void print_crypt(std::initializer_list<bool>);
  void print_decrypt(std::initializer_list<bool>);
  char encrypt(char);
  char decrypt(char);

  void cypher_textfile(std::string);
  void decypher_textfile(std::string);

  void cypher_binfile(std::string);
  void decypher_binfile(std::string);

  void print_textfile(std::string);
};

} // namespace sdes_cypher
