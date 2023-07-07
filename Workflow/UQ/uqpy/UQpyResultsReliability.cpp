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

//#include "UQpyResultsReliability.h"

//UQpyResultsReliability::UQpyResultsReliability()
//{

//}




#include "UQpyResultsReliability.h"
#include <QDebug>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>


#include <QLineEdit>

#include <QGridLayout>
#include <QLabel>

#include <RandomVariablesContainer.h>

#include <iostream>

UQpyResultsReliability::UQpyResultsReliability(RandomVariablesContainer *theRandomVariables, QWidget *parent)
  : UQ_Results(parent), theRVs(theRandomVariables)
{
//    layout = new QGridLayout();

//    this->setLayout(layout);


//    // clear current
//  chart = new QChart();
//  QChartView *chartView = new QChartView(chart);
//  chart->setAnimationOptions(QChart::AllAnimations);
//  chartView->setRenderHint(QPainter::Antialiasing);
//  chartView->chart()->legend()->hide();

//  //layout = new QVBoxLayout();
//  spreadsheet = new MyTableWidget();
//  QScrollArea *sa = new QScrollArea;
//  sa->setWidgetResizable(true);
//  sa->setLineWidth(0);
//  sa->setFrameShape(QFrame::NoFrame);
//  sa->setWidget(spreadsheet);
//  layout->addWidget(chartView);
//  layout->addWidget(sa);

//  mLeft = true;
//  col1 = 0;
//  col2 = 0;

//  connect(spreadsheet,SIGNAL(cellPressed(int,int)),this,SLOT(onSpreadsheetCellClicked(int,int)));
}

UQpyResultsReliability::~UQpyResultsReliability()
{

}


void UQpyResultsReliability::clear(void)
{
//  spreadsheet->clear();
//  theHeadings.clear();
//  numSpreadsheetCols = 0;
//  numSpreadsheetRows = 0;

//  mLeft = true;
//  col1 = 0;
//  col2 = 0;

//  clearLayout(layout);

}

int UQpyResultsReliability::processResults(QString &dirName)
{
  QString templateDir = tr("templatedir");
  QString errorLogFilePath = dirName + QDir::separator() + templateDir + QDir::separator() + tr("uqpy.log");
  QString resultsFilePath = dirName + QDir::separator() + templateDir + QDir::separator() + tr("uqpy_results.json");
  qDebug() << "UQpy log file path: " << errorLogFilePath << " UQpy results file path: " << resultsFilePath;

  return this->processResults(errorLogFilePath, resultsFilePath);
}


int UQpyResultsReliability::processResults(QString &errorLogFilePath, QString &resultsFilePath)
{
  statusMessage(tr("Processing UQpy Reliability Results"));

  //
  // check it actually ran with no errors
  //
  // Check if the file exists
  QFile errorLogFile(errorLogFilePath);
  if (!errorLogFile.exists()) {
      qDebug() << "UQpy error log file " << errorLogFilePath << " does not exist";
      errorMessage("UQpy error log file " + errorLogFilePath + " does not exist - UQpy did not run - problem with setup or the applications failed with inputs provided");
      return -1;
  }

  // Open the file for reading
  if (!errorLogFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
      qDebug() << "Failed to open the file: " << errorLogFile.errorString();
      errorMessage("Failed to open the file: " + errorLogFile.errorString());
      return -1;
  }
  // Display error messages in quoFEM if the error log file is not empty
  if (errorLogFile.size() == 0) {
      qDebug() << "No error messages in the error log file";
      errorLogFile.close();
  } else {
      QTextStream errorLog(&errorLogFile);
      errorMessage("Error Running UQpy: ");
      while (!errorLog.atEnd()) {
          QString line = errorLog.readLine();
          errorMessage(line);
      }
      errorLogFile.close();
  }

  //
  // read data from resultsFile
  //

  // Check if the file exists
  QFile resultsFile(resultsFilePath);
  if (!resultsFile.exists()) {
      qDebug() << "UQpy results file " << resultsFilePath << " does not exist";
      errorMessage("UQpy results file " + resultsFilePath + " does not exist - UQpy analysis did not finish successfully");
      return -1;
  }

  // Check if file can be opened
  if (!resultsFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
      qDebug() << "Failed to open the file: " << resultsFile.errorString();
      errorMessage("Failed to open the file: " + resultsFile.errorString());
      return -1;
  }
  // Read the contents of the results json file
  QString jsonContent = resultsFile.readAll();
  resultsFile.close();
  // Parse the JSON content
  QJsonDocument resultsJsonDoc = QJsonDocument::fromJson(jsonContent.toUtf8());
  if (!resultsJsonDoc.isObject()) {
      qDebug() << "Failed to parse the JSON document";
      errorMessage("Failed to parse the contents of the results JSON file");
      return -1;
  }
  // Get the root object
  QJsonObject rootObject = resultsJsonDoc.object();
  // Get the failure probability
  if (!rootObject.contains("failure_probability")) {
      qDebug() << "The results JSON file coes not contain the key 'failure_probability'";
      errorMessage("The key 'failure_probability' not found in the results JSON file");
      return -1;
  }
  // Obtain the failure probability value
  QJsonValue failureProb = rootObject.value("failure_probability");
  if (!failureProb.isDouble()) {
      qDebug() << "The value of 'failure_probability' is not a valid float";
      errorMessage("The value of 'failure_probability' is not a valid float");
      return -1;
  }
  // Add to the layout
  layout->setContentsMargins(20,10,20,10);
  QGridLayout *gridLayout = new QGridLayout();
  layout->addLayout(gridLayout);

  QLabel *label = new QLabel();
  label->setText("Probability of failure: ");
  QLineEdit *failureProbLineEdit = new QLineEdit();
  failureProbLineEdit->setText(QString::number(failureProb.toDouble()));
  failureProbLineEdit->setEnabled(false);

  gridLayout->addWidget(label, 0, 0);
  gridLayout->addWidget(failureProbLineEdit, 0, 1);

//  errorMessage((QString("")));

  return 0;
}


bool
UQpyResultsReliability::outputToJSON(QJsonObject &jsonObject)
{
    bool result = true;

    jsonObject["resultType"]=QString(tr("UQpyResultsReliability"));

    return result;
}



// if you already have a json data file then you can populate the UI with the entries from json.

bool
UQpyResultsReliability::inputFromJSON(QJsonObject &jsonObject)
{
    bool result = true;


    this->clear();

    //
    // check any data exists
    //

    QJsonObject &theObject = jsonObject;

    QJsonValue uqValue;
    if (jsonObject.contains("uqResults")) {
        uqValue = jsonObject["uqResults"];
        jsonObject = uqValue.toObject();
    } else
        theObject = jsonObject;

    return result;
}


