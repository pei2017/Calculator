#pragma once
#include <QtWidgets/QMainWindow>
#include <QLineEdit>
#include <QPushButton>

#include "ui_Calculator.h"

#include <vector>

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
	void onButtonClearClick();
	void onButtonBackSpaceClick();
	void onButtonEqualClick();
	void onButtonOperClick();
	void onButtonNumberClick();

private:
	std::vector<QString> Suffix(const std::vector<QString>& strs);
	Node* SuffixTree(const std::vector<QString>& strs);

	QString calculate();
	void displayResult();
	void onFirstOper(const QString& oper);
	void onSecondOper(const QString& oper);

	void resetInputing()
	{
		m_bInputNone = true;
		m_inputing = "0";
	}
	bool isOper(const QString& str)
	{
		return Operators.find(str) != Operators.end();
	}
	
private:
	Ui::CalculatorClass m_ui;
	
	std::vector<QString> m_inputStrings;//for calculate
	bool m_bInputNone;
	QString m_inputing;
	QString m_result;
};