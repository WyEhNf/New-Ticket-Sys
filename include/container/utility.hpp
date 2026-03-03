#ifndef SJTU_UTILITY_HPP
#define SJTU_UTILITY_HPP

#include <iostream>
#include "../memoryriver/memoryriver.hpp"

// serialization not required here - forward declaration removed

namespace sjtu {

template<class T1, class T2>
class pair {
public:
	T1 first;
	T2 second;
	constexpr pair() : first(), second() {}
	pair(const pair &other) = default;
	pair(pair &&other) = default;
	pair(const T1 &x, const T2 &y) : first(x), second(y) {}
	template<class U1, class U2>
	pair(U1 &&x, U2 &&y) : first(x), second(y) {}
	template<class U1, class U2>
	pair(const pair<U1, U2> &other) : first(other.first), second(other.second) {}
	template<class U1, class U2>
	pair(pair<U1, U2> &&other) : first(other.first), second(other.second) {}
};

// serialization helpers for pair
template<typename A, typename B>
void serialize(std::ostream &os, const pair<A, B> &p) {
    Serializer<A>::write(os, p.first);
    Serializer<B>::write(os, p.second);
}
template<typename A, typename B>
void deserialize(std::istream &is, pair<A, B> &p) {
    Serializer<A>::read(is, p.first);
    Serializer<B>::read(is, p.second);
}

}

#endif
