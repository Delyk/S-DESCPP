#include "sdes.h"
#include <algorithm>
#include <cstddef>
#include <initializer_list>
#include <iostream>
#include <ostream>
#include <random>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>
using namespace sdes_cypher;

// Внутренний класс bits
// Конструкторы
sdes::bits::bits() {}

sdes::bits::bits(const bits &right) { storage = right.storage; }

sdes::bits::bits(bits &&right) noexcept {
  storage = right.storage;
  right.storage.clear();
}

sdes::bits::bits(std::initializer_list<bool> bit_num) {
  for (auto it = bit_num.begin(); it != bit_num.end(); it++) {
    storage.push_back(*it);
  }
}

sdes::bits::bits(std::vector<bool> bool_vector) { storage = bool_vector; }

sdes::bits::bits(uint8_t number) {
  // for (int i = 0; i < 7; ++i) {
  //   bool bit = (number >> i) & 1;
  //   storage.push_back(bit);
  // }
  while (number > 0) {
    storage.push_back(number % 2);
    number /= 2;
  }
  std::reverse(storage.begin(), storage.end());
}

// Получить размер
size_t sdes::bits::size() const { return storage.size(); }

// Преобразовать в строку
std::string sdes::bits::to_string() const {
  std::string text;
  text.reserve(storage.size());
  for (bool i : storage) {
    text += (i ? '1' : '0');
  }
  return text;
}

//Проверка на пустоту
bool sdes::bits::empty() const { return storage.empty(); }

// Преобразовать обратно в число
unsigned sdes::bits::to_unsigned() const {
  unsigned number = 0;
  int power = 0;
  if (storage.empty()) {
    return number;
  }
  for (int i = storage.size() - 1; i >= 0; i--, power++) {
    if (storage[i]) {
      number += std::pow(2, power);
    }
  }
  return number;
}

// Перемешать массив по маске
void sdes::bits::mixing(std::vector<unsigned> mask) {
  std::vector<bool> old_storage(storage);
  storage.clear();
  for (size_t i = 0; i < mask.size(); i++) {
    storage.push_back(old_storage[mask[i]]);
  }
}

// Добавить биты в конец массива
void sdes::bits::push(bool bit) { storage.push_back(bit); }

// Вырезать биты из массива
sdes::bits sdes::bits::cut(unsigned start, unsigned end) {
  bits cutted;
  if ((storage.size() - 1 < end)) {
    while (storage.size() - 1 < end) {
      storage.push_back(0);
    }
    std::reverse(storage.begin(), storage.end());
  }
  if (start == end) {
    return bits({this[start]});
  } else if (start < end) {
    for (auto i = storage.begin() + start; i <= storage.begin() + end; i++) {
      try {
        cutted.push(*i);
      } catch (std::out_of_range &e) {
        std::cout << e.what() << std::endl;
        return cutted;
      }
    }
  } else if (start > end) {
    throw std::range_error("Start bigger then end");
  }
  return cutted;
}

// Объединить два битовых контейнера
void sdes::bits::unite(bits &&right) noexcept {
  storage.reserve(storage.size() + right.storage.size());
  storage.insert(storage.end(), right.storage.begin(), right.storage.end());
  right.storage.clear();
}

// Операторы
std::vector<bool>::reference sdes::bits::operator[](size_t pos) {
  return storage.at(pos);
}

sdes::bits &sdes::bits::operator<<=(size_t count) noexcept {
  for (size_t i = 0; i < count; i++) {
    bool first_bit = *storage.begin();
    for (size_t i = 0; i < storage.size(); i++) {
      if (storage[i]) {
        storage[i] = 0;
        if (i) {
          storage[i - 1] = 1;
        }
      }
    }
    *(storage.end() - 1) = first_bit;
  }
  return *this;
}

sdes::bits &sdes::bits::operator^=(const bits &right) noexcept {
  for (size_t i = 0; i < right.storage.size() && i < storage.size(); i++) {
    bool bit =
        static_cast<bool>(storage[i]) ^ static_cast<bool>(right.storage[i]);
    storage[i] = bit;
  }
  return *this;
}

sdes::bits &sdes::bits::operator=(const bits &right) {
  storage = right.storage;
  return *this;
}

sdes::bits &sdes::bits::operator=(bits &&right) noexcept {
  storage = right.storage;
  right.storage.clear();
  return *this;
}

//Деструктор
sdes::bits::~bits() { storage.clear(); }

// Конструкторы
sdes::sdes() : current_round(1) { cypher_key = get_random_key(); }

sdes::sdes(std::initializer_list<bool> key) : current_round(1) {
  cypher_key = key;
  cypher_key.mixing(key_straight_P_block);
}

sdes::sdes(uint16_t number) {
  for (int i = 0; i < 10; ++i) {
    bool bit = (number >> i) & 1;
    cypher_key.push(bit);
  }
}

// Получаем случайный ключ из генератора случайных чисел
sdes::bits sdes::get_random_key() {
  std::random_device rd;  // Устройство случайности
  std::mt19937 gen(rd()); // Генератор
  std::uniform_int_distribution<> dist(0, 1); // Диапазон распределения
  bits key;

  for (size_t i = 0; i < KEY_LEN; i++) {
    key.push(dist(gen));
  }
  key.mixing(key_straight_P_block);
  return key;
}

uint16_t sdes::get_key() const { return cypher_key.to_unsigned(); }

// Генерируем ключ для нового раунда
sdes::bits sdes::key_gen(unsigned count) {
  count = progression(count);
  // Разделяем ключи
  bits left = cypher_key.cut(0, 4);
  bits right = cypher_key.cut(5, 9);
  // Сдвиг ключа
  left <<= count;
  right <<= count;
  left.unite(std::move(right));
  left.mixing(key_compressed_P_block);
  // Объединение и сжатие ключей
  return left;
}

// S-блоки
sdes::bits sdes::S_blocks(std::vector<sdes::bits> blocks) {
  bits result;
  bits united_nums;
  for (size_t i = 0; i < blocks.size(); i++) {
    bits current_half_block = blocks[i];
    bits left{current_half_block[0], current_half_block[3]};
    bits right{current_half_block[1], current_half_block[2]};
    unsigned row = left.to_unsigned(); // Вырезаем первый и последние биты,
                                       // преобразуем в число от 0 до 3
    unsigned column = right.to_unsigned(); // Вырезаем первый и последние биты,
                                           // преобразуем в число от 0 до 3
    unsigned num =
        S_blocks_tables[i][row]
                       [column]; // Преобразованные числа указывают
                                 // координаты числа из S-блока от 0 до 3
    bits bin_num(num);
    bin_num = bin_num.cut(0, 1);
    united_nums.unite(std::move(bin_num));
  }
  return united_nums;
}

// Функция S-DES
sdes::bits sdes::sdes_function(bits text, bits key) {
  std::cout << "Half text: " << text.to_string() << std::endl;
  text.mixing(P_block_expansion);
  std::cout << "Text expansion: " << text.to_string() << std::endl;
  text ^= key; // XOR с ключом
  std::cout << "Text XOR key: " << text.to_string() << std::endl;
  std::vector<bits> splited_blocks{
      text.cut(0, 3), text.cut(4, 7)}; // Разделяем текст на блоки пополам
  bits result = S_blocks(splited_blocks); // Находим числа по S-блокам
  std::cout << "Text from S-blocks: " << result.to_string() << std::endl;
  result.mixing(P_block_straight); // Прямой S-блок

  return result;
}

sdes::bits sdes::round(bits text, bits key) {
  bits left = text.cut(0, 3);
  bits right = text.cut(4, 7);

  bits XOR_func =
      sdes_function(right,
                    key); // Применить S-DES функцию к правой половине
  std::cout << "Straight P-block: " << XOR_func.to_string() << std::endl;
  left ^= XOR_func; // Искл. ИЛИ левой половины с правой S-DES
                    // функцией
  std::cout << "XOR with first half: " << left.to_string() << std::endl;
  bits result;
  if (current_round > 1) {
    left.unite(std::move(right));
    result = left;
  } else {
    right.unite(std::move(left));
    result = right;
  }
  if (current_round < ROUND_COUNT) {
    current_round++;
  }
  return result;
}

void sdes::print(std::initializer_list<bool> text) {
  std::vector<bool> text_vec(text);
  bits text_bin(text_vec);
  std::cout << "Key: " << cypher_key.to_string() << std::endl;
  first_key = key_gen(current_round);
  std::cout << "First Key: " << first_key.to_string() << std::endl;
  std::cout << "Text: " << text_bin.to_string() << std::endl;
  text_bin.mixing(initial_P_block);
  std::cout << "Initial permutation: " << text_bin.to_string() << std::endl;
  bits first_round = round(text_bin, first_key);
  std::cout << "\nText after first round: " << first_round.to_string()
            << std::endl;
  second_key = key_gen(current_round);
  std::cout << "Second Key: " << second_key.to_string() << std::endl;
  bits second_round = round(first_round, second_key);
  std::cout << "Text after second round: " << second_round.to_string()
            << std::endl;
  second_round.mixing(final_P_block);
  std::cout << "\nFinal permutation: " << second_round.to_string() << std::endl;
}

void sdes::print_rev(std::initializer_list<bool> text) {
  current_round = 1;
  std::vector<bool> text_vec(text);
  bits text_bin(text_vec);
  std::cout << "Key: " << cypher_key.to_string() << std::endl;
  std::cout << "First Key: " << second_key.to_string() << std::endl;
  std::cout << "Text: " << text_bin.to_string() << std::endl;
  text_bin.mixing(initial_P_block);
  std::cout << "Initial permutation: " << text_bin.to_string() << std::endl;
  bits first_round = round(text_bin, second_key);
  std::cout << "\nText after first round: " << first_round.to_string()
            << std::endl;
  std::cout << "Second Key: " << first_key.to_string() << std::endl;
  bits second_round = round(first_round, first_key);
  second_round.mixing(final_P_block);
  std::cout << "\nFinal permutation: " << second_round.to_string() << std::endl;
}

char sdes::encrypt(char number) {
  current_round = 1;
  bits text(number);
  //Начальная перестановка
  text.mixing(initial_P_block);
  //Раунды
  first_key = key_gen(current_round);
  text = round(text, first_key);
  second_key = key_gen(current_round);
  text = round(text, second_key);
  //Конечная перестановка
  text.mixing(final_P_block);
  return text.to_unsigned();
}

char sdes::decrypt(char number) {
  bits text(number);
  //Начальная перестановка
  text.mixing(initial_P_block);
  //Раунды
  second_key = key_gen(ROUND_COUNT);
  text = round(text, second_key);
  first_key = key_gen(1);
  text = round(text, first_key);
  //Конечная перестановка
  text.mixing(final_P_block);
  return text.to_unsigned();
}
