#include <iostream>
#include <cstddef>
#include <iomanip>
#include <cctype>

namespace malashenko {
  char * createStr(size_t size) {
    char * str  = nullptr;
    str = new char[size];
    return str;
  }

  char * updateStr(char * oldStr, size_t oldSize) {
    char * newStr = createStr(oldSize * 2);
    for (size_t i = 0; i < oldSize; ++i) {
      newStr[i] = oldStr[i];
    }
    delete[] oldStr;
    return newStr;
  }

  std::istream & getLine(std::istream & in, char ** str, size_t & size) {
    bool is_skipws = in.flags() & std::ios_base::skipws;
    if (is_skipws) {
      in >> std::noskipws;
    }

    size_t i = 0;
    char sym = 0;

    while (in >> sym && sym != '\n') {
      if (i == size - 1) {
        *str = updateStr(*str, size);
        size *= 2;
      }
      (*str)[i++] = sym;
    }
    (*str)[i] = '\0';
    return in;

    if (is_skipws) {
      in >> std::skipws;
    }
  }

  size_t repDgt(char * str, size_t size) {
    int nums[10] = {};
    size_t numSize = 0;

    for (size_t i = 0; i < size; ++i) {
      if (isdigit(str[i])) {
        size_t j = 0;
        for (; j < numSize; ++j) {
          if (nums[j] == str[i]) {
            return 1;
          }
        }
        nums[j] = str[i];
        numSize++;
      }
    }
    return 0;
  }


  char * rmvVol(char * str, size_t size) {
    char vols[] = "aeiouAEIOU";
    char * newStr = createStr(size);
    size_t strLen = 0;
    for (size_t i = 0; i < size; ++i) {
      bool isVol = 0;
      if (std::isalpha(str[i])) {
        for (size_t j = 0; j < 10; ++j) {
          isVol = isVol || (str[i] == vols[j]);
        }
      }
      if (isVol == 0) {
        newStr[strLen] = str[i];
        strLen++;
      }
    }
    newStr[strLen] = '\0';
    return newStr;
  }
}

int main() {
  namespace mal = malashenko;

  size_t size = 8;
  char * str = nullptr;
  str = mal::createStr(size);
  if (!str) {
    delete[] str;
    std::cerr << "problem with memory allocation\n";
    return 1;
  }

  if (!mal::getLine(std::cin, &str, size)) {
    delete[] str;
    std::cerr << "problem with input\n";
    return 1;
  }

  size_t isRepDgt = mal::repDgt(str, size);
  char * noVolStr = mal::rmvVol(str, size);

  std::cout << isRepDgt << "\n";
  std::cout << noVolStr << "\n";

  delete[] str;
  delete[] noVolStr;
  return 0;
}
