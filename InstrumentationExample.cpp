#include <iostream>
#include<string>
using namespace std;

template <class T>
class MyType {
public:
	T val;
	string name;

	operator T() {
		cout << "variable " << name << " had been read and the value was: " << val << endl;
		return val;
	}


	MyType() { }
	MyType(const char* n) : name(n) { }
	MyType(const char* n, const T& r) : name(n), val(r) { }

	MyType(T x) : val(x) {}
	MyType operator = (T x) {
		cout << "variable " << name << " had been assigned and the value was: " << val << endl;
		val = x;
		return *this;
	}
	MyType operator =(MyType<T> other) {
		cout << "variable " << name << " had been assigned to "+other.name+" and the value was: " << other.val << endl;
		val = other.val;
		return *this;
	}

	ostream& operator << (ostream& out)
	{
		out <<name<<" "<< val;
		return out;
	}
	
};
template <class T>
class MyArray {
public:
	T* val;
	bool* dontCare;
	string name;
	

	//operator T () {
	//	cout << "variable " << name << " had been read and the value was: " << val << endl;
		//return val;
	//}

	MyArray() {}
	MyArray(const char* n) : name(n) {}
	MyArray(const char* n, T* r) : name(n), val(r) {}

	MyArray(T* x) : val(x) {}
	

	T& operator [] (int index) {
		cout << name+"["+to_string(index)+"] was read and the value was: "+to_string(*(val+index))<<endl;
		return *(val+index);

	}

};

MyType<int> linear_search(int* a, int left, int right, int d, int n) {

	if (left < 0 || right >= n || left > right) {
		return -2; 
	}

	int i = left;
	for (; i <= right; i = i + 1) {// pay attention to ++;
		if (a[i] == d) {
			return i;
		}
	}

	return -1;

}
MyType<int> linear_search_instrumented(MyArray<int> a, MyType<int> left, MyType<int> right, MyType<int> d, MyType<int> n) {

	if (left < 0 || right >= n || left > right) {
		return MyType<int>("ToReturn", -2); 
	}

	MyType<int> i("i");
	i = left;
	for (; i <= right; i=i+1) {
		if (a[i] == d) {			
			return MyType<int>("ToReturn",i); 
		}
	}

	return MyType<int>("toReturn",-1);
	
}

int min_index(int* a, int left, int right, int n) {
	if (left < 0 || right >= n)
		return -1;

	int min_index = left;

	for (int i = left; i < right; i = i + 1) {
		if (a[i] < a[min_index])
			min_index = i;
	
	}
	return min_index;



}
MyType<int> min_index_instrumented(MyArray<int> a, MyType<int> left, MyType<int> right, MyType<int> n) {
	if (left < 0 || right >= n)
		return MyType<int>("ToReturn",-1);

	MyType<int> min_index("min_index");
	min_index = left;

	for (MyType<int> i = left; i < right; i = i + 1) {
		if (a[i] < a[min_index])
			min_index = i;

	}
	return MyType<int>("ToReturn",min_index);



}

int main() {

	
	int* arr = new int[5];
	arr[0] = arr[1] = arr[2] = arr[4] = 5;
	arr[3] = -2;
	MyArray<int> Arr("MyArray", arr);
	MyType<int>i("i", 3);
	Arr[0] = -1;
	MyType<int> c1 = min_index_instrumented(Arr, MyType<int>("left",0),MyType<int>("right",4), MyType<int>("n",5));
	cout << "MinIndex: "<<c1;
	cout << endl << endl;
	MyType<int> c = linear_search_instrumented(Arr, MyType<int>("left", 0), MyType<int>("right", 4), MyType<int>("d", -2), MyType<int>("n", 5));
	cout <<"LinearSearch: "<< c;
	return 0;
}