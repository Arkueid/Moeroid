#include <Win/HistoryView.h>

#include <Sqlite/sqlite3.h>

#include <Log.hpp>
#include <Sqlite/SQLite.h>
#include <Windows.h>
#include <QDate>

HistoryView::HistoryView(const QString &path)
{
    audioDir = path;

    ui.setupUi(this);

    setWindowFlags(Qt::Tool|Qt::WindowStaysOnTopHint);

    ui.messageList->setWordWrap(true);

    fetchDates();

    connect(ui.messageList, &QListWidget::itemDoubleClicked, [&](QListWidgetItem *item)
            {
        const int index = ui.messageList->indexFromItem(item).row();
        const QString& s = audioList[index];
        if (s.isEmpty())
        {
            PlaySound(nullptr, nullptr, 0);
            return;
        }
        PlaySound((audioDir + "/" + s).toStdWString().c_str(), nullptr, SND_FILENAME | SND_ASYNC); });
}

HistoryView::~HistoryView() = default;

void HistoryView::showEvent(QShowEvent *event)
{
    fetchDates();
    updateList();
}

void HistoryView::onMsgReceived()
{
    if (ui.dateSelector->currentText() != QDate::currentDate().toString("yyyy-MM-dd") || !this->isVisible())
    {
        return;
    }

    sqlite3 *db = SQLite::getInstance();
    sqlite3_stmt *stmt;
    int startIndex = ui.messageList->count();

    const std::string &date = ui.dateSelector->currentText().toStdString();
    const char *sql = "select * from messages where date(ct) = ? order by datetime(ct) asc";
    if (SQLITE_OK != sqlite3_prepare(db, sql, strlen(sql), &stmt, nullptr))
    {
        Error("failed to fetch history.");
        return;
    }
    sqlite3_bind_text(stmt, 1, date.c_str(), date.size(), nullptr);
    int index = 0;
    while (SQLITE_ROW == sqlite3_step(stmt))
    {
        if (index++ < startIndex)
            continue;

        const QString content = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 0));
        const QString role = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
        const QString audio = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2));
        ui.messageList->addItem("【" + role + "】\n  " + content + "\n");
        audioList.append(audio);
    }

    ui.messageList->setCurrentRow(ui.messageList->count() - 1);
    sqlite3_finalize(stmt);
}

void HistoryView::updateList()
{
    sqlite3 *db = SQLite::getInstance();
    sqlite3_stmt *stmt;
    ui.messageList->clear();
    audioList.clear();

    const std::string &date = ui.dateSelector->currentText().toStdString();
    const char *sql = "select * from messages where date(ct) = ? order by datetime(ct) asc";
    if (SQLITE_OK != sqlite3_prepare(db, sql, strlen(sql), &stmt, nullptr))
    {
        Error("failed to fetch history.");
        return;
    }
    sqlite3_bind_text(stmt, 1, date.c_str(), date.size(), nullptr);
    while (SQLITE_ROW == sqlite3_step(stmt))
    {
        const QString content = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 0));
        const QString role = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
        const QString audio = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2));
        ui.messageList->addItem("【" + role + "】\n  " + content + "\n");
        audioList.append(audio);
    }

    ui.messageList->setCurrentRow(ui.messageList->count() - 1);
    sqlite3_finalize(stmt);
}

#include <QDate>

void HistoryView::fetchDates()
{
    disconnect(conn);

    QStringList dates;

    sqlite3 *db = SQLite::getInstance();
    const char *sql = "select date(ct) from messages group by date(ct) order by date(ct) desc";

    sqlite3_stmt *stmt;
    int r = sqlite3_prepare(db, sql, strlen(sql), &stmt, nullptr);
    if (r != SQLITE_OK)
    {
        Error("failed to fetch dates.");
        return;
    }

    while (SQLITE_ROW == sqlite3_step(stmt))
    {
        QString d = (const char *)sqlite3_column_text(stmt, 0);
        dates.append(d);
    }
    sqlite3_finalize(stmt);

    ui.dateSelector->clear();
    ui.dateSelector->addItems(dates);

    conn = connect(ui.dateSelector, &QComboBox::currentIndexChanged, [&](int)
                   { updateList(); });
}
