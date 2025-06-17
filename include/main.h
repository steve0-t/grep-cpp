#pragma once
#ifndef MAIN_H
#define MAIN_H

#define _POSIX_C_SOURCE 200809L

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <dirent.h>
#include <fstream>
#include <iostream>
#include <ostream>
#include <pthread.h>
#include <string.h>
#include <string>
#include <thread>

class Node {
public:
  char *m_filename;
  Node *m_next;

  Node() : m_next(nullptr) {}
};

class Queue {
public:
  Queue() : head(nullptr), tail(nullptr), queue_size(0) {}

  bool is_empty() const { return tail == nullptr; }

  void enqueue(const char *filename) {
    Node *node = new Node;
    node->m_filename = strdup(filename);
    node->m_next = nullptr;

    if (is_empty()) {
      head = tail = node;
    } else {
      tail->m_next = node;
      tail = node;
    }
    queue_size += 1;

    // print_queue();
    // print_queue_size();
    return;
  }

  void dequeue() {
    if (is_empty())
      return;
    Node *tmp = head;
    head = head->m_next;
    if (!head)
      tail = nullptr;
    // print_queue();
    if (tmp->m_filename)
      free(tmp->m_filename);
    delete tmp;
    queue_size -= 1;
    return;
  }

  char *get_filename() const {
    return (head->m_filename) ? head->m_filename : nullptr;
  }

  /*
  void print_queue() {
    if (is_empty()) {
      std::cout << "Queue is empty." << "\n";
      return;
    }

    Node *tmp = head;
    while (tmp) {
      std::cout << tmp->m_filename << " ";
      tmp = tmp->m_next;
    }
    std::cout << "\n";
    return;
  }
  */

  void print_queue_size() {
    std::cout << "Queue size is: " << queue_size << std::endl;
    return;
  }

  ~Queue() {
    while (head) {
      Node *tmp = head;
      head = head->m_next;
      if (tmp->m_filename)
        free(tmp->m_filename);
      delete tmp;
    }
  }

private:
  Node *head;
  Node *tail;
  size_t queue_size;
};

#endif
