#pragma once
#include <QObject>

class LLMTTSWorker : public QObject
{
    Q_OBJECT

signals:
    void audioReceived(QByteArray array);
    void textReceived(const QString& text);
    void textReceiveFinished();

protected:
    void emitAudioReceived(unsigned char* bytes)
    {
        emit audioReceived(reinterpret_cast<char*>(bytes));
    }

    void emitTextReceived(const QString& text)
    {
        emit textReceived(text);
    }

    void emitTextReceiveFinished()
    {
        emit textReceiveFinished();
    }

public:
    LLMTTSWorker();
    ~LLMTTSWorker() override;

public slots:
    void startWork(QString text);
};
