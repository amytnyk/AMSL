#include "amsl.hpp"
#include "lexer.hpp"

int main() {
  #include SOURCE_FILE
//  AMSL{}.execute<source>();
  auto tokens = Lexer{source}.tokenize();
  return 0;
}
