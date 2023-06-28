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
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
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

#include <AffineInvariantMCMCInputsWidget.h>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QLabel>
#include <QValidator>
#include <QJsonObject>
#include <QFileInfo>
#include <QFileDialog>
#include <QPushButton>

AffineInvariantMCMCInputsWidget::AffineInvariantMCMCInputsWidget(QWidget *parent)
: UQ_Method(parent)
{
    auto layout = new QGridLayout();
    int row = 0;

    // burn_length=0, jump=1, n_chains=None, random_state=None, nsamples=None, scale=2.0

    // create layout label and entry for # samples
    numSamples = new QLineEdit();
    numSamples->setText(tr("500"));
    intValidator = new QIntValidator;
    intValidator->setBottom(0);
    numSamples->setValidator(intValidator);
    numSamples->setToolTip("Specify the total number of samples");

    layout->addWidget(new QLabel("# samples"), row, 0);
    layout->addWidget(numSamples, row++, 1);

    // create layout label and entry for # chains
    numChains = new QLineEdit();
    numChains->setText(tr("1"));
    numChains->setValidator(intValidator);
    numChains->setToolTip("Specify the number of MCMC chains");

    layout->addWidget(new QLabel("# chains"), row, 0);
    layout->addWidget(numChains, row++, 1);

    // create layout label and entry for burn in length
    numBurnIn = new QLineEdit();
    numBurnIn->setText(tr("0"));
    numBurnIn->setValidator(intValidator);
    numBurnIn->setToolTip("Specify the number of samples in the burn-in period");

    layout->addWidget(new QLabel("# burn-in"), row, 0);
    layout->addWidget(numBurnIn, row++, 1);

    // create layout label and entry for dimensions
    dimension = new QLineEdit();
    dimension->setText(tr("1"));
    dimension->setValidator(intValidator);
    dimension->setToolTip("Specify the number of MCMC dimensions");

    layout->addWidget(new QLabel("# dimensions"), row, 0);
    layout->addWidget(dimension, row++, 1);

    // create layout label and entry for jump
    jump = new QLineEdit();
    jump->setText(tr("1"));
    int1Validator = new QIntValidator;
    int1Validator->setBottom(1);
    jump->setValidator(int1Validator);
    jump->setToolTip("Thinning parameter, used to reduce correlation between samples. Setting `Jump=n` corresponds to skipping `n-1` states between accepted states of the chain. Default is 1 (no thinning).");

    layout->addWidget(new QLabel("Jump"), row, 0);
    layout->addWidget(jump, row++, 1);

    // create layout label and entry for scale
    scale = new QLineEdit();
    scale->setText(tr("2.0"));
    doubleValidator = new QDoubleValidator;
    doubleValidator->setBottom(0.0);
    scale->setValidator(doubleValidator);
    scale->setToolTip("Specify the scale parameter of the stretch algorithm");

    layout->addWidget(new QLabel("Scale"), row, 0);
    layout->addWidget(scale, row++, 1);


    // create label and entry for seed to layout
    srand(time(NULL));
    int randomNumber = rand() % 1000 + 1;
    randomSeed = new QLineEdit();
    randomSeed->setText(QString::number(randomNumber));
    randomSeed->setValidator(new QIntValidator);
    randomSeed->setToolTip("Set the seed");

    layout->addWidget(new QLabel("Seed"), row, 0);
    layout->addWidget(randomSeed, row++, 1);

    // create label and lineedit for loglikelihood script and add to layout
//    logLikelihoodScript = new QLineEdit();
//    logLikelihoodScript->setPlaceholderText("(Optional)");
//    layout->addWidget(new QLabel("Log-likelihood script"), row, 0);
//    layout->addWidget(logLikelihoodScript, row, 1, 1, 2);

//    QPushButton *chooseFile = new QPushButton("Choose");
//    connect(chooseFile, &QPushButton::clicked, this, [=](){
//        logLikelihoodScript->setText(QFileDialog::getOpenFileName(this,tr("Open File"),"C://", "All files (*.py)"));
//    });
//    layout->addWidget(chooseFile, row++, 3);

    layout->setRowStretch(row, 1);
    layout->setColumnStretch(4, 1);
    this->setLayout(layout);
}

AffineInvariantMCMCInputsWidget::~AffineInvariantMCMCInputsWidget()
{

}

bool
AffineInvariantMCMCInputsWidget::outputToJSON(QJsonObject &jsonObj){

    bool result = true;
    jsonObj["burn-in"]=numBurnIn->text().toInt();
    jsonObj["jump"]=jump->text().toInt();
    jsonObj["numChains"]=numChains->text().toInt();
    jsonObj["samples"]=numSamples->text().toInt();
    jsonObj["scale"]=scale->text().toDouble();
    jsonObj["dimension"]=dimension->text().toInt();
    jsonObj["randomState"]=randomSeed->text().toDouble();

//    QString logLike = logLikelihoodScript->text();
//    QFileInfo fileInfo(logLike);
//    jsonObj["logLikelihoodFile"]=fileInfo.fileName();
//    QString path = fileInfo.absolutePath();
//    jsonObj["logLikelihoodPath"]=path;
    return result;
}

bool
AffineInvariantMCMCInputsWidget::inputFromJSON(QJsonObject &jsonObject){

    bool result = false;
    if (jsonObject.contains("samples")) {
        int samples=jsonObject["samples"].toInt();
        numSamples->setText(QString::number(samples));
    } else {
        errorMessage(QString("ERROR: Did not find number of samples"));
        return false;
    }
    if (jsonObject.contains("numChains")) {
        int chains=jsonObject["numChains"].toInt();
        numChains->setText(QString::number(chains));
    } else {
        errorMessage(QString("ERROR: Did not find number of chains"));
        return false;
    }
    if (jsonObject.contains("burn-in")) {
        int burnIn=jsonObject["burn-in"].toInt();
        numBurnIn->setText(QString::number(burnIn));
    } else {
        errorMessage(QString("ERROR: Did not find number of burn-in samples"));
        return false;
    }
    if (jsonObject.contains("jump")) {
        int jmp=jsonObject["jump"].toInt();
        jump->setText(QString::number(jmp));
    } else {
        errorMessage(QString("ERROR: Did not find number of samples to jump"));
        return false;
    }
    if (jsonObject.contains("scale")) {
        double scl=jsonObject["scale"].toDouble();
        scale->setText(QString::number(scl));
    } else {
        errorMessage(QString("ERROR: Did not find the scale"));
        return false;
    }
    if (jsonObject.contains("randomState")) {
        double rs=jsonObject["randomState"].toDouble();
        randomSeed->setText(QString::number(rs));
    } else {
        errorMessage(QString("ERROR: Did not find random state information"));
        return false;
    }
//    if (jsonObject.contains("logLikelihoodPath") && jsonObject.contains("logLikelihoodFile")) {
//        QString file = jsonObject["logLikelihoodFile"].toString();
//        QString path = jsonObject["logLikelihoodPath"].toString();
//        if (!(file.trimmed().isEmpty() && path.trimmed().isEmpty())) {
//            logLikelihoodScript->setText(path + "/" + file);
//        }

//    }

    result = true;
    return result;
}

void
AffineInvariantMCMCInputsWidget::clear(void)
{

}


int
AffineInvariantMCMCInputsWidget::getNumberTasks()
{
  return numChains->text().toInt();
}
