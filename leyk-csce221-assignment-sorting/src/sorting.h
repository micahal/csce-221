#pragma once

#include <functional> // std::less
#include <iterator> // std::iterator_traits

namespace sort {

	// This is C++ magic which will allows our function
	// to default to using a < b if the comparator arg
	// is unspecified. It uses defines std::less<T>
	// for the iterator's value_type.
	//
	// For example: if you have a vector<float>, the 
	// iterator's value type will be float. std::less 
	// will select the < for sorting floats as the 
	// default comparator.
	template<typename RandomIter>
	using less_for_iter = std::less<typename std::iterator_traits<RandomIter>::value_type>;

	/* Efficiently swap two items - use this to implement your sorts */
	template<typename T>
	void swap(T & a, T & b) noexcept { /* TODO */ 
		T temp = std::move(a);
		a = std::move(b);
		b = std::move(temp);
	}

	template<typename RandomIter, typename Comparator = less_for_iter<RandomIter>>
	void bubble(RandomIter begin, RandomIter end, Comparator comp = Comparator{}) {
		// Random access iterators have the same traits you defined in the Vector class
		// For instance, difference_type represents an iterator difference
		// You may delete the types you don't use to remove the compiler warnings
		using _it             = std::iterator_traits<RandomIter>;
		using difference_type = typename _it::difference_type;
		using value_type      = typename _it::value_type;
		using reference       = typename _it::reference;
		using pointer         = typename _it::pointer;

		// TODO
		for (auto i = begin; i != end; i++) {
			bool swapmade = false;
			for (auto j = begin; j != end - (i - begin) - 1; j++) {
				if (comp(*(j+1), *j)) {
					swap(*j, *(j+1));
					swapmade = true;
				}
			}
			if (swapmade == false) {
				break;
			}
		}
	}

	template<typename RandomIter, typename Comparator = less_for_iter<RandomIter>>
	void insertion(RandomIter begin, RandomIter end, Comparator comp = Comparator{}) { /* TODO */ 
		for (auto i = begin; i != end; i++) {
			auto key = std::move(*i);
			auto j = std::move((i - 1));
			while (j >= begin && comp(key, *j)) {
				*(j+1) = std::move(*j);
				j = j - 1;
			}
			*(j+1) = std::move(key);
		}
	}

	template<typename RandomIter, typename Comparator = less_for_iter<RandomIter>>
	void selection(RandomIter begin, RandomIter end, Comparator comp = Comparator{}) { /* TODO */ 
		for (auto i = begin; i != end; i++) {
			auto k = std::move(i);
			for (auto j = i + 1; j != end; j++) {
				if (comp(*j, *k) == true) {
					k = std::move(j);
				}
			}
			swap(*i, *k);
		}
	
	
	}
}