#include <iostream>

#include "grep.h"


int main(int argc, char * argv[]) {

  // defaults
  std::string start_directory{"."};
  std::string log_name{"weird-grep.log"};
  std::string result_name{"weird-grep.txt"};
  std::string pattern;
  size_t threads{4};

  // argument parser
  for(int i{1}; i < argc; i++) {
    std::string option{argv[i]};

    if(option == "-d" or option == "--dir")
      start_directory = std::string{argv[i + 1]};
    else if(option == "-l" or option == "--log_file") 
      log_name = std::string{argv[i + 1]};
    else if(option == "-r" or option == "--result_file")
      result_name = std::string{argv[i + 1]};
    else if(option == "-t" or option == "--threads")
      threads = std::stoi(argv[i + 1]);
    else if(i + 1 == argc) {
      pattern = option;
    } else {
      std::cerr << "Invalid argument provided." << std::endl;
      exit(1);
    }
    i++;
  }

  if(not pattern.length()) {
    std::cerr << "No pattern specified" << std::endl;
    exit(1);
  }

  // main program
  Grep g{threads, result_name, log_name};

  g.run(start_directory, pattern);
  g.output_results();

  return 0;
}