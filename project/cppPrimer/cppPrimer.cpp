// cppPrimer.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>

#include "lambda.h"
namespace lambdaTest
{
	void testLambda()
	{
		fcn1();
		fcn2();
		fcn3();
		fcn4();
		fcn5();
	}
}

#include "generics/Blob.h"
#include "generics/Generics.h"
#include "generics/Sales_data.h"
#include <string>
#include <iostream>
#include <list>
using namespace std;
namespace GenericsTest
{
	void testprint()
	{
		int a1[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
		int a2[] = { 1, 3, 5 };
		string a3[4] = { "mike" };
		print(a1); // instantiates print(int (&arr)[10])
		print(a2); // instantiates print(int (&arr)[3])
		print(a3); // instantiates print(string (&arr)[42])
	}
	int compareString(const char* const& v1, const char* const& v2)
	{
		return std::strcmp(v1, v2);
	}

	void testCompare()
	{
		cout << compare(1, 0) << endl;			  // T is int
		// instantiates int compare(const vector<int>&, const vector<int>&)
		vector<int> vec1{ 1, 2, 3 }, vec2{ 4, 5, 6 }; // 字典序
		cout << compare(vec1, vec2) << endl;	  // T is vector<int>

		const char* cp1 = "hi", * cp2 = "world";
		cout << "string" << compare(cp1, cp2, compareString) << endl; // converts arguments to string
	}
	void testBlob()
	{
		Blob<int> ia;						 // empty Blob<int>
		Blob<int> ia2 = { 0, 1, 2, 3, 4 };	 // Blob<int> with five elements
		vector<int> v1(10, 0);				 // ten elements initialized to 0
		Blob<int> ia3(v1.begin(), v1.end()); // copy elements from v1
		cout << "blobTest" << endl;
		cout << ia << "\n"
			<< ia2 << "\n"
			<< ia3 << endl;

		// these definitions instantiate two distinct Blob types
		Blob<string> names;	 // Blob that holds strings
		Blob<double> prices; // different element type

		// instantiates Blob<string> class and its
		//  initializer_list<const char*> constructor
		Blob<string> articles = { "a", "an", "the" }; // three elements

		// instantiates Blob<int> and the initializer_list<int> constructor
		Blob<int> squares = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

		// instantiates Blob<int>::size() const
		cout << squares << endl;
		for (size_t i = 0; i != squares.size(); ++i)
			squares[i] = i * i; // instantiates Blob<int>::operator[](size_t)
		cout << squares << endl;

		// instantiates the Blob<int> constructor that has
		// two vector<long>::iterator parameters
		vector<long> vl = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
		Blob<int> a1(vl.begin(), vl.end()); // copy from a vector

		// instantiates the Blob<int> class
		// and the Blob<int> constructor that has two int* parameters
		int arr[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
		Blob<int> a2(begin(arr), end(arr)); // copy from an array

		list<int> li(10, 0);				   // 10 elements all zeros
		Blob<int> zeros(li.begin(), li.end()); // copy from a list

		cout << a1 << "\n"
			<< zeros << endl;

		a1.swap(zeros);
		cout << a1 << "\n"
			<< zeros << endl;

		list<const char*> w = { "now", "is", "the", "time" };

		// instantiates the Blob<string> class and the Blob<string>
		// constructor that has two (list<const char*>::iterator parameters
		Blob<string> a3(w.begin(), w.end()); // copy from a list
		cout << a3 << endl;
	}

	void testGenerics()
	{
		testprint();
		testCompare();
		testBlob();
	}
}

int main()
{
	lambdaTest::testLambda();
	GenericsTest::testGenerics();
}