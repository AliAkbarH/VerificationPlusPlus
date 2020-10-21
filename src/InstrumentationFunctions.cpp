#include <iostream>
#include<string>
#include<fstream>
#include<vector>
#include <iterator>
#include<sstream>
#include<algorithm>
using namespace std;


class Variable
{
public:
	Variable() :Name(""), Type(""), Value(""), declLine(-1), declCol(-1), isBOp(false), isUOp(false), isInput(false), isOutput(false) {};

	Variable(string name, string type, string value = "") :Name(name), Type(type), Value(value), declLine(-1), declCol(-1), isBOp(false), isUOp(false), isInput(false), isOutput(false) {};

	string Name;
	string Type;
	string Value;
	int declLine;
	int declCol;
	bool isBOp;
	bool isUOp;
	bool isInput;
	bool isOutput;


};

bool operator < (const Variable& rhs, const Variable& lhs) {
	if (rhs.declLine < lhs.declLine)return true;
	else if (rhs.declLine > lhs.declLine)return false;
	else return rhs.declCol < lhs.declCol;

}
template <class T>
class MyElement {

public:
	T* arr;
	int index;
	string name;
	bool isRead, isAssigned;
	MyElement() :arr(NULL), index(0), name(""), isRead(false), isAssigned(false) {};
	MyElement(T* a, int ind, const string n) : arr(a), index(ind), name(n) {}

	MyElement operator = (T x) {
		cout << "Array Elemet " << name << "[" << index << "] had been assigned and the value was: " << x << endl;
		isAssigned = true;
		arr[index] = x;
		return *this;
	}
	MyElement operator = (MyElement<T> x) {
		cout << "Array Elemet " << name << "[" << index << "] had been assigned and the value was: " << x.arr[x.index] << endl;
		isAssigned = true;
		arr[index] = x.arr[x.index];
		return *this;
	}

	operator T() {
		cout << "Array element " << name << "[" << index << "] had been read and the value was: " << arr[index] << endl;
		isRead = true;
		return arr[index];
	}
};

template <class T>
class MyType {
public:
	T val;
	string name;
	bool isRead;
	bool isAssigned;

	operator T() {
		cout << "variable " << name << " had been read and the value was: " << val << endl;
		isRead = true;
		return val;
	}


	MyType() { }
	MyType(const char* n) : name(n), isRead(false), isAssigned(false) { }
	MyType(const char* n, const T& r) : name(n), val(r) { }

	MyType(T x) : val(x) {}
	MyType operator = (T x) {
		cout << "variable " << name << " had been assigned and the value was: " << x << endl;
		isAssigned = true;
		val = x;
		return *this;
	}

	MyType(const MyType<T>& other) {
		val = other.val;
		name = other.name;
		isRead = other.isRead;
		isAssigned = other.isAssigned;

	}
	ostream& operator << (ostream& out)
	{
		out << name << " " << val;
		return out;
	}

};

template <class T>
class MyArray {
public:
	T* val;
	string name;
	int size;
	MyElement<T> el;//temporary for array access


	operator T () {
		return *val;
	}

	MyArray() {}
	MyArray(const char* n) : name(n) { val = NULL; size = 0; }
	MyArray(const char* n, int s) : name(n), size(s), el(val, 0, name) {
		val = new T(size);
	}

	MyArray(T* x) : val(x) {}
	MyArray operator = (T* other) {
		val = other;
		return *this;
	}

	MyElement<T>& operator [] (int index) {
		el.arr = val;
		el.index = index;
		el.name = name;
		//cout << "Access operator of array" << name << " at index " << index << " called" << endl;
		return el;
	}

};

int linear_search(int* a, int left, int right, int d, int n) {

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

	MyType<int> i("i", left);

	for (; i <= right; i = i + 1) {
		if (a[i] == d) {
			return MyType<int>("ToReturn", i);
		}
	}

	return MyType<int>("toReturn", -1);

}

int min_index(int* a, int left, int right, int n) {
	if (left < 0 || right >= n)
		return -1;

	int min_index = left;

	for (int i = left; i <= right; i = i + 1) {
		if (a[i] < a[min_index])
			min_index = i;

	}
	return min_index;
}


MyType<int> min_index(MyArray<int> a, MyType<int> left, MyType<int> right, MyType<int> n) {
	if (left < 0 || right >= n)
		return MyType<int>("ToReturn", -1);

	MyType<int> min_index("min_index", left);
	for (MyType<int> i = left; i <= right; i = i + 1) {
		if (a[i] < a[min_index])
			min_index = i;
	}
	return MyType<int>("ToReturn", min_index);
}


string instrumentVariable(string decl, Variable var) {
	string value;
	string instrumented_line = "";
	string varName;
	int declarationLength = 0;
	int temp = var.Name.length() - 1;
	while (var.Name[temp] != '_')temp--;
	varName = var.Name.substr(0, temp);
	string delimiter(1, decl[decl.length() - 1]);

	bool isInit = (decl.find('=') != string::npos);

	if (var.Type.find('*') == string::npos) {
		instrumented_line += "MyType<" + var.Type + "> " + varName;
	}
	else {
		instrumented_line += "MyArray<" + var.Type.substr(0, var.Type.find('*') - 1) + "> " + varName;
	}
	if (!var.isInput) {
		instrumented_line = instrumented_line + "(\"" + varName + "\"";

	}
	if (!isInit)
		if (!var.isInput)
			instrumented_line += ")" + delimiter;
		else
			instrumented_line += delimiter;
	else {
		int valStart = decl.find('=') + 1;
		value = decl.substr(valStart, decl.length() - valStart - 1);

		instrumented_line += "," + value + ")" + delimiter;
	}

	return instrumented_line;
}

string instrumentReturn(string decl, Variable ret, Variable func) {
	if (func.Type == "void")return "return;";
	string instrumented_line = "return MyType<" + func.Type + ">(\"ToReturn\"," + decl.substr(7, decl.length() - 8) + ");";
	return instrumented_line;

}

int linear_search(MyArray<int> a, MyType<int> left, MyType<int> right, MyType<int> d, MyType<int> n) {

	if (left < 0 || right >= n || left > right) {
		return MyType<int>("ToReturn", -2);
	}
	MyType<int> i("i", left);
	for (; i <= right; i = i + 1) {// pay attention to ++;
		if (a[i] == d) {
			return MyType<int>("ToReturn", i);
		}
	}
	return MyType<int>("ToReturn", -1);
}

vector<string> ParseArrayValues(string arrayVal) {
	string arrayValues = arrayVal;
	arrayValues.erase(0, 1);
	vector<string> toReturn;
	size_t pos = 0;
	string token;
	while ((pos = arrayValues.find(",")) != string::npos) {
		token = arrayValues.substr(0, pos);
		toReturn.push_back(token);
		arrayValues.erase(0, pos + 2);
	}
	return toReturn;


}

string getVariableDeclaration(Variable var) {

	string line;
	int temp = var.Name.length() - 1;
	while (var.Name[temp] != '_')temp--;
	string varName = var.Name.substr(0, temp);
	bool isArray = var.Type.find("*") != string::npos || var.Type.find("[]") != string::npos;
	if (isArray && var.Type.find("*") != string::npos) {
		vector<string> values = ParseArrayValues(var.Value);
		string l = "MyArray<" + var.Type.substr(0, var.Type.find("*") - 1) + "> " + varName + "(\"" + varName + ");\n";;
		l += "\t" + var.Type + " " + var.Name + " = new int (" + to_string(values.size()) + "\");\n";
		for (int i = 0; i < values.size(); i++) {
			l += "\t" + varName + "[" + to_string(i) + "] = " + values[i] + ";\n";

		}
		l += "\t" + varName + " = " + var.Name + ";\n";

		return l;
	}
	else
		return "MyType<" + var.Type + "> " + varName + "(\"" + varName + "\"," + var.Value + ");\n";

}

void instrument(string functionName, string fileLocation, vector<Variable> vars, Variable Function, vector<Variable> returns) {

	sort(vars.begin(), vars.end());
	int currentLine = 1;
	int currentReturn = 0;
	int currentVar = 0;
	fstream file, output;
	file.open(fileLocation);
	output.open("Output.txt");
	string line;
	output << "#include \"mytypes.h\"" << endl;
	while (getline(file, line)) {
		string instrumented_line;
		int prevDeclEnd = -1;
		while (currentVar < vars.size() && currentLine == vars[currentVar].declLine) {
			string decl;
			int declarationLength = 0;
			int declarationBeg = vars[currentVar].declCol - 1;
			int declarationEnd = declarationBeg;
			for (int i = declarationBeg; i < line.length(); i++) {
				declarationLength++;
				if (line[i] == ';' || line[i] == ',' || line[i] == ')')
				{
					declarationEnd = i;
					break;
				}
			}
			decl = line.substr(declarationBeg, declarationLength);
			instrumented_line = instrumented_line + line.substr(prevDeclEnd + 1, declarationBeg - prevDeclEnd - 1) + instrumentVariable(decl, vars[currentVar]);
			currentVar++;
			prevDeclEnd = declarationEnd;
		}
		while (currentReturn < returns.size() && currentLine == returns[currentReturn].declLine) {
			string decl;
			int declarationLength = 0;
			int declarationBeg = returns[currentReturn].declCol - 1;
			int declarationEnd = declarationBeg;
			for (int i = declarationBeg; i < line.length(); i++) {
				declarationLength++;
				if (line[i] == ';' || line[i] == ',' || line[i] == ')')
				{
					declarationEnd = i;
					break;
				}
			}
			decl = line.substr(declarationBeg, declarationLength);
			instrumented_line = instrumented_line + line.substr(prevDeclEnd + 1, declarationBeg - prevDeclEnd - 1) + instrumentReturn(decl, returns[currentReturn], Function);
			currentReturn++;
			prevDeclEnd = declarationEnd;
		}

		instrumented_line += line.substr(prevDeclEnd + 1, line.length() - 1 - prevDeclEnd);
		if (currentLine == Function.declLine && Function.Type != "void") {
			int temp = Function.declCol;
			string inst = instrumented_line.substr(0, Function.declCol - 1);
			while (instrumented_line[temp] != ' ')temp++;
			temp++;
			inst = inst + "MyType<" + Function.Type + "> " + instrumented_line.substr(temp, instrumented_line.length() - temp);
			instrumented_line = inst;
		}
		output << instrumented_line << endl;
		currentLine++;
	}

	output.close();
	file.close();
}

void createMain(vector<Variable> vars, Variable Function) {
	sort(vars.begin(), vars.end());
	fstream output;
	output.open("main.cpp");
	output << "#include<iostream>" << endl;
	output << "#include \"" + Function.Name + "\"" << endl;
	output << "using namespace std;" << endl;
	output << "int main()" << endl << "{" << endl;

	for (int i = 0; i < vars.size(); i++) {
		if (!vars[i].isInput)continue;

		string declaration = getVariableDeclaration(vars[i]);
		output << "\t" << declaration;
	}
	int temp = Function.Name.length() - 1;
	while (Function.Name[temp] != '_')temp--;
	string functionName = Function.Name.substr(0, temp);
	if (Function.Type != "void") {
		
		string lin = "\tMyType<" + Function.Type + "> out = " + functionName + "(";
		for (int i = 0; i < vars.size(); i++) {
			if (!vars[i].isInput || vars[i].declLine != Function.declLine)continue;
			int temp = vars[i].Name.length() - 1;
			while (vars[i].Name[temp] != '_')temp--;
			string name = vars[i].Name.substr(0, temp);
			lin += name + ",";

		}
		lin.erase(lin.length() - 1, 1);
		lin += ");";
		output << lin << endl;


	}
	else {
		string lin = "\t" + functionName + "(";
		for (int i = 0; i < vars.size(); i++) {
			if (!vars[i].isInput || vars[i].declLine != Function.declLine)continue;
			int temp = vars[i].Name.length() - 1;
			while (vars[i].Name[temp] != '_')temp--;
			string name = vars[i].Name.substr(0, temp);
			lin += name + ",";

		}
		lin.erase(lin.length() - 1, 1);
		lin += ");";
		output << lin << endl;


	}
	output << "return 0;" << endl << "}";
	output.close();

}



int main() {
	Variable a("a_5", "int *", "{1, 3, 4, 5}");
	a.declCol = 20;
	a.declLine = 4;
	a.isInput = true;
	Variable left("left_5", "int", "3");
	left.declCol = 28;
	left.declLine = 4;
	left.isInput = true;
	Variable right("right_5", "int", "0");
	right.declCol = 38;
	right.declLine = 4;
	right.isInput = true;
	Variable d("d_4", "int", "1");
	d.declCol = 49;
	d.declLine = 4;
	d.isInput = true;
	Variable n("n_4", "int", "2");
	n.declCol = 56;
	n.declLine = 4;
	n.isInput = true;
	Variable i("i_9", "int");
	i.declCol = 2;
	i.declLine = 9;
	i.isInput = false;
	vector<Variable> vars;
	vars.push_back(a);
	vars.push_back(n);
	vars.push_back(d);
	vars.push_back(left);
	vars.push_back(right);
	vars.push_back(i);
	Variable r1("", "", "");
	r1.declCol = 3;
	r1.declLine = 7;
	Variable r2("", "", "");
	r2.declCol = 4;
	r2.declLine = 12;
	Variable r3("", "", "");
	r3.declCol = 2;
	r3.declLine = 15;

	vector<Variable> returns = { r1,r2,r3 };
	Variable func("linear_search", "int");
	func.declCol = 1;
	func.declLine = 4;

	instrument("linear_search", "InstrumentationTest.txt", vars, func, returns);
	createMain(vars, func);
	
	return 0;
}
	

