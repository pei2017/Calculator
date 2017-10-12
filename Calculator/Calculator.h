#pragma once
#include <QtWidgets/QMainWindow>
#include <QLineEdit>
#include <QPushButton>

#include "ui_Calculator.h"

#include <vector>
#include <stack>

class Node
{
public:
	virtual double cal() const = 0;
	virtual ~Node() {}
};

class NumNode : public Node
{
private:
	double num_;
public:
	NumNode(double num) :num_(num) {}
	virtual double cal() const { return num_; }
};

class UnaryNode : public Node
{
protected:
	Node* node_;
};

class BinaryNode : public Node
{
protected:
	Node* left_;
	Node* right_;
public:
	BinaryNode(Node* l, Node* r) :left_(l), right_(r) {}
	//virtual double cal() const {}
	~BinaryNode()
	{
		delete left_;
		delete right_;
	}
};

class AddNode :public BinaryNode
{
public:
	AddNode(Node* l, Node* r) : BinaryNode(l, r) {}
	virtual double cal() const
	{
		return left_->cal() + right_->cal();
	}
};

class SubNode :public BinaryNode
{
public:
	SubNode(Node* l, Node* r) : BinaryNode(l, r) {}
	virtual double cal() const
	{
		return left_->cal() - right_->cal();
	}
};

class MutiNode :public BinaryNode
{
public:
	MutiNode(Node* l, Node* r) : BinaryNode(l, r) {}
	virtual double cal() const
	{
		return left_->cal() * right_->cal();
	}
};

class DivNode :public BinaryNode
{
public:
	DivNode(Node* l, Node* r) : BinaryNode(l, r) {}
	virtual double cal() const
	{
		return double(left_->cal()) / right_->cal();
	}
};

class Calculator : public QMainWindow
{
	Q_OBJECT
private:
	static const QMap<QString, int> Operators;
public:
	Calculator(QWidget *parent = Q_NULLPTR);

public slots:
	void onClick();

private:
	std::vector<QString> Suffix(const std::vector<QString>& strs);
	Node* SuffixTree(const std::vector<QString>& strs);

	QString cal();
	void calAndDis();

	void onFirstOper(const QString& oper);
	void onSecondOper(const QString& oper);

	void removeLast(QLineEdit* edit);//for curLine_
	void changeLast(QLineEdit* edit, const QString& str);//for inputLine_

	void reset();
	bool isOper(const QString& str)
	{
		return Operators.find(str) != Operators.end();
	}
	
private:
	Ui::CalculatorClass ui;

	QLineEdit* inputLine_;
	QLineEdit* curLine_;
	std::vector<QString> input_;//for cal()
	QString cur_;		//current input number
	bool curIsDefalut_;	//true means cur_ is 0 and there is no input_number 
	QString result_;	//can be empty which means is inputing number, or it means the last result.

	std::vector<QPushButton*> numButtons_;//0 ~ 9 .
	std::vector<QPushButton*> operButtons_;//+ - * / 
	QPushButton* calButton_;
	QPushButton* backButton_;
	QPushButton* resetButton_;
	std::vector<QPushButton*> allButtons_;
};