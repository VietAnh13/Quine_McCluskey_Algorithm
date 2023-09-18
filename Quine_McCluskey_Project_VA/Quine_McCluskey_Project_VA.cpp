#include <iostream>
#include <math.h>
#include <string>
#include <vector>
#include <algorithm>
#include <Bits.h>

using namespace std;

#define superLine cout << endl << "---------------------------------------------------------------------------------------------------" << endl;

int vars;

string PrintfCharFuntion()
{
	string PrintfChar = "\0";
	for (int i = 0; i < vars; i++) {
		PrintfChar += char(i + 'a');
		if (i < vars - 1) PrintfChar += ",";
	}
	return PrintfChar;
}
void CreateMintermAndDontCare(string Text, vector<int>& minterms,
	vector<int>& implicants, bool dontcare)
{
	bool Arr = false, stop = false;
	string vText = "\0", vText2 = "\0";
	for (size_t i = 0; i < Text.length(); i++) {
		if (i == Text.length() - 1) stop = true;
		if (Text[i] != ',') {
			if (Text[i] == '-') {
				Arr = true; vText2 = vText; vText = "\0";
			}
			else vText += Text[i];
		}
		else stop = true;
		if (stop) {
			if (Arr) {
				int j = stoi(vText2), key = stoi(vText) + 1;
				do {
					if (!dontcare) minterms.push_back(j);
					implicants.push_back(j);
				} while (++j != key);
				vText2 = vText = "\0"; Arr = false;
			}
			else {
				if (!dontcare) minterms.push_back(stoi(vText));
				implicants.push_back(stoi(vText)); vText = "\0";
			}
			stop = false;
		}
	}
}
void Input(vector<int>& minterms, vector<int>& implicants, bool mints)
{
	cout << "Enter the number of variables: "; cin >> vars;
	cout << "Max minterms = " << (1 << vars) << endl;
	string Input = "\0", dontCare = "\0"; bool dC;
	if (mints) cout << "Enter minterms f(";
	else cout << "Enter maxterms f(";
	cout << PrintfCharFuntion() << "): "; cin >> Input;
	CreateMintermAndDontCare(Input, minterms, implicants, false);
	cout << "Have don't care ? (Yes(1) or No(0)): "; cin >> dC;
	if (dC) {
		cout << "Enter don't care: "; cin >> dontCare;
		CreateMintermAndDontCare(dontCare, minterms, implicants, true);
	}
}
void ChangeToBinary(vector<int> implicants, vector<vector<int>>& binary)
{
	for (size_t i = 0; i != implicants.size(); i++) {
		int vMints = implicants[i]; vector<int> row;
		for (int j = 0; j != vars; j++) {
			if (vMints == 0) row.push_back(0);
			else {
				row.push_back(vMints % 2); vMints >>= 1;
			}
		}
		binary.push_back(row);
	}
}
void CreateOnePI(int key, vector<vector<int>> binary, vector<vector<int>>& PI)
{
	vector<int> row = binary[key];
	PI.push_back(row);
}
void CompareMinterm(vector<vector<int>>& binary, vector<vector<int>>& PI)
{
	if (binary.size() < 4) {
		for (size_t i = 0; i < binary.size(); i++) {
			CreateOnePI(i, binary, PI);
		}
		binary.clear();
	}
	else {
		vector<bool> checkMintermAlone;
		for (size_t i = 0; i < binary.size(); i++) {
			checkMintermAlone.push_back(0);
		}
		vector<vector<int>> binaryGroup;
		for (size_t i = 0; i < binary.size() - 1; i++) {
			for (size_t j = i + 1; j < binary.size(); j++) {
				bool countSame = false; int key = 0;
				for (int l = 0; l < vars; l++) {
					if (binary[i][l] != binary[j][l]) {
						if (countSame) { countSame = false; break; }
						else { countSame = true; key = l; }
					}
				}
				if (!countSame) continue;
				else {
					vector<int> row = binary[i]; row[key] = 2;
					if (binaryGroup.empty()) binaryGroup.push_back(row);
					else {
						bool checkAlreadyExist = false;
						for (size_t k = 0; k < binaryGroup.size(); k++) {
							if (row == binaryGroup[k]) { checkAlreadyExist = true; break; }
						}
						if (!checkAlreadyExist) binaryGroup.push_back(row);
					}
					checkMintermAlone[i] = checkMintermAlone[j] = 1;
				}
			}
		}
		for (size_t i = 0; i < binary.size(); i++) {
			if (checkMintermAlone[i] == 0) CreateOnePI(i, binary, PI);
		}
		binary.clear(); binary = binaryGroup;
	}
}
void CreatePrimeImplicants(vector<vector<int>>& binary, vector<vector<int>>& PI)
{
	do {
		CompareMinterm(binary, PI);
	} while (!binary.empty());
}
void CreateMatrixMintermAndPI(vector<int> minterms, vector<vector<int>> PI,
	vector<vector<bool>>& matrixMintsPI)
{
	for (size_t i = 0; i < minterms.size(); i++) {
		vector<bool> row;
		for (size_t j = 0; j < PI.size(); j++) {
			row.push_back(0);
		}
		matrixMintsPI.push_back(row);
	}
	vector<vector<int>> binaryMinterms;
	ChangeToBinary(minterms, binaryMinterms);
	for (size_t i = 0; i < minterms.size(); i++) {
		for (size_t j = 0; j < PI.size(); j++) {
			bool checkSum = true;
			for (int k = 0; k < vars; k++) {
				if (PI[j][k] != 2) {
					if (PI[j][k] != binaryMinterms[i][k]) { checkSum = false; break; }
				}
			}
			if (checkSum) matrixMintsPI[i][j] = 1;
		}
	}
}
void CheckEPIAndNonEPI(vector<vector<int>> PI, vector<vector<int>>& ePI,
	vector<vector<int>>& nonEPI, vector<vector<bool>> matrixMintsPI)
{
	vector<bool> isEssentialPI;
	for (size_t i = 0; i < PI.size(); i++) {
		isEssentialPI.push_back(0);
	}
	for (size_t i = 0; i < matrixMintsPI.size(); i++) {
		bool checkSum = false; int key = 0;
		for (size_t j = 0; j < PI.size(); j++) {
			if (matrixMintsPI[i][j] == true) {
				if (checkSum) { checkSum = false; break; }
				else { checkSum = true; key = j; }
			}
		}
		if (checkSum) isEssentialPI[key] = 1;
	}
	for (size_t i = 0; i < PI.size(); i++) {
		if (isEssentialPI[i] == 1) CreateOnePI(i, PI, ePI);
		else CreateOnePI(i, PI, nonEPI);
	}
}
void RemoveSelectedMinterm(vector<int>& minterms, vector<vector<int>> ePI)
{
	vector<bool> removeMinterms;
	for (size_t i = 0; i < minterms.size(); i++) {
		removeMinterms.push_back(0);
	}
	vector<vector<int>> binaryMinterms;
	ChangeToBinary(minterms, binaryMinterms);
	for (size_t i = 0; i < ePI.size(); i++) {
		for (size_t j = 0; j < minterms.size(); j++) {
			bool checkSum = true;
			for (int k = 0; k < vars; k++) {
				if (ePI[i][k] != 2) {
					if (ePI[i][k] != binaryMinterms[j][k]) { checkSum = false; break; }
				}
			}
			if (checkSum) removeMinterms[j] = 1;
		}
	}
	vector<int> reductMinterms;
	for (size_t i = 0; i < removeMinterms.size(); i++) {
		if (removeMinterms[i] == 0) reductMinterms.push_back(minterms[i]);
	}
	minterms.clear(); minterms = reductMinterms;
}
void ReductMatrixMintermAndPI(vector<int>& minterms, vector<vector<int>> ePI,
	vector<vector<int>> nonEPI, vector<vector<bool>>& matrixMintsPI)
{
	RemoveSelectedMinterm(minterms, ePI);
	matrixMintsPI.clear();
	if (!minterms.empty()) {
		CreateMatrixMintermAndPI(minterms, nonEPI, matrixMintsPI);
	}
}
void CountNonEssentialPIHaveMinterm(vector<vector<bool>>& matrixMintsPI,
	vector<int>& countNEPHaveMints, vector<vector<int>>& nonEPI)
{
	countNEPHaveMints.clear();
	for (size_t i = 0; i < matrixMintsPI.size(); i++) {
		countNEPHaveMints.push_back(0);
	}
	for (size_t i = 0; i < matrixMintsPI.size(); i++) {
		int virtualCheckSum = 0;
		for (size_t j = 0; j < matrixMintsPI[i].size(); j++) {
			if (matrixMintsPI[i][j] == 1) virtualCheckSum += 1;
		}
		countNEPHaveMints[i] = virtualCheckSum;
	}
	for (size_t i = 0; i < matrixMintsPI.size(); i++) {
		if (countNEPHaveMints[i] == 0) {
			countNEPHaveMints.erase(countNEPHaveMints.begin() + i);
			matrixMintsPI.erase(matrixMintsPI.begin() + i);
			nonEPI.erase(nonEPI.begin() + i);
			i -= 1;
		}
	}
}
void InverseMatrix(vector<vector<bool>> matrixA, vector<vector<bool>>& matrixB)
{
	vector<vector<bool>> bufferMatrix;
	for (size_t j = 0; j < matrixA[0].size(); j++) {
		vector<bool> row;
		for (size_t i = 0; i < matrixA.size(); i++) {
			row.push_back(matrixA[i][j]);
		}
		bufferMatrix.push_back(row);
	}
	matrixB.clear(); matrixB = bufferMatrix;
}
bool SortMass(const vector<bool>& v1, const vector<bool>& v2)
{
	return v1 > v2;
}
void SortMatrix(vector<vector<bool>>& matrixMintsPI)
{
	sort(matrixMintsPI.begin(), matrixMintsPI.end());
	vector<vector<bool>>::iterator re;
	re = unique(matrixMintsPI.begin(), matrixMintsPI.end());
	matrixMintsPI.resize(distance(matrixMintsPI.begin(), re));
	sort(matrixMintsPI.begin(), matrixMintsPI.end(), SortMass);
}
void CreateOneColumn(int key, vector<int> countNEPHaveMints,
	vector<vector<bool>> matrixMintsPI, vector<vector<int>>& minNEP)
{
	int vRememberNonEPI = 0;
	for (int i = 0; i < countNEPHaveMints[key]; i++) {
		for (size_t j = vRememberNonEPI; j < matrixMintsPI[key].size(); j++) {
			if (matrixMintsPI[key][j] == 1) { vRememberNonEPI = j; break; }
		}
		vector<int> row; row.push_back(vRememberNonEPI);
		minNEP.push_back(row); vRememberNonEPI += 1;
	}
}
bool SortLength(const vector<int>& v1, const vector<int>& v2)
{
	return v1.size() < v2.size();
}
bool CheckBooleAbsorption(int key1, vector<vector<int>> min1,
	int key2, vector<vector<int>> min2)
{
	bool checkSum = true;
	for (size_t i = 0; i < min1[key1].size(); i++) {
		if (!binary_search(begin(min2[key2]), end(min2[key2]), min1[key1][i])) {
			checkSum = false; break;
		}
	}
	return checkSum;
}
void GroupColumns(vector<int> countNEPHaveMints, vector<vector<bool>> matrixMintsPI,
	int key, vector<vector<int>>& minNEP, vector<vector<int>>& minNEP2)
{
	if (minNEP.empty()) CreateOneColumn(key, countNEPHaveMints, matrixMintsPI, minNEP);
	if (minNEP2.empty()) CreateOneColumn(key + 1, countNEPHaveMints, matrixMintsPI, minNEP2);
	vector<vector<int>> bufferMin;
	for (size_t i = 0; i < minNEP.size(); i++) {
		for (size_t j = 0; j < minNEP2.size(); j++) {
			bufferMin.push_back(minNEP[i]);
			size_t count = bufferMin.size() - 1;
			for (size_t k = 0; k < minNEP2[j].size(); k++) {
				bufferMin[count].push_back(minNEP2[j][k]);
			}
			sort(bufferMin[count].begin(), bufferMin[count].end());
			vector<int>::iterator re = unique(bufferMin[count].begin(), bufferMin[count].end());
			bufferMin[count].resize(distance(bufferMin[count].begin(), re));
		}
	}
	minNEP.clear(); minNEP = bufferMin; bufferMin.clear();
	sort(minNEP.begin(), minNEP.end());
	vector<vector<int>>::iterator re = unique(minNEP.begin(), minNEP.end());
	minNEP.resize(distance(minNEP.begin(), re));
	sort(minNEP.begin(), minNEP.end(), SortLength);
	for (size_t i = 0; i < minNEP.size() - 1; i++) {
		for (size_t j = i + 1; j < minNEP.size(); j++) {
			if (minNEP[i].size() < minNEP[j].size()) {
				bool checkBooleAbsorption = false;
				checkBooleAbsorption = CheckBooleAbsorption(i, minNEP, j, minNEP);
				if (checkBooleAbsorption) {
					minNEP.erase(minNEP.begin() + j); j -= 1;
				}
			}
		}
	}
}
vector<vector<int>> FindOptimalNonEssentialPI(int first, int last,
	vector<int> countNEPHaveMints, vector<vector<bool>> matrixMintsPI)
{
	if (last == first) {
		vector<vector<int>> minNEP1;
		CreateOneColumn(first, countNEPHaveMints, matrixMintsPI, minNEP1);
		return minNEP1;
	}
	else if (last == first + 1) {
		vector<vector<int>> minNEP1, minNEP2;
		GroupColumns(countNEPHaveMints, matrixMintsPI, first, minNEP1, minNEP2);
		return minNEP1;
	}
	else {
		int key = (first + last) / 2;
		vector<vector<int>> minNEP1, minNEP2;
		minNEP1 = FindOptimalNonEssentialPI(first, key, countNEPHaveMints, matrixMintsPI);
		minNEP2 = FindOptimalNonEssentialPI(key + 1, last, countNEPHaveMints, matrixMintsPI);
		GroupColumns(countNEPHaveMints, matrixMintsPI, first, minNEP1, minNEP2);
		return minNEP1;
	}
}
int FindMinLengthOfOptimalNonEssentialPI(vector<vector<int>> minNEP)
{
	size_t min = minNEP[0].size();
	for (size_t i = 1; i < minNEP.size(); i++) {
		if (minNEP[i].size() < min) min = minNEP[i].size();
	}
	return min;
}
void CreateOptimalNonEssentialPIFinal(vector<vector<int>>& minNEP)
{
	vector<vector<int>> bufferMin;
	int minLength = FindMinLengthOfOptimalNonEssentialPI(minNEP);
	for (size_t i = 0; i < minNEP.size(); i++) {
		if (minNEP[i].size() == minLength) {
			vector<int> row = minNEP[i];
			bufferMin.push_back(row);
		}
	}
	minNEP.clear(); minNEP = bufferMin;
}
string TextMinterm(int key, vector<vector<int>> ePI)
{
	string Text = "\0";
	for (int i = vars; i > 0; i--) {
		if (ePI[key][i - 1] == 1) Text += char((vars - i) + 'a');
		else if (ePI[key][i - 1] == 0) Text = Text + char((vars - i) + 'a') + '\'';
	}
	return Text;
}
string ShowResultEPIMints(vector<vector<int>> ePI)
{
	string Text = "f(" + PrintfCharFuntion() + ") = ";
	for (size_t i = 0; i < ePI.size(); i++) {
		Text += TextMinterm(i, ePI);
		if (i < ePI.size() - 1) Text += " + ";
	}
	return Text;
}
void ShowFinalResultMints(vector<vector<int>> ePI, vector<vector<int>> nonEPI,
	vector<vector<int>> minNEP)
{
	if (minNEP.empty()) cout << "Result 1: " << ShowResultEPIMints(ePI) << endl;
	else {
		for (size_t i = 0; i < minNEP.size(); i++) {
			cout << "Result " << i + 1 << ": " << ShowResultEPIMints(ePI);
			string Text = "\0";
			if (ePI.size() > 0) Text += " + ";
			for (size_t j = 0; j < minNEP[i].size(); j++) {
				Text += TextMinterm(minNEP[i][j], nonEPI);
				if (j < minNEP[i].size() - 1) Text += " + ";
			}
			cout << Text;
			if (i < minNEP.size() - 1) cout << endl;
			cout << endl;
		}
	}
	superLine;
}
void CheckResultMints(string Text, vector<vector<int>> ePI,
	vector<vector<int>> nonEPI, vector<vector<int>> minNEP)
{
	bool isAlreadyExist = false;
	for (size_t i = 0; i < minNEP.size(); i++) {
		string BufferCopy = "\0"; int checkNumberPI = 0;
		for (size_t j = 0; j < Text.length(); j++) {
			bool stop = false;
			if (Text[j] != ' ') BufferCopy += Text[j];
			if (j == Text.length() - 1 || Text[j] == ' ') stop = true;
			if (stop) {
				bool isCheckEPI = false;
				for (size_t k = 0; k < ePI.size(); k++) {
					if (BufferCopy == TextMinterm(k, ePI)) {
						checkNumberPI += 1; isCheckEPI = true; break;
					}
				}
				if (!isCheckEPI) {
					for (size_t k = 0; k < minNEP[i].size(); k++) {
						if (BufferCopy == TextMinterm(minNEP[i][k], nonEPI)) {
							checkNumberPI += 1; break;
						}
					}
				}
				if (j != Text.length() - 1) j += 2;
				BufferCopy = "\0";
			}
			if (j == Text.length() - 1) {
				if (checkNumberPI == ePI.size() + minNEP[i].size()) {
					cout << endl << "True" << endl;
					cout << "Result " << i + 1 << ": " << ShowResultEPIMints(ePI);
					string Text = "\0";
					if (ePI.size() > 0) Text += " + ";
					for (size_t j = 0; j < minNEP[i].size(); j++) {
						Text += TextMinterm(minNEP[i][j], nonEPI);
						if (j < minNEP[i].size() - 1) Text += " + ";
					}
					cout << Text; isAlreadyExist = true;
				}
			}
		}
	}
	if (!isAlreadyExist) cout << endl << "Result Not Found !!!";
}
string TextMaxterm(int key, vector<vector<int>> ePI)
{
	string Text = "(";
	for (int i = vars; i > 0; i--) {
		if (ePI[key][i - 1] == 0) Text = Text + char((vars - i) + 'a') + " + ";
		else if (ePI[key][i - 1] == 1) Text = Text + char((vars - i) + 'a') + "' + ";
	}
	Text.erase(Text.length() - 3);
	Text += ')';
	return Text;
}
string ShowResultEPIMaxterm(vector<vector<int>> ePI)
{
	string Text = "f(" + PrintfCharFuntion() + ") = ";
	for (size_t i = 0; i < ePI.size(); i++) {
		Text += TextMaxterm(i, ePI);
	}
	return Text;
}
void ShowFinalResultMaxterm(vector<vector<int>> ePI,
	vector<vector<int>> nonEPI, vector<vector<int>> minNEP)
{
	if (minNEP.empty()) cout << "Result 1: " << ShowResultEPIMaxterm(ePI) << endl;
	else {
		for (size_t i = 0; i < minNEP.size(); i++) {
			cout << "Result " << i + 1 << ": " << ShowResultEPIMaxterm(ePI);
			string Text = "\0";
			for (size_t j = 0; j < minNEP[i].size(); j++) {
				Text += TextMaxterm(minNEP[i][j], nonEPI);
			}
			cout << Text;
			if (i < minNEP.size() - 1) cout << endl;
			cout << endl;
		}
	}
	superLine;
}
void CheckResultMaxTerm(string Text, vector<vector<int>> ePI,
	vector<vector<int>> nonEPI, vector<vector<int>> minNEP)
{
	bool isAlreadyExist = false;
	for (size_t i = 0; i < minNEP.size(); i++) {
		string BufferCopy = "\0";
		int checkNumberPI = 0;
		for (size_t j = 0; j < Text.length(); j++) {
			bool stop = false;
			BufferCopy = BufferCopy + Text[j];
			if (Text[j] == ')') stop = true;
			if (stop) {
				bool isCheckSPI = false;
				for (size_t k = 0; k < ePI.size(); k++) {
					if (BufferCopy == TextMaxterm(k, ePI)) {
						checkNumberPI += 1; isCheckSPI = true; break;
					}
				}
				if (!isCheckSPI) {
					for (size_t k = 0; k < minNEP[i].size(); k++) {
						if (BufferCopy == TextMaxterm(minNEP[i][k], nonEPI)) {
							checkNumberPI += 1; break;
						}
					}
				}
				BufferCopy = "\0";
			}
			if (j == Text.length() - 1) {
				if (checkNumberPI == ePI.size() + minNEP[i].size()) {
					cout << endl << "True" << endl;
					cout << "Result " << i + 1 << ": " << ShowResultEPIMaxterm(ePI);
					string Text = "\0";
					for (size_t j = 0; j < minNEP[i].size(); j++) {
						Text += TextMaxterm(minNEP[i][j], nonEPI);
					}
					cout << Text; isAlreadyExist = true;
				}
			}
		}
	}
	if (!isAlreadyExist) cout << endl << "Result Not Found !!!";
}
void Menu()
{
	bool programExit = false;
	do {
		bool chooseSOPorPOS;
		cout << "Choose SOP(0) or POS(1): "; cin >> chooseSOPorPOS;
		system("cls");
		vector<int> minterms, implicants;
		if (!chooseSOPorPOS) {
			cout << "Sum of Products (SOP)" << endl;
			Input(minterms, implicants, true);
		}
		else {
			cout << "Product of Sums (POS)" << endl;
			Input(minterms, implicants, false);
		}
		superLine; cout << endl;
		if (minterms.empty() || minterms.size() == pow(2, vars)) {
			cout << "f(" << PrintfCharFuntion() << ") = ";
			if (minterms.size() == pow(2, vars)) cout << "1";
			else cout << "0";
			cout << endl;
			programExit = true;
		}
		else {
			vector<vector<int>> binary, PI, essentialPI, nonEssentialPI;
			vector<vector<bool>> matrixMintermAndPI;
			ChangeToBinary(implicants, binary);
			CreatePrimeImplicants(binary, PI);
			CreateMatrixMintermAndPI(minterms, PI, matrixMintermAndPI);
			CheckEPIAndNonEPI(PI, essentialPI, nonEssentialPI, matrixMintermAndPI);
			vector<int> countNonEssentialPIHaveMinterm;
			vector<vector<int>> minNonEssentialPI;
			if (!nonEssentialPI.empty()) {
				ReductMatrixMintermAndPI(minterms, essentialPI, nonEssentialPI,
					matrixMintermAndPI);
				if (!matrixMintermAndPI.empty()) {
					for (int i = 0; i < 2; i++) {
						InverseMatrix(matrixMintermAndPI, matrixMintermAndPI);
						if (i == 1) SortMatrix(matrixMintermAndPI);
						CountNonEssentialPIHaveMinterm(matrixMintermAndPI,
							countNonEssentialPIHaveMinterm, nonEssentialPI);
					}
					int key = countNonEssentialPIHaveMinterm.size() - 1;
					minNonEssentialPI = FindOptimalNonEssentialPI(0, key,
						countNonEssentialPIHaveMinterm, matrixMintermAndPI);
					CreateOptimalNonEssentialPIFinal(minNonEssentialPI);
				}
			}
			if (!chooseSOPorPOS)
				ShowFinalResultMints(essentialPI, nonEssentialPI, minNonEssentialPI);
			else ShowFinalResultMaxterm(essentialPI, nonEssentialPI, minNonEssentialPI);
			if (minNonEssentialPI.size() != 0) {
				string text;
				cin.ignore();
				cout << "Enter Result Check:" << endl;
				cout << "f(" << PrintfCharFuntion() << ") = ";
				getline(cin, text);
				if (!chooseSOPorPOS)
					CheckResultMints(text, essentialPI, nonEssentialPI, minNonEssentialPI);
				else CheckResultMaxTerm(text, essentialPI, nonEssentialPI, minNonEssentialPI);
				superLine;
				cout << endl << endl;
			}
			programExit = true;
		}
	} while (!programExit);
}
void CreateProjectQuineMcCluskey()
{
	superLine;
	cout << "#PROJECTTEAM#PROJECTVIETANH" << endl;
	cout << "Version 1.13.12 @CopyRightByVietAnh" << endl;
	cout << "PROGRAM WRITE BY @BUI_VIET_ANH";
	superLine;
	cout << endl;
}
int main()
{
	Menu();
	CreateProjectQuineMcCluskey();
	system("pause");
	return 0;
}

/*
# Case test SOP:
Enter the number of variables: 4
Max minterms = 16
Enter minterms f(a,b,c,d): 1-8,13
Have don't care ? (Yes(1) or No(0)): 1
Enter don't care: 9,12,15

# Copy here
4
1-8,13
1
9,12,15


# Case test POS:
Enter the number of variables: 5
Max minterms = 32
Enter maxterms f(a,b,c,d,e): 0,1,2,4-12,14,16,19,20,21,23,25,26,30
Have don't care ? (Yes(1) or No(0)): 1
Enter don't care: 15,24,27,31

# Copy here
5
0,1,2,4-12,14,16,19,20,21,23,25,26,30
1
15,24,27,31
*/
