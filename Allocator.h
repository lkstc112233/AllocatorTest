//
//  Allocator.hpp
//  AllocatorTest
//
//  Created by 煦 柯 on 16/1/9.
//  Copyright © 2016年 Must Studio. All rights reserved.
//

#ifndef Allocator_hpp
#define Allocator_hpp

#include <memory>
#include <thread>
#include <vector>

template <typename T>
class CAllocator
{
public:
    static const int CHUNK_SIZE=512;
    CAllocator()
    {
        chunks.push_back(std::shared_ptr<CChunk>(new CChunk()));
        pthread_mutex_init(&mutex, NULL);
    }
    ~CAllocator()
    {
    }
private:
    union Node{
        Node* next;
        char i[sizeof(T)];
    };
    class CChunk {
    public:
        CChunk()
        {
            Node* array=reinterpret_cast<Node*>(chunk);
            head=array;
            Node* last=head;
            for (int i=0;i<CHUNK_SIZE;++i)
            {
                array[i].next=last;
                last=array+i;
            }
            head->next=last;
        }
        ~CChunk()
        {
        }
    private:
        char chunk[CHUNK_SIZE*sizeof(Node)];
        Node* head;
    public:
        T* popHead()
        {
            Node* temp=nullptr;
            if (head==nullptr)
                return nullptr;
            if (head->next==head)
            {
                temp=head;
                head=nullptr;
            }
            else
            {
                temp=head->next;
                head->next=temp->next;
            }
            return reinterpret_cast<T*>(temp);
        }
        void pushHead(void* pointer)
        {
            if (head==nullptr)
            {
                head=reinterpret_cast<Node*>(pointer);
                return;
            }
            Node* temp=head->next;
            reinterpret_cast<Node*>(pointer)->next=temp;
            head->next=reinterpret_cast<Node*>(pointer);
        }
        void linkHead(CChunk& chunkToLink)
        {
            if (head==nullptr)
            {
                head=chunkToLink.head;
                return;
            }
            if (chunkToLink.head==nullptr)
                return;
            Node* temp=head->next;
            head->next=chunkToLink.head->next;
            chunkToLink.head->next=temp;
            chunkToLink.head=nullptr;
        }

    };
public:
    template<typename... ARGS>
    T *allocate(ARGS ...args)
    {
        T* pnt=nullptr;
        pthread_mutex_lock(&mutex);
        if ((pnt=chunks.front()->popHead()))
        {
            pthread_mutex_unlock(&mutex);
            pnt=new(pnt) T(args...);
            return pnt;
        }
        chunks.push_back(std::shared_ptr<CChunk>(new CChunk()));
        chunks.front()->linkHead(*chunks.back());
        pnt=chunks.front()->popHead();
        pthread_mutex_unlock(&mutex);
        pnt=new (pnt) T(args...);
        return pnt;
    }
    void deallocate(T *pointer)
    {
        pthread_mutex_lock(&mutex);
        pointer->~T();
        chunks.front()->pushHead(pointer);
        pthread_mutex_unlock(&mutex);
    }
private:
    std::vector<std::shared_ptr<CChunk>> chunks;
    pthread_mutex_t mutex;
};


#endif /* Allocator_hpp */
