#ifndef LINELIST_H
#define LINELIST_H

#include <iostream>
#include <stdexcept>

class LineListException : public std::runtime_error {
public:
    LineListException() : std::runtime_error("Error in LineList operation") {}
};

template<class T>
class LineList;

template<class T>
class LineListElem {
    T data;
    LineListElem<T>* next;

public:
    LineListElem(const T& adata, LineListElem<T>* anext);
    
    const T& getData() const;
    LineListElem<T>* getNext();

    friend class LineList<T>;
};

template<class T>
class LineList {
    LineListElem<T>* start;
    LineListElem<T>* tail;

    LineList(const LineList& list);
    LineList& operator=(const LineList& list);

public:
    LineList();
    ~LineList();

    LineListElem<T>* getStart();
    LineListElem<T>* getTail();

    void deleteFirst();
    void deleteAfter(LineListElem<T>* ptr);

    void insertFirst(const T& data);
    void insertAfter(LineListElem<T>* ptr, const T& data);

    template<class U>
    friend std::ostream& operator<<(std::ostream& out, LineList<U>& list);
};

template<class T>
std::ostream& operator<<(std::ostream& out, LineList<T>& list);

#include "LineList.cpp"

#endif