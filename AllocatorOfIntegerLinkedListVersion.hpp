//
//  AllocatorOfIntegerLinkedListVersion.hpp
//  AllocatorTest
//
//  Created by 煦 柯 on 16/1/9.
//  Copyright © 2016年 Must Studio. All rights reserved.
//

#ifndef AllocatorOfIntegerLinkedListVersion_hpp
#define AllocatorOfIntegerLinkedListVersion_hpp

#include <memory>
#include <thread>
#include <vector>

class CAllocatorOfInteger2
{
public:
    static const int CHUNK_SIZE=512;
    CAllocatorOfInteger2();
    ~CAllocatorOfInteger2();
private:
    union Node{
        Node* next;
        int i;
    };
    class CIntegerChunk {
    public:
        CIntegerChunk();
        ~CIntegerChunk();
    private:
        char chunk[CHUNK_SIZE*sizeof(Node)];
        Node* head;
    public:
        int* popHead();
        void pushHead(void*);
        void linkHead(CIntegerChunk&);
    };
public:
    int *allocate();
    void deallocate(int *);
private:
    std::vector<std::shared_ptr<CIntegerChunk>> chunks;
    pthread_mutex_t mutex;
};


#endif /* AllocatorOfIntegerLinkedListVersion_hpp */
