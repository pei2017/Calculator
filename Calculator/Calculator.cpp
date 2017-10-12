#include "Calculator.h"
#include <QGridLayout> 

const QMap<QString, int> Calculator::Operators =
{

	{ "*", 2 }, { "/", 2 },
	{ "+", 3 }, { "-", 3 },
	{ "(", 9 }
};

Calculator::Calculator(QWidget *parent)
	: QMainWindow(parent),
	cur_("0"),
	curIsDefalut_(true),
	result_("0")
{
	ui.setupUi(this);

	QWidget *widget = new QWidget;
	this->setCentralWidget(widget);

	inputLine_ = new QLineEdit;
	//inputLine_->setText("0");
	inputLine_->setFixedHeight(50);
	inputLine_->setReadOnly(true);

	curLine_ = new QLineEdit;
	curLine_->setText("0");
	curLine_->setFixedHeight(50);
	curLine_->setReadOnly(true);

	const char *numButtonText[] = { "0", "1", "2", "3", "4", "5", "6", "7", "8", "9",  "." };
	const char *operButtonsText[] = { "+", "-", "*", "/", "(", ")" };
	for (int i = 0; i < 11; i++)
	{
		numButtons_.push_back(new QPushButton(numButtonText[i]));
		allButtons_.push_back(numButtons_[i]);
		//numButtons_[i]->setFixedSize(30, 30);
	}
	for (int i = 0; i < 6; i++)
	{
		operButtons_.push_back(new QPushButton(operButtonsText[i]));
		allButtons_.push_back(operButtons_[i]);
	}
	calButton_ = new QPushButton("=");
	backButton_ = new QPushButton("<-");
	resetButton_ = new QPushButton("C");
	allButtons_.push_back(calButton_);
	allButtons_.push_back(backButton_);
	allButtons_.push_back(resetButton_);
	for (auto x : allButtons_)
	{
		connect(x, &QPushButton::clicked, this, &Calculator::onClick);
	}

	int line = 0;
	QGridLayout *layout = new QGridLayout(this);

	layout->addWidget(inputLine_, line, 0, 2, 4); line += 2;
	layout->addWidget(curLine_, line, 0, 2, 4); line += 2;

	layout->addWidget(resetButton_, line, 0);
	layout->addWidget(operButtons_[3], line, 1);
	layout->addWidget(operButtons_[2], line, 2);
	layout->addWidget(backButton_, line, 3); line += 1;

	layout->addWidget(numButtons_[7], line, 0);
	layout->addWidget(numButtons_[8], line, 1);
	layout->addWidget(numButtons_[9], line, 2);
	layout->addWidget(operButtons_[1], line, 3); line += 1;

	layout->addWidget(numButtons_[4], line, 0);
	layout->addWidget(numButtons_[5], line, 1);
	layout->addWidget(numButtons_[6], line, 2);
	layout->addWidget(operButtons_[0], line, 3); line += 1;

	layout->addWidget(numButtons_[1], line, 0);
	layout->addWidget(numButtons_[2], line, 1);
	layout->addWidget(numButtons_[3], line, 2);
	layout->addWidget(calButton_, line, 3, 2, 1); line += 1;

	layout->addWidget(numButtons_[0], line, 0, 1, 2);
	layout->addWidget(numButtons_[10], line, 2);

	widget->setLayout(layout);
	widget->setFixedSize(widget->sizeHint());
	this->setFixedSize(this->sizeHint());
}


/*
1 + 2 is ok, result = 3
. 8 + .9 is ok, result = 1.7
1 + - 2 is ok, result = -1
1+ 2 + 3 is ok, result = 3 and 6
1 + 2 = + 3 is ok, result = 3 and 6
*/

void Calculator::onClick()
{
	QPushButton* button = dynamic_cast<QPushButton*>(sender());
	if (button != NULL)
	{
		QString str = button->text();
		if (str == "<-")//back
		{
			if (curIsDefalut_)
			{
				curLine_->setText(cur_);
			}
			if (cur_.size() == 1)
			{
				curIsDefalut_ = true;
				cur_ = "0";
			}
			else
				cur_.remove(cur_.size() - 1, 1);
			curLine_->setText(cur_);
		}
		else if (str == "C")
		{
			reset();
		}
		else if (str == "=")
		{
			input_.push_back(cur_);
			inputLine_->setText("");
			calAndDis();
		}
		else if (isOper(str))
		{
			if (!result_.isEmpty())//cal with last result
			{
				cur_ = result_;
				onFirstOper(str);
			}
			else if (input_.empty())//+1 is ok
			{
				input_.push_back(cur_);//num
				onFirstOper(str);
			}
			else if (curIsDefalut_ && isOper(input_.back()))//1+ change to 1- when push button "-"
			{
				onSecondOper(str);
			}
			else//1 change to 1- when push button "-"
			{
				input_.push_back(cur_);//num
				onFirstOper(str);
			}
		}
		else //number
		{
			if (!result_.isEmpty())//when input number,  set result_ empty
				result_ = "";
			if (curIsDefalut_ && str != ".")
			{
				curIsDefalut_ = false;
				cur_ = str;
				curLine_->setText(cur_);
			}
			else//.9+.8 is ok
			{
				curIsDefalut_ = false;
				cur_ += str;
				curLine_->setText(cur_);
			}
		}
	}
}


QString Calculator::cal()
{
	std::vector<QString> suffix = Suffix(input_);
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

void Calculator::calAndDis()//result displays in curLine_
{
	if (result_.isEmpty())
	{
		result_ = cal();
		input_.clear();
		input_.push_back(result_);
	}
	curLine_->setText(result_);	
	curIsDefalut_ = true;
	cur_ = "0";
}

void Calculator::onFirstOper(const QString & oper)
{
	inputLine_->setText(inputLine_->text() + cur_ + oper);
	calAndDis();
	result_ = "";
	input_.push_back(oper);//oper
}

void Calculator::onSecondOper(const QString & oper)//1 + - change to 1 -
{
	changeLast(inputLine_, oper);
	input_.pop_back();
	input_.push_back(oper);
}

void Calculator::removeLast(QLineEdit * edit)//for curLine_
{
	if (cur_.size() == 1)
	{
		curIsDefalut_ = true;
		cur_ = "0";
	}
	else
		cur_.remove(cur_.size() - 1, 1);
	curLine_->setText(cur_);
}

void Calculator::changeLast(QLineEdit * edit, const QString & str)//for inputLine_
{
	QString input = edit->text();
	input.remove(input.size() - 1, 1);
	edit->setText(input + str);
}

void Calculator::reset()
{
	result_ = "0";
	input_.clear();
	inputLine_->setText("");
	curIsDefalut_ = true;
	cur_ = "0";
	curLine_->setText(cur_);
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