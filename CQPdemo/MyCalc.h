#pragma once

//#ifndef __MYCALC_H__
//#define __MYCALC_H__

#include <string>
#include <cstdlib>
#include <stack>

#define MYRAND_MAX 0x7FFFFFFF
//	宏定义，判断字符n是数字还是运算符
#define ISNUM(n) (n >= '0' && n <= '9' || n == '.')

//	括号不匹配
class BucketNoMatch_exception : public std::exception {
public: BucketNoMatch_exception() : std::exception::exception("错误：括号不匹配。\n") {};
};
//	同一个数字中出现多个小数点
class TooManyDots_exception : public std::exception {
public: TooManyDots_exception() : std::exception::exception("错误：数字中不能出现多个小数点。\n") {};
};
//	运算符数量过多导致操作数栈空栈
class TooManyOperators_exception : public std::exception {
public: TooManyOperators_exception() : std::exception::exception("错误：使用了过多的运算符。\n") {};
};
//	运算符k单独出现
class KNotFollowD_exception : public std::exception {
public:	KNotFollowD_exception() : std::exception::exception("错误：运算符k不能单独使用，必须跟在运算符d之后。\n") {};
};
//	运算符d的前操作数不是整数
class Dn1NotInt_exception : public std::exception {
public: Dn1NotInt_exception() : std::exception::exception("错误：运算符d的前操作数必须是整数。\n") {};
};
//	运算符d的前操作数小于0
class Dn1LessThan0_exception : public std::exception {
public: Dn1LessThan0_exception() : std::exception::exception("错误：运算符d的前操作数必须大于或等于0。\n") {};
};
//	运算符d的后操作数不是整数
class Dn2NotInt_exception : public std::exception {
public: Dn2NotInt_exception() : std::exception::exception("错误：运算符d的后操作数必须是整数。\n") {};
};
//	运算符d的后操作数小于1
class Dn2LessThan1_exception : public std::exception {
public: Dn2LessThan1_exception() : std::exception::exception("错误：运算符d的后操作数必须大于或等于1。\n") {};
};
//	运算符d的后操作数太大
class Dn2TooLarge_exception : public std::exception {
public: Dn2TooLarge_exception() : std::exception::exception("错误：运算符d的后操作数必须小于或等于2147483647（2^31 - 1）。\n") {};
};
//	运算符k的后操作数不是整数
class Kn2NotInt_exception : public std::exception {
public: Kn2NotInt_exception() : std::exception::exception("错误：运算符k的后操作数必须是整数。\n") {};
};
//	运算符k的后操作数小于0
class Kn2LessThan0_exception : public std::exception {
public: Kn2LessThan0_exception() : std::exception::exception("错误：运算符k的后操作数必须大于或等于0。\n") {};
};
//	运算符k的后操作数大于d的前操作数
class Kn2GreatThanDn1_exception : public std::exception {
public: Kn2GreatThanDn1_exception() : std::exception::exception("错误：运算符k的后操作数不能大于运算符d的前操作数。\n") {};
};
//	运算符/的后操作数为0
class Devn2Eq0_exception : public std::exception {
public: Devn2Eq0_exception() : std::exception::exception("错误：运算符/的后操作数不能为0。\n") {};
};
//	表达式不合法
class IllegalExpression_exception : public std::exception {
public: IllegalExpression_exception() : std::exception::exception("错误：表达式不合法。\n") {};
};

//	int64_t格式的长随机数
int myrand()
{
	int result = 0;
	result += rand() + rand();
	result <<= 16;
	result += rand() + rand();
	result &= 0x7fffffff;
	return result;
}

//	运算符优先级
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
		//初始化，创建运算符栈
		postfix = "";
		std::stack<char> optStack;

		/*	标志，标识上一次读取的字符。
		true	数字
		false	运算符
		初始值设为 true ，因为正常的表达式会以数字或者 d （骰子）开头。*/
		bool last_char_is_num = true;

		char opt;

		//	遍历每个字符，开始进行中缀表达式到后缀表达式的转换（这个算法请自行查询并了解）
		for (std::string::iterator it = infix.begin(); it != infix.end(); it++)
		{
			//	由于格式已经经过统一，所有的空格都是仅为美观而加的，所以遇到空格直接跳过
			if (*it == ' ') continue;
			//	读到数字
			else if (ISNUM(*it))
			{
				//	如果上一个字符是运算符，说明现在读到了一个新的运算数，所以加空格与前面的运算符分开
				if (!last_char_is_num) postfix += ' ';
				//	运算数直接写入后缀表达式
				postfix += *it;
				last_char_is_num = true;
			}
			//	读到左括号，直接压栈
			else if (*it == '(') optStack.push(*it);
			//	读到右括号
			else if (*it == ')')
			{
				//	弹栈并写入后缀表达式，直到遇见一个左括号
				for (; !optStack.empty() && optStack.top() != '('; postfix += ' ', postfix += optStack.top(), optStack.pop());
				if (optStack.empty()) throw (BucketNoMatch_exception());
				//	丢弃左括号
				optStack.pop();
			}
			//	其他运算符
			else
			{
				//	n是负数标志
				if (*it == '-' && ISNUM(*(it + 1))) opt = 'n';
				else opt = *it;
				//	如果有运算符k要入栈，当前栈顶弹出的运算符必须为d，否则说明k单独出现了
				if (*it == 'k' && (optStack.empty() || optStack.top() != 'd')) throw (KNotFollowD_exception());
				//	如果栈顶运算符不小于当前运算符，弹栈，直到空栈或者当前运算符优先级高于栈顶运算符
				for (int i = lvl(opt); !optStack.empty() && lvl(optStack.top()) >= i; postfix += ' ', postfix += optStack.top(), optStack.pop());
				//	当前运算符压栈
				optStack.push(opt);
				last_char_is_num = false;
			}
		}
		//	清栈
		for (; !optStack.empty(); optStack.pop())
		{
			if (optStack.top() == '(') throw (BucketNoMatch_exception());
			postfix += ' ', postfix += optStack.top();
		}

		Run();
	}
	void Run()
	{
		//	初始化，创建运算数栈
		result = 0;
		std::stack<double> numStack;
		//	动态申请骰子栈（因为 stack 类没有清栈函数，所以通过 delete 和 new 来变相实现清栈
		std::stack<int>* diceStack = nullptr;
		int sum = 0;

		double temp = 0;
		int dot = -1;

		double n1 = 0;
		double n2 = 0;

		for (std::string::iterator it = postfix.begin(); it != postfix.end(); it++)
		{
			if (*it == ' ') continue;
			//	读到数字
			else if (ISNUM(*it))
			{
				//	初始化
				temp = 0;
				dot = -1;
				//	直接将整个数字的值取出，并将迭代器放置在最后一位数字处，由外层大循环控制自增
				for (; *it != ' ' && it != postfix.end(); it++)
				{
					//	如果已经是小数部分，小数位数+1
					if (dot != -1) dot++;
					if (*it == '.')
					{
						if (dot != -1) throw (TooManyDots_exception());
						//	如果有小数点，接下来小数位数会开始计数
						else dot = 0;
					}
					else temp *= 10, temp += *it - '0';
				}
				it--;
				//	如果有小数位，将小数点移动到正确的位置
				for (; dot > 0; dot--) temp /= 10;
				//	运算数压栈
				numStack.push(temp);
			}
			//	读到运算符
			else
			{
				//	从栈顶取运算数
				if (numStack.empty()) throw (TooManyOperators_exception());
				n2 = numStack.top();
				numStack.pop();
				if (*it != 'n')
				{
					if (numStack.empty()) throw (TooManyOperators_exception());
					n1 = numStack.top();
					numStack.pop();
				}
				//	将运算结果压栈
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
					//	保留这一次掷骰原值，后面可能会有 k 运算符
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

					//	临时出栈排序（从大到小）,然后重新入栈
					for (int i = 0; i != dicec; i++)
					{
						dices[i] = diceStack->top();
						diceStack->pop();
					}
					for (int i = 0; i != dicec - 1; i++) for (int j = 0; j != dicec - 1 - i; j++) if (dices[j] < dices[j + 1]) std::swap(dices[j], dices[j + 1]);
					for (int i = 0; i != dicec; i++) diceStack->push(dices[i]);
					delete[] dices;

					//	从掷骰结果里减去若干较小值
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

		//	统一格式，将 std::string exp 中的所有空格清除
		for (std::string::iterator it = exp.begin(); it != exp.end(); it++)
		{
			if (*it == ' ')
			{
				exp.erase(it);
				it--;
			}
			//	所有大写字母改为小写
			else if (*it >= 'A' && *it <= 'Z') *it += 'a' - 'A';
		}

		/*	标志，标识上一次读取的字符。
		true	数字
		false	运算符
		初始值设为 false ，因为正常的表达式会以数字或者 d （骰子）开头。*/
		bool last_char_is_num = false;

		//	遍历每个字符，统一格式
		for (std::string::iterator it = exp.begin(); it != exp.end(); it++)
		{
			//	读到数字
			if (ISNUM(*it))
			{
				if (*it == '.')
				{
					//	如果小数点前面没有数字，补0
					if (!last_char_is_num) infix += '0';
					//	如果小数点后面没有数字，删去小数点
					if (it + 1 == exp.end() || !ISNUM(*(it + 1))) continue;
				}
				infix += *it;
				last_char_is_num = true;
			}
			//	读到括号
			else if (*it == '(' || *it == ')')
			{
				infix += *it;
				//	括号只改变运算顺序，不影响格式，所以不设标志
			}
			//	其他情况按照一般运算符来处理
			else
			{
				//	补全操作符d前省略的1
				if (*it == 'd' && !last_char_is_num) infix += '1';
				if (*it == '-' && (it == exp.begin() || *(it - 1) == '(')) infix += *it;
				//	在运算符前后各加一个空格可以使表达式更美观，但这些运算符除外
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