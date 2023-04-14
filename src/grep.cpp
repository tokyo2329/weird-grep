#include <iostream>
#include <fstream>
#include <algorithm>
#include <numeric>
#include <thread>
#include <filesystem>
#include <unordered_map>
#include <chrono>

#include "grep.h"


Grep::Grep(size_t number_of_threads, std::string result_file, std::string log_file)
  : _thread_number{number_of_threads}, _result_file{result_file}, _log_file{log_file} {}


void Grep::run(std::string starting_path, std::string pattern) {

  auto start_clock = std::chrono::high_resolution_clock::now();

  ThreadPool pool{_thread_number};
  
  using namespace std::filesystem;

  for(auto& entry : recursive_directory_iterator(starting_path, directory_options::skip_permission_denied)) {
    
    if(entry.path().filename().string() == _log_file 
      or entry.path().filename().string() == _result_file)
      continue;

    if(is_regular_file(entry) and entry.path().extension() == ".txt") {

      std::function<void()> f = [this, entry, pattern](){
        _search_file(entry.path(), pattern);
      };

      pool.add_task(f);
    }
  }

  pool.wait();

  auto end_clock = std::chrono::high_resolution_clock::now();

  _time = std::chrono::duration_cast<std::chrono::milliseconds>(end_clock - start_clock).count();
}


void Grep::output_results() {

  // output file
  std::ofstream output_file{_result_file, std::ios::trunc};
  
  if(not output_file.is_open()) {
    std::cerr << "couldn't write to the result file" << std::endl;
    return;
  }

  // sort proccessed files by number of matches
  std::ranges::sort(_proccessed, [](auto a, auto b){
    return a.get_matches().size() > b.get_matches().size();
  });

  // print all
  for(const auto& proccessed_file : _proccessed) {
    for(const auto& match : proccessed_file.get_matches()) {
      output_file << proccessed_file.get_filepath() << ":" << match.get() << std::endl;
    }
  }

  output_file.close();

  // log file
  std::ofstream log_file{_log_file, std::ios::trunc};

  if(not log_file.is_open()) {
    std::cerr << "couldn't write to the log file" << std::endl;
    return;
  }

  // group the proccessed objects
  std::unordered_map<std::thread::id, std::vector<Grep::_Proccessed>> grouped_proccessed;
  for(const auto& proccessed : _proccessed)
    grouped_proccessed[proccessed.get_thread_id()].push_back(proccessed);

  // print all
  for(const auto& [thread_id, objs] : grouped_proccessed) {
    log_file << thread_id << ": ";
    for(const auto& obj : objs)
      log_file << obj.get_filepath() << ", ";
    log_file << std::endl;
  }

  log_file.close();
  

  auto patterns_number = std::accumulate(_proccessed.begin(), _proccessed.end(), 0,
    [](int sum, const auto& obj){ return sum + obj.get_matches().size(); }
  );

  auto files_with_pattern = std::ranges::count_if(_proccessed, [](const auto& obj){
    return obj.get_matches().size();
  });

  // standard output
  std::cout << "Searched files: " << _proccessed.size() << std::endl;
  std::cout << "Files with pattern: " << files_with_pattern << std::endl;
  std::cout << "Patterns number: " << patterns_number << std::endl;
  std::cout << "Result file: " << _result_file << std::endl;
  std::cout << "Log file: " << _log_file << std::endl;
  std::cout << "Used threads: " << _thread_number << std::endl;
  std::cout << "Elapsed time: " << _time << " (ms)" << std::endl; 
}


void Grep::_search_file(std::string filename, std::string pattern) {
  std::ifstream file{filename};

  if(not file.is_open()) {
    std::cerr << "failed to open " << filename << std::endl;
    return;
  }

  std::string current_line;
  int current_line_number{0};

  Grep::_Proccessed current_process{filename, std::this_thread::get_id()};

  while(std::getline(file, current_line)) {
    current_line_number++;

    size_t pos{0};

    while((pos = current_line.find(pattern, pos)) != std::string::npos) {
      current_process.add_match(current_line_number, current_line);
      pos += pattern.length();
    }
  }

  {
    std::unique_lock lock{_processed_mtx};
    _proccessed.push_back(current_process);
  }
  file.close();
}


Grep::_Proccessed::_Proccessed(std::string filepath, std::thread::id thread_id)
  : _proccessed_by_thread{thread_id}, _filepath{filepath} {}


void Grep::_Proccessed::add_match(int line_number, std::string line_content) {
  _matches.emplace_back(line_number, line_content);
}


std::vector<Grep::_Proccessed::_Match> Grep::_Proccessed::get_matches() const {
  return _matches;
}


std::string Grep::_Proccessed::get_filepath() const {
  return _filepath;
}


std::thread::id Grep::_Proccessed::get_thread_id() const {
  return _proccessed_by_thread;
}


Grep::_Proccessed::_Match::_Match(int line_number, std::string line_content)
  : _line_number{line_number}, _line_content{line_content} {}


std::string Grep::_Proccessed::_Match::get() const {
  return std::string{std::to_string(_line_number) + ": " + _line_content};
}
