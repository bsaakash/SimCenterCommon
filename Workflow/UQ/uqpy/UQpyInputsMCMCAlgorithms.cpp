#include "UQpyInputsMCMCAlgorithms.h"
#include <RandomVariablesContainer.h>
#include <DakotaResultsBayesianCalibration.h>
#include <QJsonObject>

UQpyInputsMCMCAlgorithms::UQpyInputsMCMCAlgorithms(QWidget *parent)
: UQ_Engine(parent)
{

    layout = new QVBoxLayout();
    mLayout = new QVBoxLayout();

    //
    // create layout for selection box for method type to layout
    //

    QHBoxLayout *methodLayout= new QHBoxLayout;
    QLabel *label1 = new QLabel();
    label1->setText(QString("MCMC algorithm"));
    mcmcMethod = new QComboBox();
    mcmcMethod->addItem(tr("Stretch"));
    mcmcMethod->setMaximumWidth(200);
    mcmcMethod->setMinimumWidth(200);

    methodLayout->addWidget(label1);
    methodLayout->addWidget(mcmcMethod);
    methodLayout->addStretch(1);

    mLayout->addLayout(methodLayout);

    //
    // qstacked widget to hold all widgets
    //

    theStackedWidget = new QStackedWidget();

    theStretch = new AffineInvariantMCMCInputsWidget();
    theStackedWidget->addWidget(theStretch);

    // set current widget to index 0
    theStackedWidget->setCurrentIndex(0);
    theCurrentMethod = theStretch;

    mLayout->addWidget(theStackedWidget);
    layout->addLayout(mLayout);

    this->setLayout(layout);

    connect(mcmcMethod, SIGNAL(currentTextChanged(QString)), this, SLOT(onTextChanged(QString)));

}


void UQpyInputsMCMCAlgorithms::onTextChanged(const QString &text)
{
  if (text=="Stretch") {
    theStackedWidget->setCurrentIndex(0);
    theCurrentMethod = theStretch;
  }
}


UQpyInputsMCMCAlgorithms::~UQpyInputsMCMCAlgorithms()
{

}

int
UQpyInputsMCMCAlgorithms::getMaxNumParallelTasks(void){
  return theCurrentMethod->getNumberTasks();
}

void UQpyInputsMCMCAlgorithms::clear(void)
{

}

void UQpyInputsMCMCAlgorithms::numModelsChanged(int numModels) {
    emit onNumModelsChanged(numModels);
}

bool
UQpyInputsMCMCAlgorithms::outputToJSON(QJsonObject &jsonObject)
{
    bool result = true;

    QJsonObject uq;
    uq["method"]=mcmcMethod->currentText();
    result = theCurrentMethod->outputToJSON(uq);

    jsonObject["mcmcMethodData"]=uq;

    return result;
}


bool
UQpyInputsMCMCAlgorithms::inputFromJSON(QJsonObject &jsonObject)
{
  bool result = false;
  this->clear();

  //
  // get mcmcMethodData, if not present it's an error
  //

  if (jsonObject.contains("mcmcMethodData")) {
      QJsonObject uq = jsonObject["mcmcMethodData"].toObject();
      if (uq.contains("method")) {

          QString method =uq["method"].toString();
          int index = mcmcMethod->findText(method);
          if (index == -1) {
              return false;
          }
          mcmcMethod->setCurrentIndex(index);
          result = theCurrentMethod->inputFromJSON(uq);
          if (result == false)
              return result;

      }
  }

  return result;
}

bool
UQpyInputsMCMCAlgorithms::outputAppDataToJSON(QJsonObject &jsonObject)
{
    bool result = true;

    jsonObject["Application"] = "UQpy";
    QJsonObject uq;
    uq["method"]=mcmcMethod->currentText();
    theCurrentMethod->outputToJSON(uq);
    jsonObject["ApplicationData"] = uq;

    return result;
}


bool
UQpyInputsMCMCAlgorithms::inputAppDataFromJSON(QJsonObject &jsonObject)
{
    bool result = false;
    this->clear();
    //
    // get mcmcMethodData, if not present it's an error

    if (jsonObject.contains("ApplicationData")) {
        QJsonObject uq = jsonObject["ApplicationData"].toObject();

        if (uq.contains("method")) {
          QString method = uq["method"].toString();
          int index = mcmcMethod->findText(method);

          if (index == -1) {
              errorMessage(QString("ERROR: Unknown Method ") + method);
              return false;
          }
          mcmcMethod->setCurrentIndex(index);
          return theCurrentMethod->inputFromJSON(uq);
        }

    } else {
        errorMessage("ERROR: Sampling Input Widget - no \"mcmcMethodData\" input");
        return false;
    }

    return result;
}


UQ_Results *
UQpyInputsMCMCAlgorithms::getResults(void) {
  return new DakotaResultsBayesianCalibration(RandomVariablesContainer::getInstance());
}

void
UQpyInputsMCMCAlgorithms::setRV_Defaults(void) {
  RandomVariablesContainer *theRVs = RandomVariablesContainer::getInstance();
  QString classType = "Uncertain";
  QString engineType = "UQpy";

  theRVs->setDefaults(engineType, classType, Normal);
}

QString
UQpyInputsMCMCAlgorithms::getMethodName(void){
  return QString("MCMC");
}

