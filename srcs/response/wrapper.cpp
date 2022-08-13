#include "wrapper.hpp"

int Wrapper::Pipe(int fildes[2]) {
  int res = pipe(fildes);
  if (res < 0) PrintError("pipe");
  return res;
}

int Wrapper::Close(int fildes) {
  int res = close(fildes);
  if (res == -1) PrintError("close");
  return res;
}

ssize_t Wrapper::Write(int fildes, const void *buf, size_t nbyte) {
  ssize_t res = write(fildes, buf, nbyte);
  if (res <= 0) PrintError("write");
  return res;
}

ssize_t Wrapper::Read(int fildes, void *buf, size_t nbyte) {
  ssize_t res = read(fildes, buf, nbyte);
  if (res == -1) PrintError("read");
  return res;
}

pid_t Wrapper::Fork(void) {
  pid_t pid = fork();
  if (pid < 0) PrintError("fork");
  return pid;
}

int Wrapper::Dup2(int fildes, int fildes2) {
  int res = dup2(fildes, fildes2);
  if (res == -1) PrintError("dup2");
  return res;
}

int Wrapper::Execve(const char *path, char *const argv[], char *const envp[]) {
  int res = execve(path, argv, envp);
  if (res == -1) PrintError("execve");
  return res;
}

pid_t Wrapper::Waitpid(pid_t pid, int *stat_loc, int options) {
  pid_t res = waitpid(pid, stat_loc, options);
  if (res == -1) PrintError("waitpid");
  return res;
}

void Wrapper::PrintError(std::string func_name) {
  std::cerr << "Error: " << func_name << " failed" << std::endl;
}
