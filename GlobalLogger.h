#include <QPlainTextEdit>
#include <QScrollBar>
#include <QDateTime>

//------------------------------------------------------------------------------
// GlobalLogger (singleton)
//------------------------------------------------------------------------------
class GlobalLogger : protected QPlainTextEdit
{
public:

    static void AppendMessage(const QString& text)
    {
        qint64 qiTimestamp = QDateTime::currentMSecsSinceEpoch();
        QDateTime dt;
        dt.setTime_t(qiTimestamp / 1000);
        auto timestamp = dt.toString("yyyy-MM-dd hh:mm:ss");

        Instance()->appendPlainText(QString("[%1] %2").arg(timestamp).arg(text)); // Adds the message to the widget
        Instance()->verticalScrollBar()->setValue(Instance()->verticalScrollBar()->maximum()); // Scrolls to the bottom
    }

    static QWidget* Get()
    {
        return static_cast<QWidget*>(Instance());
    }

private:

    GlobalLogger()
    {
        this->setReadOnly(true);
    }

    ~GlobalLogger() = default;

    static GlobalLogger* Instance()
    {
        static GlobalLogger s_instance;
        return &s_instance;
    }
};