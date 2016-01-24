//
//  AllocatorOfIntegerLinkedListVersion.cpp
//  AllocatorTest
//
//  Created by 煦 柯 on 16/1/9.
//  Copyright © 2016年 Must Studio. All rights reserved.
//

#include <memory>
using std::shared_ptr;

#include "AllocatorOfIntegerLinkedListVersion.hpp"


CAllocatorOfInteger2::CAllocatorOfInteger2()
{
    chunks.push_back(shared_ptr<CIntegerChunk>(new CIntegerChunk()));
    pthread_mutex_init(&mutex, NULL);
}


CAllocatorOfInteger2::~CAllocatorOfInteger2()
{
}

int * CAllocatorOfInteger2::allocate()
{
    int* pnt=nullptr;
    pthread_mutex_lock(&mutex);
    if ((pnt=chunks.front()->popHead()))
    {
        pthread_mutex_unlock(&mutex);
        return pnt;
    }
    chunks.push_back(shared_ptr<CIntegerChunk>(new CIntegerChunk()));
    chunks.front()->linkHead(*chunks.back());
    pthread_mutex_unlock(&mutex);
    return chunks.front()->popHead();
}

void CAllocatorOfInteger2::deallocate(int *pointer)
{
    pthread_mutex_lock(&mutex);
    chunks.front()->pushHead(pointer);
    pthread_mutex_unlock(&mutex);
}

CAllocatorOfInteger2::CIntegerChunk::CIntegerChunk()
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

CAllocatorOfInteger2::CIntegerChunk::~CIntegerChunk()
{
}

int* CAllocatorOfInteger2::CIntegerChunk::popHead()
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
    return reinterpret_cast<int*>(temp);
}

void CAllocatorOfInteger2::CIntegerChunk::pushHead(void *pointer)
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

void CAllocatorOfInteger2::CIntegerChunk::linkHead(CIntegerChunk &chunkToLink)
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
}
