#pragma once
#ifndef QUEUE_H
#define QUEUE_H

#include <atomic>
#include <cstdlib>
#include <iostream>
#include <mutex>
#include <string.h>
#include <syncstream>
#include <thread>

class Node {
public:
  std::string m_filename;
  Node *m_next;

  Node() : m_next(nullptr) {}
};

class Queue {
public:
  Queue() : head(nullptr), tail(nullptr) {}

  bool is_empty() const {
    const std::lock_guard<std::mutex> lock(m_queue_mutex);
    return head == nullptr;
  }

  void enqueue(const std::string &filename) {
    Node *node = new Node;
    node->m_filename = std::move(filename);

    if (is_empty()) {
      head = tail = node;
    } else {
      tail->m_next = node;
      tail = node;
    }
    return;
  }

  void dequeue() {
    if (is_empty())
      return;
    Node *tmp = head;
    head = head->m_next;
    if (head == nullptr)
      tail = nullptr;
    tmp->m_filename.clear();
    delete tmp;
    return;
  }

  std::string get_filename() const { return head->m_filename; }

  ~Queue() {
    while (head) {
      Node *tmp = head;
      head = head->m_next;
      tmp->m_filename.clear();
      delete tmp;
    }
    head = nullptr;
  }

private:
  Node *head;
  Node *tail;
  mutable std::mutex m_queue_mutex;
};

#endif
