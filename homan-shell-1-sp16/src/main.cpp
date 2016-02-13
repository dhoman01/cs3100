#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <string>
#include <chrono>
#include <vector>
#include <algorithm>

// Parse agruments from string
auto getArgs(std::string command)
{
  std::vector<char*> args;
  char* arg = std::strtok((char*)command.c_str()," ");
  while(arg != NULL)
  {
    args.push_back(arg);
    arg = std::strtok(NULL, " ");
  }
  args.push_back(0);
  return args;
}

// Execute process with current PATH
auto doChildWork(std::string command)
{
  std::vector<char*> args = getArgs(command);

  execvp(args[0], args.data());
  // If execvp returns that means it failed to execute
  std::cout << "Couldn't find the command: " << command << std::endl;
  return EXIT_FAILURE;
}

auto doCommandHistory(std::string& command, std::vector<std::string> command_history)
{
  try
  {
    auto hist_position = std::stoi(command.substr(2));
    if(hist_position <= command_history.size())
    {
      std::cout << command_history[hist_position - 1] << std::endl;
      command = command_history[hist_position - 1];
      return 200;
    }
    else
    {
      std::cout << "No commands back that far. Try a smaller number." << std::endl;
      return 100;
    }
  }
  catch(const std::invalid_argument& ia)
  {
    std::cout << command.substr(2) << " is an invalid argument. The argument to '^' needs to be an integer." << std::endl;
    return 100;
  }
}

auto doPrintHistory(std::vector<std::string> command_history)
{
  auto i = command_history.size();
  std::for_each(command_history.rbegin(), command_history.rend(),[&i](auto& command){
    std::cout << "^ " << i << ": " << command << std::endl;
    i--;
  });
  return 100;
}

auto doPrintRunningTime(std::chrono::duration<double> child_runningtime)
{
  auto sec = std::chrono::duration_cast<std::chrono::seconds>(child_runningtime);
  child_runningtime -= sec;
  auto mill = std::chrono::duration_cast<std::chrono::milliseconds>(child_runningtime);
  child_runningtime -= mill;
  auto micro = std::chrono::duration_cast<std::chrono::microseconds>(child_runningtime);
  std::cout << "***     Time spent executing child processes: " << sec.count() << " seconds ";
  std::cout << mill.count() << " milliseconds and " << micro.count() << " microseconds     ***" << std::endl;
  return 200;
}

// Parse out shell commands from executibles
// return 100 is a shell command not to store
// return 200 is an executible or shell command to store
// Also EXIT_SUCCESS and EXIT_FAILURE are possible returns
auto parseCommand(std::string& command, std::vector<std::string>& command_history, std::chrono::duration<double> child_runningtime)
{
  if(command == "exit") return EXIT_SUCCESS;
  else if(command[0] == '^') return doCommandHistory(command, command_history);
  else if(command == "history") return doPrintHistory(command_history);
  else if(command == "") return 100;
  else if(command == "ptime") return doPrintRunningTime(child_runningtime);
  return 200;
}

int main()
{
  std::string command = "";
  std::vector<std::string> command_history;
  std::chrono::duration<double> child_runningtime (0);

  while(true)
  {

    std::cout << "[cmd]: ";
    std::getline(std::cin,command); // Get command

    auto cmd = parseCommand(command, command_history, child_runningtime);

    if(cmd == EXIT_SUCCESS) return EXIT_SUCCESS;
    else if(cmd == 100){ /* Do nothing as all work has already been done */}
    else if (cmd == 200)
    {
      if(command_history.size() == 0 || command_history.front() != command)
        command_history.insert(command_history.begin(),command);
      if(command != "ptime")
      {
        auto start =std::chrono::steady_clock::now();
        auto pid = fork();
        if(pid < 0)
        {
          std::cerr << "Failure to fork" << std::endl;
          return EXIT_FAILURE;
        }
        if(pid > 0)
        {
          waitpid(pid, nullptr,0); // Wait for child process to finish
          auto end = std::chrono::steady_clock::now();

          // Increment child running times
          child_runningtime += (end - start);
        }
        else
        {
          return doChildWork(command); // Do child work
        }
      }
    }
  }
  // If this is reached the program
  // is exiting with out "exit" being
  // called. Therefore a failure...
  return EXIT_FAILURE;
}
