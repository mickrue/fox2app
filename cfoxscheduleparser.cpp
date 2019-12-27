#include "cfoxscheduleparser.h"

/**
 * @brief CFoxScheduleParser::CFoxScheduleParser
 * @param inFile
 */
CFoxScheduleParser::CFoxScheduleParser(QTextStream& inFile)
{
    foxFile.setDevice(inFile.device());
}

/**
 * @brief CFoxScheduleParser::getLastError
 * @return
 */
QString CFoxScheduleParser::getLastError()
{
    return strLastError;
}

/**
 * @brief CFoxScheduleParser::isValidFoxFile
 * @return
 */
bool CFoxScheduleParser::isValidFoxFile()
{
    // Save the current file position
    qint64 savePos = foxFile.pos();
    // Now ensure we are at the start of the file - the position we need to be to
    // validate the file
    foxFile.seek(0);
    // Next we read a full line, and see if we can find the desired columns
    QString strHeader = foxFile.readLine();
    // The exported files shall use ';' to separate the columns
    QStringList columns = strHeader.split(';');
    // We should have at least 11 columns, more is no issue - More than 20 would be wrong as well
    if (columns.size() < 11) {
        strLastError = QString::fromUtf8("Die CSV Datei hat weniger als 11 Spalten, somit ungültiges Format.");
        foxFile.seek(savePos);
        return false;
    }
    if (columns.size() > 15) {
        strLastError = QString::fromUtf8("Die CSV Datei hat mehr als 15 Spalten, somit ungültiges Format.");
        foxFile.seek(savePos);
        return false;
    }

    columns = columns.replaceInStrings("\"","");

    // So far, so good. We will now search and determine the needed positions of the columns used for the
    // Android App
    int iColumnPos = columns.indexOf("TEXT");
    if (-1 == iColumnPos) {
        strLastError = QString::fromUtf8("Die Header Zeile der CSV Datei enthält keine Spalte: TEXT");
        foxFile.seek(savePos);
        return false;
    }
    columnPosition["TEXT"] = iColumnPos;

    iColumnPos = columns.indexOf("BEMERKUNG");
    if (-1 == iColumnPos) {
        strLastError = QString::fromUtf8("Die Header Zeile der CSV Datei enthält keine Spalte: BEMERKUNG");
        foxFile.seek(savePos);
        return false;
    }
    columnPosition["BEMERKUNG"] = iColumnPos;

    iColumnPos = columns.indexOf("PERSONENKREIS");
    if (-1 == iColumnPos) {
        strLastError = QString::fromUtf8("Die Header Zeile der CSV Datei enthält keine Spalte: PERSONENKREIS");
        foxFile.seek(savePos);
        return false;
    }
    columnPosition["PERSONENKREIS"] = iColumnPos;

    iColumnPos = columns.indexOf("DATUM");
    if (-1 == iColumnPos) {
        strLastError = QString::fromUtf8("Die Header Zeile der CSV Datei enthält keine Spalte: DATUM");
        foxFile.seek(savePos);
        return false;
    }
    columnPosition["DATUM"] = iColumnPos;

    iColumnPos = columns.indexOf("ZEIT");
    if (-1 == iColumnPos) {
        strLastError = QString::fromUtf8("Die Header Zeile der CSV Datei enthält keine Spalte: ZEIT");
        foxFile.seek(savePos);
        return false;
    }
    columnPosition["ZEIT"] = iColumnPos;

    iColumnPos = columns.indexOf("AUSFÜHRENDER");
    if (-1 == iColumnPos) {
        strLastError = QString::fromUtf8("Die Header Zeile der CSV Datei enthält keine Spalte: AUSFÜHRENDER");
        foxFile.seek(savePos);
        return false;
    }
    columnPosition["AUSFÜHRENDER"] = iColumnPos;

    iColumnPos = columns.indexOf("ANZUG");
    if (-1 == iColumnPos) {
        strLastError = QString::fromUtf8("Die Header Zeile der CSV Datei enthält keine Spalte: ANZUG");
        foxFile.seek(savePos);
        return false;
    }
    columnPosition["ANZUG"] = iColumnPos;

    foxFile.seek(savePos);
    return true;
}

/**
 * @brief CFoxScheduleParser::moveToFirstDataLine
 */
void CFoxScheduleParser::moveToFirstDataLine()
{
    foxFile.seek(0);
    foxFile.readLine(); // <-- This reads the header line and places the read position at the first data line
}

/**
 * @brief CFoxScheduleParser::atEnd
 * @return
 */
bool CFoxScheduleParser::atEnd()
{
    return foxFile.atEnd();
}

/**
 * @brief CFoxScheduleParser::formatTime
 * @param foxDate
 * @param foxTime
 * @return
 */
QString CFoxScheduleParser::formatTime(QString foxDate, QString foxTime) {
    QString result;
    // For sure we need a date
    QDate date = QDate::fromString(foxDate,"dd.MM.yyyy");
    result = date.toString("yyyy-MM-dd");
    if (foxTime.length() == 8 || foxTime.length() == 5) {
        // We have a time to append
        result.append(" ").append(foxTime.left(5));
    }

    return result;
}

/**
 * @brief CFoxScheduleParser::appendDataItemToXml
 * @param xmlWriter
 * @return
 */
bool CFoxScheduleParser::appendDataItemToXml(QXmlStreamWriter& xmlWriter)
{
    if (foxFile.atEnd()) {
        strLastError = "Ende der Datei erreicht, kann keine weiteren Elemente lesen.";
        return false;
    }

    // Read the next data line from the file
    QString line = foxFile.readLine();
    // The CSV uses ';' to separate columns.
    QStringList columns = line.split(';');

    columns = columns.replaceInStrings("\"","");

    // Start the item
    xmlWriter.writeStartElement("item");
    QString members;
    // Specify the item type ...
    if (columns.at(columnPosition["TEXT"]).contains("Funkdienst",Qt::CaseInsensitive) or
        columns.at(columnPosition["TEXT"]).contains("Funkübung",Qt::CaseInsensitive)) {
        xmlWriter.writeAttribute("type","funken");
        members = columns.at(columnPosition["BEMERKUNG"]);
    } else if (columns.at(columnPosition["TEXT"]).contains("Fahrdienst",Qt::CaseInsensitive)) {
        xmlWriter.writeAttribute("type","fahren");
        members = columns.at(columnPosition["BEMERKUNG"]);
    } else {
        xmlWriter.writeAttribute("type","normal");
        members = columns.at(columnPosition["PERSONENKREIS"]);
        if (columns.at(columnPosition["BEMERKUNG"]).length() > 0) {
            // Depending on, if the members string is empty or not, we directly write the Info with comma or not:
            if (members.length() > 0) {
                members.append(", Info: ");
            } else {
                members.append("Info: ");
            }
            // Now we add the addtional information:
            members.append(columns.at(columnPosition["BEMERKUNG"]));
        }
    }
    xmlWriter.writeTextElement("date",formatTime(columns.at(columnPosition["DATUM"]), columns.at(columnPosition["ZEIT"])));
    xmlWriter.writeTextElement("title",columns.at(columnPosition["TEXT"]));
    xmlWriter.writeTextElement("owner",columns.at(columnPosition["AUSFÜHRENDER"]).length()!=0 ? columns.at(columnPosition["AUSFÜHRENDER"]) : "--");
    xmlWriter.writeTextElement("members",members);
    xmlWriter.writeTextElement("dresscode",columns.at(columnPosition["ANZUG"]));
    // Close the item
    xmlWriter.writeEndElement();

    return true;
}
