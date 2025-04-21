#pragma once
#include <ui_About.h>
#include <QTimer>


class About : public QWidget
{
    Ui::About ui;

	void showEvent(QShowEvent* event) override;
	void hideEvent(QHideEvent* event) override;

public slots:
	void onUpdate();

	QTimer updateTimer;
public:
	About(QWidget *parent = 0);
	~About() = default;
};