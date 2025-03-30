//
// Created by lxq on 2025/3/13.
//
#include <cerrno>
#include <chrono>
#include <cstring>
#include <fcntl.h>
#include <stdio.h>
#include <string>
#include <unistd.h>

#include "hf3fs_usrbio.h"

// 假设KVCache的大小
constexpr uint64_t KVCACHE_SIZE = (1 << 20);  // 1MB
constexpr uint64_t NUM_IOS = 1024;
constexpr uint64_t BLOCK_SIZE = (1 << 20);
const std::string mountpoint = "/3fs/stage/";

// 模拟KVCache数据
void *generate_kvcache() {
  void *kvcache = new char[KVCACHE_SIZE];
  // 这里可以添加实际的KVCache生成逻辑
  std::memset(kvcache, 'a', KVCACHE_SIZE);
  return kvcache;
}

// 释放KVCache数据
void free_kvcache(void *kvcache) { delete[] static_cast<char *>(kvcache); }

int main() {
  printf("enter main\n");
  // auto t1 = std::chrono::high_resolution_clock::now();

  struct hf3fs_iov iov;
  int ret = hf3fs_iovcreate(&iov, mountpoint.c_str(), NUM_IOS * BLOCK_SIZE, 0, -1);
  printf("hf3fs_iocreate return: %d\n", ret);
  if (ret != 0) return -1;

  // 生成KVCache数据
  void *kvcache = generate_kvcache();

  // 打开KVCache文件
  const std::string filename = mountpoint + "lxq/kvcache.bin";
  int fd = open(filename.c_str(), O_RDWR | O_CREAT | O_APPEND, 0666);
  printf("open return: %d, errno: %d\n, file: %s", fd, errno, filename.c_str());
  if (fd == -1) return -1;

  ret = hf3fs_reg_fd(fd, 0);
  printf("hf3fs_reg_fd return: %d\n", ret);
  if (ret > 0) return -1;

  uint64_t write_total_len = 0;
  uint64_t read_total_len = 0;

  // ----> write
  struct hf3fs_ior write_ior;
  ret = hf3fs_iorcreate4(&write_ior, mountpoint.c_str(), NUM_IOS, false, 0, 0, -1, 0);
  printf("write, hf3fs_iorcreate4 return: %d\n", ret);
  if (ret != 0) return -1;

  auto t1 = std::chrono::high_resolution_clock::now();
  // 存储KVCache数据到文件
  for (uint64_t i = 0; i < NUM_IOS; i++) {
    uint64_t len = KVCACHE_SIZE;
    uint64_t offset = len * i;
    std::memcpy(iov.base + offset, static_cast<char *>(kvcache), len);
    ret = hf3fs_prep_io(&write_ior, &iov, false, iov.base + offset, fd, offset, len, nullptr);
    // printf("[%lu]write, hf3fs_prep_io return: %d\n", i, ret);
    if (ret < 0) return -1;
    write_total_len += len;
  }
  ret = hf3fs_submit_ios(&write_ior);
  printf("write, hf3fs_submit_ios return: %d\n", ret);
  if (ret != 0) return -1;

  hf3fs_cqe cqes[NUM_IOS];
  int ior_count = hf3fs_wait_for_ios(&write_ior, cqes, NUM_IOS, NUM_IOS, nullptr);
  printf("write, hf3fs_wait_for_ios return: %d\n", ior_count);
  if (ior_count < 0) return -1;
  auto t2 = std::chrono::high_resolution_clock::now();

  // ----> read
  struct hf3fs_ior read_ior;
  ret = hf3fs_iorcreate4(&read_ior, mountpoint.c_str(), NUM_IOS, true, 0, 0, -1, 0);
  printf("read, hf3fs_iorcreate4 return: %d\n", ret);
  if (ret != 0) return -1;

  // 读取KVCache数据
  auto t3 = std::chrono::high_resolution_clock::now();
  for (uint64_t i = 0; i < NUM_IOS; i++) {
    uint64_t len = KVCACHE_SIZE;
    uint64_t offset = len * i;
    ret = hf3fs_prep_io(&read_ior, &iov, true, iov.base + offset, fd, offset, len, nullptr);
    // printf("[%lu]read, hf3fs_prep_io return: %d\n", i, ret);
    if (ret < 0) return -1;
    read_total_len += len;
  }
  ret = hf3fs_submit_ios(&read_ior);
  printf("read, hf3fs_submit_ios return: %d\n", ret);
  if (ret != 0) return -1;

  ior_count = hf3fs_wait_for_ios(&read_ior, cqes, NUM_IOS, 1, nullptr);
  printf("read, hf3fs_wait_for_ios return: %d\n", ior_count);
  if (ior_count < 0) return -1;
  auto t4 = std::chrono::high_resolution_clock::now();

  auto write_cost = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1);
  auto read_cost = std::chrono::duration_cast<std::chrono::microseconds>(t4 - t3);
  auto total_cost = std::chrono::duration_cast<std::chrono::microseconds>(t4 - t1);
  printf("===> write size %lu byte, cost: %ld us; read size: %lu byte, cost: %ld us; total cost: %ld us\n",
         write_total_len,
         write_cost.count(),
         read_total_len,
         read_cost.count(),
         total_cost.count());

  // 验证读取的数据（这里可以添加实际的验证逻辑）
  for (uint64_t i = 0; i < NUM_IOS; i++) {
    uint64_t len = KVCACHE_SIZE;
    uint64_t offset = len * i;
    if (std::memcmp(kvcache, iov.base + offset, len) == 0) {
      // printf("[%lu]Read data is correct!\n", i);
    } else {
      printf("[%lu]Read data is incorrent!\n", i);
    }
  }

  hf3fs_dereg_fd(fd);

  ret = close(fd);
  printf("close return: %d\n", ret);

  hf3fs_iovdestroy(&iov);

  hf3fs_iordestroy(&write_ior);
  hf3fs_iordestroy(&read_ior);

  // 释放KVCache数据
  free_kvcache(kvcache);

  return 0;
}
