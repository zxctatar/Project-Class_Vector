#pragma once
#include <iostream>
#include <cstring>
#include <mutex>

template<typename T>
class Vector {
private:

	template <typename T>
	class _it {
	public:
		T* ptr;
	public:
		_it(T* p) : ptr(p) {}

		size_t index() const { return ptr; }

		_it& operator++() {
			ptr++;
			return *this;
		}

		_it& operator--() {
			ptr--;
			return *this;
		}

		T& operator*() {
			return *ptr;
		}

		bool operator==(const _it& other) {
			return ptr == other.ptr;
		}

		bool operator!=(const _it& other) {
			return ptr != other.ptr;
		}
	};

	std::mutex mtx;
	size_t _capacity = 0;
	size_t _size = 0;
	T* vec;

public:

	// a()
	Vector<T>() : _capacity(0), _size(0), vec(new T[_capacity]) {}

	// a(1)
	Vector(const T v) : _capacity(1), _size(1), vec(new T[_capacity]) {
		vec[0] = v;
	}

	// a{1,2,3,...)
	Vector(const std::initializer_list<T>& v) : _capacity(v.size()), _size(v.size()), vec(new T[_capacity]) {
		std::copy(v.begin(), v.end(), vec);
	}

	// a(b)
	Vector(const Vector<T>& other) : _capacity(other._capacity), _size(other._size), vec(new T[_capacity]) {
		for (int i = 0; i < _capacity; ++i) {
			vec[i] = other.vec[i];
		}
	}

	// a(std::move(b))
	Vector(Vector<T>&& other) noexcept : _capacity(other._capacity), _size(other._size), vec(new T[_capacity]) {
		vec = other.vec;
		other.vec = nullptr;
	}

	Vector(const size_t& sz, const T& val) : _capacity(sz), _size(sz), vec(new T[_capacity]) {
		for (int i = 0; i < _capacity; ++i) {
			vec[i] = val;
		}
	}

	size_t size() {
		return _size;
	}

	size_t capacity() {
		return _capacity;
	}

	T& at(const size_t& index) {
		std::lock_guard<std::mutex> lock(mtx);
		if (index >= _size) {
			throw std::out_of_range("Index out of bounds");
		}
		return vec[index];
	}

	T& front() const {
		return vec[0];
	}

	T& back() const {
		return vec[_size - 1];
	}

	T* data() const {
		return vec;
	}

	bool empty() {
		return _size;
	}

	_it<T> begin() {
		return _it<T>(vec);
	}

	_it<T> end() {
		return _it<T>(vec + _size);
	}

	void reserve(const size_t val) {
		std::lock_guard<std::mutex> lock(mtx);
		if (_capacity >= val) {
			return;
		}
		_capacity = val;
		T* temp = new T[_capacity];
		for (int i = 0; i < _size; ++i) {
			temp[i] = vec[i];
		}
		delete[] vec;
		vec = temp;
	}

	void shrink_to_fit() {
		std::lock_guard<std::mutex> lock(mtx);
		_capacity = _size;
		T* temp = new T[_capacity];
		memcpy(temp, vec, _size);
		delete[] vec;
		vec = temp;
	}

	void clear() {
		std::lock_guard<std::mutex> lock(mtx);
		_size = 0;
		T* temp = new T[_capacity];
		delete[] vec;
		vec = temp;
	}

	// (ny i gavno da)
	void insert(const _it<T>& it, const T& val) {
		std::lock_guard<std::mutex> lock(mtx);
		size_t index = std::distance(vec, it.ptr);
		if (index > _size) {
			throw std::out_of_range("Index out of bounds");
		}
		_capacity++;
		T* temp = new T[_capacity];
		int i = 0;
		for (; i <= index; ++i) {
			temp[i] = vec[i];
			if (i == index) {
				temp[i] = val;
			}
		}
		for (; i < _capacity; ++i) {
			temp[i] = vec[i - 1];
		}
		delete[] vec;
		vec = temp;
		_size++;
	}

	void insert(const size_t& index, const T& val) {
		std::lock_guard<std::mutex> lock(mtx);
		if (index > _size) {
			throw std::out_of_range("Index out of bounds");
		}
		_capacity++;
		T* temp = new T[_capacity];
		int i = 0;
		for (; i <= index; ++i) {
			temp[i] = vec[i];
			if (i == index) {
				temp[i] = val;
			}
		}
		for (; i < _capacity; ++i) {
			temp[i] = vec[i - 1];
		}
		delete[] vec;
		vec = temp;
		_size++;
	}

	void pop_back() {
		std::lock_guard<std::mutex> lock(mtx);
		_size--;
		T* temp = new T[_capacity];
		for (int i = 0; i < _size; ++i) {
			temp[i] = vec[i];
		}
		delete[] vec;
		vec = temp;
	}

	void push_back(const T& val) {
		std::lock_guard<std::mutex> lock(mtx);
		if (_capacity > _size + 1) {
			vec[_size] = val;
			_size++;
		}
		else {
			_capacity++;
			T* temp = new T[_capacity];
			for (int i = 0; i <= _size; ++i) {
				temp[i] = vec[i];
				if (i == _size) {
					temp[i] = val;
				}
			}
			delete[] vec;
			vec = temp;
			_size++;
		}
	}

	void push_front(const T& val) {
		std::lock_guard<std::mutex> lock(mtx);
		if (_capacity <= _size + 1) {
			_capacity++;
		}
		T* temp = new T[_capacity];
		temp[0] = val;
		for (int i = 0; i < _capacity; ++i) {
			temp[i + 1] = vec[i];
		}
		delete[] vec;
		vec = temp;
		_size++;
	}

	void resize(const size_t& sz) {
		std::lock_guard<std::mutex> lock(mtx);
		T a{};
		if (_size < sz) {
			if (_capacity > _size + sz) {
				for (int i = _size; i < _size + sz; ++i) {
					vec[i] = a;
				}
				_size = sz;
			}
			else {
				_capacity = _size + sz;
				T* temp = new T[_capacity];
				for (int i = 0; i < _size; ++i) {
					temp[i] = vec[i];
				}
				for (int i = _size; i < sz; ++i) {
					temp[i] = a;
				}
				delete[] vec;
				vec = temp;
				_size = sz;
			}
		}
		else {
			_size = sz;
			T* temp = new T[_capacity];
			for (int i = 0; i < _size; ++i) {
				temp[i] = vec[i];
			}
			delete[] vec;
			vec = temp;
		}
	}

	void resize(const size_t& sz, const T& val) {
		std::lock_guard<std::mutex> lock(mtx);
		if (_size < sz) {
			if (_capacity > _size + sz) {
				for (int i = _size; i < _size + sz; ++i) {
					vec[i] = val;
				}
				_size = sz;
			}
			else {
				_capacity = _size + sz;
				T* temp = new T[_capacity];
				for (int i = 0; i < _size; ++i) {
					temp[i] = vec[i];
				}
				for (int i = _size; i < sz; ++i) {
					temp[i] = val;
				}
				delete[] vec;
				vec = temp;
				_size = sz;
			}
		}
		else {
			_size = sz;
			T* temp = new T[_capacity];
			for (int i = 0; i < _size; ++i) {
				temp[i] = vec[i];
			}
			delete[] vec;
			vec = temp;
		}
	}

	Vector& slice(const int& start) {
		std::lock_guard<std::mutex> lock(mtx);
		if (start > _size || start < 0) {
			throw std::out_of_range("Index out of bounds");
		}
		T* temp = new T[_capacity];
		for (int i = start; i < _size; ++i) {
			temp[i - start] = vec[i];
		}
		_size -= start;
		delete[] vec;
		vec = temp;
		return *this;
	}

	Vector& slice(const int& start, const int& end) {
		std::lock_guard<std::mutex> lock(mtx);
		if (start < 0 || end < 0 || start > _size || end > _size) {
			throw std::out_of_range("Index out of bounds");
		}
		T* temp = new T[_capacity];
		_size = 0;
		for (int i = start; i < end; ++i) {
			temp[i - start] = vec[i];
			_size++;
		}
		delete[] vec;
		vec = temp;
		return *this;
	}

	Vector& slice(const int& start, const int& end, const int& step) {
		std::lock_guard<std::mutex> lock(mtx);
		if (start < 0 || end < 0 || start > _size || end > _size) {
			throw std::out_of_range("Index out of bounds");
		}
		if (step < 0) {
			T* temp = new T[_capacity];
			_size = 0;
			int j = 0;
			for (int i = start; i > end; i += step) {
				temp[j] = vec[i];
				j++;
				_size++;
			}
			delete[] vec;
			vec = temp;
		}
		else {
			T* temp = new T[_capacity];
			_size = 0;
			int j = 0;
			for (int i = start; i < end; i += step) {
				temp[j] = vec[i];
				j++;
				_size++;
			}
			delete[] vec;
			vec = temp;
		}
		return *this;
	}

	T& operator[](const size_t& index) {
		if (index >= _size) {
			throw std::out_of_range("Index out of bounds");
		}
		return vec[index];
	}

	// a = b
	Vector& operator=(const Vector<T>& other) {
		std::lock_guard<std::mutex> lock(mtx);
		_size = other._size;
		_capacity = other._capacity;
		delete[] vec;
		vec = new T[_capacity];
		for (int i = 0; i < _capacity; ++i) {
			vec[i] = other.vec[i];
		}
		return *this;
	}

	// a = std::move(b)
	Vector& operator=(Vector<T>&& other) noexcept {
		std::lock_guard<std::mutex> lock(mtx);
		_size = other._size;
		_capacity = other._capacity;
		delete[] vec;
		vec = other.vec;
		other.vec = nullptr;
		return *this;
	}

	// a = {1,2,3,4,5,...}
	Vector& operator=(const std::initializer_list<T>& v) {
		std::lock_guard<std::mutex> lock(mtx);
		_size = v.size();
		_capacity = _size;
		delete[] vec;
		vec = new T[_capacity];
		std::copy(v.begin(), v.end(), vec);
		return *this;
	}

	bool operator==(const Vector<T>& other) {
		return _size == other._size;
	}

	bool operator<(const Vector<T>& other) {
		return _size < other._size;
	}

	bool operator>(const Vector<T>& other) {
		return _size > other._size;
	}

	bool operator<=(const Vector<T>& other) {
		return _size <= other._size;
	}

	bool operator>=(const Vector<T>& other) {
		return _size >= other._size;
	}

	~Vector() {
		delete[] vec;
	}
};

