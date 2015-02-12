#ifndef INSERTSLICE_H
#define INSERTSLICE_H

#include <QObject>
#include <QDebug>
#include <vtkSmartPointer.h>
#include <vtkImageData.h>
#include <vtkTransform.h>
#include <vtkMatrix4x4.h>

#include "imageprocessor.h"

class InsertSlice : public QObject
{
    Q_OBJECT

public:
    explicit InsertSlice(QObject *parent = 0);

private:
    vtkSmartPointer<vtkImageData> ReconstructedVolume;
    vtkSmartPointer<vtkImageData> accBuffer;
    double OutputOrigin[3];
    double OutputSpacing[3];
    int OutputExtent[6];


public slots:
    int pasteSlice(vtkImageData *image, vtkMatrix4x4 *transformation);
    int resetOutput();
    virtual vtkImageData* getReconstructedVolume();
    virtual vtkImageData* getAccumulationBuffer();
    void setOutputOrigin(double OutputOrigin[3]);
    void setOutputSpacing(double OutputSpacing[3]);
    void setOutputExtent(int OutputExtent[6]);
};

#endif // INSERTSLICE_H
