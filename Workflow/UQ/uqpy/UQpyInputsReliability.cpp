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
#include "UQpyInputsReliability.h"

//#include <UQpyResultsReliability.h>
#include <DakotaResultsReliability.h>
#include <RandomVariablesContainer.h>


#include <QPushButton>
#include <QScrollArea>
#include <QJsonArray>
#include <QJsonObject>
#include <QLabel>
#include <QLineEdit>
#include <QDebug>
#include <QFileDialog>
#include <QPushButton>
#include <sectiontitle.h>

#include <QStackedWidget>
#include <UQpyInputsSubsetSimulation.h>


UQpyInputsReliability::UQpyInputsReliability(QWidget *parent)
: UQ_Engine(parent)
{
    layout = new QVBoxLayout();
    mLayout = new QVBoxLayout();

    //
    // create layout for selection box for method type to layout
    //

    QHBoxLayout *methodLayout= new QHBoxLayout;
    QLabel *label1 = new QLabel();
    label1->setText(QString("Reliability Method"));
    reliabilityMethod = new QComboBox();
    reliabilityMethod->setMaximumWidth(200);
    reliabilityMethod->setMinimumWidth(200);
    reliabilityMethod->addItem(tr("Subset Simulation"));

    methodLayout->addWidget(label1);
    methodLayout->addWidget(reliabilityMethod,2);
    methodLayout->addStretch(4);

    mLayout->addLayout(methodLayout);

    //
    // qstacked widget to hold all widgets
    //

    theStackedWidget = new QStackedWidget();

    theSubsetSim = new UQpyInputsSubsetSimulation();
    theStackedWidget->addWidget(theSubsetSim);

    // set current widget to index 0
    theCurrentMethod = theSubsetSim;

    mLayout->addWidget(theStackedWidget);
    layout->addLayout(mLayout);
    layout->addStretch();

    // finally add the EDP layout & set widget layout

    this->setLayout(layout);

    connect(reliabilityMethod, SIGNAL(currentTextChanged(QString)), this, SLOT(onTextChanged(QString)));

}

void UQpyInputsReliability::onTextChanged(QString text)
{
  if (text=="Subset Simulation") {
    theStackedWidget->setCurrentIndex(0);
    theCurrentMethod = theSubsetSim;
  }
}

UQpyInputsReliability::~UQpyInputsReliability()
{

}

int
UQpyInputsReliability::getMaxNumParallelTasks(void){
  return theCurrentMethod->getNumberTasks();
}

void UQpyInputsReliability::clear(void)
{

}

void UQpyInputsReliability::numModelsChanged(int numModels) {
    emit onNumModelsChanged(numModels);
}



bool
UQpyInputsReliability::outputToJSON(QJsonObject &jsonObject)
{
    bool result = true;

    QJsonObject uq;
    uq["method"]=reliabilityMethod->currentText();
    theCurrentMethod->outputToJSON(uq);

    jsonObject["reliabilityMethodData"]=uq;

    return result;
}


bool
UQpyInputsReliability::inputFromJSON(QJsonObject &jsonObject)
{
  bool result = false;
  this->clear();

  //
  // get reliabilityMethodData, if not present it's an error

  if (jsonObject.contains("reliabilityMethodData")) {
      QJsonObject uq = jsonObject["reliabilityMethodData"].toObject();
      if (uq.contains("method")) {
          QString method =uq["method"].toString();
          int index = reliabilityMethod->findText(method);
          if (index == -1) {
              return false;
          }
          reliabilityMethod->setCurrentIndex(index);
          result = theCurrentMethod->inputFromJSON(uq);
          if (result == false)
              return result;
      }
  }

  return result;
}

bool
UQpyInputsReliability::outputAppDataToJSON(QJsonObject &jsonObject)
{
    bool result = true;

    jsonObject["Application"] = "UQpy";
    QJsonObject uq;
    uq["method"]=reliabilityMethod->currentText();
    theCurrentMethod->outputToJSON(uq);
    jsonObject["ApplicationData"] = uq;

    return result;
}


bool
UQpyInputsReliability::inputAppDataFromJSON(QJsonObject &jsonObject)
{
    bool result = false;
    this->clear();
    //
    // get reliabilityMethodData, if not present it's an error

    if (jsonObject.contains("ApplicationData")) {
        QJsonObject uq = jsonObject["ApplicationData"].toObject();

        if (uq.contains("method")) {
          QString method = uq["method"].toString();
          int index = reliabilityMethod->findText(method);

          if (index == -1) {
              errorMessage(QString("ERROR: Unknown Method ") + method);
              return false;
          }
          reliabilityMethod->setCurrentIndex(index);
          return theCurrentMethod->inputFromJSON(uq);
        }

    } else {
        errorMessage(QString("ERROR: Reliability Widget - no \"reliabilityMethodData\" input"));
        return false;
    }

    return result;
}

UQ_Results *
UQpyInputsReliability::getResults(void) {

    return new DakotaResultsReliability(RandomVariablesContainer::getInstance());
//  return new UQpyResultsReliability(RandomVariablesContainer::getInstance());
}

void
UQpyInputsReliability::setRV_Defaults(void) {
  RandomVariablesContainer *theRVs = RandomVariablesContainer::getInstance();
  QString classType = "Uncertain";
  QString engineType = "UQpy";

  theRVs->setDefaults(engineType, classType, Normal);
}


QString
UQpyInputsReliability::getMethodName(void){
  return QString("reliability");
}
