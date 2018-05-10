//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2017 Media Design School
//
// Description  : A header only collection of useful utility functions and types.
// Author       : Lance Chaney
// Mail         : lance.cha7337@mediadesign.school.nz
//

#pragma once

#include <array>
#include <random>
#include <iterator>
#include <sstream>
#include <iomanip>
#include <functional>
#include <future>
#include <chrono>

// A simple mulidimensional array
template <typename T, size_t DimFirst, size_t... Dims>
class NDArray : public std::array<NDArray<T, Dims...>, DimFirst> {};

// A simple mulidimensional array
template <typename T, size_t DimLast>
class NDArray<T, DimLast> : public std::array<T, DimLast> {};

// Erases an element from a vector in O(1) time without preserving order.
template <typename Vector>
typename Vector::iterator unorderedErase(Vector& v, typename Vector::iterator it)
{
	if (it == std::prev(v.end()))
		it = v.end();
	else
		*it = std::move(v.back());
	v.pop_back();
	return it;
}

// Erases an element from a vector in O(1) time without preserving order
template <typename Vector>
void unorderedErase(Vector& v, size_t i)
{
	if (i != v.size() - 1)
		v.at(i) = std::move(v.back());
	v.pop_back();
}

// Returns a generator for generating random numbers
inline std::mt19937& getRandomGenerator()
{
	static std::random_device rd;
	static std::mt19937 gen(rd());
	return gen;
}

// Returns a random real number, uniformly distributed in the range [min, max)
template<typename RealT = double>
RealT randomReal(RealT min = 0, RealT max = 1)
{
	std::uniform_real_distribution<RealT> dist(min, max);
	return dist(getRandomGenerator());
}

// Returns a random integer, uniformly distributed in the range [min, max] inclusive
template<typename IntT = int>
IntT randomInt(IntT min = 0, IntT max = 1)
{
	std::uniform_int_distribution<IntT> dist(min, max);
	return dist(getRandomGenerator());
}

// Returns an iterator to a random element in the range [iterStart, iterEnd] inclusive
template<typename Iter, typename RandomGenerator>
Iter selectRandomly(Iter start, Iter end, RandomGenerator& g)
{
	std::uniform_int_distribution<std::_Iter_diff_t<Iter>> dist(0, std::distance(start, end) - 1);
	std::advance(start, dist(g));
	return start;
}

// Returns an iterator to a random element in the range [iterStart, iterEnd] inclusive
template<typename Iter>
Iter selectRandomly(Iter start, Iter end)
{
	return selectRandomly(start, end, getRandomGenerator());
}

// Returns a random permutation of the collection
template<typename CollectionT>
CollectionT getRandomPermutation(const CollectionT& collection) {
	CollectionT randomPermutation = collection;
	for (size_t i = 0; i < collection.size(); ++i) {
		size_t selectedIdx = randomInt(static_cast<size_t>(i), collection.size() - 1);
		std::swap(randomPermutation.at(i), randomPermutation.at(selectedIdx));
	}

	return randomPermutation;
}

// Checks if the specified element exists in the range [start, end) exclusive
template<typename IterT, typename T>
bool isIn(IterT start, IterT end, T val) {
	auto resultIt = std::find(start, end, val);
	if (resultIt != end)
		return true;
	else
		return false;
}

// Checks if the specified element exists in the range [start, end) exclusive
template<typename ContainerT, typename T>
bool isIn(ContainerT container, T val) {
	return isIn(container.begin(), container.end(), val);
}

// Checks if the specified element does not exists in the range [start, end) exclusive
template<typename IterT, typename T>
bool isNotIn(IterT start, IterT end, T val) {
	return !isIn(start, end, val);
}
// Checks if the specified element does not exists in the range [start, end) exclusive
template<typename ContainerT, typename T>
bool isNotIn(ContainerT container, T val) {
	return isNotIn(container.begin(), container.end(), val);
}

// Converts a number to a string with the specified number of decimal places
template <typename T>
std::string toString(const T value, const int decimalPlaces = 2)
{
	std::ostringstream out;
	out << std::fixed;
	out << std::setprecision(decimalPlaces) << value;
	return out.str();
}

template <typename Arg>
struct InvokeType : std::add_lvalue_reference<Arg> { };

template <typename T>
struct InvokeType<std::reference_wrapper<T>> {
	using type = T&;
};

// Used for perfect forwarding bind arguments
// template<class ...Args>
// void bind_and_forward(Args&&... args)
// {
//                                         vvvvvvvvvvvvvvvvvvvvvvvvvvv - stored as lvalues
//     auto binder = std::bind(f<Args...>, std::forward<Args>(args)...); 
//                             ^^^^^^^^^ - Error specializing Invoke if any args have rvalue or std::reference_wrapper<T> type
//     binder();
// }
// Bind always passes its stored arguments by lvalue reference when the bound function is invoked.
// For std::reference_wrapper<T> types the get() member function is called when passing to the 
// bound function in order to convert it to a normal lvalue reference. If the function prototype
// f<Args...>, stored by bind, contains any rvalue references or std::reference_wrapper<T> types,
// then the compiler will fail to specialize due to a type mismatch. The solution is to replace
// f<Args...> with f<InvokeTypeT<Arg>> to ensure all stored arguments can be passed by lvalue
// reference.
template <typename T>
using InvokeTypeT = typename InvokeType<T>::type;

// Returns true when a future is ready (when the associated task has finished its work)
template<typename T>
bool isReady(const std::future<T>& future)
{
	return future.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
}

// Returns true when all futures in the collection are ready (when the associated tasks hav finished their work)
template <typename CollectionT>
bool futuresReady(const CollectionT& collection) {
	for (auto& future : collection) {
		if (!isReady(future))
			return false;
	}

	return true;
}

// Lerps between two different values by a scaler (usually between 0 and 1)
template <typename T>
T lerp(T start, T end, double alpha) {
	return static_cast<T>(start + alpha * (end - start));
}

// Clamps the value to the given range using the specified comparator
template<class T, class Compare>
constexpr const T& clamp(const T& v, const T& lo, const T& hi, Compare comp)
{
	return assert(!comp(hi, lo)),
		comp(v, lo) ? lo : comp(hi, v) ? hi : v;
}

// Clamps the value to the given range
template<class T>
constexpr const T& clamp(const T& v, const T& lo, const T& hi)
{
	return clamp(v, lo, hi, std::less<>());
}

// Returns the sign of a number (+1 or -1)
template <typename T> int sign(T val) {
	return (T(0) < val) - (val < T(0));
}