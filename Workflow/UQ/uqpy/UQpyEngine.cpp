// Written: fmckenna

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

// Written: fmckenna

#include "UQpyEngine.h"
#include <QDebug>
#include <RandomVariablesContainer.h>
#include <UQ_Results.h>
#include <GoogleAnalytics.h>

#include <QStackedWidget>
#include <QComboBox>
#include <QJsonArray>
#include <QJsonObject>
#include <QVBoxLayout>
#include <QLabel>
#include <QCheckBox>
#include <QSpacerItem>

#include <QDebug>

#include <UQpyInputsReliability.h>
#include <UQpyInputsMCMCAlgorithms.h>


UQpyEngine::UQpyEngine(UQ_EngineType type, QWidget *parent)
    : UQ_Engine(parent)
{
  /*************************  at some point need to redo so no new
    QString classType("Uncertain");
    theRandomVariables =  new RandomVariablesContainer(classType);
    theResults = new UQ_Results();
  ***************************************************************/
    QVBoxLayout *layout = new QVBoxLayout();

    QHBoxLayout *theSelectionLayout = new QHBoxLayout();
    QLabel *label = new QLabel();
    label->setText(QString("UQpy Method Category"));
    theEngineSelectionBox = new QComboBox();
    theEngineSelectionBox->addItem(tr("Reliability"));
    theEngineSelectionBox->addItem(tr("MCMC Sampling"));
    theEngineSelectionBox->setMinimumWidth(600);

    theSelectionLayout->addWidget(label);
    theSelectionLayout->addWidget(theEngineSelectionBox);
    //theSelectionLayout->addStretch();
    //theSelectionLayout->addWidget(new QSpacerItem(20,5));
    parallelCheckBox = new QCheckBox("Parallel Execution  ");
    parallelCheckBox->setChecked(true);

    theSelectionLayout->addWidget(parallelCheckBox);
    theSelectionLayout->addStretch();

    layout->addLayout(theSelectionLayout);

    //
    // create the stacked widget
    //

    theStackedWidget = new QStackedWidget();

    //
    // create the individual widgets add to stacked widget
    //

    theReliabilityEngine = new UQpyInputsReliability();
    theMCMCEngine = new UQpyInputsMCMCAlgorithms();

    theStackedWidget->addWidget(theReliabilityEngine);
    theStackedWidget->addWidget(theMCMCEngine);

    layout->addWidget(theStackedWidget);
    this->setLayout(layout);
    theCurrentEngine=theReliabilityEngine;

    connect(theEngineSelectionBox, SIGNAL(currentTextChanged(QString)), this, SLOT(engineSelectionChanged(QString)));
    // connect(theSamplingEngine, SIGNAL(onNumModelsChanged(int)), this, SLOT(numModelsChanged(int)));

    theCurrentEngine = theReliabilityEngine;

}

UQpyEngine::~UQpyEngine()
{

}

void UQpyEngine::engineSelectionChanged(const QString &arg1)
{
    if ((arg1 == QString("Reliability")) || (arg1 == QString("Reliability Analysis"))) {

      theStackedWidget->setCurrentIndex(0);
      theCurrentEngine = theReliabilityEngine;

    } else if ((arg1 == QString("MCMC")) || (arg1 == QString("MCMC Sampling"))) {

        theStackedWidget->setCurrentIndex(1);
        theCurrentEngine = theMCMCEngine;

    } else {
      qDebug() << "ERROR .. UQpyEngine selection .. type unknown: " << arg1;
    }

    emit onUQ_EngineChanged();
}


int
UQpyEngine::getMaxNumParallelTasks(void) {
    return theCurrentEngine->getMaxNumParallelTasks();
}

bool
UQpyEngine::outputToJSON( QJsonObject &jsonObject) {

    QString uqMethod = theEngineSelectionBox->currentText();
    jsonObject["uqType"] = uqMethod;
    jsonObject["parallelExecution"] = parallelCheckBox->isChecked();
//    jsonObject["saveWorkDir"] = removeWorkdirCheckBox->isChecked();

    return theCurrentEngine->outputToJSON(jsonObject);
}


bool
UQpyEngine::inputFromJSON(QJsonObject &jsonObject) {
    bool result = false;

    QString uqMethod = jsonObject["uqType"].toString();

    bool doParallel = true;
    if (jsonObject.contains("parallelExecution"))
        doParallel = jsonObject["parallelExecution"].toBool();

    parallelCheckBox->setChecked(doParallel);


    int index = theEngineSelectionBox->findText(uqMethod);
    theEngineSelectionBox->setCurrentIndex(index);
    this->engineSelectionChanged(uqMethod);
    if (theCurrentEngine != 0)
        result = theCurrentEngine->inputFromJSON(jsonObject);
    else
        result = false; // don't emit error as one should have been generated

    return result;
}

bool
UQpyEngine::outputAppDataToJSON(QJsonObject &jsonObject)
{
    jsonObject["Application"] = "UQpy";
    QJsonObject dataObj;
    jsonObject["ApplicationData"] = dataObj;

    return true;
}

bool
UQpyEngine::inputAppDataFromJSON(QJsonObject &jsonObject)
{
    Q_UNUSED(jsonObject);
    return true;
}


RandomVariablesContainer *
UQpyEngine::getParameters() {
  QString classType("Uncertain");
  return RandomVariablesContainer::getInstance();
}

UQ_Results *
UQpyEngine::getResults(void) {
  return theCurrentEngine->getResults();
}


void
UQpyEngine::clear(void) {
    return;
}


void
UQpyEngine::setRV_Defaults(void) {
  RandomVariablesContainer *theRVs = RandomVariablesContainer::getInstance();
  QString classType = "Uncertain";
  QString engineType = "UQpy";

  theRVs->setDefaults(engineType, classType, Normal);
  return theCurrentEngine->setRV_Defaults();
}

QString
UQpyEngine::getProcessingScript() {
    return QString("parseUQpy.py");
}

QString
UQpyEngine::getMethodName() {
    return theCurrentEngine->getMethodName();
}

void
UQpyEngine::numModelsChanged(int newNum) {
    emit onNumModelsChanged(newNum);
}


bool
UQpyEngine::copyFiles(QString &fileDir) {
    QString googleString=QString("UQ-UQpy-") + this->getMethodName();
    GoogleAnalytics::ReportAppUsage(googleString);

    return theCurrentEngine->copyFiles(fileDir);
}

