#ifndef UQPYRESULTSRELIABILITY_H
#define UQPYRESULTSRELIABILITY_H
/* *****************************************************************************
Copyright (c) 2016-2017, The Regents of the University of California (Regents).
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of the FreeBSD Project.

REGENTS SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
THE SOFTWARE AND ACCOMPANYING DOCUMENTATION, IF ANY, PROVIDED HEREUNDER IS
PROVIDED "AS IS". REGENTS HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT,
UPDATES, ENHANCEMENTS, OR MODIFICATIONS.

*************************************************************************** */

// Written: fmckenna, bsaakash
#include <QGridLayout>
#include <UQ_Results.h>
#include <QtCharts/QChart>
#include <QMessageBox>
#include <QPushButton>

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
using namespace QtCharts;
#endif

class QTextEdit;
class QTabWidget;
class MyTableWidget;
class MainWindow;
class RandomVariablesContainer;

//class QChart;

class UQpyResultsReliability : public UQ_Results
{
    Q_OBJECT
public:
  explicit UQpyResultsReliability(RandomVariablesContainer *, QWidget *parent = 0);
    ~UQpyResultsReliability();

    bool outputToJSON(QJsonObject &rvObject);
    bool inputFromJSON(QJsonObject &rvObject);

    int processResults(QString &dirName);

signals:

public slots:
   void clear(void);
//   void onSpreadsheetCellClicked(int, int);
//   void onSaveSpreadsheetClicked();

   // modified by padhye 08/25/2018

private:
   int processResults(QString &filenameResults, QString &filenameTab);


   RandomVariablesContainer *theRVs;

//   MyTableWidget *spreadsheet;
//   QChart *chart;
//   int numSpreadsheetRows;
//   int numSpreadsheetCols;

//   int col1, col2;
//   bool mLeft;
//   QStringList theHeadings;

//   void clearLayout(QLayout *layout);


   //   QVector<QString>theHeadings;
};

#endif // UQPYRESULTSRELIABILITY_H

