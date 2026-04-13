#ifndef LINELIST_H
#define LINELIST_H

#include <iostream>
using namespace std;

class LineListException {};

template <class T> class LineList;

template <class T> class LineListElem {
    T data;
    LineListElem* next;
public:
    LineListElem(const T& adata, LineListElem* anext);
    const T& getData() const;
    LineListElem* getNext();
    
    friend class LineList<T>;
};

template <class T> class LineList {
    LineListElem<T>* start;
    LineList(const LineList& list); 
    LineList& operator =(const LineList& list);
public:
    LineList();
    ~LineList();
    LineListElem<T>* getStart();
    void deleteFirst();
    void deleteAfter(LineListElem<T>* ptr);
    void insertFirst(const T& data);
    void insertAfter(LineListElem<T>* ptr, const T& data);

    template <class U> 
    friend ostream& operator <<(ostream& out, LineList<U>& list);
};



template <class T> 
LineListElem<T>::LineListElem(const T& adata, LineListElem<T>* anext) {
    data = adata;
    next = anext;
}


template <class T>
const T& LineListElem<T>::getData() const {
    return data;
}

template <class T>
LineListElem<T>* LineListElem<T>::getNext() {
    return next;
}



template <class T> 
LineList<T>::LineList() {
    start = 0;
}

template <class T> 
LineList<T>::~LineList() {
    while (start)
        deleteFirst();
}

template <class T>
LineListElem<T>* LineList<T>::getStart() {
    return start;
}

template <class T> 
void LineList<T>::deleteFirst() {
    if (start) {
        LineListElem<T>* temp = start->next;
        delete start;
        start = temp;
    } else throw LineListException();
}

template <class T> 
void LineList<T>::insertFirst(const T& data) {
    start = new LineListElem<T>(data, start);
}

template <class T> 
void LineList<T>::deleteAfter(LineListElem<T>* ptr) {
    if (ptr && ptr->next) {
        LineListElem<T>* temp = ptr->next;
        ptr->next = ptr->next->next;
        delete temp;
    } else throw LineListException();
}

template <class T> 
void LineList<T>::insertAfter(LineListElem<T>* ptr, const T& data) {
    if (ptr) {
        ptr->next = new LineListElem<T>(data, ptr->next);
    }
}

template <class T> 
ostream& operator << (ostream& out, LineList<T>& list) {
    LineListElem<T>* ptr = list.start;
    if (!ptr)
        out << "EMPTY ";
    else {
        while (ptr) {
            out << ptr->getData() << ' ';
            ptr = ptr->getNext();
        }
    }
    return out;
}

#endif