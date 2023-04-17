#ifndef UQPYRESULTSSUBSETSIMULATION_H
#define UQPYRESULTSSUBSETSIMULATION_H

#include <QJsonObject>
#include <QString>

class UQpyResultsSubsetSimulation
{
public:
    UQpyResultsSubsetSimulation();

    ~UQpyResultsSubsetSimulation();

    bool outputToJSON(QJsonObject &rvObject);
    bool inputFromJSON(QJsonObject &rvObject);

    int processResults(QString &dirName);
};

#endif // UQPYRESULTSSUBSETSIMULATION_H
