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
#include <vtkImageExtractComponents.h>
#include <vtkMetaImageWriter.h>
#include "imageprocessor.h"

class VolumeReconstructor : public QObject
{
    Q_OBJECT

public:
    explicit VolumeReconstructor(QObject *parent = 0);

private:
    double outputOrigin[3];
    double outputSpacing[3];
    int outputExtent[6] = { 0, 0, 0, 0, 0, 0 };
    vtkImageData *ReconstructedVolume;
    vtkSmartPointer<vtkPoints> positions;
    vtkSmartPointer<vtkPoints> directions;
    vtkSmartPointer<vtkPNGReader> pngReader;
    vtkSmartPointer<vtkImageData> imageData;
    //vtkSmartPointer<vtkImageData> outData;
    vtkSmartPointer<vtkImageData> accBuffer;
    vtkSmartPointer<vtkMatrix4x4> transformImageToReference;
    vtkSmartPointer<vtkTransform> tVolumePixFromRef;
    vtkSmartPointer<vtkTransform> tRefFromImage;
    vtkSmartPointer<vtkTransform> tImageFromImagePix;
    vtkSmartPointer<vtkTransform> tImagePixToVolumePix;
    vtkSmartPointer<vtkMatrix4x4> mImagePixToVolumePix;
    std::vector< vtkSmartPointer<vtkImageData> > images;

    //ImageProcessor *processor;

    void saveOutputVolume();

public slots:
    int generateVolume(QString inputTSV);
    void readPNGImages(QString directory);
    void setOutputExtent();
    int insertSlice(vtkImageData *image);
    int resetOutput();
};

#endif // VolumeReconstructor_H
