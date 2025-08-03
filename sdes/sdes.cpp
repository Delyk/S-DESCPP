#include "sdes.h"
#include <algorithm>
#include <bitset>
#include <cstddef>
#include <iostream>
#include <ostream>
#include <random>
#include <string>
#include <vector>
using namespace sdes_cypher;

// Внутренний класс bits
// Конструкторы
sdes::bits::bits(size_t size) {
  storage.resize(size);
  std::fill(storage.begin(), storage.end(), 0);
}

sdes::bits::bits(std::vector<bool> bit_num) { storage = bit_num; }

sdes::bits::bits(unsigned number) {
  for (size_t i = storage.size() - 1; i >= 0; i--) {
    storage[i] = ((number << i) & 1);
  }
}

sdes::bits::bits(char number) {
  for (size_t i = storage.size() - 1; i >= 0; i--) {
    storage[i] = ((number << i) & 1);
  }
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

// Преобразовать обратно в число
unsigned sdes::bits::to_unsigned() const {
  unsigned number = 0;
  for (size_t i = 0; i < storage.size(); i++) {
    if (storage[i]) {
      number |= (1 << i);
    }
  }
  return number;
}

// Перемешать массив по маске
void sdes::bits::mixing(unsigned mask[]) {
  std::vector<bool> old_storage(storage);
  for (size_t i = 0; i < storage.size(); i++) {
    storage[i] = old_storage[mask[i]];
  }
}

// Добавить биты в конец массива
void sdes::bits::push(bool bit) { storage.push_back(bit); }

// Вырезать биты из массива
sdes::bits sdes::bits::cut(unsigned start, unsigned end) {
  size_t size = end - start;
  bits cutted(size);
  for (size_t i = start; i <= end; i++) {
    cutted.push(storage[i]);
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

sdes::bits &sdes::bits::operator<<=(size_t) noexcept {
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

// Разделить блок на части
template <unsigned long bit_count, unsigned long split_count>
std::vector<std::bitset<bit_count / split_count>>
sdes::split(std::bitset<bit_count> block) {
  const unsigned long bits = bit_count / split_count;
  std::vector<std::bitset<bits>> blocks;

  for (size_t i = 0; i < split_count; i++) {
    std::bitset<bits> tmp_block; // Временный блок для хранения одной части
    for (size_t j = 0, k = bits * i; j < bits; j++) {
      tmp_block[j] =
          block[k + j]; // Последовательно записываем в блок биты (k -
                        // коэфициент смещения по базовому блоку, кол-во битов в
                        // подблоке на текущий подблок)
    }
    blocks.push_back(tmp_block); // Созданные блоки помещаем в массив
  }

  return blocks;
}

// Соединить части в блок
template <unsigned long bit_count, unsigned long split_count>
std::bitset<bit_count>
sdes::unite(std::vector<std::bitset<bit_count / split_count>> blocks) {
  std::bitset<bit_count> block;
  int k = 0;
  for (auto i : blocks) {
    for (size_t j = 0; j < i.size(); j++) {
      block[k] =
          i[j]; // k - индекс в основном блоке, j - локальный индекс в подблоке
      k++;      // Индекс k увеличивается постоянно
    }
  }
  return block;
}

// Вырезать биты
template <size_t N>
unsigned long sdes::cut(std::bitset<N> bits, unsigned first, unsigned second) {
  std::bitset<2> cutted;
  cutted[0] = bits[first];
  cutted[1] = bits[second];
  return cutted.to_ulong();
}

// Блоки
//  P-блок сжатия
std::bitset<BLOCK_LEN> sdes::key_compress(std::bitset<KEY_LEN> block) {
  std::bitset<BLOCK_LEN> compressed_block;

  for (size_t i = 0; i < compressed_block.size(); i++) {
    compressed_block[i] = block[key_compressed_P_block[i]];
  }

  return compressed_block;
}

// Начальная перестановка
std::bitset<BLOCK_LEN> sdes::initial_permutation(std::bitset<BLOCK_LEN> text) {
  std::bitset<BLOCK_LEN> reverse_text = text;
  std::bitset<BLOCK_LEN> final_text;
  for (size_t i = 0; i < final_text.size(); i++) {
    final_text[i] = reverse_text[initial_P_block[i]];
  }

  return final_text;
}

// Конечная перестановка
std::bitset<BLOCK_LEN> sdes::final_permutation(std::bitset<BLOCK_LEN> text) {
  std::bitset<BLOCK_LEN> reverse_text = text;
  std::bitset<BLOCK_LEN> final_text;
  for (size_t i = 0; i < final_text.size(); i++) {
    final_text[i] = reverse_text[final_P_block[i]];
  }

  return final_text;
}

// P-блок расширения
std::bitset<BLOCK_LEN>
sdes::expansion_P_block(std::bitset<BLOCK_LEN / 2> text) {
  std::bitset<BLOCK_LEN / 2> reverse_text = text;
  std::bitset<BLOCK_LEN> final_text;
  for (size_t i = 0; i < final_text.size(); i++) {
    final_text[i] = reverse_text[P_block_expansion[i]];
  }

  return final_text;
}

// Прямой P-блок
std::bitset<BLOCK_LEN / 2>
sdes::straight_P_block(std::bitset<BLOCK_LEN / 2> block) {
  std::bitset<BLOCK_LEN / 2> reverse_block = block;
  std::bitset<BLOCK_LEN / 2> final_block;
  for (size_t i = 0; i < block.size(); i++) {
    final_block[i] = reverse_block[P_block_straight[i]];
  }

  return final_block;
}

// S-блоки
std::bitset<BLOCK_LEN / 2>
sdes::S_blocks(std::vector<std::bitset<BLOCK_LEN / 2>> blocks) {
  std::bitset<BLOCK_LEN / 2> result;
  std::vector<std::bitset<2>> united_nums;
  for (size_t i = 0; i < blocks.size(); i++) {
    std::bitset<BLOCK_LEN / 2> current_half_block = blocks[i];
    unsigned long row = cut(
        current_half_block, 0,
        3); // Вырезаем первый и последние биты, преобразуем в число от 0 до 3
    unsigned long column =
        cut(current_half_block, 1,
            2); // Вырезаем второй и третий биты, преобразуем в число от 0 до 3
    unsigned num =
        S_blocks_tables[i][row]
                       [column]; // Преобразованные числа указывают
                                 // координаты числа из S-блока от 0 до 3
    std::bitset<2> bin_num(
        num); // Выбранное число преобразуем в двоичное представление
    bin_num = bin_num;
    united_nums.push_back(bin_num);
  }
  return unite<BLOCK_LEN / 2>(united_nums); // Объединяем разделённый блок
}

// Конструкторы
sdes::sdes() : current_round(1) { cypher_key = get_random_key(); }

sdes::sdes(std::bitset<KEY_LEN> key) : current_round(1) {
  for (size_t i = 0; i < key.size(); i++) {
    cypher_key[i] = key[key_straight_P_block[i]];
  }
}

// Получаем случайный ключ из генератора случайных чисел
std::bitset<KEY_LEN> sdes::get_random_key() {
  std::random_device rd;                      // Устройство случайности
  std::mt19937 gen(rd());                     // Генератор
  std::uniform_int_distribution<> dist(0, 1); // Диапазон распределения
  std::bitset<KEY_LEN> key;

  for (size_t i = 0; i < KEY_LEN; i++) {
    key[i] = dist(gen);
  }
  std::bitset<KEY_LEN> tmp_key;
  // Прямой P-блок для ключа
  for (size_t i = 0; i < KEY_LEN; i++) {
    tmp_key[i] = key[key_straight_P_block[i]];
  }
  return tmp_key;
}

// Генерируем ключ для нового раунда
std::bitset<BLOCK_LEN> sdes::key_gen() {
  std::bitset<BLOCK_LEN> round_key;
  // Разделяем ключи
  std::vector<std::bitset<KEY_LEN / 2>> split_block =
      split<KEY_LEN>(cypher_key);
  // Сдвиг ключа
  for (auto it = split_block.begin(); it != split_block.end(); it++) {
    *it <<= progression(current_round);
  }
  // Объединение и сжатие ключей
  round_key = key_compress(unite<KEY_LEN>(split_block));
  if (current_round < ROUND_COUNT) {
    current_round++;
  }
  return round_key;
}

// Функция S-DES
std::bitset<BLOCK_LEN / 2> sdes::sdes_function(std::bitset<BLOCK_LEN / 2> text,
                                               std::bitset<BLOCK_LEN> key) {
  std::cout << "Half text: " << text.to_string() << std::endl;
  std::bitset<BLOCK_LEN> extended_text =
      expansion_P_block(text); // P-блок расширения
  std::cout << "Text expansion: " << extended_text.to_string() << std::endl;
  extended_text ^= key; // XOR с ключом
  std::cout << "Text XOR key: " << extended_text.to_string() << std::endl;
  extended_text = extended_text;
  std::vector<std::bitset<BLOCK_LEN / 2>> splited_blocks =
      split(extended_text); // Разделяем текст на блоки пополам
  std::bitset<BLOCK_LEN / 2> result =
      S_blocks(splited_blocks); // Находим числа по S-блокам
  std::cout << "Text from S-blocks: " << result << std::endl;
  result = straight_P_block(result); // Прямой S-блок

  return result;
}

std::bitset<BLOCK_LEN> sdes::round(std::bitset<BLOCK_LEN> text,
                                   std::bitset<BLOCK_LEN> key) {
  std::vector<std::bitset<BLOCK_LEN / 2>> split_block =
      split(text); // Разделить текст

  std::bitset<BLOCK_LEN / 2> XOR_func =
      sdes_function(split_block[1],
                    key); // Применить S-DES функцию к правой половине
  std::cout << "Straight P-block: " << split_block[1] << std::endl;
  split_block[0] ^= XOR_func; // Искл. ИЛИ левой половины с правой S-DES
                              // функцией
  std::cout << "XOR with first half: " << split_block[0] << std::endl;
  std::iter_swap(split_block.begin(),
                 split_block.end() -
                     1); // Поменять местами левую и правую половину

  std::bitset<BLOCK_LEN> result = unite(split_block);

  return result;
}

void sdes::print(std::bitset<BLOCK_LEN> text) {
  std::cout << "Key: " << cypher_key.to_string() << std::endl;
  std::bitset<BLOCK_LEN> first_key = key_gen();
  std::cout << "First Key: " << first_key << std::endl;
  std::cout << "Text: " << text.to_string() << std::endl;
  std::bitset<BLOCK_LEN> initial = initial_permutation(text);
  std::cout << "Initial permutation: " << initial.to_string() << std::endl;
  std::bitset<BLOCK_LEN> first_round = round(initial, first_key);
  std::cout << "\nText after first round: " << first_round << std::endl;
  std::bitset<BLOCK_LEN> second_key = key_gen();
  std::cout << "Second Key: " << second_key << std::endl;
  std::bitset<BLOCK_LEN> second_round = round(first_round, second_key);
  std::cout << "\nText after second round: " << second_round << std::endl;
}
