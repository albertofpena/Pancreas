#include "imageprocessor.h"

/*ImageProcessor::ImageProcessor(QObject *parent) :
    QObject(parent)
{
    this->point[3] = 0;
    this->outExt[6] = 0;
}

void ImageProcessor::outputSliceTransformation(vtkPasteSliceIntoVolumeInsertSliceParams *insertionParams)
{
    int hit = 0;

    this->outData = insertionParams->outData;
    this->outPtr = reinterpret_cast<unsigned char*>(insertionParams->outPtr);
    this->accPtr = insertionParams->accPtr;
    this->inData = insertionParams->inData;
    this->inPtr = reinterpret_cast<unsigned char*>(insertionParams->inPtr);
    this->inExt = insertionParams->inExt;
    this->accOverflowCount = insertionParams->accOverflowCount;

    this->matrix = reinterpret_cast<double*>(insertionParams->matrix);
    qDebug() << matrix[0]<<" "<<matrix[1]<<" "<<matrix[2]<<" "<<matrix[3]<<"; "
             <<matrix[4]<<" "<<matrix[5]<<" "<<matrix[6]<<" "<<matrix[7]<<"; "
             <<matrix[8]<<" "<<matrix[9]<<" "<<matrix[10]<<" "<<matrix[11]<<"; "
             <<matrix[12]<<" "<<matrix[13]<<" "<<matrix[14]<<" "<<matrix[15];
    qDebug() << "Extent:" << inExt[0]<< inExt[1] << inExt[2] << inExt[3] << inExt[4] << inExt[5];

    double inSpacing[3];
    this->inData->GetSpacing(inSpacing);
    double inOrigin[3];
    this->inData->GetOrigin(inOrigin);

    this->outData->GetExtent(this->outExt);

    vtkIdType outInc[3] = {0};
    this->outData->GetIncrements(outInc);

    vtkIdType inIncX = 0, inIncY = 0, inIncZ = 0;
    this->inData->GetContinuousIncrements(inExt, inIncX, inIncY, inIncZ);
    int numscalars = this->inData->GetNumberOfScalarComponents();

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
                    this->point[i] = matrix[rowindex  ] * inPoint[0] +
                                     matrix[rowindex+1] * inPoint[1] +
                                     matrix[rowindex+2] * inPoint[2] +
                                     matrix[rowindex+3] * inPoint[3] ;
                }

                this->point[0] /= this->point[3];
                this->point[1] /= this->point[3];
                this->point[2] /= this->point[3];
                this->point[3] = 1;

                hit += nearestNeighborInterpolation(numscalars, outInc);

            }
        }
    }
    qDebug() << "hit" << hit;
}

int ImageProcessor::nearestNeighborInterpolation(int numscalars,
                                                 vtkIdType outInc[])
{
    int i;
    int outIdX = round(this->point[0]) - this->outExt[0];
    int outIdY = round(this->point[1]) - this->outExt[2];
    int outIdZ = round(this->point[2]) - this->outExt[4];

    qDebug() << "Márgenes:" << (outExt[5]-outExt[4] - outIdZ);
    if ((outIdX | (this->outExt[1]-this->outExt[0] - outIdX) |
         outIdY | (this->outExt[3]-this->outExt[2] - outIdY) |
         outIdZ | (this->outExt[5]-this->outExt[4] - outIdZ)) >= 0)
    {
        int inc = outIdX * outInc[0] + outIdY * outInc[1] + outIdZ * outInc[2];
        this->outPtr += inc;

//        this->accPtr += inc/outInc[0];

//        int newa = *this->accPtr + ACCUMULATION_MULTIPLIER;
//        if (newa > ACCUMULATION_THRESHOLD)
//            (*accOverflowCount) += 1;

        for (i = 0; i < numscalars; i++)
        {
            if (*this->inPtr > *this->outPtr)
                *this->outPtr = *this->inPtr;
            this->inPtr++;
            this->outPtr++;
        }

//        *this->accPtr = ACCUMULATION_MAXIMUM;
//        if (newa < ACCUMULATION_MAXIMUM)
//        {
//            *this->accPtr = newa;
//        }
        return 1;
    }
    //qDebug() << "No hit";
    return 0;
}
*/
