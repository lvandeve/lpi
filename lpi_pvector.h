/*
Copyright (c) 2005-2008 Lode Vandevenne
All rights reserved.

This file is part of Lode's Programming Interface.

Lode's Programming Interface is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Lode's Programming Interface is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Lode's Programming Interface.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef LPI_PVECTOR_H_INCLUDED
#define LPI_PVECTOR_H_INCLUDED

#include <vector>

/*
lpi::pvector is a class similar to std::vector with one difference:
Whenever an element is removed from the pvector (including at the pvector's destruction),
the delete operator is called on it.
So a pvector can only contain pointers that were allocated with new, and it deletes them automatically.
This is handy for arrays of polymorphic objects.

Due to the deleting, some functionality of the regular std::vector had to be left out:
-operations that duplicate pointers to another vector, such as operator=
-operations that create multiple times the same pointer in 1 vector, such as assign, resize
-operations that allow the user to modify the pointer without the vector's knowledge. For this are disabled:
any non-const iterator or reference, the non-const operator[] and at()
-erase and insert still work, but with const_iterators instead of iterators

The following operations of pvector will result in calling delete on elements:
-clear
-destructor
-erase
-pop_back
*/

namespace lpi
{

template<typename T, typename A = std::allocator<T> >
class pvector
{
  protected:
    typedef std::vector<T, A> vector_type;
    std::vector<T, A> v;
    
    void deleteAll()
    {
      for(size_type i = 0; i < v.size(); i++) delete v[i];
    }
    
    typedef typename vector_type::iterator iterator;
  
  public:
    
    typedef T value_type;
    typedef A allocator_type;
    typedef typename vector_type::const_pointer const_pointer;
    typedef typename vector_type::const_reference const_reference;
    typedef typename vector_type::const_iterator const_iterator;
    typedef typename vector_type::const_reverse_iterator const_reverse_iterator;
    typedef typename vector_type::size_type size_type;
    typedef typename vector_type::difference_type difference_type;
    
  public:

    explicit pvector(const A& a = A()) : v(a) {}

    ~pvector()
    {
      deleteAll();
    }

    const_iterator begin() const { return v.begin(); }
    const_iterator end() const { return v.end(); }
    const_reverse_iterator rbegin() const { return v.rbegin(); }
    const_reverse_iterator rend() const { return v.rend(); }

    size_type size() const { return v.size(); }

    size_type max_size() const { return v.max_size(); }

    bool empty() const { return v.empty(); }

    size_type capacity() const { return v.capacity(); }
    void reserve(size_type n) { v.reserve(n); }
    
    const_reference operator[](size_type n) const { return v[n]; }
    
    const_reference at(size_type n) const { return v.at(n); }
    
    const_reference front() const { return v.front(); }
    const_reference back() const { return v.back(); }

    void push_back(const T& x) { v.push_back(x); }

    void pop_back()
    {
      delete v.back();
      v.pop_back();
    }

    const_iterator insert(const_iterator position, const T& x)
    {
      iterator it = v.begin() + (position - v.begin());
      it = v.insert(it, x);
      return v.begin() + (it - v.begin());
    }

    const_iterator erase(const_iterator position)
    {
      delete *position;
      iterator it = v.begin() + (position - v.begin());
      it = v.erase(it);
      return v.begin() + (it - v.begin());
    }
    
    const_iterator erase(const_iterator first, const_iterator last)
    {
      for(const_iterator i = first; i != last; ++i) delete *i;
      iterator it_first = v.begin() + (first - v.begin());
      iterator it_last = v.begin() + (last - v.begin());
      it_last = v.erase(it_first, it_last);
      return v.begin() + (it_last - v.begin());
    }

    void swap(pvector& x)
    {
      v.swap(x.v);
    }

    void clear()
    {
      deleteAll();
      v.clear();
    }
    
    /*
    Public members of std::vector that may not be in pvector due to allowing duplicating a pointer or changing a pointer without delete
    
    typedef typename vector_type::pointer           pointer;
    typedef typename vector_type::reference         reference;
    typedef typename vector_type::iterator          iterator;
    typedef typename vector_type::reverse_iterator reverse_iterator;
    explicit pvector(size_type n, const T& value = T(), const A& a = A()) : v(n, value, a)
    pvector(const pvector& x) : v(x.v)
    template<typename _InputIterator> vector(_InputIterator first, _InputIterator last, const A& a = A()) : v(first, last, a)
    pvector& operator=(const pvector& x); //disallowed for pvector
    void assign(size_type n, const T& val)
    iterator begin() { return v.begin(); }
    iterator end() { return v.end(); }
    reverse_iterator rbegin() { return v.rbegin(); }
    reverse_iterator rend() { return v.rend(); }
    void resize(size_type new_size, T x = T())
    reference operator[](size_type n)
    reference at(size_type n)
    reference front() { return *begin(); }
    reference back() { return *(end() - 1); }
    iterator insert(iterator position, const T& x);
    void insert(iterator position, size_type n, const T& x)
    template<typename _InputIterator> void insert(iterator position, _InputIterator first, _InputIterator last)
    iterator erase(iterator position);
    iterator erase(iterator first, iterator last);
    */
};

template<typename T, typename A>
inline bool operator==(const pvector<T, A>& x, const pvector<T, A>& y)
{
  return x.v == y.v;
}

template<typename T, typename A>
inline bool operator<(const pvector<T, A>& x, const pvector<T, A>& y)
{
  return x.v < y.v;
}


template<typename T, typename A>
inline bool operator!=(const pvector<T, A>& x, const pvector<T, A>& y)
{
  return x.v != y.v;
}

template<typename T, typename A>
inline bool operator>(const pvector<T, A>& x, const pvector<T, A>& y)
{
  return x.v > y.v;
}

template<typename T, typename A>
inline bool operator<=(const pvector<T, A>& x, const pvector<T, A>& y)
{
  return x.v <= y.v;
}

template<typename T, typename A>
inline bool operator>=(const pvector<T, A>& x, const pvector<T, A>& y)
{
  return x.v >= y.v;
}

template<typename T, typename A>
inline void swap(pvector<T, A>& x, pvector<T, A>& y)
{
  return x.v.swap(y.v);
}

////////////////////////////////////////////////////////////////////////////////

/*
VectorGuard is a class that will delete all elements of a vector in its destructor.
This makes it easier to use a regular std::vector where you want all its elements
to be deleted when it goes out of scope.
*/
template<typename T>
class VectorGuard
{
  private:
    T& v; //the std::vector

  public:
    VectorGuard(T& v)
    : v(v)
    {
    }
    
    ~VectorGuard()
    {
      for(size_t i = 0; i < v.size(); i++)
      {
        delete v[i];
      }
    }
};

} //namespace lpi


#endif

