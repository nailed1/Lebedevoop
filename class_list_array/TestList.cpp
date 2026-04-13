#include "LineList.h"
#include <iostream>

using namespace std;

int main(){
    LineList<int> list;

    cout<<"Insers:"<<endl;
    for  (int i=10;i>0;i--)
        list.insertFirst(i);
    cout<<list<<endl;

    LineListElem<int>* ptr = list.getStart();
    ptr=ptr->getNext();
    list.insertAfter(ptr,100);
    cout<<list<<endl;

    cout<<"Deletes:"<<endl;
    list.deleteFirst();
    cout<<list<<endl;

    list.deleteAfter(ptr);
    cout<<list<<endl;

    cout<<"Exception:"<<endl;
    LineList<int> list2;
    
    try
    {
        list2.deleteFirst();
    }
    catch(const LineListException& e)
    {
        cout<<"Error caught"<< endl;
    }

    return 0;
}