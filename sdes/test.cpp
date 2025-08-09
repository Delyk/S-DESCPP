#include "sdes.h"
#include <gtest/gtest.h>
using namespace std;
using namespace sdes_cypher;

class SDESTest : public ::testing::Test {
protected:
  sdes default_sdes;
  sdes key_sdes = 0b1010011010;
  const std::string text_input_file = "text";
  const std::string text_encrypted_file = "_text";
  const std::string text_decrypted_file = "_text_";

  const std::string bin_input_file = "bin";
  const std::string bin_encrypted_file = "_bin";
  const std::string bin_decrypted_file = "_bin_";
  // Генерация случайной строки заданной длины
  std::string generate_random_text(size_t length) {
    static const char charset[] = "abcdefghijklmnopqrstuvwxyz"
                                  "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                  "0123456789,.!? \n";

    static std::mt19937 rng(std::random_device{}());
    static std::uniform_int_distribution<> dist(0, sizeof(charset) - 2);

    std::string result;
    result.reserve(length);

    for (size_t i = 0; i < length; ++i) {
      result += charset[dist(rng)];
    }
    return result;
  }

  void SetUp() override {
    // Создаём исходный текстовый файл
    std::ofstream ofs_text(text_input_file);
    ofs_text << generate_random_text(1000);
    ofs_text.close();

    // Создаём исходный бинарный файл с некоторыми байтами
    std::ofstream ofs_bin(bin_input_file, std::ios::binary);
    unsigned char data[] = {0x12, 0xAB, 0x00, 0xFF, 0x45, 0x33};
    ofs_bin.write(reinterpret_cast<const char *>(data), sizeof(data));
    ofs_bin.close();
  }

  void TearDown() override {
    // Удаляем все временные файлы, если они существуют
    std::remove(text_input_file.c_str());
    std::remove(text_encrypted_file.c_str());
    std::remove(text_decrypted_file.c_str());
    std::remove(bin_input_file.c_str());
    std::remove(bin_encrypted_file.c_str());
    std::remove(bin_decrypted_file.c_str());
  }

  // Сравнение файлов
  static bool CompareFiles(const std::string &file1, const std::string &file2,
                           bool binary = false) {
    std::ifstream f1(file1, binary ? std::ios::binary : std::ios::in);
    std::ifstream f2(file2, binary ? std::ios::binary : std::ios::in);
    if (!f1.is_open() || !f2.is_open())
      return false;

    std::istreambuf_iterator<char> begin1(f1), end1;
    std::istreambuf_iterator<char> begin2(f2);

    return std::equal(begin1, end1, begin2);
  }
};

// Инициализация ключа
TEST_F(SDESTest, KeyInitialization) {
  EXPECT_EQ(key_sdes.getKey(), 0b1010011010);
}

// Автогенерация ключа
TEST_F(SDESTest, DefaultConstructorKeyNotZero) {
  EXPECT_NE(default_sdes.getKey(), 0);
  std::cout << "Key: " << default_sdes.getKey() << std::endl;
}

// Шифрование, расшифрование символа
TEST_F(SDESTest, EncryptDecryptCharIdentity) {
  char original = 'A';
  char encrypted = key_sdes.encrypt(original);
  char decrypted = key_sdes.decrypt(encrypted);
  EXPECT_EQ(decrypted, original);
}

// Шифрованный текст отличается от оригинального
TEST_F(SDESTest, EncryptDifferentFromOriginal) {
  char original = 'B';
  char encrypted = key_sdes.encrypt(original);
  EXPECT_NE(encrypted, original);
}

// Повторное шифрование одного и того же символа с одним и тем же ключом даёт
// одинаковый результат
TEST_F(SDESTest, EncryptConsistent) {
  char original = 'C';
  char encrypted1 = key_sdes.encrypt(original);
  char encrypted2 = key_sdes.encrypt(original);
  EXPECT_EQ(encrypted1, encrypted2);
}

// Повторное дешифрование одного и того же символа с одним и тем же ключом
// даёт одинаковый результат
TEST_F(SDESTest, DecryptConsistent) {
  char original = 'D';
  char encrypted = key_sdes.encrypt(original);
  char decrypted1 = key_sdes.decrypt(encrypted);
  char decrypted2 = key_sdes.decrypt(encrypted);
  EXPECT_EQ(decrypted1, decrypted2);
}

// Шифрование текстового файла
TEST_F(SDESTest, TextFileEncryptionDecryption) {
  // Шифруем текстовый файл
  default_sdes.cypher_textfile(text_input_file);

  // Дешифруем обратно
  default_sdes.decypher_textfile(text_encrypted_file);

  // Сравниваем исходный и дешифрованный файлы
  EXPECT_TRUE(CompareFiles(text_decrypted_file, text_input_file));
}

// Шифрование бинарного файла
TEST_F(SDESTest, BinaryFileEncryptionDecryption) {
  default_sdes.cypher_binfile(bin_input_file);

  default_sdes.decypher_binfile(bin_encrypted_file);

  EXPECT_TRUE(CompareFiles(bin_decrypted_file, bin_input_file, true));
}

TEST_F(SDESTest, RandomKeyAndTextEncryptDecrypt) {

  // Проверяем, что ключ не равен нулю (или другому фиксированному значению)
  EXPECT_NE(default_sdes.getKey(), 0);

  // Генерируем случайный текст для шифрования
  const size_t text_len = 1000;
  std::string original_text = generate_random_text(text_len);

  // Шифруем каждый символ текста
  std::string encrypted_text;
  encrypted_text.reserve(text_len);
  for (char c : original_text) {
    encrypted_text += default_sdes.encrypt(c);
  }

  // Дешифруем зашифрованный текст
  std::string decrypted_text;
  decrypted_text.reserve(text_len);
  for (char c : encrypted_text) {
    decrypted_text += default_sdes.decrypt(c);
  }

  // Проверяем, что исходный и расшифрованный тексты совпадают
  EXPECT_EQ(decrypted_text, original_text);
}

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}
