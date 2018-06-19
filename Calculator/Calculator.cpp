#include "Calculator.h"
#include <QGridLayout> 
#include <assert.h>
#include <stack>

const QMap<QString, int> Calculator::Operators =
{

	{ "*", 2 }, { "/", 2 },
	{ "+", 3 }, { "-", 3 },
	{ "(", 9 }
};

Calculator::Calculator(QWidget *parent)
	: QMainWindow(parent),
	m_inputing("0"),
	m_bInputNone(true),
	m_result("0")
{
	m_ui.setupUi(this);
	connect(m_ui.pushButton_clear, &QPushButton::clicked, this, &Calculator::onButtonClearClick);
	connect(m_ui.pushButton_backspace, &QPushButton::clicked, this, &Calculator::onButtonBackSpaceClick);
	connect(m_ui.pushButton_equal, &QPushButton::clicked, this, &Calculator::onButtonEqualClick);

	connect(m_ui.pushButton_0, &QPushButton::clicked, this, &Calculator::onButtonNumberClick);
	connect(m_ui.pushButton_1, &QPushButton::clicked, this, &Calculator::onButtonNumberClick);
	connect(m_ui.pushButton_2, &QPushButton::clicked, this, &Calculator::onButtonNumberClick);
	connect(m_ui.pushButton_3, &QPushButton::clicked, this, &Calculator::onButtonNumberClick);
	connect(m_ui.pushButton_4, &QPushButton::clicked, this, &Calculator::onButtonNumberClick);
	connect(m_ui.pushButton_5, &QPushButton::clicked, this, &Calculator::onButtonNumberClick);
	connect(m_ui.pushButton_6, &QPushButton::clicked, this, &Calculator::onButtonNumberClick);
	connect(m_ui.pushButton_7, &QPushButton::clicked, this, &Calculator::onButtonNumberClick);
	connect(m_ui.pushButton_8, &QPushButton::clicked, this, &Calculator::onButtonNumberClick);
	connect(m_ui.pushButton_9, &QPushButton::clicked, this, &Calculator::onButtonNumberClick);
	connect(m_ui.pushButton_point, &QPushButton::clicked, this, &Calculator::onButtonNumberClick);

	connect(m_ui.pushButton_add, &QPushButton::clicked, this, &Calculator::onButtonOperClick);
	connect(m_ui.pushButton_minus, &QPushButton::clicked, this, &Calculator::onButtonOperClick);
	connect(m_ui.pushButton_multiply, &QPushButton::clicked, this, &Calculator::onButtonOperClick);
	connect(m_ui.pushButton_divide, &QPushButton::clicked, this, &Calculator::onButtonOperClick);
}


/*
1 + 2 is ok, result = 3
. 8 + .9 is ok, result = 1.7
1 + - 2 is ok, result = -1
1+ 2 + 3 is ok, result = 3 and 6
1 + 2 = + 3 is ok, result = 3 and 6
*/

void Calculator::onButtonClearClick()
{
	m_result = "0";
	m_inputStrings.clear();
	resetInputing();
	m_ui.lineEdit_1->setText("");
	m_ui.lineEdit_2->setText("0");
}

void Calculator::onButtonBackSpaceClick()
{
	if (m_bInputNone)//无法退格
		return;
	if (m_inputing.size() == 1)
	{
		resetInputing();
		m_ui.lineEdit_2->setText("0");
		return;
	}
	m_inputing.remove(m_inputing.size() - 1, 1);
	m_ui.lineEdit_2->setText(m_inputing);
}

void Calculator::onButtonEqualClick()
{
	m_ui.lineEdit_1->setText("");
	m_inputStrings.push_back(m_inputing);
	displayResult();
}

void Calculator::onButtonNumberClick()
{
	QPushButton* button = dynamic_cast<QPushButton*>(sender());
	QString inputNumber = button->text();

	if (inputNumber == "." && m_inputing.contains('.'))//只能一个小数点
		return;
	if (m_bInputNone && inputNumber == "0")//把000001视作1
		return;

	if (!m_result.isEmpty())
		m_result = "";
	if (m_bInputNone)
	{
		m_inputing = inputNumber;
		m_bInputNone = false;
	}
	else
	{
		m_inputing += inputNumber;
	}
	m_ui.lineEdit_2->setText(m_inputing);
}

void Calculator::onButtonOperClick()
{
	QPushButton* button = dynamic_cast<QPushButton*>(sender());
	QString inputChar = button->text();

	if (!m_result.isEmpty())//把上一次计算得到结果作为输入，继续操作。
	{
		m_inputing = m_result;
		m_inputStrings.push_back(m_inputing);//num
		onFirstOper(inputChar);
	}
	else if (m_bInputNone && isOper(m_inputStrings.back()))//1+ change to 1- when pushButtonMinus
	{
		onSecondOper(inputChar);
	}
	else
	{
		m_inputStrings.push_back(m_inputing);//num
		onFirstOper(inputChar);
	}
}

QString Calculator::calculate()
{
	std::vector<QString> suffix = Suffix(m_inputStrings);
	Node * root = SuffixTree(suffix);
	if (root)
	{
		double result = root->cal();
		delete root;
		return QString::number(result);
	}
	else
		return QString("0");
}

void Calculator::displayResult()//result displays in curLine_
{
	if (m_result.isEmpty())
	{
		m_result = calculate();
		m_inputStrings.clear();
		m_inputStrings.push_back(m_result);
	}
	m_ui.lineEdit_2->setText(m_result);	
	resetInputing();
}

void Calculator::onFirstOper(const QString & oper)
{
	m_ui.lineEdit_1->setText(m_ui.lineEdit_1->text() + m_inputing + oper);
	displayResult();
	m_result = "";
	m_inputStrings.push_back(oper);//oper
}

void Calculator::onSecondOper(const QString & oper)//1 + - change to 1 -
{
	QString input = m_ui.lineEdit_1->text();
	input.remove(input.size() - 1, 1);
	m_ui.lineEdit_1->setText(input + oper);

	m_inputStrings.pop_back();
	m_inputStrings.push_back(oper);
}

std::vector<QString> Calculator::Suffix(const std::vector<QString>& strs)//3.0 + 4.0 * 2.0  ==>   3.0 4.0 2.0 * +
{
	std::stack<QString> operations;
	std::vector<QString> ret;
	if (strs.empty())
	{
		//?
	}

	int m = strs.size();
	for (int i = 0; i < m; i++)
	{
		if (strs[i].isEmpty()) {}

		if (strs[i][0] >= '0' && strs[i][0] <= '9')
		{
			ret.push_back(strs[i]);
		}
		else
		{
			if (operations.empty() || strs[i] == "(")
			{
				operations.push(strs[i]);
			}
			else if (strs[i] == ")")
			{
				while (operations.top() != "(")
				{
					ret.push_back(operations.top());
					operations.pop();
				}
				operations.pop();
			}
			else
			{
				while (!operations.empty() && Operators[strs[i]] >= Operators[operations.top()])
				{
					ret.push_back(operations.top());
					operations.pop();
				}
				operations.push(strs[i]);
			}
		}
	}
	while (!operations.empty())
	{
		ret.push_back(operations.top());
		operations.pop();
	}
	return ret;
}

Node* Calculator::SuffixTree(const std::vector<QString>& strs)
{
	if (strs.empty())
	{
		return NULL;
	}
	int m = strs.size();
	std::stack<Node*> nums;
	for (int i = 0; i < m; i++)
	{
		bool isNumber = !isOper(strs[i]);
		if (isNumber)
		{

			Node* temp = new NumNode(strs[i].toDouble());
			nums.push(temp);
		}
		else
		{
			Node *left, *right;
			char c = strs[i][0].toLatin1();
			switch (c)
			{
			case '+':
				right = nums.top();
				nums.pop();
				left = nums.top();
				nums.pop();
				nums.push(new AddNode(left, right));
				break;
			case '-':
				right = nums.top();
				nums.pop();
				left = nums.top();
				nums.pop();
				nums.push(new SubNode(left, right));
				break;
			case '*':
				right = nums.top();
				nums.pop();
				left = nums.top();
				nums.pop();
				nums.push(new MutiNode(left, right));
				break;
			case '/':
				right = nums.top();
				nums.pop();
				left = nums.top();
				nums.pop();
				nums.push(new DivNode(left, right));
				break;
			default:
				break;
			}
		}
	}
	return nums.top();
}