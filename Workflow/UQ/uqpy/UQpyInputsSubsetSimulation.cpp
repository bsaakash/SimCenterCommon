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
// Modified: Dimitris, Aakash

#include <UQpyInputsSubsetSimulation.h>
#include <UQpyInputsMCMCAlgorithms.h>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QLabel>
#include <QValidator>
#include <QJsonObject>
#include <QButtonGroup>
#include <QCheckBox>
#include <QJsonArray>
#include <QDebug>

UQpyInputsSubsetSimulation::UQpyInputsSubsetSimulation(QWidget *parent)
: UQ_Method(parent)
{
  layout = new QGridLayout();
  int row = 0;

  // create layout label and entry for # samples
  // sampling, samples_init=None, conditional_probability=0.1, nsamples_per_subset=1000, max_level=10

  layout->addWidget(new QLabel("Conditional probability"), row, 1);
  conditionalProbLineEdit = new QLineEdit();
  conditionalProbLineEdit->setText(tr("0.1"));
  doubleValidator = new QDoubleValidator();
  doubleValidator->setBottom(0.0);
  conditionalProbLineEdit->setValidator(doubleValidator);
  layout->addWidget(conditionalProbLineEdit, row++, 2);

  QString lbl = QString("Max. ") + QString(" # levels");
  layout->addWidget(new QLabel(lbl), row, 1);
  maxLevelsLineEdit = new QLineEdit();
  maxLevelsLineEdit->setText(tr("10"));
  intValidator = new QIntValidator();
  intValidator->setBottom(0);
  maxLevelsLineEdit->setValidator(intValidator);
  layout->addWidget(maxLevelsLineEdit, row++, 2);

  layout->addWidget(new QLabel("Failure threshold"), row, 1);
  thresholdLineEdit = new QLineEdit();
  thresholdLineEdit->setValidator(new QDoubleValidator());
  layout->addWidget(thresholdLineEdit, row++, 2);

//  layout->addWidget(new QLabel("# Samples per subset"), 2, 1);
//  numSamplesLineEdit = new QLineEdit();
//  intValidator = new QIntValidator();
//  intValidator->setBottom(0);
//  numSamplesLineEdit->setValidator(intValidator);
//  layout->addWidget(numSamplesLineEdit, 2, 2);

  mcmcAlgorithmsWidget = new UQpyInputsMCMCAlgorithms();
  layout->addWidget(mcmcAlgorithmsWidget, row++, 1, 5, 2);

  layout->setColumnStretch(3,4);
  layout->setRowStretch(row,1);

  this->setLayout(layout);
}

UQpyInputsSubsetSimulation::~UQpyInputsSubsetSimulation()
{


}

bool
UQpyInputsSubsetSimulation::outputToJSON(QJsonObject &jsonObject){

    bool result = true;
    QJsonObject uq;
    uq["conditionalProbability"] = conditionalProbLineEdit->text().toDouble();
    uq["maxLevels"] = maxLevelsLineEdit->text().toInt();
    uq["failureThreshold"] = thresholdLineEdit->text().toDouble();
    mcmcAlgorithmsWidget->outputToJSON(uq);
    jsonObject["subsetSimulationData"] = uq;
    return result;
}

bool
UQpyInputsSubsetSimulation::inputFromJSON(QJsonObject &jsonObject){
    bool result = false;

    if ( (jsonObject.contains("subsetSimulationData"))) {
        QJsonObject uq = jsonObject["subsetSimulationData"].toObject();
        double condProb = uq["conditionalProbability"].toDouble();
        int maxLevel = uq["maxLevels"].toInt();
        double thresh = uq["failureThreshold"].toDouble();
        conditionalProbLineEdit->setText(QString::number(condProb));
        maxLevelsLineEdit->setText(QString::number(maxLevel));
        thresholdLineEdit->setText(QString::number(thresh));
        bool result = mcmcAlgorithmsWidget->inputFromJSON(uq);
        return result;
    }

    return result;
}

void
UQpyInputsSubsetSimulation::clear(void)
{
}

int
UQpyInputsSubsetSimulation::getNumberTasks()
{
    // TODO: get the number of tasks = number of MCMC chains
//    return mcmcAlgorithmsWidget->theCurrentMethod->numChains->text().toInt();
    return -1;
}
