#include "wrapper.hpp"

#include <sys/socket.h>

int Wrapper::Pipe(int fildes[2]) {
  int res = pipe(fildes);
  if (res < 0) PrintError("pipe");
  return res;
}

int Wrapper::Open(std::string path, int flags) {
  int res = open(path.c_str(), flags);
  if (res == -1) PrintError("open");
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

ssize_t Wrapper::Send(int sockfd, const void *buf, size_t len) {
  ssize_t res = send(sockfd, buf, len, 0);
  if (res <= 0) PrintError("send");
  return res;
}

ssize_t Wrapper::Recv(int sockfd, void *buf, size_t len) {
  ssize_t res = recv(sockfd, buf, len, 0);
  if (res <= 0) PrintError("recv");
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

void Wrapper::PrintMsg(const std::string &msg) {
#ifdef LOG
  std::cout << msg << std::endl;
#else
  (void)msg;
#endif
}

void Wrapper::PrintError(const std::string &func_name) {
#ifdef LOG
  std::cerr << "Error: " << func_name << " failed" << std::endl;
#else
  (void)func_name;
#endif
}
