#ifndef CFOXSCHEDULEPARSER_H
#define CFOXSCHEDULEPARSER_H

#include <QTextStream>
#include <QMap>
#include <QXmlStreamWriter>
#include <QDate>

class CFoxScheduleParser
{
private:
    QTextStream foxFile;
    QString strLastError;
    QMap<QString, int> columnPosition;
    //
    QString formatTime(QString foxDate, QString foxTime);

public:
    CFoxScheduleParser(QTextStream& inFile);
    //
    bool isValidFoxFile();
    QString getLastError();
    void moveToFirstDataLine();
    bool atEnd();
    bool appendDataItemToXml(QXmlStreamWriter& xmlWriter);
};

#endif // CFOXSCHEDULEPARSER_H
