#ifndef IMAGEPROCESSOR_H
#define IMAGEPROCESSOR_H

#include <QObject>
#include <vtkImageData.h>
#include <vtkIdTypeArray.h>
#include <QDebug>

#define ACCUMULATION_MULTIPLIER 256
#define ACCUMULATION_MAXIMUM 65535
#define ACCUMULATION_THRESHOLD 65279

struct vtkPasteSliceIntoVolumeInsertSliceParams
{
    // information on the volume
    vtkImageData* outData;            // the output volume
    void* outPtr;                     // scalar pointer to the output volume over the output extent
    unsigned short* accPtr;           // scalar pointer to the accumulation buffer over the output extent
    vtkImageData* inData;             // input slice
    void* inPtr;                      // scalar pointer to the input volume over the input slice extent
    int* inExt;                       // array size 6, input slice extent (could have been split for threading)
    unsigned int* accOverflowCount;   // the number of voxels that may have error due to accumulation overflow

    // transform matrix for images -> volume
    void* matrix;
};

class ImageProcessor : public QObject
{
    Q_OBJECT

public:
    explicit ImageProcessor(QObject *parent = 0);

private:
    vtkImageData *outData;
    unsigned char *outPtr;
    unsigned short *accPtr;
    vtkImageData *inData;
    unsigned char *inPtr;
    int *inExt;
    unsigned int *accOverflowCount;
    double *matrix;

    double point[3];
    int outExt[6];

    int nearestNeighborInterpolation(int numscalars, vtkIdType outInc[3]);

public slots:
    void outputSliceTransformation(vtkPasteSliceIntoVolumeInsertSliceParams *insertionParams);
};

#endif // IMAGEPROCESSOR_H
