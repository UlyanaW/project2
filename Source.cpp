#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <regex>
#include <stdexcept>
#include <cctype>
#include <cstdlib>
#include <stack>
#include <stdexcept>
using namespace std;

typedef string ValueType;
typedef vector<ValueType> ValueTypeVector;

ValueTypeVector& split(const std::string& s, char delim, ValueTypeVector& elems)
{
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim))
	{
		elems.push_back(item);//std::stoi(item));
	}
	return elems;
}
ValueTypeVector split(const std::string& str, char delim)
{
	ValueTypeVector elems;
	split(str, delim, elems);
	return elems;
}

class Formula
{
public:
	string calculate(const string origin)
	{
		string err = origin.substr(0, 5);
		if (err == "ERROR")
		{
			return "ERROR";
		}
		string rpn_st = rpn(origin);
		stack<double> numbers;
		string temp;
		string::const_iterator it;

		for (it = rpn_st.begin(); it < rpn_st.end(); ++it)
		{
			if (isdigit(*it) || *it == '.')
				temp += *it;
			else if (*it == ' ')
			{
				if (temp == "")
				{
					continue;
				}
				numbers.push(atof(temp.c_str()));
				temp.clear();;
			}
			else
			{
				double temp1 = numbers.top();
				numbers.pop();
				double temp2 = numbers.top();
				numbers.pop();
				double result = 0;

				if (*it == '*')
					result += (temp2 * temp1);
				else if (*it == '/')
				{
					if (temp1 == 0)
						return "ERROR";
					else
						result += (temp2 / temp1);
				}
				else if (*it == '+')
					result += (temp2 + temp1);
				else if (*it == '-')
					result += (temp2 - temp1);
				numbers.push(result);
			}
		}
		return to_string(numbers.top());
	}

private:
	int priority(const char symbol)
	{
		if (symbol == '+' || symbol == '-')
			return 1;
		else if (symbol == '*' || symbol == '/')
			return 2;
		else
			return 0;
	}
	string rpn(const string origin)
	{
		stack<char> operation;
		string result;
		string::const_iterator it;

		for (it = origin.begin(); it < origin.end(); ++it)
		{
			if (isdigit(*it) || *it == '.')
			{
				result += *it;
			}
			else
			{
				int currentPr = priority(*it);
				if (currentPr)
				{
					result += " ";

					if (!operation.empty())
					{
						while (!operation.empty() && currentPr <= priority(operation.top()))
						{
							result = result + operation.top() + " ";
							operation.pop();
						}
					}
					operation.push(*it);
				}
			}
		}
		while (!operation.empty())
		{
			result = result + " " + operation.top();
			operation.pop();
		}
		return result;
	}
};

class Table
{
public:
	bool isNotEmpty = false;
	Table(Formula* f)
	{
		formula = f;
	}
	void Open(const string& name_file)
	{
		ifstream f_in(name_file);

		if (!f_in.is_open())
		{
			ofstream f_of(name_file);
		}
		this->name_file = name_file;
		while (!f_in.eof())
		{
			string row;
			getline(f_in, row);
			if (row.size() != 0)
			{
				for (int i = 0; i < row.size() - 1; i++)
				{
					if (row[i] == (char)44 && row[i + 1] == (char)44)
					{
						row.insert(i + 1, " ");
					}
					if (row[i] == (char)92 && row[i + 1] == (char)92)
					{
						row.erase(i, 1);
					}
					if (row[i] == (char)34 && row[i + 1] == (char)92)
					{
						row.erase(i, 2);
					}
				}
				table.push_back(split(row, ','));
				this->maxLength = GetLongestCellWord();
				this->isNotEmpty = true;
			}
		}
	}
	void Edit(const int& row, const int& col, const string& new_value)
	{
		if (row < 0 || row >= table.size() || col < 0 || col >= table[0].size())
		{
			throw std::invalid_argument("Invalid row or column");
		}
		table[row][col] = new_value;
	}
	void DisplayTable()
	{
		for (int i = 0; i < table.size(); i++)
		{
			for (int j = 0; j < table[i].size(); j++)
			{
				cout << "|" << setw(this->maxLength) << table[i][j];
			}
			cout << endl;
		}
	}
	string GetCell(int r, int c)
	{
		if (r > table.size() || c > table[r].size())
		{
			return "0";
		}
		string str = table[r][c];
		string::const_iterator it;
		for (it = str.begin(); it < str.end(); ++it)
		{
			if (*it == '+' || *it == '-' || *it == '/' || *it == '*' || *it == ' ')
			{
				return str;
			}
			else if (!isdigit(*it) && *it != '.')
			{
				return "0";
			}
		}
		int dots = 0;
		for (int i = 0; i < table[r][c].size(); i++)
		{
			if (str[i] == '.')
			{
				dots += 1;
			}
			if ((str[i] < '0' || str[i]>'9') && dots > 1)
			{
				return "0";
			}
		}
		return table[r][c];
	}
	string ParseCell(string str)
	{
		if (str == "+" || str == "-" || str == "/" || str == "*" || str == " ")
		{
			return str;
		}
		regex integ("[0-9]+");
		regex doub("[0-9]+\\.[0-9]+");
		regex reg("(R(\\d+)C(\\d+))");
		cmatch result;
		if (regex_match(str.c_str(), result, reg))
		{
			return GetCell(stoi(result[2]), stoi(result[3]));
		}
		else if (regex_match(str.c_str(), result, doub))
		{
			return to_string(stod(result[0]));
		}
		else if (regex_match(str.c_str(), result, integ))
		{
			return to_string(stoi(result[0]));
		}
		return "ERROR";
	}
	void CalculateAll()
	{
		string temp;
		for (int i = 0; i < table.size(); i++)
		{
			for (int j = 0; j < table[i].size(); j++)
			{
				ValueTypeVector result = split(table[i][j], ' ');
				for (int i = 0; i < result.size(); i++)
				{
					temp += ParseCell(result[i]);
				}
				if (result.size() != 1)
				{
					table[i][j] = formula->calculate(temp);
				}
				temp = "";
			}
		}
	}
	void Save()
	{
		ofstream f_in(name_file);
		for (int i = 0; i < table.size(); i++)
		{
			for (int j = 0; j < table[i].size(); j++)
			{
				f_in << table[i][j] << ",";
			}
			f_in << endl;
		}
	}
	void SaveAs(const string& file)
	{
		ofstream f_in(file);
		for (int i = 0; i < table.size(); i++)
		{
			for (int j = 0; j < table[i].size(); j++)
			{
				f_in << table[i][j] << ",";
			}
			f_in << endl;
		}
	}
	void Close()
	{
		table.clear();
	}
	void Help()
	{
		cout << "Open - opens file" << endl;
		cout << "Close - closes currently opened file" << endl;
		cout << "Edit - edit the info at the table" << endl;
		cout << "Print - displays the table" << endl;
		cout << "Save - saves the currently open file" << endl;
		cout << "SaveAs file - saves the currently open file in file" << endl;
		cout << "Help - prints this information" << endl;
		cout << "Exit - exists the program" << endl;
		// TODO: 
		// help
		//The following commands are supported :
		//open <file> opens <file>
		//    close closes currently opened file
		//    save saves the currently open file
	   //    saveas <file> saves the currently open file in <file>
	  //      help prints this information
	  //      exit exists the program
	}
private:
	vector<ValueTypeVector> table;
	Formula* formula;
	string name_file;
	int maxLength;
	int GetLongestCellWord()
	{
		int max = table[0][0].size();
		for (int i = 0; i < table.size(); i++)
		{
			for (int j = 0; j < table[i].size(); j++)
			{
				if (table[i][j].size() > max)
				{
					max = table[i][j].size();
				}
			}
		}
		return max;
	}
};


void Menu()
{
	cout << "1. Open" << endl;
	cout << "2. Close" << endl;
	cout << "3. Edit" << endl;
	cout << "4. Print" << endl;
	cout << "5. Save" << endl;
	cout << "6. Save as" << endl;
	cout << "7. Help" << endl;
	cout << "8. Exit" << endl;
}

int main()
{
	try
	{
		Formula f;
		Table t(&f);
		/*t.Open("Out.txt");

		string begin;
		string rpn_st;
		t.CalculateAll();
		t.DisplayTable();*/
		int num;
		int row;
		int col;
		bool fileIsOpened = false;
		string SaveAsFilePath;
		string fEdit;
		do
		{
			Menu();
			cout << "---------------------------" << endl;
			cout << "Enter the number for choose: " << endl;
			cin >> num;
			string temp;
			system("cls");
			switch (num)
			{
			case 1:
				t.Open("Out.txt");
				fileIsOpened = true;
				cin >> temp;
				system("cls");
				break;
			case 2:
				t.Close();
				fileIsOpened = false;
				cin >> temp;
				system("cls");
				break;
			case 3:
				if (fileIsOpened)
				{
					cout << "Enter the row: " << endl;
					cin >> row;
					cout << "Enter the col: " << endl;
					cin >> col;
					cout << "Enter data to replace: " << endl;
					cin >> fEdit;
					t.Edit(row, col, fEdit);
				}
				else
					cout << "File is closed" << endl;
				cin >> temp;
				system("cls");
				break;
			case 4:
				if (fileIsOpened)
				{
					t.CalculateAll();
					t.DisplayTable();
				}
				else
					cout << "File is closed" << endl;
				cin >> temp;
				system("cls");
				break;
			case 5:
				if (fileIsOpened)
				{
					t.Save();
				}
				else
					cout << "File is closed" << endl;
				cin >> temp;
				system("cls");
				break;
			case 6:
				if (fileIsOpened)
				{
					cout << "Enter the name of the file: " << endl;
					cin >> SaveAsFilePath;
					t.SaveAs(SaveAsFilePath);
				}
				else
					cout << "File is closed" << endl;
				cin >> temp;
				system("cls");
				break;
			case 7:
				if (fileIsOpened)
				{
					t.Help();
				}
				else
					cout << "File is closed" << endl;
				cin >> temp;
				system("cls");
				break;
			case 8:
				return 0;
				system("cls");
				break;
			}
		} while (num<8);
		
	}
	catch (exception ex)
	{
		cout << "\nException occured :  " << ex.what() << endl;
	}

}