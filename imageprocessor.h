#ifndef IMAGEPROCESSOR_H
#define IMAGEPROCESSOR_H

#include <QObject>
#include <vtkImageData.h>
#include <vtkIdTypeArray.h>
#include <QDebug>

#include <QFile>
#include <QTextStream>

#define ACCUMULATION_MULTIPLIER 256
#define ACCUMULATION_MAXIMUM 65535
#define ACCUMULATION_THRESHOLD 65279

struct vtkPasteSliceIntoVolumeInsertSliceParams
{
    // information on the volume
    vtkImageData* outData;            // the output volume
    void* outPtr;                     // scalar pointer to the output volume over the output extent
    void* accPtr;           // scalar pointer to the accumulation buffer over the output extent
    vtkImageData* inData;             // input slice
    void* inPtr;                      // scalar pointer to the input volume over the input slice extent
    int* inExt;                       // array size 6, input slice extent (could have been split for threading)
    unsigned int* accOverflowCount;   // the number of voxels that may have error due to accumulation overflow

    // transform matrix for images -> volume
    void* matrix;
};

template <class F, class T>
static int nearestNeighborInterpolation(F *point,
                                        T *inPtr,
                                        T *outPtr,
                                        unsigned short *accPtr,
                                        int numscalars,
                                        int outExt[6],
                                        vtkIdType outInc[3],
                                        unsigned int* accOverflowCount)
{
    int i;
    int outIdX = round(point[0]) - outExt[0];
    int outIdY = round(point[1]) - outExt[2];
    int outIdZ = round(point[2]) - outExt[4];

    //qDebug() << "outIdX, outIdY, outIdZ:" << outIdX << outIdY << outIdZ;
    if ((outIdX | (outExt[1]-outExt[0] - outIdX) |
         outIdY | (outExt[3]-outExt[2] - outIdY) |
         outIdZ | (outExt[5]-outExt[4] - outIdZ)) >= 0)
    {
        int inc = outIdX * outInc[0] + outIdY * outInc[1] + outIdZ * outInc[2];
        outPtr += inc;

        // MEAN COMPOUND MODE
        accPtr += inc/outInc[0];
        if (*accPtr <= ACCUMULATION_THRESHOLD)
        {
            int newa = *accPtr + ACCUMULATION_MULTIPLIER;
            if (newa > ACCUMULATION_THRESHOLD)
                (*accOverflowCount) += 1;

            for (i = 0; i < numscalars; i++)
            {
                *outPtr = ((*inPtr++)*ACCUMULATION_MULTIPLIER + (*outPtr)*(*accPtr))/newa;
                outPtr++;
            }

            *accPtr = ACCUMULATION_MAXIMUM;
            if (newa < ACCUMULATION_MAXIMUM)
            {
                *accPtr = newa;
            }
        }
        else
        {// overflow, use recursive filtering with 255/256 and 1/256 as the weights, since 255 voxels have been inserted so far
            // TODO: Should do this for all the scalars, and accumulation?
            *outPtr = (T)(0.99609375 * (*inPtr++) + 0.00390625 * (*outPtr));
        }

        // MAXIMUM COMPOUND MODE
        /*accPtr += inc/outInc[0];
        int newa = *accPtr + ACCUMULATION_MULTIPLIER;
        if (newa > ACCUMULATION_THRESHOLD)
            (*accOverflowCount) += 1;

        for (i = 0; i < numscalars; i++)
        {
            if (*inPtr > *outPtr)
                *outPtr = *inPtr;
            inPtr++;
            outPtr++;
        }

        *accPtr = ACCUMULATION_MAXIMUM;
        if (newa < ACCUMULATION_MAXIMUM)
            *accPtr = newa;*/


        return 1;
    }
    //qDebug() << "No hit";
    return 0;
}

template <class F, class T>
static void outputSliceTransformation(vtkPasteSliceIntoVolumeInsertSliceParams *insertionParams)
{
    //QFile file("out.m");
    //file.open(QIODevice::WriteOnly | QIODevice::Text);
    //QTextStream out(&file);
    //out << "A = [" << endl;


    int hit = 0;

    vtkImageData *outData = insertionParams->outData;
    T* outPtr = reinterpret_cast<unsigned char*>(insertionParams->outPtr);
    unsigned short *accPtr = reinterpret_cast<unsigned short*>(insertionParams->accPtr);
    vtkImageData *inData = insertionParams->inData;
    T *inPtr = reinterpret_cast<unsigned char*>(insertionParams->inPtr);
    int *inExt = insertionParams->inExt;
    unsigned int *accOverflowCount = insertionParams->accOverflowCount;

    F *matrix = reinterpret_cast<F*>(insertionParams->matrix);
    qDebug() << "imageprocessor.h" << __LINE__ << "matrix:" << matrix[0]<<" "<<matrix[1]<<" "<<matrix[2]<<" "<<matrix[3]<<"; "
             <<matrix[4]<<" "<<matrix[5]<<" "<<matrix[6]<<" "<<matrix[7]<<"; "
             <<matrix[8]<<" "<<matrix[9]<<" "<<matrix[10]<<" "<<matrix[11]<<"; "
             <<matrix[12]<<" "<<matrix[13]<<" "<<matrix[14]<<" "<<matrix[15];
    qDebug() << "imageprocessor.h" << __LINE__ << "Extent:" << inExt[0]<< inExt[1] << inExt[2] << inExt[3] << inExt[4] << inExt[5];

    double inSpacing[3];
    inData->GetSpacing(inSpacing);
    double inOrigin[3];
    inData->GetOrigin(inOrigin);

    int outExt[6];
    outData->GetExtent(outExt);

    vtkIdType outInc[3] = {0};
    outData->GetIncrements(outInc);

    vtkIdType inIncX = 0, inIncY = 0, inIncZ = 0;
    inData->GetContinuousIncrements(inExt, inIncX, inIncY, inIncZ);
    int numscalars = inData->GetNumberOfScalarComponents();
    double point[4];
    double inPoint[4];
    inPoint[3] = 1;
    for (int idZ = inExt[4]; idZ <= inExt[5]; idZ++, inPtr += inIncZ)
    {
        for (int idY = inExt[2]; idY <= inExt[3]; idY++, inPtr += inIncY)
        {
            for (int idX = inExt[0]; idX <= inExt[1]; idX++, inPtr += numscalars)
            {
                inPoint[0] = idX;
                inPoint[1] = idY;
                inPoint[2] = idZ;

                for (int i = 0; i < 4; i++)
                {
                    int rowindex = i << 2;
                    point[i] = matrix[rowindex  ] * inPoint[0] +
                               matrix[rowindex+1] * inPoint[1] +
                               matrix[rowindex+2] * inPoint[2] +
                               matrix[rowindex+3] * inPoint[3] ;
                }

                point[0] /= point[3];
                point[1] /= point[3];
                point[2] /= point[3];
                point[3] = 1;

                //out << point[0] << " " << point [1]<< " " << point[2] << endl;
                hit += nearestNeighborInterpolation<double, T>(point, inPtr, outPtr, accPtr, numscalars, outExt, outInc, accOverflowCount);

            }
        } 
    }

    //out << "];\nB = transpose(A);\nscatter3(B(1,:),B(2,:),B(3,:))";
    //file.close();
    qDebug() << "hit" << hit;
}

#endif // IMAGEPROCESSOR_H
