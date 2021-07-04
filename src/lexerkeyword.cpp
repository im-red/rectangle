/*********************************************************************************
 * Copyright (C) 2020  Jia Lihong
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 ********************************************************************************/

#include "lexer.h"

namespace rectangle {
namespace frontend {

static inline Token::TokenType classify2(const char *s) {
  if (s[0] == 'i') {
    if (s[1] == 'f') {
      return Token::T_IF;
    }
  }
  return Token::T_IDENTIFIER;
}

static inline Token::TokenType classify3(const char *s) {
  if (s[0] == 'd') {
    if (s[1] == 'e') {
      if (s[2] == 'f') {
        return Token::T_DEF;
      }
    }
  }
  if (s[0] == 'i') {
    if (s[1] == 'n') {
      if (s[2] == 't') {
        return Token::T_INT;
      }
    }
  }
  return Token::T_IDENTIFIER;
}

static inline Token::TokenType classify4(const char *s) {
  if (s[0] == 'e') {
    if (s[1] == 'l') {
      if (s[2] == 's') {
        if (s[3] == 'e') {
          return Token::T_ELSE;
        }
      }
    }
    if (s[1] == 'n') {
      if (s[2] == 'u') {
        if (s[3] == 'm') {
          return Token::T_ENUM;
        }
      }
    }
  }
  if (s[0] == 'l') {
    if (s[1] == 'i') {
      if (s[2] == 's') {
        if (s[3] == 't') {
          return Token::T_LIST;
        }
      }
    }
  }
  if (s[0] == 'v') {
    if (s[1] == 'o') {
      if (s[2] == 'i') {
        if (s[3] == 'd') {
          return Token::T_VOID;
        }
      }
    }
  }
  return Token::T_IDENTIFIER;
}

static inline Token::TokenType classify5(const char *s) {
  if (s[0] == 'b') {
    if (s[1] == 'r') {
      if (s[2] == 'e') {
        if (s[3] == 'a') {
          if (s[4] == 'k') {
            return Token::T_BREAK;
          }
        }
      }
    }
  }
  if (s[0] == 'f') {
    if (s[1] == 'l') {
      if (s[2] == 'o') {
        if (s[3] == 'a') {
          if (s[4] == 't') {
            return Token::T_FLOAT;
          }
        }
      }
    }
  }
  if (s[0] == 'w') {
    if (s[1] == 'h') {
      if (s[2] == 'i') {
        if (s[3] == 'l') {
          if (s[4] == 'e') {
            return Token::T_WHILE;
          }
        }
      }
    }
  }
  return Token::T_IDENTIFIER;
}

static inline Token::TokenType classify6(const char *s) {
  if (s[0] == 'r') {
    if (s[1] == 'e') {
      if (s[2] == 't') {
        if (s[3] == 'u') {
          if (s[4] == 'r') {
            if (s[5] == 'n') {
              return Token::T_RETURN;
            }
          }
        }
      }
    }
  }
  if (s[0] == 's') {
    if (s[1] == 't') {
      if (s[2] == 'r') {
        if (s[3] == 'i') {
          if (s[4] == 'n') {
            if (s[5] == 'g') {
              return Token::T_STRING;
            }
          }
        }
      }
    }
  }
  return Token::T_IDENTIFIER;
}

static inline Token::TokenType classify8(const char *s) {
  if (s[0] == 'c') {
    if (s[1] == 'o') {
      if (s[2] == 'n') {
        if (s[3] == 't') {
          if (s[4] == 'i') {
            if (s[5] == 'n') {
              if (s[6] == 'u') {
                if (s[7] == 'e') {
                  return Token::T_CONTINUE;
                }
              }
            }
          }
        }
      }
    }
  }
  return Token::T_IDENTIFIER;
}

Token::TokenType Lexer::classify(const char *s, int n) {
  switch (n) {
    case 2:
      return classify2(s);
    case 3:
      return classify3(s);
    case 4:
      return classify4(s);
    case 5:
      return classify5(s);
    case 6:
      return classify6(s);
    case 8:
      return classify8(s);
    default:
      return Token::T_IDENTIFIER;
  }
}

}  // namespace frontend
}  // namespace rectangle
