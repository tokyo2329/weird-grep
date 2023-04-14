#pragma once

#include <vector>

#include "threadpool.h"


class Grep {

public:
  explicit Grep(size_t, std::string, std::string);

  void run(std::string, std::string);

  void output_results();

private:
  void _search_file(std::string, std::string);

  class _Proccessed {
  
  public:
    _Proccessed(std::string, std::thread::id);

    void add_match(int, std::string);

    class _Match {
    
    public:
      _Match(int, std::string);

      std::string get() const;

    private:
      int _line_number;
      std::string _line_content;

    };

    std::vector<_Match> get_matches() const;

    std::string get_filepath() const;

    std::thread::id get_thread_id() const;

  private:
    std::vector<_Match> _matches;
    std::thread::id _proccessed_by_thread;
    std::string _filepath;
  };


  size_t _thread_number;
  std::string _result_file, _log_file;

  std::vector<_Proccessed> _proccessed;
  std::mutex _processed_mtx;

};
