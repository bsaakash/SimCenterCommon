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
    mcmcMethod->addItem(tr("Modified Metropolis Hastings"));
    mcmcMethod->setMaximumWidth(300);
    mcmcMethod->setMinimumWidth(300);

    methodLayout->addWidget(label1);
    methodLayout->addWidget(mcmcMethod);
    methodLayout->addStretch(1);

    mLayout->addLayout(methodLayout);

    //
    // qstacked widget to hold all widgets
    //

    theStackedWidget = new QStackedWidget();

    theStretch = new AffineInvariantMCMCInputsWidget();
    theMmh =  new ModifiedMetropolisHastingsWidget();

    theStackedWidget->addWidget(theStretch);
    theStackedWidget->addWidget(theMmh);

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
  } else if (text=="Modified Metropolis Hastings"){
    theStackedWidget->setCurrentIndex(1);
    theCurrentMethod = theMmh;
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

    jsonObject["method"]=mcmcMethod->currentText();
    result = theCurrentMethod->outputToJSON(jsonObject);

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
  if (jsonObject.contains("method")) {

      QString method =jsonObject["method"].toString();
      int index = mcmcMethod->findText(method);
      if (index == -1) {
          return false;
      }
      mcmcMethod->setCurrentIndex(index);
      result = theCurrentMethod->inputFromJSON(jsonObject);
      if (result == false)
          return result;

  }

  return result;
}

bool
UQpyInputsMCMCAlgorithms::outputAppDataToJSON(QJsonObject &jsonObject)
{
    Q_UNUSED(jsonObject);
    return true;
}


bool
UQpyInputsMCMCAlgorithms::inputAppDataFromJSON(QJsonObject &jsonObject)
{
    Q_UNUSED(jsonObject);
    return true;
}


UQ_Results *
UQpyInputsMCMCAlgorithms::getResults(void) {
    int numBurn = 0;
  return new DakotaResultsBayesianCalibration(numBurn);
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

