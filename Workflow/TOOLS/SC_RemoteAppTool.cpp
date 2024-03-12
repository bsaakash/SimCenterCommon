#include "SC_RemoteAppTool.h"
#include "Utils/ProgramOutputDialog.h"
#include "SimCenterAppWidget.h"
#include <RemoteService.h>
#include <SimCenterPreferences.h>
#include <ZipUtils/ZipUtils.h>
#include <RemoteJobManager.h>
#include <SC_ResultsWidget.h>
#include <Utils/RelativePathResolver.h>

#include <QDebug>
#include <QStackedWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QDir>
#include <QApplication>
#include <QCoreApplication>
#include <QLineEdit>
#include <QJsonDocument>
#include <QMessageBox>
#include <QFileDialog>


SC_RemoteAppTool::SC_RemoteAppTool(QString appName,
				   QList<QString> queus,
				   RemoteService *theRemoteService,
				   SimCenterAppWidget* theEnclosedApp,
				   QDialog *enclosingDialog,
           QString remoteSystemName)
:SimCenterAppWidget(), systemName(remoteSystemName), theApp(theEnclosedApp), theService(theRemoteService), tapisAppName(appName)
{
  QVBoxLayout *theMainLayout = new QVBoxLayout(this);
  theMainLayout->addWidget(theApp);


  QGridLayout *theButtonLayout = new QGridLayout();
  QPushButton *fileLoadButton = new QPushButton("LOAD File");
  QPushButton *fileSaveButton = new QPushButton("SAVE File");  
  QPushButton *runRemoteButton = new QPushButton("RUN at DesignSafe");
  QPushButton *getRemoteButton = new QPushButton("GET from DesignSafe");

  theButtonLayout->addWidget(fileLoadButton,0,0);
  theButtonLayout->addWidget(fileSaveButton,0,1);  
  theButtonLayout->addWidget(runRemoteButton,0,2);
  theButtonLayout->addWidget(getRemoteButton,0,3);
  
  theMainLayout->addLayout(theButtonLayout);

  // now create the Dialog for the Remote Applications

  QDialog *theRemoteDialog = new QDialog(this);
  // QString workflowScriptName = "FMK-NAME";
  QString shortDirName = QCoreApplication::applicationName() + ":";

  //shortDirName = workflowScriptName;
  //shortDirName = name.chopped(3); // remove .py
  //shortDirName.chop(3);
  
  QGridLayout *remoteLayout = new QGridLayout();
  QLabel *nameLabel = new QLabel();
  
  int numRow = 0;

  remoteLayout->addWidget(new QLabel("Job Name:"), numRow,0);
  nameLineEdit = new QLineEdit();
  nameLineEdit->setToolTip(tr("A meaningful name to provide for you to remember run later (days and weeks from now)"));
  remoteLayout->addWidget(nameLineEdit,numRow,1);
  
  numRow++;
  remoteLayout->addWidget(new QLabel("Remote DesignSafe HPC:"), numRow,0);
  systemLineEdit = new QLineEdit();
  systemLineEdit->setText(systemName);
  systemLineEdit->setToolTip(tr("Name of the remote HPC system to run the job on using DesignSafe resources. Options are frontera and lonestar6. To use GPUs, specify as frontera-gpu or lonestar6-gpu"));
  remoteLayout->addWidget(systemLineEdit,numRow,1);
  
  
  numRow++;
  QLabel *numCPU_Label = new QLabel();
  remoteLayout->addWidget(new QLabel("Num Nodes:"),numRow,0);
  
  numCPU_LineEdit = new QLineEdit();
  numCPU_LineEdit->setText("1");
  numCPU_LineEdit->setToolTip(tr("Total # of nodes to use (each node has many cores)"));
  remoteLayout->addWidget(numCPU_LineEdit,numRow,1);
  
  numRow++;
  remoteLayout->addWidget(new QLabel("# Processors Per Node"),numRow,0);
  
  int maxProcPerNode = 56; //theApp->getMaxNumProcessors(56);
  numProcessorsLineEdit = new QLineEdit();
  numProcessorsLineEdit->setText(QString::number(maxProcPerNode));
  numProcessorsLineEdit->setText(QString::number(maxProcPerNode));  
  numProcessorsLineEdit->setToolTip(tr("Total # of Processes to Start"));
  remoteLayout->addWidget(numProcessorsLineEdit,numRow,1);

  numRow++;
  QLabel *numGPU_Label = new QLabel();
  remoteLayout->addWidget(new QLabel("Num GPUs:"),numRow,0);
  
  numGPU_LineEdit = new QLineEdit();
  numGPU_LineEdit->setText("0");
  numGPU_LineEdit->setToolTip(tr("Total # of GPUs to use (across all nodes)"));
  remoteLayout->addWidget(numGPU_LineEdit,numRow,1);
  
  //  QString appName = QCoreApplication::applicationName();
  

  numRow++;
  remoteLayout->addWidget(new QLabel("Max Run Time:"),numRow,0);
  runtimeLineEdit = new QLineEdit();
  runtimeLineEdit->setText("00:20:00");
  runtimeLineEdit->setToolTip(tr("Run time Limit on running Job hours:Min:Sec. Job will be stopped if while running it exceeds this"));
  remoteLayout->addWidget(runtimeLineEdit,numRow,1);
  
  numRow++;
  submitButton = new QPushButton();
  submitButton->setText("Submit");
  submitButton->setToolTip(tr("Press to launch job on remote machine. After pressing, window closes when Job Starts"));
  remoteLayout->addWidget(submitButton,numRow,1);
  
  theRemoteDialog->setLayout(remoteLayout);
  theRemoteDialog->hide();

  
  connect(fileLoadButton, &QPushButton::clicked, this,
	  [=]() {
	    //QString fileName=QFileDialog::getOpenFileName(this,tr("Open File"),"C://", "All files (*.*)");
        QString fileName=QFileDialog::getOpenFileName(this,tr("Open JSON File"),"", "JSON file (*.json)"); // sy - to continue from the previously visited directory
	    QFile file(fileName);
	    if (!file.open(QFile::ReadOnly | QFile::Text)) {
	      emit errorMessage(QString("Could Not Open File: ") + fileName);
	      return -1;
	    }

	    //
	    // place contents of file into json object
	    //
	    
	    QString val;
	    val=file.readAll();
	    QJsonDocument doc = QJsonDocument::fromJson(val.toUtf8());
	    QJsonObject jsonObj = doc.object();

	    // close file
	    file.close();
	    
	    //Resolve absolute paths from relative ones
	    QFileInfo fileInfo(fileName);
	    SCUtils::ResolveAbsolutePaths(jsonObj, fileInfo.dir());    
    
	    //
	    // clear current and input from new JSON
	    //
	    
	    theApp->clear();	    
	    theApp->inputFromJSON(jsonObj);
	  });


  connect(fileSaveButton, &QPushButton::clicked, this,
	  [=]() {

        QString fileName=QFileDialog::getSaveFileName(this, tr("Save JSON File"),"", "JSON file (*.json)"); // sy - to continue from the previously visited directory
	    QFile file(fileName);

	    if (!file.open(QFile::WriteOnly | QFile::Text)) {
	      QMessageBox::warning(this, tr("Application"),
				   tr("Cannot write file %1:\n%2.")
				   .arg(QDir::toNativeSeparators(fileName),
					file.errorString()));
	      return false;
	    }
	    
	    //
	    // create a json object, fill it in & then use a QJsonDocument
	    // to write the contents of the object to the file in JSON format
	    //
	    
	    QJsonObject json;
	    theApp->outputToJSON(json);
	    
	    //Resolve relative paths before saving
	    QFileInfo fileInfo(fileName);
	    SCUtils::ResolveRelativePaths(json, fileInfo.dir());
	    
	    QJsonDocument doc(json);
	    file.write(doc.toJson());
	    
	    // close file
	    file.close();	    
	    
	  });
  
  connect(getRemoteButton,&QPushButton::clicked, this, [=](){
    this->onGetRemoteButtonPressed();
  });
  
  connect(runRemoteButton,&QPushButton::clicked, this, [=](){
    theRemoteDialog->show();
  });    

  connect(submitButton,&QPushButton::clicked,this, [=]() {
    this->submitButtonPressed();
    theRemoteDialog->close();
  });  

  if (enclosingDialog != nullptr) {
    QPushButton *closeButton = new QPushButton("Close");  
    theButtonLayout->addWidget(closeButton,0,4);
    connect(closeButton,&QPushButton::clicked,enclosingDialog,&QDialog::close);
    connect(closeButton,&QPushButton::clicked,theRemoteDialog,&QDialog::close);
    connect(closeButton,&QPushButton::clicked,[=]() {
      theJobManager->hide();
    });
  }


  
  QStringList filesToDownload; filesToDownload << "scInput.json" << "results.zip" << "inputData.zip";
  theJobManager = new RemoteJobManager(theService);
  theJobManager->setFilesToDownload(filesToDownload);
  theJobManager->hide();
  connect(theJobManager,SIGNAL(processResults(QString&)), this, SLOT(processResults(QString&)));
}

SC_RemoteAppTool::~SC_RemoteAppTool()
{
  qDebug() << "SC_RemoteAppTool::Destructor";
}

void SC_RemoteAppTool::clear()
{
  theApp->clear();
}

// this method display the RemoteApp dialog widget

void
SC_RemoteAppTool::submitButtonPressed() {

  //
  // first check if logged in
  //
  
  bool loggedIn = theService->isLoggedIn();

  if (loggedIn != true) {
    errorMessage("ERROR - You Need to Login to DesignSafe to run a remote application.");
    QMessageBox msg;
    msg.setText("You need to go back to the Main Application and Login to DesignSafe to run a remote application.");
    msg.show();
    return;
  }

  //
  // create tmp directory in which we will place files to be sent
  // 
  

  QDir workDir(SimCenterPreferences::getInstance()->getRemoteWorkDir());

  QUuid uniqueName = QUuid::createUuid();
  QString strUnique = uniqueName.toString();
  strUnique = strUnique.mid(1,36);
  tmpDirName = QString("tmp.SimCenter") + strUnique;
  
  //QString tmpDirName = QString("tmp.SimCenter");  
  QString tmpDirectory = workDir.absoluteFilePath(tmpDirName);
  QDir destinationDirectory(tmpDirectory);

  if(destinationDirectory.exists()) {
    destinationDirectory.removeRecursively();
  } else
    destinationDirectory.mkpath(tmpDirectory);

  //
  // in tmpDir, create another "inputs" in which we will place all the files needed by the app & then zip it up
  //
  
  QString inputsDirectory  = destinationDirectory.absoluteFilePath("inputData");
  destinationDirectory.mkpath(inputsDirectory);

  QString zipFile(destinationDirectory.absoluteFilePath("inputData.zip"));
  QDir inputDataDir(destinationDirectory.absoluteFilePath("inputData"));  
  
  theApp->copyFiles(inputsDirectory);  
  
  ZipUtils::ZipFolder(inputDataDir, zipFile);

  // remove inputData so not sent
  inputDataDir.removeRecursively();
  
  //
  // in tmpDir create the input file
  //

  QString inputFile = destinationDirectory.absoluteFilePath("scInput.json");
  
  QFile file(inputFile);
  if (!file.open(QFile::WriteOnly | QFile::Text)) {
    //errorMessage();
    return;
  }
  
  QJsonObject json;

  QJsonObject appData;
  theApp->outputAppDataToJSON(json);
  json["ApplicationData"]=appData;
  
  theApp->outputToJSON(json);

  json["workingDir"]=SimCenterPreferences::getInstance()->getRemoteWorkDir(); 
  json["runDir"]=tmpDirectory;
  json["remoteAppDir"]=SimCenterPreferences::getInstance()->getRemoteAppDir();    
  json["runType"]=QString("runningRemote");
  systemName = systemLineEdit->text();
  int nodeCount = numCPU_LineEdit->text().toInt();
  int gpuCount = numGPU_LineEdit->text().toInt();
  int numProcessorsPerNode = numProcessorsLineEdit->text().toInt();
  json["system"]=systemLineEdit->text();
  json["nodeCount"]=nodeCount;
  json["numP"]=nodeCount*numProcessorsPerNode;  
  json["gpus"]=gpuCount;  
  json["processorsOnEachNode"]=numProcessorsPerNode;    

  QJsonDocument doc(json);
  file.write(doc.toJson());
  file.close();

  //
  // now send directory containing inputFile and inputData.zip across
  //

  QString dirName = destinationDirectory.dirName();
  
  QString remoteHomeDirPath = theService->getHomeDir();
  if (remoteHomeDirPath.isEmpty()) {
    qDebug() << "RemoteApplication:: - remoteHomeDir is empty!!";
    return;
  }
  remoteDirectory = remoteHomeDirPath + QString("/") + dirName;
  submitButton->setEnabled(false);
  
  connect(theService, SIGNAL(uploadDirectoryReturn(bool)), this, SLOT(uploadDirReturn(bool)));
  theService->uploadDirectoryCall(tmpDirectory, remoteHomeDirPath);        

}  


void
SC_RemoteAppTool::uploadDirReturn(bool result)
{
  disconnect(theService, SIGNAL(uploadDirectoryReturn(bool)), this, SLOT(uploadDirReturn(bool)));
    
  //
  // now start the app
  //

  if (result == true) {
    
    //
    // create the json needed to run the remote application
    //
    
    QJsonObject job;
    
    //submitButton->setDisabled(true);

    QString shortDirName = QCoreApplication::applicationName() + ": ";
    
    job["name"]=shortDirName + nameLineEdit->text();
    // job["system"]=systemLineEdit->text();
    int nodeCount = numCPU_LineEdit->text().toInt();
    int gpuCount = numGPU_LineEdit->text().toInt();
    int numProcessorsPerNode = numProcessorsLineEdit->text().toInt();

    job["nodeCount"]=nodeCount;
    // job["gpus"]=gpuCount; // TODO: --gres=gpu:{gpuName}:{gpuCount} may be a neccesary format for slurm
    //job["processorsPerNode"]=nodeCount*numProcessorsPerNode; // DesignSafe has inconsistant documentation
    job["processorsOnEachNode"]=numProcessorsPerNode;
    job["maxRunTime"]=runtimeLineEdit->text();

    QString queue = "small";
    //QString queue = "development";
    if (nodeCount > 2)
      queue = "normal";
    if (nodeCount > 512)
      queue = "large";

    if (gpuCount) {
      if (systemName == "lonestar6-gpu" || systemName == "ls6-gpu" || systemName == "lonestar6" || systemName == "ls6") 
      {
        if (nodeCount > 0 && nodeCount <= 2)
          queue = "gpu-a100-dev"; // TODO: Don't use gpu-a100-dev queue in release, try gpu-a100-small (one 40GB GPU) 
        else if (nodeCount > 2 && nodeCount <= 6)
          queue = "gpu-a100"; // (three 40 GB GPU, 1-6 nodes)
        else 
          qDebug() << "ERROR: Requested node count is too high for lonestar6-gpu system, node count: " << nodeCount;
      } 
      else if (systemName == "frontera-gpu" || systemName == "frontera-rtx" || systemName == "frontera") 
      {
        if (nodeCount > 0 && nodeCount < 2)
          queue = "rtx-dev";
        else if (nodeCount >= 2 && nodeCount <= 6)
          queue = "rtx";
        else 
          qDebug() << "ERROR: Requested node count is too high for frontera-gpu system, node count: " << nodeCount;
      }
      else 
        qDebug() << "ERROR: Requested system is not a GPU system, system name: " << systemName;
    }

    job["appId"]=tapisAppName;
    
    job["memoryPerNode"]= "1GB";
    job["archive"]=true;
    job["batchQueue"]=queue;      
    job["archivePath"]="";
    job["archiveSystem"]="designsafe.storage.default";  

    QJsonObject parameters;    
    parameters["inputFile"]="scInput.json";
    parameters["modules"]="petsc,python3";
    theApp->outputAppDataToJSON(parameters);
    job["parameters"]=parameters;

    QJsonObject inputs;
    inputs["inputDirectory"]=remoteDirectory;
    job["inputs"]=inputs;

    qDebug() << job;

    connect(theService,SIGNAL(startJobReturn(QString)), this, SLOT(startJobReturn(QString)));      
    theService->startJobCall(job);
    
  } else {
    submitButton->setEnabled(true);          
  }
}

void
SC_RemoteAppTool::startJobReturn(QString result)
{
  //
  // job started
  //

  disconnect(theService,SIGNAL(startJobReturn(QString)), nullptr, nullptr);      
  Q_UNUSED(result);
  submitButton->setEnabled(true);      
}


void
SC_RemoteAppTool::onGetRemoteButtonPressed() {

    this->errorMessage("");

    bool loggedIn = theService->isLoggedIn();

    if (loggedIn == true) {

        theJobManager->hide();
        theJobManager->updateJobTable("");
        theJobManager->show();

    } else {
        errorMessage("ERROR - You Need to Login");
    }  
}

void
SC_RemoteAppTool::processResults(QString &dirName){

  //
  // get results widget from app and process
  //

  QString localDir = SimCenterPreferences::getInstance()->getRemoteWorkDir();
  QDir localWork(localDir);
  
  if (!localWork.exists())
    if (!localWork.mkpath(localDir)) {
      emit errorMessage(QString("Could not create Working Dir: ") + localDir + QString(" . Try using an existing directory or make sure you have permission to create the working directory."));
      return;
    }
  
    
  SC_ResultsWidget *theResults=theApp->getResultsWidget();
  if (theResults == NULL) {
    this->errorMessage("FATAL - App cannot process Results");
    return;
  }

  QString blankFileName("scInput.json");
  theResults->processResults(blankFileName,dirName);

}



	     





