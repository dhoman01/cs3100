#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <string>
#include <chrono>
#include <vector>
#include <algorithm>

int main()
{
  std::string command = "";
  std::vector<std::string> command_history;
  while(true)
  {
    std::cout << "[cmd]: ";
    std::getline(std::cin,command);
    if(command == "exit") return EXIT_SUCCESS;
    if(command[0] == '^')
    {
      int hist_position = std::stoi(command.substr(2));
      if(hist_position <= command_history.size()){
        std::cout << command_history[hist_position - 1] << std::endl;
        command = command_history[hist_position - 1];
      } else {
        command = "history";
      }
    }
    else if (command == "history")
    {
      auto i = command_history.size();
      std::for_each(command_history.rbegin(), command_history.rend(),[&i](auto& command){
        std::cout << "^ " << i << ": " << command << std::endl;
        i--;
      });
    }
    else
    {
      if(command_history.size() == 0 || command_history.front() != command)
        command_history.insert(command_history.begin(),command);
    }
    if(command != "history"){
      auto pid = fork();
      if(pid < 0)
      {
        std::cerr << "Failure to fork" << std::endl;
        return EXIT_FAILURE;
      }
      if(pid > 0)
      {
        auto start =std::chrono::steady_clock::now();
        waitpid(pid, nullptr,0);
        auto end = std::chrono::steady_clock::now();
        auto time_span = end - start;
        std::cout << "Process " << pid << " executed in " << time_span.count() << std::endl;
      }
      else
      {
        std::vector<char*> temp_args;
        char* arg = std::strtok((char*)command.c_str()," ");
        while(arg != NULL)
        {
          temp_args.push_back(arg);
          arg = std::strtok(NULL, " ");
        }
        temp_args.push_back(0);

        char* args[temp_args.size()];
        for(auto i = 0; i < temp_args.size(); i++)
        {
          args[i] = temp_args[i];
        }

        execvp(args[0], args);
        std::cout << command << " failed to execute..." << std::endl;
        return EXIT_FAILURE;
      }
    }
  }

  return EXIT_FAILURE;
}
