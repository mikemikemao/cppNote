#include <iostream>
#include <list>
using namespace std;

#include "Generics.h"
#include "Blob.h"
// #include "Sales_data.h"
namespace cppPrimer
{
	namespace GenericsTest
	{
		void testprint()
		{
			int a1[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
			int a2[] = {1, 3, 5};
			string a3[4] = {"mike"};
			print(a1); // instantiates print(int (&arr)[10])
			print(a2); // instantiates print(int (&arr)[3])
			print(a3); // instantiates print(string (&arr)[42])
		}
		int compareString(const char *const &v1, const char *const &v2)
		{
			return std::strcmp(v1, v2);
		}

		void testCompare()
		{
			cout << compare(1, 0) << endl;			  // T is int
													  // instantiates int compare(const vector<int>&, const vector<int>&)
			vector<int> vec1{1, 2, 3}, vec2{4, 5, 6}; // 字典序
			cout << compare(vec1, vec2) << endl;	  // T is vector<int>

			const char *cp1 = "hi", *cp2 = "world";
			cout << "string" << compare(cp1, cp2, compareString) << endl; // converts arguments to string
		}
		void testBlob()
		{
			Blob<int> ia;						 // empty Blob<int>
			Blob<int> ia2 = {0, 1, 2, 3, 4};	 // Blob<int> with five elements
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
			Blob<string> articles = {"a", "an", "the"}; // three elements

			// instantiates Blob<int> and the initializer_list<int> constructor
			Blob<int> squares = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

			// instantiates Blob<int>::size() const
			cout << squares << endl;
			for (size_t i = 0; i != squares.size(); ++i)
				squares[i] = i * i; // instantiates Blob<int>::operator[](size_t)
			cout << squares << endl;

			// instantiates the Blob<int> constructor that has
			// two vector<long>::iterator parameters
			vector<long> vl = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
			Blob<int> a1(vl.begin(), vl.end()); // copy from a vector

			// instantiates the Blob<int> class
			// and the Blob<int> constructor that has two int* parameters
			int arr[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
			Blob<int> a2(begin(arr), end(arr)); // copy from an array

			list<int> li(10, 0);				   // 10 elements all zeros
			Blob<int> zeros(li.begin(), li.end()); // copy from a list

			cout << a1 << "\n"
				 << zeros << endl;

			a1.swap(zeros);
			cout << a1 << "\n"
				 << zeros << endl;

			list<const char *> w = {"now", "is", "the", "time"};

			// instantiates the Blob<string> class and the Blob<string>
			// constructor that has two (list<const char*>::iterator parameters
			Blob<string> a3(w.begin(), w.end()); // copy from a list
			cout << a3 << endl;
		}

		void testGenerics()
		{
			// testprint();
			testCompare();
			// testBlob();
		}
	}

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

	namespace unorderedMapTest
	{
		
	}

}

///////////////////////////////////////////////////////
#include "ListGraph.h"
#include <cstring>
#include <string>
namespace alg
{
	namespace graph
	{
		void testListGraph()
		{
			ListGraph<string, int> graph;
			graph.addEdge("V0", "V1");
			graph.addEdge("V1", "V0");

			graph.addEdge("V0", "V2");
			graph.addEdge("V2", "V0");

			graph.addEdge("V0", "V3");
			graph.addEdge("V3", "V0");

			graph.addEdge("V1", "V2");
			graph.addEdge("V2", "V1");

			graph.addEdge("V2", "V3");
			graph.addEdge("V3", "V2");

			graph.print();

			// graph.addEdge("V1", "V0", 9);
			// graph.addEdge("V1", "V2", 3);
			// graph.addEdge("V2", "V0", 2);
			// graph.addEdge("V2", "V3", 5);
			// graph.addEdge("V3", "V4", 1);
			// graph.addEdge("V0", "V4", 6);

			// graph.removeEdge("V0", "V4");
			// graph.removeVertex("V0");

			// graph.print();
		}

	}
}
#include <unordered_map>
#include "Sales_data.h"
void testUnorderedMap(){
	unordered_map<Sales_data,int> nameSalesData;
	Sales_data item1("978-0590353401", 15, 15.99);
	Sales_data item2("978-0590353402", 20, 15.99);
	Sales_data item3("978-0590353403", 25, 15.99);

	nameSalesData.insert(make_pair(item1,15));
	nameSalesData.insert(make_pair(item2,20));
	nameSalesData.insert(make_pair(item3,25));

	cout << nameSalesData[item1] << endl;
	cout << nameSalesData[item2] << endl;
	cout << nameSalesData[item3] << endl;

	cout << "========================================================" << endl;
	unordered_map<string,Sales_data> strSalesData;

	strSalesData.insert(make_pair("mike",item1));
	strSalesData.insert(make_pair("mao",item2));
	strSalesData.insert(make_pair("tom",item3));

	cout << strSalesData["1"] << endl;
	string tmp = "123";
	if(strSalesData.find(tmp) == strSalesData.end()){
		cout << "no " << tmp << endl;
	}else{
		cout << strSalesData[tmp] << endl;
	}

	cout << strSalesData["mike"] << endl;
	cout << strSalesData["mao"] << endl;
	cout << strSalesData["tom"] << endl;
}




///////////////////////////////////////////////////////
#include "sipTest.h"
namespace app
{
	namespace sip
	{
		void testSip()
		{
			sipTest();
		}

	}
}

int main(int argc, char **argv)
{
	cout << "main" << endl;
	// GenericsTest :: testGenerics();
	// lambdaTest :: testLambda();
	//alg::graph::testListGraph();
	//testUnorderedMap();
	app::sip::testSip();
	return 0;
}