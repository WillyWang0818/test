#include "stdafx.h"
#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <set>
#include <cmath>
#include <vector>
//test
using namespace std;
//fuckfuck
int inputVertex = 0;
vector<string> input;
//test
int commandSOP = 0;
//Don't care
int	commandX;
vector<string> sop;
vector<string> x;
set<int> productTerms, dontCareTerms;

struct minterm
{
	string binary = "";
	set<int> contentMin;
	bool used = false;
};
vector<vector<minterm>> oneNumber;

vector<minterm> asterisk;
struct sopTerm
{
	int sopIndex = 0;
	vector<int> ptr;
};

vector<sopTerm> petrick;

int outputVertex = 0;
vector<string> output;


int main()
{
	string filename;

	cout << "Enter the filename:" << endl;
	cin >> filename;

	string outputFileName = "output.pla";

	ifstream fin;
	fin.open(filename);

	if (!fin)
	{
		cout << "無法讀檔" << endl;
		return 1;
	}

	// 讀檔
	string line = "";
	while (getline(fin, line))
	{
		stringstream temp(line);
		string word = "";
		while (temp >> word)
		{
			if (word == ".i")
			{
				temp >> inputVertex;
			}
			else if (word == ".o")
			{
				temp >> outputVertex;
			}
			else if (word == ".ilb")
			{
				for (int i = 0; i < inputVertex; i++)
				{
					temp >> word;
					input.push_back(word);
				}
			}
			else if (word == ".ob")
			{
				for (int i = 0; i < outputVertex; i++)
				{
					temp >> word;
					output.push_back(word);
				}
			}
			else if (word == ".p")
			{
				int totalCommand = 0;
				temp >> totalCommand;
				for (int i = 0; i < totalCommand; i++)
				{
					getline(fin, line);

					temp.clear();
					temp.str(line);

					string term = "";
					char answer = '\0';
					temp >> term;
					temp >> answer;

					if (answer == '1')
					{
						commandSOP++;
						sop.push_back(term);
					}
					else if (answer == '-')
					{
						commandX++;
						x.push_back(term);
					}
				}
			}
			else if (word == ".e")
			{
				cout << "結束" << endl;
			}
			else
			{
				cout << "錯誤的input" << endl;
				return 1;
			}
		}
		temp.str("");
	}
	fin.close();

	// Processing
	//找SOP
	for (int i = 0; i < commandSOP; i++)
	{
		vector<int> number;
		for (int j = 0, w = (inputVertex - 1); j < inputVertex; j++, w--)
		{
			if (sop[i][j] == '1')
			{
				if (j == 0)
					number.push_back(pow(2, w));
				else
					for (int a = 0; a < number.size(); a++)
						number[a] += pow(2, w);
			}
			else if (sop[i][j] == '0')
			{
				if (j == 0)
					number.push_back(0);
			}
			else if (sop[i][j] == '-')
			{
				if (j == 0)
				{
					number.push_back(0);
					number.push_back(pow(2, w));
				}
				else
				{
					int amount = number.size();
					for (int a = 0; a < amount; a++)
					{
						number.push_back(number[a] + pow(2, w));
					}
				}
			}
		}
		for (int a = 0; a < number.size(); a++)
		{
			productTerms.insert(number[a]);
		}
	}

	//找Don't care
	for (int i = 0; i < commandX; i++)
	{
		vector<int> number;
		for (int l = 0, w = (inputVertex - 1); l < inputVertex; l++, w--)
		{
			if (x[i][l] == '1')
			{
				if (l == 0)
					number.push_back(pow(2, w));
				else
					for (int a = 0; a < number.size(); a++)
						number[a] += pow(2, w);
			}
			else if (x[i][l] == '0')
			{
				if (l == 0)
					number.push_back(0);
			}
			else if (x[i][l] == '-')
			{
				if (l == 0)
				{
					number.push_back(0);
					number.push_back(pow(2, w));
				}
				else
				{
					int amount = number.size();
					for (int a = 0; a < amount; a++)
					{
						number.push_back(number[a] + pow(2, w));
					}
				}
			}
		}
		for (int a = 0; a < number.size(); a++)
		{
			dontCareTerms.insert(number[a]);
		}
	}


	for (int i = 0; i <= inputVertex; i++)
	{
		vector<minterm> temp;
		oneNumber.push_back(temp);
	}

	// 轉成int
	// SOP
	set<int>::iterator itOne;
	for (itOne = productTerms.begin(); itOne != productTerms.end(); itOne++)
	{
		int numberOfOne = 0;
		stringstream stream;
		// Decimal to Binary
		for (int d = inputVertex - 1; d >= 0; d--)
		{
			stream << ((*itOne >> d) % 2);
			if ((*itOne >> d) % 2)
				numberOfOne++;
		}
		minterm temp;
		stream >> temp.binary;
		temp.contentMin.insert(*itOne);
		oneNumber[numberOfOne].push_back(temp);
	}
	// Don't care
	for (itOne = dontCareTerms.begin(); itOne != dontCareTerms.end(); itOne++)
	{
		int numberOfOne = 0;
		stringstream stream;
		// Decimal to Binary
		for (int d = inputVertex - 1; d >= 0; d--)
		{
			stream << ((*itOne >> d) % 2);
			if ((*itOne >> d) % 2)
				numberOfOne++;
		}
		minterm temp;
		stream >> temp.binary;
		temp.contentMin.insert(*itOne);
		oneNumber[numberOfOne].push_back(temp);
	}


	// Quine–McCluskey algorithm
	bool finish = false;
	while (!finish)
	{
		for (int o = 0, on = 1; o < oneNumber.size() - 1; o++, on++)
		{
			vector<minterm> replace;
			// Compare o and o+1 content minterm
			for (int i = 0; i < oneNumber[o].size(); i++)
			{
				for (int in = 0; in < oneNumber[on].size(); in++)
				{
					// Check whether only one different
					bool onlyOne = false;
					int diffPosition = 0;
					for (int b = 0; b < inputVertex; b++)
					{
						if (oneNumber[o][i].binary[b] != oneNumber[on][in].binary[b])
						{
							if (onlyOne == false)
							{
								onlyOne = true;
								diffPosition = b;
							}
							else
							{
								onlyOne = false;
								break;
							}
						}
					}

					// 只有一個不同
					if (onlyOne)
					{
						oneNumber[o][i].used = true;
						oneNumber[on][in].used = true;
						// Push into replacement vector
						minterm temp;
						temp = oneNumber[o][i];
						// Change the index of the binary representation
						temp.binary[diffPosition] = '-';
						// Check whether replace already has the binary representation
						bool exist = false;
						for (int r = 0; r < replace.size(); r++)
						{
							if (replace[r].binary == temp.binary)
								exist = true;
						}
						if (!exist)
						{
							// Combine two contentMin
							set<int>::iterator c;
							for (c = oneNumber[on][in].contentMin.begin(); c != oneNumber[on][in].contentMin.end(); c++)
							{
								temp.contentMin.insert(*c);
							}
							// Reset used;
							temp.used = false;
							replace.push_back(temp);
						}
					}
				}
				if (oneNumber[o][i].used == false)
				{
					asterisk.push_back(oneNumber[o][i]);
				}
			}
			// Kill all of o's minterm
			oneNumber[o].clear();
			// Reset the minterm with replace
			for (int r = 0; r < replace.size(); r++)
			{
				oneNumber[o].push_back(replace[r]);
			}
		}
		// Check whether the least number of 1 has unused
		vector<minterm> least;
		for (int l = 0; l < oneNumber[oneNumber.size() - 1].size(); l++)
		{
			if (oneNumber[oneNumber.size() - 1][l].used == false)
			{
				minterm temp = oneNumber[oneNumber.size() - 1][l];
				bool exist = false;
				for (int lr = 0; lr < least.size(); lr++)
				{
					if (least[lr].binary == temp.binary)
						exist = true;
				}
				if (!exist)
				{
					// Reset used;
					temp.used = false;
					least.push_back(temp);
				}
			}
		}
		// Combine least with asterisk
		for (int l = 0; l < least.size(); l++)
		{
			asterisk.push_back(least[l]);
		}
		// Clear least
		least.clear();
		// Delete the least number of 1
		oneNumber.erase(oneNumber.end() - 1);
		// Check whether all oneNumber's content is Clear
		finish = true;
		for (int o = 0; o < oneNumber.size(); o++)
		{
			if (oneNumber[o].size() != 0)
				finish = false;
		}
	}

	oneNumber.clear();

	// Petrick's method
	set<int>::iterator it;
	// Set the petrick table
	for (it = productTerms.begin(); it != productTerms.end(); it++)
	{
		sopTerm temp;
		temp.sopIndex = *it;
		petrick.push_back(temp);
	}

	for (int a = 0; a < asterisk.size(); a++)
	{
		int petrickC = 0;
		for (it = asterisk[a].contentMin.begin(); it != asterisk[a].contentMin.end(); it++)
		{
			bool find = false,
				isDontCare = false;
			set<int>::iterator dc;

			for (dc = dontCareTerms.begin(); dc != dontCareTerms.end(); dc++)
			{
				if (*it == *dc)
					isDontCare = true;
			}
			while (!find && !isDontCare)
			{
				if (petrick[petrickC].sopIndex == *it)
				{
					petrick[petrickC].ptr.push_back(a);
					find = true;
				}
				petrickC++;
			}
		}
	}

	//Output
	ofstream fout;
	fout.open(outputFileName);

	if (!fout)
	{
		cout << "無法寫檔" << endl;
		return 1;
	}

	fout << ".i " << inputVertex << endl;

	fout << ".o " << outputVertex << endl;

	fout << ".lib";
	for (int i = 0; i < inputVertex; i++)
		fout << " " << input[i];
	fout << endl;

	fout << ".ob";
	for (int o = 0; o < outputVertex; o++)
		fout << " " << output[o];
	fout << endl;

	fout << ".p " << asterisk.size() << endl;

	for (int i = 0; i < asterisk.size(); i++)
	{
		string command = asterisk[i].binary;
		fout << command << " 1\n";
	}

	fout << ".e\n";
	fout.close();

	return 0;
}
