#pragma once
#include <ui_HistoryView.h>

class HistoryView: public QWidget
{
    Q_OBJECT

    Ui::HistoryView ui;

    void fetchDates();

    QStringList audioList;

    QString audioDir;

public:
    HistoryView(const QString& path);
    ~HistoryView() override;

protected:
    void showEvent(QShowEvent* event) override;
public slots:
    void updateList();
};