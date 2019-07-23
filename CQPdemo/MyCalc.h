#pragma once

//#ifndef __MYCALC_H__
//#define __MYCALC_H__

#include <string>
#include <cstdlib>
#include <stack>

#define MYRAND_MAX 0x7FFFFFFF
//	�궨�壬�ж��ַ�n�����ֻ��������
#define ISNUM(n) (n >= '0' && n <= '9' || n == '.')

//	���Ų�ƥ��
class BucketNoMatch_exception : public std::exception {
public: BucketNoMatch_exception() : std::exception::exception("�������Ų�ƥ�䡣\n") {};
};
//	ͬһ�������г��ֶ��С����
class TooManyDots_exception : public std::exception {
public: TooManyDots_exception() : std::exception::exception("���������в��ܳ��ֶ��С���㡣\n") {};
};
//	������������ർ�²�����ջ��ջ
class TooManyOperators_exception : public std::exception {
public: TooManyOperators_exception() : std::exception::exception("����ʹ���˹�����������\n") {};
};
//	�����k��������
class KNotFollowD_exception : public std::exception {
public:	KNotFollowD_exception() : std::exception::exception("���������k���ܵ���ʹ�ã�������������d֮��\n") {};
};
//	�����d��ǰ��������������
class Dn1NotInt_exception : public std::exception {
public: Dn1NotInt_exception() : std::exception::exception("���������d��ǰ������������������\n") {};
};
//	�����d��ǰ������С��0
class Dn1LessThan0_exception : public std::exception {
public: Dn1LessThan0_exception() : std::exception::exception("���������d��ǰ������������ڻ����0��\n") {};
};
//	�����d�ĺ��������������
class Dn2NotInt_exception : public std::exception {
public: Dn2NotInt_exception() : std::exception::exception("���������d�ĺ������������������\n") {};
};
//	�����d�ĺ������С��1
class Dn2LessThan1_exception : public std::exception {
public: Dn2LessThan1_exception() : std::exception::exception("���������d�ĺ������������ڻ����1��\n") {};
};
//	�����d�ĺ������̫��
class Dn2TooLarge_exception : public std::exception {
public: Dn2TooLarge_exception() : std::exception::exception("���������d�ĺ����������С�ڻ����2147483647��2^31 - 1����\n") {};
};
//	�����k�ĺ��������������
class Kn2NotInt_exception : public std::exception {
public: Kn2NotInt_exception() : std::exception::exception("���������k�ĺ������������������\n") {};
};
//	�����k�ĺ������С��0
class Kn2LessThan0_exception : public std::exception {
public: Kn2LessThan0_exception() : std::exception::exception("���������k�ĺ������������ڻ����0��\n") {};
};
//	�����k�ĺ����������d��ǰ������
class Kn2GreatThanDn1_exception : public std::exception {
public: Kn2GreatThanDn1_exception() : std::exception::exception("���������k�ĺ���������ܴ��������d��ǰ��������\n") {};
};
//	�����/�ĺ������Ϊ0
class Devn2Eq0_exception : public std::exception {
public: Devn2Eq0_exception() : std::exception::exception("���������/�ĺ����������Ϊ0��\n") {};
};
//	���ʽ���Ϸ�
class IllegalExpression_exception : public std::exception {
public: IllegalExpression_exception() : std::exception::exception("���󣺱��ʽ���Ϸ���\n") {};
};

//	int64_t��ʽ�ĳ������
int myrand()
{
	int result = 0;
	result += rand() + rand();
	result <<= 16;
	result += rand() + rand();
	result &= 0x7fffffff;
	return result;
}

//	��������ȼ�
int lvl(char opt)
{
	switch (opt)
	{
	case 'd': case 'k':
		return 4;
	case '^':
		return 3;
	case '*': case '/':
		return 2;
	case '+': case '-':
		return 1;
	case '(':
		return 0;
	}
	throw (IllegalExpression_exception());
}

class MyCalc
{
	double result;

	std::string infix;
	std::string postfix;

	void Convert()
	{
		//��ʼ�������������ջ
		postfix = "";
		std::stack<char> optStack;

		/*	��־����ʶ��һ�ζ�ȡ���ַ���
		true	����
		false	�����
		��ʼֵ��Ϊ true ����Ϊ�����ı��ʽ�������ֻ��� d �����ӣ���ͷ��*/
		bool last_char_is_num = true;

		char opt;

		//	����ÿ���ַ�����ʼ������׺���ʽ����׺���ʽ��ת��������㷨�����в�ѯ���˽⣩
		for (std::string::iterator it = infix.begin(); it != infix.end(); it++)
		{
			//	���ڸ�ʽ�Ѿ�����ͳһ�����еĿո��ǽ�Ϊ���۶��ӵģ����������ո�ֱ������
			if (*it == ' ') continue;
			//	��������
			else if (ISNUM(*it))
			{
				//	�����һ���ַ����������˵�����ڶ�����һ���µ������������Լӿո���ǰ���������ֿ�
				if (!last_char_is_num) postfix += ' ';
				//	������ֱ��д���׺���ʽ
				postfix += *it;
				last_char_is_num = true;
			}
			//	���������ţ�ֱ��ѹջ
			else if (*it == '(') optStack.push(*it);
			//	����������
			else if (*it == ')')
			{
				//	��ջ��д���׺���ʽ��ֱ������һ��������
				for (; !optStack.empty() && optStack.top() != '('; postfix += ' ', postfix += optStack.top(), optStack.pop());
				if (optStack.empty()) throw (BucketNoMatch_exception());
				//	����������
				optStack.pop();
			}
			//	���������
			else
			{
				//	n�Ǹ�����־
				if (*it == '-' && ISNUM(*(it + 1))) opt = 'n';
				else opt = *it;
				//	����������kҪ��ջ����ǰջ�����������������Ϊd������˵��k����������
				if (*it == 'k' && (optStack.empty() || optStack.top() != 'd')) throw (KNotFollowD_exception());
				//	���ջ���������С�ڵ�ǰ���������ջ��ֱ����ջ���ߵ�ǰ��������ȼ�����ջ�������
				for (int i = lvl(opt); !optStack.empty() && lvl(optStack.top()) >= i; postfix += ' ', postfix += optStack.top(), optStack.pop());
				//	��ǰ�����ѹջ
				optStack.push(opt);
				last_char_is_num = false;
			}
		}
		//	��ջ
		for (; !optStack.empty(); optStack.pop())
		{
			if (optStack.top() == '(') throw (BucketNoMatch_exception());
			postfix += ' ', postfix += optStack.top();
		}

		Run();
	}
	void Run()
	{
		//	��ʼ��������������ջ
		result = 0;
		std::stack<double> numStack;
		//	��̬��������ջ����Ϊ stack ��û����ջ����������ͨ�� delete �� new ������ʵ����ջ
		std::stack<int>* diceStack = nullptr;
		int sum = 0;

		double temp = 0;
		int dot = -1;

		double n1 = 0;
		double n2 = 0;

		for (std::string::iterator it = postfix.begin(); it != postfix.end(); it++)
		{
			if (*it == ' ') continue;
			//	��������
			else if (ISNUM(*it))
			{
				//	��ʼ��
				temp = 0;
				dot = -1;
				//	ֱ�ӽ��������ֵ�ֵȡ�����������������������һλ���ִ���������ѭ����������
				for (; *it != ' ' && it != postfix.end(); it++)
				{
					//	����Ѿ���С�����֣�С��λ��+1
					if (dot != -1) dot++;
					if (*it == '.')
					{
						if (dot != -1) throw (TooManyDots_exception());
						//	�����С���㣬������С��λ���Ὺʼ����
						else dot = 0;
					}
					else temp *= 10, temp += *it - '0';
				}
				it--;
				//	�����С��λ����С�����ƶ�����ȷ��λ��
				for (; dot > 0; dot--) temp /= 10;
				//	������ѹջ
				numStack.push(temp);
			}
			//	���������
			else
			{
				//	��ջ��ȡ������
				if (numStack.empty()) throw (TooManyOperators_exception());
				n2 = numStack.top();
				numStack.pop();
				if (*it != 'n')
				{
					if (numStack.empty()) throw (TooManyOperators_exception());
					n1 = numStack.top();
					numStack.pop();
				}
				//	��������ѹջ
				switch (*it)
				{
				case '+':
					numStack.push(n1 + n2);
					continue;
				case '-':
					numStack.push(n1 - n2);
					continue;
				case '*':
					numStack.push(n1 * n2);
					continue;
				case '/':
					if (n2 == 0) throw (Devn2Eq0_exception());
					numStack.push(n1 / n2);
					continue;
				case '^':
					numStack.push(pow(n1, n2));
					continue;
				case 'n':
					numStack.push(0 - n2);
					continue;
				case 'd':
					if (n1 != round(n1)) throw (Dn1NotInt_exception());
					if (n1 < 0) throw (Dn1LessThan0_exception());
					if (n2 != round(n2)) throw (Dn2NotInt_exception());
					if (n2 < 1) throw (Dn2LessThan1_exception());
					if (n2 > MYRAND_MAX) throw (Dn2TooLarge_exception());
					//	������һ������ԭֵ��������ܻ��� k �����
					if (diceStack != nullptr) delete diceStack;
					diceStack = new std::stack<int>;
					for (int i = 0; i != n1; i++)
					{
						diceStack->push(myrand() % (int)n2 + 1);
						sum += diceStack->top();
					}
					numStack.push(sum);
					sum = 0;
					break;
				case 'k':
					if (n2 != round(n2)) throw (Kn2NotInt_exception());
					if (n2 < 0) throw (Kn2LessThan0_exception());
					int dicec = diceStack->size();
					if (n2 > dicec) throw (Kn2GreatThanDn1_exception());
					int* dices = new int[dicec];

					//	��ʱ��ջ���򣨴Ӵ�С��,Ȼ��������ջ
					for (int i = 0; i != dicec; i++)
					{
						dices[i] = diceStack->top();
						diceStack->pop();
					}
					for (int i = 0; i != dicec - 1; i++) for (int j = 0; j != dicec - 1 - i; j++) if (dices[j] < dices[j + 1]) std::swap(dices[j], dices[j + 1]);
					for (int i = 0; i != dicec; i++) diceStack->push(dices[i]);
					delete[] dices;

					//	������������ȥ���ɽ�Сֵ
					for (int i = 0; i != dicec - n2; i++)
					{
						n1 -= diceStack->top();
						diceStack->pop();
					}
					numStack.push(n1);
					break;
				}
			}
		}
		result = numStack.top();
		if (diceStack != nullptr) delete diceStack;
	}

public:
	MyCalc(std::string exp)
	{
		Set(exp);
	}
	void Set(std::string exp)
	{
		infix = "";

		//	ͳһ��ʽ���� std::string exp �е����пո����
		for (std::string::iterator it = exp.begin(); it != exp.end(); it++)
		{
			if (*it == ' ')
			{
				exp.erase(it);
				it--;
			}
			//	���д�д��ĸ��ΪСд
			else if (*it >= 'A' && *it <= 'Z') *it += 'a' - 'A';
		}

		/*	��־����ʶ��һ�ζ�ȡ���ַ���
		true	����
		false	�����
		��ʼֵ��Ϊ false ����Ϊ�����ı��ʽ�������ֻ��� d �����ӣ���ͷ��*/
		bool last_char_is_num = false;

		//	����ÿ���ַ���ͳһ��ʽ
		for (std::string::iterator it = exp.begin(); it != exp.end(); it++)
		{
			//	��������
			if (ISNUM(*it))
			{
				if (*it == '.')
				{
					//	���С����ǰ��û�����֣���0
					if (!last_char_is_num) infix += '0';
					//	���С�������û�����֣�ɾȥС����
					if (it + 1 == exp.end() || !ISNUM(*(it + 1))) continue;
				}
				infix += *it;
				last_char_is_num = true;
			}
			//	��������
			else if (*it == '(' || *it == ')')
			{
				infix += *it;
				//	����ֻ�ı�����˳�򣬲�Ӱ���ʽ�����Բ����־
			}
			//	�����������һ�������������
			else
			{
				//	��ȫ������dǰʡ�Ե�1
				if (*it == 'd' && !last_char_is_num) infix += '1';
				if (*it == '-' && (it == exp.begin() || *(it - 1) == '(')) infix += *it;
				//	�������ǰ�����һ���ո����ʹ���ʽ�����ۣ�����Щ���������
				else if (*it == 'd' || *it == 'k' || *it == '^') infix += *it;
				else
				{
					infix += ' ';
					infix += *it;
					infix += ' ';
				}
				last_char_is_num = false;
			}
		}

		Convert();
	}
	std::string Expression()
	{
		return infix;
	}
	std::string PostfixExpression()
	{
		return postfix;
	}
	double Result()
	{
		return result;
	}
	void refresh()
	{
		Run();
	}
};

//#endif