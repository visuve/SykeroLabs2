#pragma once

class mutex_guard {
public:
	mutex_guard(mutex_t* mutex);
	~mutex_guard();

private:
	mutex_t* _mutex;
};

class recursive_mutex_guard {
public:
	recursive_mutex_guard(recursive_mutex_t* mutex);

	~recursive_mutex_guard();

private:
	recursive_mutex_t* _mutex;
};