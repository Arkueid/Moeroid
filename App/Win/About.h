#pragma once
#include <ui_About.h>


class About : public QWidget
{
    Ui::About ui;

public:
	About(QWidget *parent = 0);
	~About() = default;
};