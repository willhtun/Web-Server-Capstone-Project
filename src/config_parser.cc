// An nginx config file parser.
//
// See:
//   http://wiki.nginx.org/Configuration
//   http://blog.martinfjordvald.com/2010/07/nginx-primer/
//
// How Nginx does it:
//   http://lxr.nginx.org/source/src/core/ngx_conf_file.c

#include <cstdio>
#include <fstream>
#include <iostream>
#include <memory>
#include <stack>
#include <string>
#include <vector>

#include "server_object.h"
#include "config_parser.h"

std::string NginxConfig::ToString(int depth) {
  std::string serialized_config;
  for (const auto& statement : statements_) {
    serialized_config.append(statement->ToString(depth));
  }
  return serialized_config;
}

// test review
void NginxConfig::GetServerObject() {
  for (const auto& statement : statements_) {
    if (statement->ToString(0).substr(0,6) == "http {") {
      std::size_t index = statement->ToString(0).find("server {");
      if (index != std::string::npos) {
        int i = 0;
        std::string temp_str = statement->ToString(0);
        int opened_brackets = 1;
        int count = 0;
        while (count < temp_str.length()) {
          if (temp_str[index + i] == '}' && opened_brackets == 1) {
            i++;
            break;
          }
          if (temp_str[index + i] == '{')
            opened_brackets++;
          if (temp_str[index + i] == '}')
            opened_brackets--;
          i++;
          count++;
        }
        this->LoadServerObject(temp_str.substr(index, index+i)); // Pass in the server {...} block to extract the parameters
      }
    }
  }
}

void NginxConfig::LoadServerObject(std::string config_string) {
  // Only loads port number for now
  NginxConfigParser parser;
  Server_o* output = new Server_o;

  // Port
  std::size_t index = config_string.find("listen");
  if (index != std::string::npos) {
    int ending_pos = 0;
    while (config_string[index + 6 + ending_pos] != ';') {
      if ((index + 6 + ending_pos) >= config_string.length()) // 6 to move to end of listen
        return;
      ending_pos++;
    }
    try {
      ServerObject::port = std::atoi(config_string.substr(index + 7, ending_pos).c_str()); // start reading the value 1 space after listen
    }
    catch (std::exception& e) {
      return;
    }
  }

  // Static files directory
  for (int i = 0; i < 10; i++) { 
    int dir_index = config_string.find("directory {");
    if (dir_index == std::string::npos) {
      break;
    }
    int dir_length = dir_index;
    while (config_string[dir_length] != '}')
      dir_length++;
    std::string dir_block = config_string.substr(dir_index, dir_length);

    index = dir_block.find("location");
    if (index != std::string::npos) {
      int ending_pos = 0;
      while (dir_block[index + 8 + ending_pos] != ';') {
        if ((index + 8 + ending_pos) >= dir_block.length())
          return;
        ending_pos++;
      }
      try {
        if (index + 8 >= ending_pos) // 8 to move to end of location
          ServerObject::staticfile_dir.push_back(dir_block.substr(index + 9, ending_pos - 1)); // start reading the value 1 space after location
      }
      catch (std::exception& e) {
        return;
      }
    }

    index = dir_block.find("url");
    if (index != std::string::npos) {
      int ending_pos = 0;
      while (dir_block[index + 3 + ending_pos] != ';') {
        if ((index + 3 + ending_pos) >= dir_block.length())
          return;
        ending_pos++;
      }
      try {
        if (index + 3 >= ending_pos) // 8 to move to end of location
          ServerObject::staticfile_url.push_back(dir_block.substr(index + 4, ending_pos - 1)); // start reading the value 1 space after location
      }
      catch (std::exception& e) {
        return;
      }
    }
    config_string = config_string.substr(dir_index + 10, config_string.length() - dir_index + 10);
  }
  return;
}


std::string NginxConfigStatement::ToString(int depth) {
  std::string serialized_statement;
  for (int i = 0; i < depth; ++i) {
    serialized_statement.append("  ");
  }
  for (unsigned int i = 0; i < tokens_.size(); ++i) {
    if (i != 0) {
      serialized_statement.append(" ");
    }
    serialized_statement.append(tokens_[i]);
  }
  if (child_block_.get() != nullptr) {
    serialized_statement.append(" {\n");
    serialized_statement.append(child_block_->ToString(depth + 1));
    for (int i = 0; i < depth; ++i) {
      serialized_statement.append("  ");
    }
    serialized_statement.append("}");
  } else {
    serialized_statement.append(";");
  }
  serialized_statement.append("\n");
  return serialized_statement;
}

const char* NginxConfigParser::TokenTypeAsString(TokenType type) {
  switch (type) {
    case TOKEN_TYPE_START:         return "TOKEN_TYPE_START";
    case TOKEN_TYPE_NORMAL:        return "TOKEN_TYPE_NORMAL";
    case TOKEN_TYPE_START_BLOCK:   return "TOKEN_TYPE_START_BLOCK";
    case TOKEN_TYPE_END_BLOCK:     return "TOKEN_TYPE_END_BLOCK";
    case TOKEN_TYPE_COMMENT:       return "TOKEN_TYPE_COMMENT";
    case TOKEN_TYPE_STATEMENT_END: return "TOKEN_TYPE_STATEMENT_END";
    case TOKEN_TYPE_EOF:           return "TOKEN_TYPE_EOF";
    case TOKEN_TYPE_ERROR:         return "TOKEN_TYPE_ERROR";
    default:                       return "Unknown token type";
  }
}

NginxConfigParser::TokenType NginxConfigParser::ParseToken(std::istream* input,
                                                           std::string* value) {
  TokenParserState state = TOKEN_STATE_INITIAL_WHITESPACE;
  while (input->good()) {
    const char c = input->get();
    if (!input->good()) {
      break;
    }
    switch (state) {
      case TOKEN_STATE_INITIAL_WHITESPACE:
        switch (c) {
          case '{':
            *value = c;
            return TOKEN_TYPE_START_BLOCK;
          case '}':
            *value = c;
            return TOKEN_TYPE_END_BLOCK;
          case '#':
            *value = c;
            state = TOKEN_STATE_TOKEN_TYPE_COMMENT;
            continue;
          case '"':
            *value = c;
            state = TOKEN_STATE_DOUBLE_QUOTE;
            continue;
          case '\'':
            *value = c;
            state = TOKEN_STATE_SINGLE_QUOTE;
            continue;
          case ';':
            *value = c;
            return TOKEN_TYPE_STATEMENT_END;
          case ' ':
          case '\t':
          case '\n':
          case '\r':
            continue;
          default:
            *value += c;
            state = TOKEN_STATE_TOKEN_TYPE_NORMAL;
            continue;
        }
      case TOKEN_STATE_SINGLE_QUOTE:
        // TODO: the end of a quoted token should be followed by whitespace.
        // TODO: Maybe also define a QUOTED_STRING token type.
        // TODO: Allow for backslash-escaping within strings.
        *value += c;
        if (c == '\'') {
          return TOKEN_TYPE_NORMAL;
        }
        continue;
      case TOKEN_STATE_DOUBLE_QUOTE:
        *value += c;
        if (c == '"') {
          return TOKEN_TYPE_NORMAL;
        }
        continue;
      case TOKEN_STATE_TOKEN_TYPE_COMMENT:
        if (c == '\n' || c == '\r') {
          return TOKEN_TYPE_COMMENT;
        }
        *value += c;
        continue;
      case TOKEN_STATE_TOKEN_TYPE_NORMAL:
        if (c == ' ' || c == '\t' || c == '\n' || c == '\t' ||
            c == ';' || c == '{' || c == '}') {
          input->unget();
          return TOKEN_TYPE_NORMAL;
        }
        *value += c;
        continue;
    }
  }

  // If we get here, we reached the end of the file.
  if (state == TOKEN_STATE_SINGLE_QUOTE ||
      state == TOKEN_STATE_DOUBLE_QUOTE) {
    return TOKEN_TYPE_ERROR;
  }

  return TOKEN_TYPE_EOF;
}

bool NginxConfigParser::Parse(std::istream* config_file, NginxConfig* config) {
  std::stack<NginxConfig*> config_stack;
  config_stack.push(config);
  TokenType last_token_type = TOKEN_TYPE_START;
  TokenType token_type;
  int bracket_count = 0;

  while (true) {
    std::string token;
    token_type = ParseToken(config_file, &token);
    //printf ("%s: %s\n", TokenTypeAsString(token_type), token.c_str());
    if (token_type == TOKEN_TYPE_ERROR) {
      break;
    }

    if (token_type == TOKEN_TYPE_COMMENT) {
      // Skip comments.
      continue;
    }

    if (token_type == TOKEN_TYPE_START) {
      // Error.
      break;
    } else if (token_type == TOKEN_TYPE_NORMAL) {
      if (last_token_type == TOKEN_TYPE_START ||
          last_token_type == TOKEN_TYPE_STATEMENT_END ||
          last_token_type == TOKEN_TYPE_START_BLOCK ||
          last_token_type == TOKEN_TYPE_END_BLOCK ||
          last_token_type == TOKEN_TYPE_NORMAL) {
        if (last_token_type != TOKEN_TYPE_NORMAL) {
          config_stack.top()->statements_.emplace_back(
              new NginxConfigStatement);
        }
        config_stack.top()->statements_.back().get()->tokens_.push_back(
            token);
      } else {
        // Error.
        break;
      }
    } else if (token_type == TOKEN_TYPE_STATEMENT_END) {
      if (last_token_type != TOKEN_TYPE_NORMAL) {
        // Error.
        break;
      }
    } else if (token_type == TOKEN_TYPE_START_BLOCK) {
      bracket_count++;

      if (last_token_type != TOKEN_TYPE_NORMAL) {
        // Error.
        break;
      }
      NginxConfig* const new_config = new NginxConfig;
      config_stack.top()->statements_.back().get()->child_block_.reset(
          new_config);
      config_stack.push(new_config);
    } else if (token_type == TOKEN_TYPE_END_BLOCK) {
      bracket_count--;

      // let }} pass for nested blocks
      if (last_token_type != TOKEN_TYPE_STATEMENT_END &&
          last_token_type != TOKEN_TYPE_END_BLOCK && 
          last_token_type != TOKEN_TYPE_START_BLOCK) {
        // Error.
        break;
      }

      // check too many closing brackets
      if (bracket_count < 0) {
       printf("Incorrect number of brackets, too many closing brackets\n");
       return false; 
      }

      config_stack.pop();
    } else if (token_type == TOKEN_TYPE_EOF) {
      if (last_token_type != TOKEN_TYPE_STATEMENT_END &&
          last_token_type != TOKEN_TYPE_END_BLOCK) {
        // Error.
        break;
      }

      // check incorrect bracket count
      if (bracket_count != 0) {
        printf("Incorrect number of brackets, too many opening brackets\n");
        return false;
      }

      return true;
    } else {
      // Error. Unknown token.
      break;
    }
    last_token_type = token_type;
  }

  printf ("Bad transition from %s to %s\n",
          TokenTypeAsString(last_token_type),
          TokenTypeAsString(token_type));
  return false;
}

bool NginxConfigParser::Parse(const char* file_name, NginxConfig* config) {
  std::ifstream config_file;
  config_file.open(file_name);
  if (!config_file.good()) {
    printf ("Failed to open config file: %s\n", file_name);
    return false;
  }

  const bool return_value =
      Parse(dynamic_cast<std::istream*>(&config_file), config);
  config_file.close();
  return return_value;
}
