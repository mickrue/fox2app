#include <QObject>
#include <QCoreApplication>
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <iostream>
#include <QDate>
#include <QXmlStreamWriter>

#include "cfoxscheduleparser.h"

/**
 * @brief usage
 *
 * Help text that is printed on command line in case of insifficuent parameters specified.
 * Note that the message is printed in UTF-8, so the console / terminal must support this
 * font or garbage with appear.
 */
void usage(QTextStream& out)
{
    out << QString::fromUtf8("Diese Anwendung erzeugt aus dem aus Fox-112 exportierten Dienstplan,") << endl;
    out << QString::fromUtf8("den XML Terminplan für die Android App.") << endl;
    out << endl;
    out << QString::fromUtf8("Aufruf: fox2xml Dienstbuch.csv") << endl;
    out << QString::fromUtf8("Die Ausgabe Datei heißt termine-ffw.xml und kann einfach auf den Web Server") << endl;
    out << QString::fromUtf8("geladen werden.") << endl;
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QTextStream out(stdout);

    if (a.arguments().size() < 2) {
        usage(out);
        return -1;
    }

    // Try to open the file
    QFile fox(a.arguments().at(1));
    if (!fox.open(QIODevice::ReadOnly)) {
        out << QString::fromUtf8("Kann angegebene Fox CSV Datei nicht öffnen: %1").arg(a.arguments().at(1)) << endl;
        usage(out);
        return -1;
    }

    // Lets open the output file
    QFile ffwapp("termine-ffw.xml");
    if (!ffwapp.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        out << QString::fromUtf8("Kann die Ausgabe Datei termine_ffw.xml nicht erzeugen.") << endl;
        usage(out);
        return -1;
    }

    // Open the out file
    QXmlStreamWriter xmlWriter(&ffwapp);
    xmlWriter.setAutoFormatting(true);
    xmlWriter.setAutoFormattingIndent(2);
    xmlWriter.writeStartDocument();
    xmlWriter.writeStartElement("items");
    xmlWriter.writeAttribute("version","1");

    // Iterate over the file contents, line by line
    QTextStream in(&fox);
    CFoxScheduleParser foxParser(in);
    if (!foxParser.isValidFoxFile()) {
        out << QString::fromUtf8("Fehler:") << endl;
        out << QString::fromUtf8("Datei hat ungültige Kopfzeile. Erwartet:") << endl;
        out << QString::fromUtf8("DATUM;ZEIT;ZEITBIS;DIENSTART;DIENSTTYP;ANZUG;ORT;AUSFÜHRENDER;PERSONENKREIS;TEXT;BEMERKUNG") << endl;
        out << QString::fromUtf8("Fehler Info:") << endl;
        out << foxParser.getLastError() << endl;
        usage(out);
        return -1;
    }

    foxParser.moveToFirstDataLine();
    while (!foxParser.atEnd()) {

        if (!foxParser.appendDataItemToXml(xmlWriter)) {
            out << QString::fromUtf8("Kann Termin nicht zur XML Datei hinzufügen!") << endl;
            out << QString::fromUtf8("Fehler Info:") << endl;
            out << foxParser.getLastError() << endl;
            break;
        }
    }

    xmlWriter.writeEndElement(); // items
    xmlWriter.writeEndDocument();
    return 0;
}
