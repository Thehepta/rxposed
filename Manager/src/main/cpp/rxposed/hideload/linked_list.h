//
// Created by chic on 2023/11/28.
//

#pragma once
#include "user_system.h"

/*android12 和android13 这个结构体的大小是不同的，会导致我们自己定义的soinfo结构体的成员变量错位*/
template<typename T>
struct LinkedListEntry {
    LinkedListEntry<T>* next;
    T* element;
};


template<typename T, typename Allocator>
class LinkedList {
    struct LinkedListHeader {
        LinkedListEntry<T>* head;
        LinkedListEntry<T>* tail;
    };
public:
//    template<typename F>
//    T* find_if(F predicate) const {
//        for (LinkedListEntry<T>* e = head_; e != nullptr; e = e->next) {
//            if (predicate(e->element)) {
//                return e->element;
//            }
//        }
//
//        return nullptr;
//    }


    template<typename F>
    T* find_if(F predicate) const {
        for (LinkedListEntry<T>* e = head(); e != nullptr; e = e->next) {
            if (predicate(e->element)) {
                return e->element;
            }
        }

        return nullptr;
    }

//    void push_back(T* const element) {
//        alloc_header();
//        LinkedListEntry<T>* new_entry = Allocator::alloc();
//        new_entry->next = nullptr;
//        new_entry->element = element;
//        if (header_->tail == nullptr) {
//            header_->tail = header_->head = new_entry;
//        } else {
//            header_->tail->next = new_entry;
//            header_->tail = new_entry;
//        }
//    }




private:

//    void alloc_header() {
//        if (header_ == nullptr) {
//            header_ = reinterpret_cast<LinkedListHeader*>(Allocator::alloc());
//            header_->head = header_->tail = nullptr;
//        }
//    }


    LinkedListEntry<T>* head() const {
        return header_ != nullptr ? header_->head : nullptr;
    }

    LinkedListHeader* header_;
//    LinkedListEntry<T>* tail_;
};



