#ifndef WRAPPER_HPP
#define WRAPPER_HPP

#include <sys/wait.h>
#include <unistd.h>

#include <iostream>
#include <string>

class Wrapper {
 public:
  static int Pipe(int fildes[2]);
  static int Close(int fildes);
  static ssize_t Write(int fildes, const void *buf, size_t nbyte);
  static ssize_t Read(int fildes, void *buf, size_t nbyte);
  static pid_t Fork(void);
  static int Dup2(int fildes, int fildes2);
  static int Execve(const char *path, char *const argv[], char *const envp[]);
  static pid_t Waitpid(pid_t pid, int *stat_loc, int options);

 private:
  static void PrintError(std::string func_name);

  Wrapper();
  virtual ~Wrapper();
  Wrapper(const Wrapper &other);
  Wrapper &operator=(const Wrapper &other);
};

#endif  // WRAPPER_HPP
