#include "sdes.h"
#include <bitset>
#include <cstddef>
#include <iostream>
#include <random>

// Инвертировать расположение битов
template <unsigned long num>
std::bitset<num> sdes::reverse(std::bitset<num> bits) {
  std::bitset<num> reversed;
  for (unsigned long i = num - 1, j = 0; i >= 0 && j < num; i--, j++) {
    reversed[j] = bits[i];
  }
  return reversed;
}

// Разделить блок на части
template <unsigned long bit_count, unsigned long split_count>
std::vector<std::bitset<bit_count / split_count>>
sdes::split(std::bitset<bit_count> block) {
  const unsigned long bits = bit_count / split_count;
  std::vector<std::bitset<bits>> blocks;

  for (size_t i = 0; i < split_count; i++) {
    std::bitset<bits> tmp_block; //Временный блок для хранения одной части
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

//Соединить части в блок
template <unsigned long bit_count, unsigned long split_count>
std::bitset<bit_count>
sdes::unite(std::vector<std::bitset<bit_count / split_count>> blocks) {
  std::bitset<bit_count> block;
  int k = 0;
  for (auto i : blocks) {
    for (size_t j = 0; j < i.size(); j++) {
      block[k] =
          i[j]; // k - индекс в основном блоке, j - локальный индекс в подблоке
      k++;      //Индекс k увеличивается постоянно
    }
  }
  return block;
}

//Конструкторы
sdes::sdes() : round(1) { cypher_key = get_random_key(); }

sdes::sdes(std::bitset<KEY_LEN> key) : round(1) {
  key = reverse<KEY_LEN>(key);
  for (size_t i = 0; i < KEY_LEN; i++) {
    cypher_key[i] = key[key_straight_P_block[i]];
  }
}

//Получаем случайный ключ из генератора случайных чисел
std::bitset<KEY_LEN> sdes::get_random_key() {
  std::random_device rd;  //Устройство случайности
  std::mt19937 gen(rd()); //Генератор
  std::uniform_int_distribution<> dist(0, 1); //Диапазон распределения
  std::bitset<KEY_LEN> key;

  for (size_t i = 0; i < KEY_LEN; i++) {
    key[i] = dist(gen);
  }
  key = reverse<KEY_LEN>(key); //Инвертируем сгенерированный ключ
  std::bitset<KEY_LEN> tmp_key;
  //Прямой P-блок для ключа
  for (size_t i = 0; i < KEY_LEN; i++) {
    tmp_key[i] = key[key_straight_P_block[i]];
  }
  return tmp_key;
}

// P-блок сжатия
std::bitset<BLOCK_LEN> sdes::key_compress(std::bitset<KEY_LEN> block) {
  std::bitset<BLOCK_LEN> compressed_block;

  for (size_t i = 0; i < compressed_block.size(); i++) {
    compressed_block[i] = block[key_compressed_P_block[i]];
  }

  return compressed_block;
}

//Генерируем ключ для нового раунда
std::bitset<BLOCK_LEN> sdes::key_gen() {
  std::bitset<BLOCK_LEN> round_key;
  //Разделяем ключи
  std::vector<std::bitset<KEY_LEN / 2>> split_block =
      split<KEY_LEN>(cypher_key);
  //Сдвиг ключа
  for (auto it = split_block.begin(); it != split_block.end(); it++) {
    *it >>= round;
  }
  //Объединение и сжатие ключей
  round_key = key_compress(unite<KEY_LEN>(split_block));
  if (round < ROUND_COUNT) {
    round++;
  }
  return round_key;
}

void sdes::print() {
  std::cout << "Key: " << reverse<KEY_LEN>(cypher_key).to_string() << std::endl;
  std::cout << "First Key: " << reverse(key_gen()).to_string() << std::endl;
  std::cout << "Second Key: " << reverse(key_gen()).to_string() << std::endl;
}
