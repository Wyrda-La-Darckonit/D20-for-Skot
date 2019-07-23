#pragma once
#include <iostream>
#include <cstdlib>
#include <string>
#include <thread>
#include <mutex>
#include <Windows.h>
#include <fstream>

#include <queue>
#include <stack>
#include <vector>
#include <map>

#include <io.h>
#include <direct.h>

#include "MyCalc.h"

constexpr auto QUIT = -1;
constexpr auto PVT = 0;
constexpr auto GRP = 1;
constexpr auto DIS = 2;
constexpr auto ING = 3;

#define ISNUM(n) (n >= '0' && n <= '9' || n == '.')

using namespace std;



///	��ȫ�־�̬������

extern int ac;
static string _home_ = "data\\app\\skot\\";
static string usrDir = _home_ + "usr\\";
static string tmpDir = _home_ + "tmp\\";
static string sudoerFile = _home_ + "sudoers.csv";
static string manFile = _home_ + "manual.txt";
static string blackFile = _home_ + "blackList.csv";

static int64_t superadmin = 95806902;

//	����xxxxxx<str>xxxxxx��[recommand]
//class xx_exception : public exception {
//public: xx_exception(string str, string recommand = "") :
//	exception::exception(("����xxxxxx" + str + "xxxxxx��" + recommand).data()) {};
//};



///	��������Ϣ��

//	������Ϣ
void PostMsg(int type, int64_t target = 0, string msg = "")
{
	switch (type)
	{
	case PVT: CQ_sendPrivateMsg(ac, target, msg.data()); break;
	case GRP: CQ_sendGroupMsg(ac, target, msg.data()); break;
	case DIS: CQ_sendDiscussMsg(ac, target, msg.data()); break;
	}
	return;
}



///	����ʼ����

//	������Ҫ���ļ���
void Init()
{
	if (_access(_home_.data(), 00) == -1) _mkdir(_home_.data());
	if (_access(usrDir.data(), 00) == -1) _mkdir(usrDir.data());
	if (_access(tmpDir.data(), 00) == -1) _mkdir(tmpDir.data());
}



///	��ָ�����ļ���

//	����ָ���������ת�壩
vector<string> to_args(string cmd)
{
	vector<string> args;
	if (cmd.find("\r\n") != string::npos) cmd.erase(cmd.find("\r\n"));

	bool escape = false;
	bool quote = false;

	if (!cmd.empty()) args.push_back("");
	for (string::iterator D2 = cmd.begin(); D2 != cmd.end(); D2++)
	{
		if (escape)
		{
			if (*D2 == '\\' || *D2 == '-') args.back() += '\\', args.back() += *D2;
			else if (*D2 == 'n') args.back() += '\n';
			else args.back() += *D2;
			escape = false;
			continue;
		}
		switch (*D2)
		{
		case '\\':
			escape = true;
			continue;
		case '"':
			quote ^= true;
			continue;
		case '-':
			if (quote) args.back() += '\\';
			args.back() += *D2;
			continue;
		case ' ':
			if (quote) args.back() += *D2;
			else
			{
				for (; D2 + 1 != cmd.end() && *(D2 + 1) == ' '; D2++);
				if (D2 + 1 == cmd.end())
				{
					cmd.erase(D2);
					D2--;
					break;
				}
				args.push_back("");
			}
			continue;
		default:
			args.back() += *D2;
		}
	}
	return args;
}
//	����ָ��ת��
void anal(string& arg)
{
	bool escape = false;
	for (string::iterator D2 = arg.begin(); D2 != arg.end(); D2++)
	{
		if (*D2 == '\\')
		{
			if (escape) escape = false;
			else
			{
				escape = true;
				arg.erase(D2);
			}
		}
		else continue;
	}
	return;
}
//	����ֵд��csvǰ����ת��
string to_csv(string str)
{
	string Ly = "";

	bool Sp = false;

	for (char D2 : str) switch (D2)
	{
	case '"': Ly += "\"";
	case ',': Sp = true;
	default: Ly += D2;
	}

	if (Sp) Ly = "\"" + Ly + "\"";

	return Ly;
}
//	����csv�ļ��У���������csvת�壩
vector<string> csv(string line)
{
	if (line.empty()) return vector<string>::vector();

	bool quote = false;
	bool escape = false;

	//	�ָ�
	vector<string> cy;
	cy.push_back("");
	quote = false;
	escape = false;

	for (string::iterator D2 = line.begin(); D2 != line.end(); D2++) switch (*D2)
	{
	case ',':
		if (quote) cy.back() += *D2;
		else cy.push_back("");
		continue;
	case '"':
		if (escape) escape = false, cy.back() += '\"';
		else if (!quote && (D2 == line.begin() || *(D2 - 1) == ',')) quote = true;
		else if (quote && (D2 + 1 == line.end() || *(D2 + 1) == ',')) quote = false;
		else escape = true;
		continue;
	default:
		cy.back() += *D2;
	}
	return cy;
}



///	��cast��

//	���ַ���תΪ����
int to_int(string str)
{
	int Ly = 0;
	bool dT = false;

	bool neg = false;
	if (*str.begin() == '-')
	{
		str.erase(0, 1);
		neg = true;
	}

	for (char D2 : str)
	{
		if (dT) continue;
		else if (D2 == '.') dT = true;
		else if (ISNUM(D2)) Ly *= 10, Ly += D2 - '0';
		else return 0;
	}

	if (neg) Ly = -Ly;
	return Ly;
}
//	���ַ���תΪС��
double to_double(string str)
{
	double Ly = 0;
	int Fred = -1;

	bool neg = false;
	if (*str.begin() == '-')
	{
		str.erase(0, 1);
		neg = true;
	}

	for (char D2 : str)
	{
		if (Fred != -1) Fred++;
		if (D2 == '.') Fred++;
		else if (ISNUM(D2)) Ly *= 10, Ly += D2 - '0';
		else return 0.0;
	}
	if (Fred != -1) for (; Fred != 0; Fred--) Ly /= 10;

	if (neg) Ly = -Ly;
	return Ly;
}
//	���ַ���תΪqq������CQ���@��
int64_t to_QQNumber(string str)
{
	if (str.find("[CQ:at,qq=") == 0 && *(str.end() - 1) == ']') str.erase(0, 10), str.erase(str.end() - 1);
	int64_t Ly = 0;
	bool dT = false;

	bool neg = false;
	if (*str.begin() == '-')
	{
		str.erase(0, 1);
		neg = true;
	}

	for (char D2 : str)
	{
		if (dT) continue;
		else if (D2 == '.') dT = true;
		else if (ISNUM(D2)) Ly *= 10, Ly += D2 - '0';
		else return 0;
	}

	if (neg) Ly = -Ly;
	return Ly;
}
//	@qq
string atQQ(int64_t QQ) { return "[CQ:at,qq=" + to_string(QQ) + "] "; }



///	��is?��

//	����Ƿ��ǺϷ���qq�ţ�����@��
bool is_QQNumber(string str)
{
	if (str.find("[CQ:at,qq=") == 0 && *(str.end() - 1) == ']') str.erase(0, 10), str.erase(str.end() - 1);
	int Fred = 0;
	for (char D2 : str)
	{
		if (D2 >= '0' && D2 <= '9') Fred++;
		else return false;
	}
	if (Fred > 10 || Fred < 5) return false;
	return true;
}
//	�жϱ��ʽ�Ƿ��ܹ���������
bool is_LegalDiceExp(string exp)
{
	try { MyCalc::MyCalc(exp); }
	catch (exception Ed) { return false; }
	return true;
}
//	����ַ����Ƿ�������
bool is_num(string str)
{
	bool Ly = true;
	for (char D2 : str) if ((D2 < '0' || D2 > '9') && D2 != '.' && D2 != '-') Ly = false;
	return Ly;
}



///	��ʹ���ֲ᡿

//	�����Ҳ���ʹ���ֲ��ļ���[recommand]
class manFile_not_found : public exception {
public: manFile_not_found(string recommand = "") :
	exception::exception(("�����Ҳ���ʹ���ֲ��ļ���" + recommand).data()) {};
};

//	ʹ���ֲ������
int manFileParagraphs(bool sudo = false)
{
	int Ly = 0;

	string CH = manFile;
	string P2 = "";

	ifstream in(CH);
	if (in.is_open())
	{
		bool dT = false;
		for (; !in.eof();)
		{
			getline(in, P2);
			if (P2._Equal(".sudo")) dT = true;
			if (P2._Equal(".flush"))
			{
				if (sudo || !dT) Ly++;
				dT = false;
			}
		}
		in.close();
		if ((sudo || !dT) && !P2._Equal(".flush")) Ly++;
	}

	return Ly;
}
//	����ʹ���ֲ��߳���ڵ�
void sendManual_main(bool sudo, int type, int QQ, int Group)
{
	string P2 = "";
	string CCC = "";
	string CH = manFile;

	ifstream in(CH);
	if (in.is_open())
	{
		bool dT = false;
		for (; !in.eof();)
		{
			getline(in, P2);
			if (P2._Equal(".sudo"))
			{
				dT = true;
				continue;
			}
			if (P2._Equal(".flush"))
			{
				CCC.erase(0, 1);
				if (!dT || sudo) Sleep(1000), PostMsg(PVT, QQ, CCC);
				CCC.clear();
				dT = false;
			}
			else if (dT && !sudo)
			{
				for (; !in.eof();)
				{
					getline(in, P2);
					if (P2._Equal(".flush")) break;
				}
				dT = false;
			}
			else CCC.append("\n" + P2);
		}
		in.close();
		if (!CCC.empty())
		{
			CCC.erase(0, 1);
			if (!dT || sudo) Sleep(1000), PostMsg(PVT, QQ, CCC);
			CCC.clear();
		}
		if (type == PVT) PostMsg(PVT, QQ, "ʹ���ֲᷢ����ϡ�");
		else PostMsg(type, Group, atQQ(QQ) + "ʹ���ֲᷢ����ϡ�");
	}
	else PostMsg(type, (type == PVT ? QQ : Group), manFile_not_found().what());
	return;
}



///	���ࡿ

//	����
class RandomAsk
{
	int64_t creatoR;
	bool readonlY;
	bool listablE;
	string questioN;
	vector<string> answeR;

public:
	RandomAsk()
	{
		creatoR = 0;
		readonlY = true;
		listablE = false;
	}
	RandomAsk(vector<string> cy)
	{
		creatoR = to_int(cy[1]);
		readonlY = cy[2]._Equal("readonly");
		listablE = cy[3]._Equal("listable");
		questioN = cy[4];
		for (int D2 = 5; D2 != cy.size(); D2++) answeR.push_back(cy[D2]);
	}
	string toString()
	{
		string Ly = "ask," + to_string(creatoR) + "," + (readonlY ? "readonly" : "editable") + "," + (listablE ? "listable" : "packaged") + "," + to_csv(questioN);
		for (auto D2 : answeR) Ly += "," + to_csv(D2);
		return Ly;
	}

	//	is?
	bool is_creator(int64_t qq) { return creatoR == qq; }
	bool readonly() { return readonlY; }
	bool listable() { return listablE; }
	bool empty() { return answeR.empty(); }

	//	set
	void setproperties(vector<string> cy)
	{
		for (auto D2 : cy)
		{
			if (D2._Equal("readonly")) readonlY = true;
			if (D2._Equal("editable")) readonlY = false;
			if (D2._Equal("listable")) listablE = true;
			if (D2._Equal("packaged")) listablE = false;
		}
	}
	void copyfrom(RandomAsk target, char mode)
	{
		vector<string> ans = target.allanswer();
		switch (mode)
		{
		case 'c': answeR = ans; break;
		case 'f': for (auto D2 : ans) if (find(D2) == answeR.end()) answeR.push_back(D2); break;
		case 'm': for (auto D2 : ans) answeR.push_back(D2); break;
		}
	}
	vector<bool> erase(vector<string> ans)
	{
		vector<bool> Ly;
		for (auto D2 : ans) if (find(D2) != answeR.end())
		{
			answeR.erase(find(D2));
			Ly.push_back(true);
		}
		else Ly.push_back(false);
		return Ly;
	}
	vector<bool> append(vector<string> ans, bool force = false)
	{
		vector<bool> Ly;
		for (auto D2 : ans) if (force || find(D2) == answeR.end())
		{
			answeR.push_back(D2);
			Ly.push_back(true);
		}
		else Ly.push_back(false);
		return Ly;
	}

	//	get
	string question() { return questioN; }
	int64_t creator() { return creatoR; }
	int size() { return answeR.size(); }
	string ask()
	{
		if (empty()) return "";
		return answeR[rand() % answeR.size()];
	}
	vector<string> allanswer() { return answeR; }
	vector<string>::iterator find(string ans)
	{
		for (vector<string>::iterator D2 = answeR.begin(); D2 != answeR.end(); D2++) if (*D2 == ans) return D2;
		return answeR.end();
	}
};
//	�������
class RandomAskManager
{
	map<string, RandomAsk> asK;

public:
	bool Load(vector<string> cy)
	{
		if (!cy[0]._Equal("ask")) return false;
		asK.insert(make_pair(cy[4], RandomAsk(cy)));
		return true;
	}
	string toString()
	{
		string Ly;
		for (auto D2 : asK)  Ly += "\n" + D2.second.toString();
		if (!Ly.empty()) Ly.erase(0, 1);
		return Ly;
	}

	//	set
	/*	0	success
		1	question_not_found
		2	question_readonly
		*/
	int delete_question(string question, int64_t oprtor)
	{
		if (!exist_question(question)) return 1;
		if (asK.find(question)->second.readonly() && (oprtor != 0 && !asK.find(question)->second.is_creator(oprtor))) return 2;
		asK.erase(asK.find(question));
		return 0;
	}
	/*	0	success
		1	question_already_exist
		*/
	int create_question(vector<string> cy)
	{
		if (exist_question(cy[4])) return 1;
		asK.insert(make_pair(cy[4], RandomAsk(cy)));
		return 0;
	}
	/*	0	success
		1	question_not_found
		2	not_creator_or_DM
		3	properties_conflict
		*/
	int set_question_properties(string question, vector<string> cy, int64_t oprtor)
	{
		if (!exist_question(question)) return 1;
		if (oprtor != 0 && !asK.find(question)->second.is_creator(oprtor)) return 2;
		bool Sp[4] = { false };
		for (auto D2 : cy)
		{
			if (D2._Equal("readonly")) Sp[0] = true;
			if (D2._Equal("editable")) Sp[1] = true;
			if (D2._Equal("listable")) Sp[2] = true;
			if (D2._Equal("packaged")) Sp[3] = true;
		}
		if ((Sp[0] && Sp[1]) || (Sp[2] && Sp[3])) return 3;
		asK.find(question)->second.setproperties(cy);
		return 0;
	}
	/*	0	success
		1	to_question_not_found
		2	from_question_not_found
		3	to_question_readonly
		4	from_question_packaged
		*/
	int copyfrom_question(string to, string from, char mode, int64_t oprtor)
	{
		if (!exist_question(to)) return 1;
		if (!exist_question(from)) return 1;
		if (asK.find(to)->second.readonly() && (oprtor != 0 && !asK.find(to)->second.is_creator(oprtor))) return 3;
		if (!asK.find(from)->second.listable() && (oprtor != 0 && !asK.find(from)->second.is_creator(oprtor))) return 4;
		asK.find(to)->second.copyfrom(asK.find(from)->second, mode);
		return 0;
	}
	/*	0	success
		1	question_not_found
		2	question_readonly
		*/
	int erase_answer(vector<bool>& result, string question, vector<string> ans, int64_t oprtor)
	{
		if (!exist_question(question)) return 1;
		if (asK.find(question)->second.readonly() && (oprtor != 0 && !asK.find(question)->second.is_creator(oprtor))) return 2;
		result = asK.find(question)->second.erase(ans);
		return 0;
	}
	/*	0	success
		1	question_not_found
		2	question_readonly
		*/
	int append_answer(vector<bool>& result, string question, vector<string> ans, bool force, int64_t oprtor)
	{
		if (!exist_question(question)) return 1;
		if (asK.find(question)->second.readonly() && (oprtor != 0 && !asK.find(question)->second.is_creator(oprtor))) return 2;
		result = asK.find(question)->second.append(ans, force);
		return 0;
	}

	//	get
	int size() { return asK.size(); }
	map<string, RandomAsk> allquestion() { return asK; }
	RandomAsk getQuestionById(string question) { return asK.find(question)->second; }

	//	is?
	bool exist_question(string question) { return asK.find(question) != asK.end(); }
};

//	�û�
class User : public RandomAskManager
{
	int64_t qq;
	map<int64_t, string> aliaS;
	map<int64_t, string> dicE;
	string CCC;

public:
	User(int64_t qq) : RandomAskManager()
	{
		this->qq = qq;

		ifstream CH(usrDir + to_string(qq) + ".csv");
		if (CH.is_open())
		{
			string P2;
			vector<string> cy;
			for (; !CH.eof();)
			{
				getline(CH, P2); if (P2.empty()) continue; cy = csv(P2);
				if (cy[0]._Equal("alias")) aliaS.insert(make_pair(to_int(cy[1]), cy[2]));
				else if (cy[0]._Equal("dice")) dicE.insert(make_pair(to_int(cy[1]), cy[2]));
				else if (cy[0]._Equal("ask")) ((RandomAskManager*)this)->Load(cy);
				else CCC += "\n" + P2;
			}
			CH.close();
		}
		CCC = CCC;
	}
	string toString()
	{
		string Ly;
		for (auto D2 : aliaS) Ly += "\nalias," + to_string(D2.first) + "," + to_csv(D2.second);
		for (auto D2 : dicE) Ly += "\ndice," + to_string(D2.first) + "," + to_csv(D2.second);
		if (((RandomAskManager*)this)->size() != 0) Ly += "\n" + ((RandomAskManager*)this)->toString();
		Ly += CCC;
		if (!Ly.empty()) Ly.erase(0, 1);
		return Ly;
	}
	void Save()
	{
		ofstream kx(usrDir + to_string(qq) + ".csv");
		if (kx.is_open())
		{
			kx << toString();
			kx.close();
		}
	}

	//	set
	void setalias(string new_alias, int64_t gid = 0)
	{
		map<int64_t, string>::iterator D2 = aliaS.find(gid);
		if (D2 != aliaS.end()) aliaS.erase(D2);
		if (!new_alias.empty()) aliaS.insert(make_pair(gid, new_alias));
	}
	void setdice(string new_dice, int64_t gid = 0)
	{
		map<int64_t, string>::iterator D2 = dicE.find(gid);
		if (D2 != dicE.end()) dicE.erase(D2);
		if (!new_dice.empty()) dicE.insert(make_pair(gid, new_dice));
	}

	//	get
	string alias(int64_t gid = 0, bool mark = false, bool flag = false)
	{
		map<int64_t, string>::iterator D2;
		D2 = aliaS.find(gid);

		//	set
		if (D2 != aliaS.end()) return D2->second + (mark ? "(" + to_string(qq) + ")" : "");
		//	PVT unset
		else if (gid == 0) return flag ? atQQ(qq) : "��";
		//	ING unset
		else return this->alias(0, mark, true);
	}
	string dice(int64_t gid = 0)
	{
		map<int64_t, string>::iterator D2;
		D2 = dicE.find(gid);
		if (D2 == dicE.end()) return "1d20";
		else return D2->second;
	}
};
//	Ⱥ��
class Group : public RandomAskManager
{
	int64_t gid;
	string aliaS;
	vector<int64_t> dM;
	string CCC;

public:
	Group(int64_t gid)
	{
		this->gid = gid;

		ifstream CH(usrDir + "g" + to_string(gid) + ".csv");
		string P2;
		if (CH.is_open())
		{
			vector<string> cy;
			for (; !CH.eof();)
			{
				getline(CH, P2); if (P2.empty()) continue;
				cy = csv(P2);
				if (cy[0]._Equal("alias")) aliaS = cy[1];
				else if (cy[0]._Equal("dm")) dM.push_back(to_int(cy[1]));
				else if (cy[0]._Equal("ask")) ((RandomAskManager*)this)->Load(cy);
				else CCC += "\n" + P2;
			}
			CH.close();
		}
	}
	string toString()
	{
		string Ly;
		if (!aliaS.empty()) Ly += "\nalias," + aliaS;
		for (auto D2 : dM) Ly += "\ndm," + to_string(D2);
		if (((RandomAskManager*)this)->size() != 0) Ly += "\n" + ((RandomAskManager*)this)->toString();
		Ly += CCC;
		if (!Ly.empty()) Ly.erase(0, 1);
		return Ly;
	}
	void Save()
	{
		ofstream kx(usrDir + "g" + to_string(gid) + ".csv");
		if (kx.is_open())
		{
			kx << toString();
			kx.close();
		}
	}

	//	set
	void setalias(string alias) { aliaS = alias; }
	bool append_dm(int64_t qq)
	{
		for (vector<int64_t>::iterator D2 = dM.begin(); D2 != dM.end(); D2++) if (*D2 == qq) return false;
		dM.push_back(qq);
		return true;
	}
	bool erase_dm(int64_t qq)
	{
		for (vector<int64_t>::iterator D2 = dM.begin(); D2 != dM.end(); D2++) if (*D2 == qq)
		{
			dM.erase(D2);
			return true;
		}
		return false;
	}

	//	get
	string alias(bool mark = false) { return !aliaS.empty() ? aliaS + (mark ? "(" + to_string(gid) + ")" : "") : "Ⱥ" + to_string(gid); }
	vector<int64_t> alldm() { return dM; }

	//	is?
	bool is_dm(int64_t qq)
	{
		for (vector<int64_t>::iterator D2 = dM.begin(); D2 != dM.end(); D2++)
			if (*D2 == qq) return true;
		return false;
	}
};



///	��ָ�

//	���󣺲���<str>ֻ��Ⱥ/���ڿ��á�[recommand]
class arg_used_in_pvt : public exception {
public: arg_used_in_pvt(string str, string recommand = "") :
	exception::exception(("���󣺲���" + str + "ֻ��Ⱥ/���ڿ��á�" + recommand).data()) {};
};
//	����<str>ȱ�ٱ�Ҫ�Ĳ�����[recommand]
class arg_not_found : public exception {
public: arg_not_found(string str, string recommand = "") :
	exception::exception(("����" + str + "ȱ�ٱ�Ҫ�Ĳ�����" + recommand).data()) {};
};
//	���󣺲���<str>���Ϸ���[recommand]
class arg_illegal : public exception {
public: arg_illegal(string str, string recommand = "") :
	exception::exception(("���󣺲���" + str + "���Ϸ���" + recommand).data()) {};
};
//	���󣺲���<str>����ͬʱʹ�á�[recommand]
class arg_use_same_time : public exception {
public: arg_use_same_time(string str, string recommand = "") :
	exception::exception(("���󣺲���" + str + "����ͬʱʹ�á�" + recommand).data()) {};
};
//	����<str>��ҪDMȨ�ޡ�[recommand]
class operator_is_not_dm : public exception {
public: operator_is_not_dm(string str, string recommand = "") :
	exception::exception(("����" + str + "��ҪDMȨ�ޡ�" + recommand).data()) {};
};



//  setnameָ��
string setname(bool sudo, int type, int64_t qq, int64_t gid, vector<string> args)
{
	string Ly = "";
	bool Sp = false;

	User Who = User(qq);
	Group Where = Group(gid);

	bool help = false;
	bool clear = false;
	bool forGroup = false;
	bool forOther = false;

	/*  id = 0  */	string alias = "";
	/*  id = 1  */	int64_t otherQQ = 0;

	stack<int> ACP;
	ACP.push(0);

	for (int i = 1; i != args.size(); i++)
	{
		if (args[i].find("-") == 0)
		{
			if (args[i]._Equal("--help")) help = true;
			else if (args[i]._Equal("-clear")) clear = true;
			else if (args[i]._Equal("-g")) forGroup = true;
			else if (args[i]._Equal("-for")) forOther = true, ACP.push(1);

			if (forGroup && type == PVT)
				throw (arg_used_in_pvt("-g"));
			if (forOther && type == PVT)
				throw (arg_used_in_pvt("-g"));
			if (forOther && !sudo && !Where.is_dm(qq))
				throw (operator_is_not_dm("����-for"));
			if (forGroup && forOther)
				throw (arg_illegal("-g��-for", "��������������ͬʱʹ�á�"));
		}
		else
		{
			anal(args[i]);
			if (!ACP.empty())
			{
				switch (ACP.top())
				{
				case 0:
					alias = args[i]; break;
				case 1:
					if (is_QQNumber(args[i])) otherQQ = to_QQNumber(args[i]);
					else throw (arg_illegal(args[i], "�����Ϊ��ȷ��QQ���룬����ֱ��@ҪЭ�����óƺ��ĳ�Ա��"));
					break;
				}
				ACP.pop();
			}
		}
	}
	if (forGroup && (clear || !alias.empty()) && !sudo && !Where.is_dm(qq))
		throw (operator_is_not_dm(string("") + "ʹ�ò���-g�޸�" + (type == GRP ? "Ⱥ" : "��") + "�ƺ�"));
	if (forOther && otherQQ == 0)
		throw (arg_not_found("-for", "��ָ��QQ���룬����ֱ��@ҪЭ�����óƺ��ĳ�Ա��"));
	if (forOther && alias.empty() && !clear)
		throw (arg_not_found("-for", "��ָ�������õĳƺ���"));
	if (!sudo && forOther && type == PVT)
		throw (arg_used_in_pvt("-for"));
	if (forGroup && type == PVT)
		throw (arg_used_in_pvt("-g"));

	if (help) return string("")
		+ "���÷���\n"
		+ ".setname [�ƺ�]\n"
		+ "�����Լ����Զ���ƺ������û���ṩ�ƺ�������ʾ��ǰ�ƺ���\n"
		+ (type == PVT ? "" : string("") + "ע�⣺���ڱ�" + (type == GRP ? "Ⱥ" : "��") + "�����õĳƺ��ᱻ����ʹ�ã�������ڱ�" + (type == GRP ? "Ⱥ" : "��") + "û�гƺ������ʹ������˽�������õĳƺ���\n")
		+ "\n"
		+ "��������\n"
		+ "-clear\n����Զ���ƺ���\n\n"
		+ ((type == PVT || (!sudo && !Where.is_dm(qq))) ? "" : "-for <QQ��>\nΪָ���ĳ�Ա���óƺ�����ҪDMȨ�ޡ�\n\n")
		+ (type == PVT ? "" : string("") + "-g\n��ʾҪ�鿴�����õ��Ǳ�" + (type == GRP ? "Ⱥ" : "��") + "�ĳƺ����������Լ��ġ����Ҫ���ñ�" + (type == GRP ? "Ⱥ" : "��") + "�ƺ�����ҪDMȨ�ޡ�\n\n")
		+ "--help\n��ʾ����Ϣ��";

	if (gid == 0)
	{
		if (clear)
		{
			Who.setalias("");
			Ly = "��ĳƺ��Ѿ������";
			Who.Save();
		}
		else if (!alias.empty())
		{
			Who.setalias(alias);
			Ly = "��ĳƺ����Ϊ��" + Who.alias() + "����";
			Who.Save();
		}
		else Ly = "��ĳƺ��ǡ�" + Who.alias() + "����";
	}
	else if (forGroup)
	{
		if (clear)
		{
			Where.setalias("");
			Ly = "��" + string(type == GRP ? "Ⱥ" : "������") + "�ĳƺ��Ѿ������";
			Where.Save();
		}
		else if (!alias.empty())
		{
			Where.setalias(alias);
			Ly = "��" + string(type == GRP ? "Ⱥ" : "������") + "�ĳƺ����Ϊ��" + Where.alias() + "����";
			Where.Save();
		}
		else Ly = "��" + string(type == GRP ? "Ⱥ" : "������") + "�ĳƺ��ǡ�" + Where.alias() + "����";
	}
	else if (forOther)
	{
		User forWhom = User(otherQQ);
		if (clear)
		{
			forWhom.setalias("");
			Ly = Who.alias(gid) + "��" + atQQ(otherQQ) + "��" + Where.alias() + "�ĳƺ������";
			forWhom.Save();
		}
		else if (!alias.empty())
		{
			forWhom.setalias(alias, gid);
			Ly = Who.alias(gid) + "��" + atQQ(otherQQ) + "��" + Where.alias() + "�ĳƺ����Ϊ��" + forWhom.alias(gid) + "����";
			forWhom.Save();
		}
	}
	else
	{
		if (clear)
		{
			Who.setalias("", gid);
			Ly = atQQ(qq) + "��" + Where.alias() + "�ĳƺ��Ѿ������";
			Who.Save();
		}
		else if (!alias.empty())
		{
			Who.setalias(alias, gid);
			Ly = atQQ(qq) + "��" + Where.alias() + "�ĳƺ����Ϊ" + Who.alias(gid);
			Who.Save();
		}
		else Ly = atQQ(qq) + "��" + Where.alias() + "�ĳƺ��ǡ�" + Who.alias(gid) + "����";
	}

	return Ly;
}
//  lsָ��
string ls(bool sudo, int type, int64_t qq, int64_t gid, vector<string> args)
{
	string Ly = "";

	bool help = false;
	bool man = false;
	bool ver = false;

	for (int i = 1; i != args.size(); i++)
	{
		if (args[i].find("-") == 0)
		{
			if (args[i]._Equal("--help")) help = true;
			else if (args[i]._Equal("--ver")) ver = true;
		}
		else
		{
			anal(args[i]);
		}
	}

	if (help) return string("")
		+ "���÷���\n"
		+ ".ls\n�г����п��õ�ָ�\n\n"
		+ "��������\n"
		+ "--ver\n��ʾ�汾��Ϣ��\n\n"
		+ "--help\n��ʾ����Ϣ��";

	if (man)
	{
		thread sendManual(sendManual_main, sudo, type, qq, gid);
		sendManual.detach();
		return "���ڷ���ʹ���ֲᡭ��";
	}

	if (ver) return string("")
		+ CQAPPID + " " + CQAPPVER + "\n"
		+ "Copyright (C) 2019 Skot\n"
		+ "���֤��GPLv3+��GNUͨ�ù������֤��3�����°汾<http://gnu.org/licenses/gpl.html>\n"
		+ "���������������������������޸ĺ����·�������\n"
		+ "�ڷ��ɷ�Χ��û���������ϡ�\n"
		+ "\n"
		+ "�� ����618A03 (95806902) ��д��\n";

	return string("")
		+ "ls - ��ʾ��Ϣ\n"
		+ "setname - ���óƺ�\n"
		+ "d - ������\n"
		+ "dm - DM���\n"
		+ "ask - ����ʴ�\n";
}
//  dָ��
string d(bool sudo, int type, int64_t qq, int64_t gid, vector<string> args)
{
	string Ly = "";

	User Who = User(qq);
	Group Where = Group(gid);

	bool help = false;
	bool set = false;
	bool hide = false;
	bool check = false;
	bool rpt = false;

	/*  id = 0  */	string exp = "";
	/*  id = 1  */	string des = "";
	/*  id = 2  */	int dc = INT_MAX;
	/*  id = 3  */	int rpt_time = INT_MAX;

	stack<int> ACP;
	ACP.push(1);
	ACP.push(0);

	for (int i = 1; i != args.size(); i++)
	{
		if (args[i].find("-") == 0)
		{
			if (args[i]._Equal("--help")) help = true;
			else if (args[i]._Equal("-set")) set = true;
			else if (args[i]._Equal("-h")) hide = true;
			else if (args[i]._Equal("-dc")) check = true, ACP.push(2);
			else if (args[i]._Equal("-r")) rpt = true, ACP.push(3);

			if (hide && type == PVT) throw (arg_used_in_pvt("-h"));
		}
		else
		{
			anal(args[i]);
			if (!ACP.empty())
			{
				switch (ACP.top())
				{
				case 0:
					exp = args[i]; break;
				case 1:
					des = args[i]; break;
				case 2:
					dc = to_int(args[i]); break;
				case 3:
					if (is_num(args[i])) rpt_time = to_int(args[i]);
					else throw (arg_illegal("(-r)" + args[i], "�����Ϊ��������"));
					if (rpt_time < 1)
						throw (arg_illegal("(-r)" + args[i], "�ظ���������Ϊ1��"));
					if (rpt_time > 10 && type != PVT && !sudo)
						throw (arg_illegal("(-r)" + args[i], string("") + "��" + (type == GRP ? "Ⱥ" : "��") + "�ڣ��ظ��������ܳ���10������Ϊ�˷�ֹˢ�������������ơ�"));
					if (rpt_time > 100)
						throw (arg_illegal("(-r)" + args[i], "�ظ��������ܳ���100������Ϊ�˷�ֹ����ʱ���������Ϣ���ȹ�����"));
				}
				ACP.pop();
			}
		}
	}
	if (rpt && rpt_time == INT_MAX)
		throw (arg_not_found("-r", "��ָ����Ҫ�ظ��Ĵ�����"));

	if (help) return string("")
		+ "���÷���\n"
		+ ".d [���ʽ] [����˵��]\n�����ӣ����û���ṩ���ʽ����ʹ��Ԥ���Ĭ�����ӡ�\n\n"
		+ "��������\n"
		+ "-r <����>\n�����ʽ�ظ������ɴΣ�����1�Σ����" + ((sudo || type == PVT) ? "100" : "10") + "�Ρ�\n\n"
		+ "-dc [�Ѷȵȼ�]\n�Լ춨����ʽ��������������ָ���Ѷȵȼ�������Ϊ�Կ��춨��\n\n"
		+ (type == PVT ? "" : string("") + "-h\n�������������ͨ��˽�ŷ��͸�" + (sudo ? "���" : "") + "��" + (type == GRP ? "Ⱥ" : "��") + "��DM��\n\n")
		+ "-set\n�����ʽ��Ϊ�Լ�ʹ�á�.d��ָ��ʱ��Ĭ�����ӣ���ʼֵ��1d20������û�и������ʽ������ʾ��ǰ��Ĭ��ֵ��\n"
		+ (type == PVT ? "" : string("") + "ע�⣺���ڱ�" + (type == GRP ? "Ⱥ" : "��") + "�����õ�Ĭ�����ӻᱻ����ʹ�ã�������ڱ�" + (type == GRP ? "Ⱥ" : "��") + "û�����ã����ʹ������˽���е����á�\n")
		+ "\n"
		+ "--help\n��ʾ����Ϣ��";

	if (set)
	{
		Ly = Who.alias(gid) + (type == PVT ? "" : "��" + Where.alias()) + "��Ĭ������";
		if (!exp.empty())
		{
			Who.setdice(exp, gid);
			Ly += "����Ϊ��\n" + Who.dice(gid);
			Who.Save();
		}
		else Ly += "�ǣ�\n" + Who.dice(gid);
		return Ly;
	}

	if (exp.empty() || (des.empty() && !is_LegalDiceExp(exp)))
	{
		des = exp;
		exp = Who.dice(gid);
	}
	try
	{
		MyCalc D20(exp);
		if (!rpt) rpt_time = 1;

		Ly += Who.alias(gid);
		if (check)
		{
			Ly += "����" + des + "�춨��\n" + D20.Expression();
			if (dc == INT_MAX) Ly += "���Կ���\n";
			else Ly += "��DC" + to_string(dc) + "��\n";
		}
		else if (!des.empty()) Ly += "��" + des + "��\n";
		else Ly += "�����ӣ�\n";

		int pass = 0;
		int fail = 0;
		string P2 = "";
		string CCC = "";

		for (int i = 0; i != rpt_time; i++)
		{
			D20.refresh();
			P2 = to_string(D20.Result());
			int loc = 0;
			for (string::iterator it = P2.begin(); it != P2.end(); it++) switch (*it)
			{
			case '.':
				loc = it - P2.begin();
				continue;
			case '0':
				continue;
			default:
				if (loc != 0) loc = it - P2.begin() + 1;
			}
			P2.erase(loc);

			if (rpt_time != 1) Ly += "��" + to_string(i + 1) + "�Σ�";
			if (check)
			{
				if (dc == INT_MAX) Ly += D20.Expression() + " = " + P2 + "\n";
				else if (D20.Result() > dc) Ly += "��  �ɹ���" + P2 + " > " + to_string(dc) + "��\n", pass++;
				else if (D20.Result() < dc) Ly += "��  ʧ�ܣ�" + P2 + " < " + to_string(dc) + "��\n", fail++;
				else Ly += "��  �ɹ���" + P2 + " = " + to_string(dc) + "��\n", pass++;
			}
			else Ly += D20.Expression() + " = " + P2 + "\n";
		}
		if (check && dc != INT_MAX && rpt_time != 1)
		{
			if (fail == 0) Ly += "ȫ���ɹ���";
			else if (pass == 0) Ly += "ȫ��ʧ�ܡ�";
			else Ly += "�ɹ�" + to_string(pass) + "�Σ�ʧ��" + to_string(fail) + "�Ρ�\n";
		}

		string Ly4DM = "";
		if (hide)
		{
			Ly4DM = "����" + Where.alias(true) + "�İ�����\n" + Ly;
			Ly4DM.erase(Ly4DM.end() - 1);
			Ly = Who.alias() + "����һ��������\n";

			vector<int64_t> DMList = Where.alldm();

			if (DMList.empty()) Ly += "��ע�⣬" + Where.alias() + "��ǰû��DM����\n";
			else for (int64_t D2 : DMList) PostMsg(PVT, D2, Ly4DM);
			if (sudo) PostMsg(PVT, qq, Ly4DM);
		}
	}
	catch (exception Ed)
	{
		return Ed.what();
	}
	return Ly;
}
//  dmָ��
string dm(bool sudo, int type, int64_t qq, int64_t gid, vector<string> args)
{
	string Ly = "";

	User Who = User(qq);
	Group Where = Group(gid);

	bool help = false;
	bool list = false;
	bool set = false;
	bool del = false;

	/*  id = 0  */	queue<int64_t> setDMList;
	/*  id = 1  */	queue<int64_t> delDMList;

	stack<int> ACP;

	for (int i = 1; i != args.size(); i++)
	{
		if (args[i].find("-") == 0)
		{
			if (args[i]._Equal("--help")) help = true;
			else if (args[i]._Equal("-l")) list = true;
			else if (args[i]._Equal("-set")) set = true, ACP.push(0);
			else if (args[i]._Equal("-del")) del = true, ACP.push(1);

			if (list && type == PVT)
				throw (arg_used_in_pvt("-l"));
			if (set && type == PVT)
				throw (arg_used_in_pvt("-set"));
			if (set && !sudo && !Where.is_dm(qq))
				throw (operator_is_not_dm("����-set"));
			if (del && type == PVT)
				throw (arg_used_in_pvt("-del"));
			if (del && !sudo && !Where.is_dm(qq))
				throw (operator_is_not_dm("����-del"));
		}
		else
		{
			anal(args[i]);
			if (!ACP.empty())
			{
				switch (ACP.top())
				{
				case 0: if (is_QQNumber(args[i]))
					for (setDMList.push(to_QQNumber(args[i])); i + 1 != args.size() && is_QQNumber(args[i + 1]); setDMList.push(to_QQNumber(args[++i])));
						else throw (arg_illegal("(-set)" + args[i], "�����Ϊ��ȷ��QQ���룬����ֱ��@Ҫ����DMȨ�޵ĳ�Ա��"));
					break;
				case 1: if (is_QQNumber(args[i]))
					for (delDMList.push(to_QQNumber(args[i])); i + 1 != args.size() && is_QQNumber(args[i + 1]); delDMList.push(to_QQNumber(args[++i])));
						else throw (arg_illegal("(-del)" + args[i], "�����Ϊ��ȷ��QQ���룬����ֱ��@Ҫ���DMȨ�޵ĳ�Ա��"));
					break;
				}
				ACP.pop();
			}
		}
	}
	if (set && setDMList.empty())
		throw (arg_not_found("-set"));
	if (del && delDMList.empty())
		throw (arg_not_found("-del"));

	if (type == PVT) return "����ָ��dmֻ����Ⱥ/����ʹ�á�";
	if (!help && !list && !set && !del) return "����ָ��dm����������ʹ�á�";

	if (help) return string("")
		+ "���÷���\n"
		+ ".dm <����>\n���ָ�����" + (type == PVT ? "��Ⱥ/����" : "") + "�������ʹ�á�\n\n"
		+ "��������\n"
		+ ((type == PVT || (!sudo && !Where.is_dm(qq))) ? "" : "-set <QQ��>\n����ָ����ԱDMȨ�ޡ���ҪDMȨ�ޡ�\n\n")
		+ ((type == PVT || (!sudo && !Where.is_dm(qq))) ? "" : "-del <QQ��>\n���ָ����Ա��DMȨ�ޡ���ҪDMȨ�ޡ�\n\n")
		+ (type == PVT ? "" : string("") + "-l\n�г���" + (type == GRP ? "Ⱥ" : "��") + "������DM��\n\n")
		+ "--help\n��ʾ����Ϣ��";

	if (del)
	{
		string suc = "";
		string err = "";
		for (; !delDMList.empty(); delDMList.pop())
		{
			if (Where.erase_dm(delDMList.front())) suc += User(delDMList.front()).alias(gid) + "\n";
			else err += User(delDMList.front()).alias(gid) + "\n";
		}
		if (!suc.empty()) Ly += Who.alias(gid) + "��������³�Ա��" + Where.alias() + "��DMȨ�ޣ�\n" + suc;
		if (!err.empty()) Ly += "���³�Ա����" + Where.alias() + "��DM��\n" + err;
		Where.Save();

	}

	if (set)
	{
		string suc = "";
		string err = "";
		for (; !setDMList.empty(); setDMList.pop())
		{
			if (Where.append_dm(setDMList.front())) suc += User(setDMList.front()).alias(gid) + "\n";
			else err += User(setDMList.front()).alias(gid) + "\n";
		}
		if (!suc.empty()) Ly += Who.alias(gid) + "�������³�Ա��" + Where.alias() + "��DMȨ�ޣ�\n" + suc;
		if (!err.empty()) Ly += "���³�Ա�Ѿ���" + Where.alias() + "��DM��\n" + err;
		Where.Save();
	}

	if (list)
	{
		vector<int64_t> DMList;
		DMList = Where.alldm();
		if (!DMList.empty())
		{
			Ly += Where.alias() + "��DM���£�\n";
			for (int64_t D2 : DMList) Ly += User(D2).alias(gid, true) + "\n";
		}
		else Ly += Where.alias() + "��ǰû��DM��\n";
	}

	return Ly;
}
//  askָ��
string ask(bool sudo, int type, int64_t qq, int64_t gid, vector<string> args)
{
	string Ly = "";

	User Who = User(qq);
	Group Where = Group(gid);

	string P2 = "";
	string CCC = "";

	bool help = false;
	bool del = false;
	bool crt = false;
	bool set = false;
	bool cp = false;
	bool cpCover = false;
	bool cpFilter = false;
	bool cpMask = false;
	bool era = false;
	bool app = false;
	bool appForce = false;
	bool list = false;
	bool listAll = false;
	bool normal_ask = true;

	bool Nid = false;

	/*  id = 0  */	string question = "";
	/*	id = 1	*/	vector<string> setProperties;
	/*  id = 2  */	string fromQuestion = "";
	/*  id = 3  */	vector<string> eraAns;
	/*  id = 4  */	vector<string> appAns;

	stack<int> ACP;
	ACP.push(0);

	/*	id = 0	*/	//cp
	/*	id = 1	*/	//app
	stack<int> ARC;

	for (int i = 1; i != args.size(); i++)
	{
		if (args[i]._Equal("-cover") || args[i]._Equal("-filter") || args[i]._Equal("-mask") || args[i]._Equal("-force")
			|| args[i]._Equal("-c") || args[i]._Equal("-f") || args[i]._Equal("-m"))
		{
			normal_ask = false;
			if (args[i]._Equal("-f") && ARC.empty())
				throw (arg_illegal(args[i], "�������ֻ����������-cp��-app��"));
			else if (args[i]._Equal("-force") && (ARC.empty() || ARC.top() != 1))
				throw (arg_illegal(args[i], "�������ֻ����������-app��"));
			else if (ARC.empty() || ARC.top() != 0)
				throw (arg_illegal(args[i], "�������ֻ����������-cp��"));

			if (args[i]._Equal("-cover") || args[i]._Equal("-c")) cpCover = true;
			if (args[i]._Equal("-filter")) cpFilter = true;
			if (args[i]._Equal("-mask") || args[i]._Equal("-m")) cpMask = true;
			if (args[i]._Equal("-force")) appForce = true;
			if (args[i]._Equal("-f")) switch (ACP.top())
			{
			case 2: cpFilter = true; break;
			case 4: appForce = true; break;
			}
		}
		else if (args[i].find("-") == 0)
		{
			normal_ask = false;
			if (!ARC.empty()) ARC.pop();

			if (!ACP.empty() && (ACP.top() == 3 || ACP.top() == 4)) ACP.pop();
			if (args[i]._Equal("--help")) help = true;
			else if (args[i]._Equal("-del")) del = true;
			else if (args[i]._Equal("-new")) crt = true;
			else if (args[i]._Equal("-set")) set = true, ACP.push(1);
			else if (args[i]._Equal("-cp")) cp = true, ACP.push(2), ARC.push(0);
			else if (args[i]._Equal("-era")) era = true, ACP.push(3);
			else if (args[i]._Equal("-app")) app = true, ACP.push(4), ARC.push(1);
			else if (args[i]._Equal("-l")) list = true;
			else if (args[i]._Equal("-all")) listAll = true;
			else if (args[i]._Equal("-this-is-nid")) Nid = true;
		}
		else
		{
			anal(args[i]);
			if (!ACP.empty())
			{
				switch (ACP.top())
				{
				case 0:
					question = args[i]; break;
				case 1:
					setProperties.push_back(args[i]); break;
				case 2:
					fromQuestion = args[i]; break;
				case 3:
					eraAns.push_back(args[i]); break;
				case 4:
					appAns.push_back(args[i]); break;
				}
				if (ACP.top() == 0 || ACP.top() == 2) ACP.pop();
			}
		}
	}
	if (question.empty() && !help && !listAll)
		throw (arg_not_found("ָ��ask", "����ʹ��--help��-all���������ָ��һ�����⡣"));
	if (set && setProperties.empty())
		throw (arg_not_found("-set", "���õ����԰�����readonly/editable��listable/packaged��"));
	if (cp && fromQuestion.empty())
		throw (arg_not_found("-cp", "��Ҫָ����һ�����⡣"));
	if (cp) switch (1000 + (cpCover ? 100 : 0) + (cpFilter ? 10 : 0) + (cpMask ? 1 : 0))
	{
	case 1000: throw (arg_not_found("-cp", "��Ҫָ������ģʽ��-cover��-filter��-mask����"));
	case 1110: throw (arg_use_same_time("-cover��-filter"));
	case 1101: throw (arg_use_same_time("-cover��-mask"));
	case 1011: throw (arg_use_same_time("-filter��-mask"));
	case 1111: throw (arg_use_same_time("-cover��-filter��-mask"));
	}
	if (era && eraAns.empty())
		throw (arg_not_found("-era", "��Ҫ�ṩҪɾ���Ļش�"));
	if (app && appAns.empty())
		throw (arg_not_found("-app", "��Ҫ�ṩҪ��ӵĻش�"));

	if (help) return string("")
		+ "���÷���\n"
		+ ".ask <����>\n��ָ���������ʡ�\n" + (Nid ? "������֮��ask��������ˣ�\n" : "") + "\n"
		+ "��������\n"
		+ "-del\nɾ�������⡣\n\n"
		+ "-new\n�����µ����⡣" + (sudo ? "sudo����������Ѿ����ڣ�����ǿ�Ƹ��ǡ�" : "") + "\n\n"
		+ "-set <����>\n���ø���������ԡ�Ŀǰ���õ�����Ϊreadonly/editable��ֻ��/��д����listable/packaged���ɼ�/��װ����\n\n"
		+ "-cp <��������> <-cover/-filter/-mask>\n����������Ļش��Ƹ���ǰ���⡣����ģʽ���Լ�дΪ-c��-f��-m��\n"
		+ "-cover������ǰ�����ǰ����ԭ�еĻش�\n"
		+ "-filter��ֻ���Ʋ���ӵ�ǰ������û�еĻش�\n"
		+ "-mask��ֱ�ӽ�������������лش���ӵ���ǰ������\n\n"
		+ "-era <�ش�>\n��������ɾȥָ���Ļش�\n\n"
		+ "-app <�ش�> [-force]\n����������ӻش����׷��-force��������ʹ�������Ѿ�����ͬ�Ļش��Կ���ӡ�-force���Լ�дΪ-f��\n\n"
		+ "-l\n�鿴��ǰ��������лش�\n\n"
		+ "-all\n�鿴" + (type == PVT ? "���Լ�������" : "��Ⱥ��") + "�������⡣�����������Ҫָ�����⣬������ֱ��ʹ��.ask -all��\n\n"
		+ "--help\n��ʾ����Ϣ��";

	if (del)
	{
		int i75 = -1;
		if (gid == 0) i75 = Who.delete_question(question, qq);
		else i75 = Where.delete_question(question, (sudo ? 0 : (Where.is_dm(qq) ? 0 : qq)));
		switch (i75)
		{
		case 0:
			Ly += Who.alias(gid) + "ɾ�������⡰" + question + "����\n";
			if (gid == 0) Who.Save();
			else Where.Save();
			break;
		case 1:
			Ly += "-del�����⡰" + question + "�������ڡ�\n";
			break;
		case 2:
			Ly += "-del�����⡰" + question + "������Ϊreadonly��ֻ�������ⴴ���߻�DMɾ����\n";
			break;
		}
	}

	if (crt)
	{
		int i75 = -1;
		if (gid == 0) i75 = Who.create_question({ "ask",to_string(qq),"editable","listable",question });
		else i75 = Where.create_question({ "ask",to_string(qq),"editable","listable",question });
		switch (i75)
		{
		case 0:
			Ly += Who.alias(gid) + "���������⡰" + question + "����\n";
			if (gid == 0) Who.Save();
			else Where.Save();
			break;
		case 1:
			Ly += "-new�����⡰" + question + "���Ѿ����ڡ�\n";
			break;
		}
	}

	if (set)
	{
		int i75 = -1;
		if (gid == 0) i75 = Who.set_question_properties(question, setProperties, qq);
		else i75 = Where.set_question_properties(question, setProperties, (sudo ? 0 : (Where.is_dm(qq) ? 0 : qq)));
		string Nozl;
		switch (i75)
		{
		case 0:
			Ly += Who.alias(gid) + "�������⡰" + question + "��������Ϊ";
			for (auto D2 : setProperties) Nozl += "��" + D2;
			if (!Nozl.empty()) Nozl.erase(0, 2);
			Ly += Nozl + "��\n";
			if (gid == 0) Who.Save();
			else Where.Save();
			break;
		case 1:
			Ly += "-set�����⡰" + question + "�������ڡ�\n";
			break;
		case 2:
			Ly += "-set��ֻ�����ⴴ���߻�DM���������������ԡ�\n";
			break;
		case 3:
			Ly += "-set�������˻�������ԡ����飬readonly��editable����ͬʱ���֡�listable��packaged����ͬʱ���֡�\n";
			break;
		}
	}

	if (cp)
	{
		int i75 = -1;
		char cpMode;
		if (cpCover) cpMode = 'c';
		if (cpFilter) cpMode = 'f';
		if (cpMask) cpMode = 'm';
		if (gid == 0) i75 = Who.copyfrom_question(question, fromQuestion, cpMode, qq);
		else i75 = Where.copyfrom_question(question, fromQuestion, cpMode, (sudo ? 0 : (Where.is_dm(qq) ? 0 : qq)));
		switch (i75)
		{
		case 0:
			Ly += Who.alias() + "�����⡰" + fromQuestion + "���Ļش��Ƹ����⡰" + question + "����\n";
			if (gid == 0) Who.Save();
			else Where.Save();
			break;
		case 1:
			Ly += "-cp��Ŀ�����⡰" + question + "�������ڡ�\n";
			break;
		case 2:
			Ly += "-cp��ԭʼ���⡰" + fromQuestion + "�������ڡ�\n";
			break;
		case 3:
			Ly += "-cp��Ŀ�����⡰" + question + "������Ϊreadonly��ֻ�������ⴴ���߻�DM�޸Ļش�\n";
			break;
		case 4:
			Ly += "-cp��ԭʼ���⡰" + fromQuestion + "����Ϊpackaged��ֻ�����ⴴ���߻�DM���Ի�ȡȫ���ش�\n";
			break;
		}
	}

	if (era)
	{
		int i75 = -1;
		vector<bool> Sp;
		string Nozl;
		if (gid == 0) i75 = Who.erase_answer(Sp, question, eraAns, qq);
		else i75 = Where.erase_answer(Sp, question, eraAns, (sudo ? 0 : (Where.is_dm(qq) ? 0 : qq)));
		switch (i75)
		{
		case 0:
			for (int D2 = 0; D2 != Sp.size(); D2++) if (Sp[D2]) Nozl += "��" + eraAns[D2];
			if (!Nozl.empty())
			{
				Nozl.erase(0, 2);
				Ly += Who.alias(gid) + "�����⡰" + question + "����ɾ���ش�" + Nozl + "��\n";
			}
			else Ly += "���⡰" + question + "����û�д𰸱�ɾ������Ϊ�ṩ�����д𰸾����ڴ��б��С�\n";
			if (gid == 0) Who.Save();
			else Where.Save();
			break;
		case 1:
			Ly += "-era�����⡰" + question + "�������ڡ�\n";
			break;
		case 2:
			Ly += "-era�����⡰" + question + "������Ϊreadonly��ֻ�������ⴴ���߻�DMɾ���ش�\n";
			break;
		}
	}

	if (app)
	{
		int i75 = -1;
		vector<bool> Sp;
		string Nozl;
		if (gid == 0) i75 = Who.append_answer(Sp, question, appAns, appForce, qq);
		else i75 = Where.append_answer(Sp, question, appAns, appForce, (sudo ? 0 : (Where.is_dm(qq) ? 0 : qq)));
		switch (i75)
		{
		case 0:
			for (int D2 = 0; D2 != Sp.size(); D2++) if (Sp[D2]) Nozl += "��" + appAns[D2];
			if (!Nozl.empty())
			{
				Nozl.erase(0, 2);
				Ly += Who.alias(gid) + "�����⡰" + question + "������ӻش�" + Nozl + "��\n";
			}
			else Ly += "���⡰" + question + "����û�д𰸱���ӣ���Ϊ�ṩ�����д𰸾����ڴ��б��С�\n";
			if (gid == 0) Who.Save();
			else Where.Save();
			break;
		case 1:
			Ly += "-app�����⡰" + question + "�������ڡ�\n";
			break;
		case 2:
			Ly += "-app�����⡰" + question + "������Ϊreadonly��ֻ�������ⴴ���߻�DM��ӻش�\n";
			break;
		}
	}

	if (list)
	{
		bool not_found = false;
		bool packaged = false;
		bool no_ans = false;
		RandomAsk D2;

		if (gid == 0) not_found = !Who.exist_question(question);
		else not_found = !Where.exist_question(question);
		if (!not_found)
		{
			if (gid == 0) D2 = Who.getQuestionById(question);
			else D2 = Where.getQuestionById(question);

			packaged = !sudo && (gid == 0 ? true : !Where.is_dm(qq)) && !D2.is_creator(qq) && !D2.listable();
		}
		if (!packaged) no_ans = D2.size() == 0;

		if (not_found) Ly += "-l�����⡰" + question + "�������ڡ�\n";
		else if (packaged) Ly += "-l�����⡰" + question + "������Ϊpackaged��ֻ�����ⴴ���߻�DM���Ի�ȡȫ���ش�\n";
		else if (no_ans) Ly += "���⡰" + question + "��û���κλش�\n";
		else
		{
			string Nozl;
			for (auto Ugin : D2.allanswer()) Nozl += "��" + Ugin;
			if (!Nozl.empty()) Nozl.erase(0, 2);
			Ly += "���⡰" + question + "����ȫ���ش�" + Nozl + "��\n";
		}
	}

	if (listAll)
	{
		map<string, RandomAsk> allQuestion;
		if (gid == 0) allQuestion = Who.allquestion();
		else allQuestion = Where.allquestion();
		if (allQuestion.size() == 0) Ly += (gid == 0 ? Who.alias(gid) : Where.alias()) + "��ǰû���κ����⡣\n";
		else
		{
			Ly += (gid == 0 ? Who.alias(gid) : Where.alias()) + "��ǰ�Ѵ��������⣺\n";
			for (auto D2 : allQuestion) Ly += (D2.first) + "��" + (gid == 0 ? "" : "�����ߣ�" + User(D2.second.creator()).alias(gid) + "��")
				+ (D2.second.readonly() ? "readonly" : "editable") + "��" + (D2.second.listable() ? "listable" : "packaged") + "��\n";
		}
	}

	if (normal_ask)
	{
		bool not_found = false;
		bool no_ans = false;
		RandomAsk D2;

		if (gid == 0) not_found = !Who.exist_question(question);
		else not_found = !Where.exist_question(question);
		if (!not_found)
		{
			if (gid == 0) D2 = Who.getQuestionById(question);
			else D2 = Where.getQuestionById(question);
			no_ans = D2.size() == 0;
		}

		if (not_found) Ly += "ask�����⡰" + question + "�������ڡ�\n";
		else if (no_ans) Ly += "���⡰" + question + "��û���κλش�\n";
		else Ly = "Re " + Who.alias(gid) + "��" + D2.ask() + "\n";
	}

	return Ly;
}
//	TODO��dndָ��
//	TODO��helpָ��



///	������������
//	������
class BlackListUnit
{
public:
	string type;
	int64_t qq;
	int64_t gid;
	BlackListUnit(string str)
	{
		qq = gid = 0;
		vector<string> cy = csv(str);
		type = cy[0];
		if (type._Equal("user")) qq = to_int(cy[1]);
		if (type._Equal("group")) gid = to_int(cy[1]);
		if (type._Equal("inGroup")) gid = to_int(cy[1]), qq = to_int(cy[2]);
	}
};
class BlackList
{
	vector<BlackListUnit> list;
public:
	BlackList()
	{
		ifstream in(blackFile);
		if (in.is_open())
		{
			string P2;
			for (; !in.eof();)
			{
				getline(in, P2); if (P2.empty()) continue;
				list.push_back(BlackListUnit(P2));
			}
			in.close();
		}
	}
	bool contains(int64_t qq, int64_t gid)
	{
		for (auto D2 : list)
		{
			if (D2.type._Equal("user") && qq == D2.qq) return true;
			if (D2.type._Equal("group") && gid == D2.gid) return true;
			if (D2.type._Equal("inGroup") && qq == D2.qq && gid == D2.gid) return true;
		}
		return false;
	}
} blackList;



///	����ڵ㡿

//  �����߳���ڵ�
void run_main(int type, int64_t qq, int64_t gid, string msg)
{
	if (blackList.contains(qq, gid)) return;

	srand(GetTickCount64());

	string Ly = "";
	string sudoStr = "";
	bool sudo = msg.find("sudo") == 0;
	msg.erase(0, (sudo ? 6 : 1));
	if (msg.empty()) return;

	vector<string> args = to_args(msg);
	if (false);
	else if (args[0]._Equal("ls"));
	else if (args[0]._Equal("d"));
	else if (args[0]._Equal("dm"));
	else if (args[0]._Equal("setname"));
	else if (args[0]._Equal("ask"));
	else return;

	if (sudo)
	{
		sudo = false;
		string P2 = "";
		string CCC = "";

		ifstream in(sudoerFile);
		if (in.is_open())
		{
			vector<string> cy;
			for (; !in.eof();)
			{
				getline(in, P2); if (P2.empty()) continue; cy = csv(P2);
				if (cy[0]._Equal(to_string(qq)))
				{
					sudo = true;
					if (cy.size() != 1) sudoStr = (string)"�����������Ѿ��Ӹ����������˽���D20��ע�����"
						+ "\n�ܽ���������������㣺"
						+ "\n"
						+ "\n    #1) ���ر��˵���Ը��"
						+ "\n    #2) ���Ǻú���ͷ��ա�"
						+ "\n    #3) Ȩ��Խ������Խ��"
						+ "\n"
						+ "\n[sudo] ";
					else sudoStr = "[sudo] ";
					CCC += "\n" + cy[0];
				}
				else CCC += "\n" + P2;
			}
			in.close();
		}

		if (!sudo)
		{
			time_t present = time(0);
			tm now;
			localtime_s(&now, &present);

			CCC += "\n" + to_string(now.tm_year + 1900) + "-";
			if (now.tm_mon + 1 < 10) CCC += "0";
			CCC += to_string(now.tm_mon + 1) + "-";
			if (now.tm_mday < 10) CCC += "0";
			CCC += to_string(now.tm_mday) + " ";
			if (now.tm_hour < 10) CCC += "0";
			CCC += to_string(now.tm_hour) + ":";
			if (now.tm_min < 10) CCC += "0";
			CCC += to_string(now.tm_min) + ":";
			if (now.tm_sec < 10) CCC += "0";
			CCC += to_string(now.tm_sec) + " ";
			CCC += to_string(qq);
			if (type == GRP) CCC += " in " + to_string(gid);
			PostMsg(type, (type == PVT ? qq : gid), "QQ " + to_string(qq) + " ���� sudoers �ļ��С����½������档");
		}

		if (!CCC.empty())
		{
			CCC.erase(0, 1);
			ofstream out(sudoerFile);
			if (out.is_open())
			{
				out << CCC;
				out.close();
			}
		}

		if (!sudo) return;
	}

	if (sudo) try
	{
		bool as = false;
		bool asG = false;
		bool asD = false;
		bool asP = false;

		/*  id = 0  */	int64_t asQQ = -1;
		/*  id = 1  */	int64_t asinGroup = -1;
		/*  id = 2  */	int64_t asinDiscuss = -1;

		stack<int> ACP;
		for (int i = 1; i != args.size(); i++)
		{
			if (args[i].find("-") == 0)
			{
				if (args[i]._Equal("-as")) args[i].clear(), as = true, ACP.push(0);
				else if (args[i]._Equal("-asg")) args[i].clear(), asG = true, ACP.push(1);
				else if (args[i]._Equal("-asd")) args[i].clear(), asD = true, ACP.push(2);
				else if (args[i]._Equal("-asp")) args[i].clear(), asP = true;

				if (asG && asD || asG && asP || asD && asP)
					throw (arg_illegal("-asg��-asd��-asp", "����������ֻ��ͬʱʹ��һ����"));
			}
			else
			{
				if (!ACP.empty())
				{
					anal(args[i]);
					switch (ACP.top())
					{
					case 0:
						if (!is_QQNumber(args[i]))
							throw (arg_illegal("(-as)" + args[i], "�����Ϊ��ȷ��QQ���룬����ֱ��@��Ա��"));
						asQQ = to_QQNumber(args[i]);
						args[i].clear();
						break;
					case 1:
						if (!is_QQNumber(args[i]))
							throw (arg_illegal("(-asg)" + args[i], "�����Ϊ��ȷ��Ⱥ���롣"));
						asinGroup = to_QQNumber(args[i]);
						args[i].clear();
						break;
					case 2:
						if (!is_QQNumber(args[i]))
							throw (arg_illegal("(-asd)" + args[i], "�����Ϊ��ȷ������롣"));
						asinGroup = to_QQNumber(args[i]);
						args[i].clear();
						break;
					}
					ACP.pop();
				}
			}
		}
		if (as && asQQ == -1)
			throw (arg_not_found("����-as", string("") + "��ָ��QQ����" + (type == PVT ? "" : "������ֱ��@��Ա") + "��"));
		if (asG && asinGroup == -1)
			throw (arg_not_found("����-asg", string("") + "��ָ��Ⱥ���롣"));
		if (asD && asinDiscuss == -1)
			throw (arg_not_found("����-asd", string("") + "��ָ������롣"));

		if (as) qq = asQQ;
		if (asinGroup != -1) type = GRP, gid = asinGroup;
		if (asinDiscuss != -1) type = DIS, gid = asinGroup;
		if (asP) type = PVT;
	}
	catch (exception Ed)
	{
		PostMsg(type, (type == PVT ? qq : gid), Ed.what());
		return;
	}

	try
	{
		if (args.back()._Equal("--debug"))
		{
			Ly += "argc = " + to_string(args.size() - 1);
			for (int i = 0; i != args.size() - 1; i++)
			{
				Ly += "\nargv[" + to_string(i) + "] = " + args[i];
			}
			PostMsg(type, (type == PVT ? qq : gid), Ly);
			return;
		}
		else if (args[0]._Equal("ls")) Ly += ls(sudo, type, qq, gid, args);
		else if (args[0]._Equal("d")) Ly += d(sudo, type, qq, gid, args);
		else if (args[0]._Equal("dm")) Ly += dm(sudo, type, qq, gid, args);
		else if (args[0]._Equal("setname")) Ly += setname(sudo, type, qq, gid, args);
		else if (args[0]._Equal("ask")) Ly += ask(sudo, type, qq, gid, args);
	}
	catch (exception Ed)
	{
		Ly += Ed.what();
	}

	if (Ly.empty()) return;

	if (*(Ly.end() - 1) == '\n') Ly.erase(Ly.end() - 1);
	if (*Ly.begin() == '\n') Ly.erase(0, 1);
	if (sudo) Ly = sudoStr + Ly;
	PostMsg(type, (type == PVT ? qq : gid), Ly);
	return;
}