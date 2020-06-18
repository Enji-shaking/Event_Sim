#ifndef HEAP_H
#define HEAP_H
#include <functional>
#include <stdexcept>
#include <vector>

template <typename T, typename PComparator = std::less<T> >
class Heap
{
public:
    /// Constructs an m-ary heap for any m >= 2
    Heap(int m, PComparator c = PComparator());

    /// Destructor as needed
    ~Heap();

    /// Adds an item
    void push(const T& item);

    /// returns the element at the top of the heap
    ///  max (if max-heap) or min (if min-heap)
    T const & top() const;

    /// Removes the top element
    void pop();

    /// returns true if the heap is empty
    bool empty() const;

private:
    /// Add whatever helper functions and data members you need below
    int childNum = 0;
    PComparator comp_;
    std::vector<T> vec_;
    void heapify(size_t index);


};

// Add implementation of member functions here


// We will start top() for you to handle the case of
// calling top on an empty heap
template <typename T, typename PComparator>
T const & Heap<T,PComparator>::top() const
{
    // Here we use exceptions to handle the case of trying
    // to access the top element of an empty heap
    if(empty()) {
        throw std::logic_error("can't top an empty heap");
    }
    // If we get here we know the heap has at least 1 item
    // Add code to return the top element
    return vec_[0];
}


// We will start pop() for you to handle the case of
// calling top on an empty heap
template <typename T, typename PComparator>
void Heap<T,PComparator>::pop()
{
    if(empty()) {
        throw std::logic_error("can't pop an empty heap");
    }
    vec_[0] = vec_[vec_.size()-1];
    vec_.pop_back();
    heapify(0);
}

template<typename T, typename PComparator>
Heap<T, PComparator>::Heap(int m, PComparator c) {
    childNum = m;
    comp_ = c;
}

template<typename T, typename PComparator>
bool Heap<T, PComparator>::empty() const {
    return vec_.size()==0;
}

template<typename T, typename PComparator>
void Heap<T, PComparator>::heapify(size_t index) {
    while(index<vec_.size()) {
        if ((index * childNum + 1) < vec_.size()){
            int bestChildIndex = index * childNum + 1;
            T bestChild = vec_[index * childNum + 1];
            for (size_t i = index * childNum + 2; i <= index * childNum + 4 && i < vec_.size(); i++) {
                if (comp_(vec_[i], bestChild)) {
                    bestChildIndex = i;
                    bestChild = vec_[i];
                }
            }
            if (comp_(bestChild, vec_[index])) {
                std::swap(vec_[index], vec_[bestChildIndex]);
                index = bestChildIndex;
            } else return;
        }else return;
    }
}


template<typename T, typename PComparator>
Heap<T, PComparator>::~Heap() {

}

template<typename T, typename PComparator>
void Heap<T, PComparator>::push(const T &item) {
    vec_.push_back(item);
    int index = vec_.size()-1;
    while(index>0&&comp_(vec_[index], vec_[(index-1)/4])){
        std::swap(vec_[index], vec_[(index-1)/4]);
        index = (index-1)/4;
    }
}


#endif

