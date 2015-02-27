#ifndef VolumeReconstructor_H
#define VolumeReconstructor_H

#include <QObject>
#include <QDebug>
#include <vtkSmartPointer.h>
#include <vtkPoints.h>
#include <vtkImageData.h>
#include <vtkPNGReader.h>
#include <vtkMatrix4x4.h>
#include <vtkInformation.h>
#include <vtkTransform.h>
#include <Parser.h>
#include "insertslice.h"
#include "imageprocessor.h"

#include <vtkImageReslice.h>
#include <vtkImageTranslateExtent.h>
#include <vtkProp3D.h>

#include <vtkImageExtractComponents.h>
#include <vtkMetaImageWriter.h>


class VolumeReconstructor : public QObject
{
    Q_OBJECT

public:
    explicit VolumeReconstructor(QObject *parent = 0);

private:
    int outputExtent[6] = { 0, 0, 0, 0, 0, 0 };
    vtkImageData *ReconstructedVolume;
    vtkSmartPointer<vtkPoints> positions;
    vtkSmartPointer<vtkPoints> directions;
    vtkSmartPointer<vtkPNGReader> pngReader;
    vtkSmartPointer<vtkImageData> imageData;
    //vtkSmartPointer<vtkImageData> outData;
    vtkSmartPointer<vtkMatrix4x4> transformImageToReference;

    InsertSlice *sliceAdder;
//    std::vector< vtkSmartPointer<vtkImageData> > images;

    //ImageProcessor *processor;

    void saveOutputVolume();

public slots:
    int generateVolume(QString inputTSV);
    vtkImageData* readPNGImages(QString directory, int index);
    void setOutputExtent();
};

#endif // VolumeReconstructor_H
