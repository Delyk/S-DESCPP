#include "sdes.h"
#include <iostream>
#include <stdexcept>
using namespace sdes_cypher;

int main() {
  sdes example; //Ключ генерируется случайно
  try {
    sdes example1(
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}); //Ключ задаётся в виде списка чисел
  } catch (std::range_error &e) {
    std::cout << e.what() << std::endl;
  }
  sdes example2(0b0110100101); //Ключ в виде числа
  std::cout << "Ключ: " << example2.getKey() << std::endl; //Получить ключ

  //Зашифровать/дешифровать с подробным выводом
  try {
    example2.print_crypt({1, 0, 1, 0, 1, 0, 1, 0});
    example2.print_decrypt({1, 0, 1, 1, 0, 1, 1, 1});
  } catch (std::range_error &e) {
    std::cout << e.what() << std::endl;
  }

  char a = 'a';
  std::cout << "\nСимвол: " << a << std::endl;
  char crypted_a = example2.encrypt(a); //Зашифровать один символ
  std::cout << "Зашифрованный: " << crypted_a << std::endl;
  char decrypted_a = example2.decrypt(crypted_a); //Дешифровать символ
  std::cout << "Дешифрованный: " << decrypted_a << std::endl;

  std::cout << "Текстовый файл: " << std::endl;
  example2.print_textfile("text"); //Вывод содержимого файла
  example2.cypher_textfile("text"); //Зашифровать текст в файле
  std::cout << "Зашифрованный файл: " << std::endl;
  example2.print_textfile("_text"); //Вывод содержимого файла
  example2.decypher_textfile("_text"); //Расшифровать текст в файле
  std::cout << "Дешифрованный файл: " << std::endl;
  example2.print_textfile("_text_"); //Вывод содержимого файла

  std::cout << "Файл: " << std::endl;
  example2.print_textfile("text"); //Вывод содержимого файла
  example2.cypher_binfile("text"); //Зашифровать бинарный файл
  std::cout << "Зашифрованный бинарный файл: " << std::endl;
  example2.print_textfile("_text"); //Вывод содержимого файла
  example2.decypher_binfile("_text"); //Расшифровать бинарный файл
  std::cout << "Дешифрованный бинарный файл: " << std::endl;
  example2.print_textfile("_text_"); //Вывод содержимого файла

  return 0;
}
