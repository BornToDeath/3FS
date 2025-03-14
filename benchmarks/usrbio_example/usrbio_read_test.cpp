//
// Created by lxq on 2025/3/13.
//
#include <atomic>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <errno.h>
#include <fcntl.h>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>

#include "hf3fs_usrbio.h"

constexpr uint64_t NUM_IOS = 1;
constexpr uint64_t BLOCK_SIZE = (16 << 20);

int main() {
  struct hf3fs_ior ior;
  int ret = hf3fs_iorcreate4(&ior, "/3fs/stage", NUM_IOS, true, 0, 0, -1, 0);
  printf("hf3fs_iorcreate4 return: %d\n", ret);
  if (ret != 0) return -1;

  struct hf3fs_iov iov;
  ret = hf3fs_iovcreate(&iov, "/3fs/stage", NUM_IOS * BLOCK_SIZE, 0, -1);
  printf("hf3fs_iovcreate return: %d\n", ret);
  if (ret != 0) return -1;

  int fd = open("/3fs/stage/lxq_usrbio_test/kvcache.bin", O_RDONLY);
  ret = hf3fs_reg_fd(fd, 0);
  printf("hf3fs_reg_fd return: %d\n", ret);

  auto t1 = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < NUM_IOS; i++) {
    ret = hf3fs_prep_io(&ior, &iov, true, iov.base + i * BLOCK_SIZE, fd, i * BLOCK_SIZE, BLOCK_SIZE, nullptr);
    printf("hf3fs_prep_io return: %d\n", ret);
  }
  ret = hf3fs_submit_ios(&ior);
  printf("hf3fs_submit_ios return: %d\n", ret);

  hf3fs_cqe cqes[NUM_IOS];
  hf3fs_wait_for_ios(&ior, cqes, NUM_IOS, NUM_IOS, nullptr);
  printf("hf3fs_wait_for_ios return: %d\n", ret);

  auto t2 = std::chrono::high_resolution_clock::now();
  auto read_cost = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1);
  printf("read: %ld us\n", read_cost.count());

  hf3fs_dereg_fd(fd);
  close(fd);
  hf3fs_iovdestroy(&iov);
  hf3fs_iordestroy(&ior);

  return 0;
}