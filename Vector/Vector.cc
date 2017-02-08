// Implementation of the templated Vector class
// ECE4893/8893 lab 3
// YOUR NAME HERE

#include <iostream> // debugging
#include "Vector.h"
#include <stdlib.h>
// Your implementation here
// Fill in all the necessary functions below
using namespace std;

// Default constructor
template <typename T>
Vector<T>::Vector() : count(0), reserved(0)
{
    elements = (T*)malloc((reserved + 1)*sizeof(T));
}

// Copy constructor
template <typename T>
Vector<T>::Vector(const Vector& rhs) : count(rhs.Size()), reserved(rhs.Size())
{
	elements = (T*)malloc((reserved + 1)*sizeof(T));
	for(size_t i = 0; i < Size(); ++i)
		new(elements + i) T(rhs[i]);
}

// Assignment operator
template <typename T>
Vector<T>& Vector<T>::operator=(const Vector& rhs)
{
	if(&rhs != this)
	{
		for(int i = 0; i < Size(); ++i)
			elements[i].~T();
		free(elements);
		count = rhs.Size();
		reserved = rhs.Size();
		elements = (T*)malloc((reserved + 1)*sizeof(T));
		for(int i = 0; i < Size(); ++i)
			new(elements + i) T(rhs[i]);
	}
	return *this;
}

#ifdef GRAD_STUDENT
// Other constructors
template <typename T>
Vector<T>::Vector(size_t nReserved) : count(0), reserved(nReserved)
{ // Initialize with reserved memory
	elements = (T*)malloc((reserved + 1)*sizeof(T));
}

template <typename T>
Vector<T>::Vector(size_t n, const T& t) : count(n), reserved(n)
{ // Initialize with "n" copies of "t"
	elements = (T*)malloc((reserved + 1)*sizeof(T));
	for(size_t i = 0; i < n; ++i)
		new(elements + i) T(t);
}
template <typename T>
void Vector<T>::Reserve(size_t n)
{
	if(n <= reserved)
		return;
	T* old_elements = elements;
	elements = (T*)malloc((n + 1)*sizeof(T));
	for(size_t i = 0; i < count; ++i)
	{
		new(elements + i) T(old_elements[i]);
		old_elements[i].~T();
	}
	reserved = n;
	free(old_elements);
}
#endif

// Destructor
template <typename T>
Vector<T>::~Vector()
{
	for(size_t i = 0; i < Size(); ++i)
		elements[i].~T();
	free(elements);

}

// Add and access front and back
template <typename T>
void Vector<T>::Push_Back(const T& rhs)
{
	if(count == reserved)
		Reserve(reserved + 1);
	new (elements + count++) T(rhs);
}

template <typename T>
void Vector<T>::Push_Front(const T& rhs)
{
	T* old_elements = elements;
	elements = (T*)malloc((reserved + 2)*sizeof(T));
	reserved++;
	count++;
	new(elements) T(rhs);
	for(size_t i = 1; i < count; ++i)
	{
		new(elements + i) T(old_elements[i - 1]);
		old_elements[i - 1].~T();
	}

	free(old_elements);
}

template <typename T>
void Vector<T>::Pop_Back()
{ // Remove last element
	if(count == 0) return;
	elements[--count].~T();
}

template <typename T>
void Vector<T>::Pop_Front()
{ // Remove first element
	for(size_t i = 1; i < count; ++i)
	{
		elements[i - 1].~T();
		new(elements + (i - 1)) T(elements[i]);
	}
	elements[--count].~T();
}

// Element Access
template <typename T>
T& Vector<T>::Front() const
{
	return elements[0];
}

// Element Access
template <typename T>
T& Vector<T>::Back() const
{
	return elements[Size() - 1];
}
template <typename T>
const T& Vector<T>::operator[](size_t i) const
{
	return elements[i];
}

template <typename T>
T& Vector<T>::operator[](size_t i)
{
	return elements[i];
}

template <typename T>
size_t Vector<T>::Size() const
{
	return count;
}

template <typename T>
bool Vector<T>::Empty() const
{
	return count == 0;
}

// Implement clear
template <typename T>
void Vector<T>::Clear()
{
	for(size_t i = 0; i < Size(); ++i)
		elements[i].~T();
	count = 0;
}

// Iterator access functions
template <typename T>
VectorIterator<T> Vector<T>::Begin() const
{
  return VectorIterator<T>(elements);
}

template <typename T>
VectorIterator<T> Vector<T>::End() const
{
	return VectorIterator<T>(elements + count);
}

#ifdef GRAD_STUDENT
// Erase and insert
template <typename T>
void Vector<T>::Erase(const VectorIterator<T>& it)
{
	int num = 0;
	for(VectorIterator<T> iter = Begin(); iter != End(); iter++, num++)
	{
		if(iter == it)
			break;
	}
	for(size_t i = num + 1; i < count; ++i)
	{
		elements[i - 1].~T();
		new(elements + (i - 1)) T(elements[i]);
	}
	elements[--count].~T();
}

template <typename T>
void Vector<T>::Insert(const T& rhs, const VectorIterator<T>& it)
{
	int num = 0;
	for(VectorIterator<T> iter = Begin(); iter != End(); iter++, num++)
	{
		if(iter == it)
			break;
	}

	T* old_elements = elements;
	elements = (T*)malloc((reserved + 2)*sizeof(T));
	reserved++;
	for(int i = 0; i < num; ++i)
	{
		new(elements + i) T(old_elements[i]);
		old_elements[i].~T();
	}
	new(elements + num) T(rhs);
	for(size_t i = num; i < count; ++i)
	{
		new(elements + (i + 1)) T(old_elements[i]);
		old_elements[i].~T();
	}
	count++;
	free(old_elements);
}
#endif

// Implement the iterators

// Constructors
template <typename T>
VectorIterator<T>::VectorIterator()
{
}

template <typename T>
VectorIterator<T>::VectorIterator(T* c) : current(c)
{
}

// Copy constructor
template <typename T>
VectorIterator<T>::VectorIterator(const VectorIterator<T>& rhs)
{
	current = &(*rhs);
}

// Iterator defeferencing operator
template <typename T>
T& VectorIterator<T>::operator*() const
{
	return *current;
}

// Prefix increment
template <typename T>
VectorIterator<T>  VectorIterator<T>::operator++()
{
	current++;
	return *this;
}

// Postfix increment
template <typename T>
VectorIterator<T> VectorIterator<T>::operator++(int num)
{
	VectorIterator<T> old = VectorIterator(*this);           
	current++;                      
	return old;
}

// Comparison operators
template <typename T>
bool VectorIterator<T>::operator !=(const VectorIterator<T>& rhs) const
{
	return current != &(*rhs);
}

template <typename T>
bool VectorIterator<T>::operator ==(const VectorIterator<T>& rhs) const
{
	return current == &(*rhs);
}




