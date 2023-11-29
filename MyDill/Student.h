#pragma once

extern "C" class __declspec(dllexport) MyClass


{
public:
	MyClass();

	~MyClass();

	int add(int a, int b);
};