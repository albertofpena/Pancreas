#include "insertslice.h"

InsertSlice::InsertSlice(QObject *parent) :
    QObject(parent)
{
    this->ReconstructedVolume=vtkImageData::New();
    this->accBuffer=vtkImageData::New();

    this->OutputOrigin[0] = 0.0;
    this->OutputOrigin[1] = 0.0;
    this->OutputOrigin[2] = 0.0;

    this->OutputSpacing[0] = 1.0;
    this->OutputSpacing[1] = 1.0;
    this->OutputSpacing[2] = 1.0;

    // Set to invalid values by default
    // If the user doesn't set the correct values then inserting the slice will be refused
    this->OutputExtent[0] = 0;
    this->OutputExtent[1] = 0;
    this->OutputExtent[2] = 0;
    this->OutputExtent[3] = 0;
    this->OutputExtent[4] = 0;
    this->OutputExtent[5] = 0;
}

int InsertSlice::pasteSlice(vtkImageData *image, vtkMatrix4x4 *transformation)
{
    int inputFrameExtent[6];
    image->GetExtent(inputFrameExtent);

    if (image->GetScalarType() != this->ReconstructedVolume->GetScalarType())
    {
        qDebug() << "Error: input ScalarType " << image->GetScalarTypeAsString() << "!= ReconstructedVolume ScalarType" << this->ReconstructedVolume->GetScalarTypeAsString();
        return -1;
    }

    void *inPtr = image->GetScalarPointerForExtent(inputFrameExtent);

    int *outExt = this->ReconstructedVolume->GetExtent();
    qDebug() << "insertslice.cpp:" << __LINE__ << "outExt:" << outExt[0] << outExt[1] << outExt[2] << outExt[3] << outExt[4] << outExt[5];
    void *outPtr = this->ReconstructedVolume->GetScalarPointerForExtent(outExt);

    void *accPtr = this->accBuffer->GetScalarPointerForExtent(outExt);

    vtkSmartPointer<vtkTransform> tVolumePixFromRef = vtkSmartPointer<vtkTransform>::New();
    tVolumePixFromRef->Translate(this->ReconstructedVolume->GetOrigin());
    tVolumePixFromRef->Scale(this->ReconstructedVolume->GetSpacing());
    tVolumePixFromRef->Inverse();

    vtkSmartPointer<vtkTransform> tRefFromImage = vtkSmartPointer<vtkTransform>::New();
    tRefFromImage->SetMatrix(transformation);

    vtkSmartPointer<vtkTransform> tImageFromImagePix = vtkSmartPointer<vtkTransform>::New();
    tImageFromImagePix->Scale(image->GetSpacing());

    vtkSmartPointer<vtkTransform> tImagePixToVolumePix = vtkSmartPointer<vtkTransform>::New();
    tImagePixToVolumePix->Concatenate(tVolumePixFromRef);
    tImagePixToVolumePix->Concatenate(tRefFromImage);
    tImagePixToVolumePix->Concatenate(tImageFromImagePix);

    vtkSmartPointer<vtkMatrix4x4> mImagePixToVolumePix = vtkSmartPointer<vtkMatrix4x4>::New();
    tImagePixToVolumePix->GetMatrix(mImagePixToVolumePix);

    // TODO initialize accBufferErrors or remove it if unused
    unsigned int *accBufferErrors;
    vtkPasteSliceIntoVolumeInsertSliceParams insertionParams;
    insertionParams.accOverflowCount = accBufferErrors;
    insertionParams.accPtr = accPtr;
    insertionParams.inData = image;
    insertionParams.inExt = inputFrameExtent;
    insertionParams.inPtr = inPtr;
    insertionParams.outData = this->ReconstructedVolume;
    insertionParams.outPtr = outPtr;

    double newmatrix[16];
    for (int i = 0; i < 4; i++)
    {
        int rowindex = (i<<2);
        newmatrix[rowindex  ] = mImagePixToVolumePix->GetElement(i,0);
        newmatrix[rowindex+1] = mImagePixToVolumePix->GetElement(i,1);
        newmatrix[rowindex+2] = mImagePixToVolumePix->GetElement(i,2);
        newmatrix[rowindex+3] = mImagePixToVolumePix->GetElement(i,3);
    }
    insertionParams.matrix = newmatrix;

//    qDebug() << "Image Scalar Type" << image->GetScalarTypeAsString();
    outputSliceTransformation<double, unsigned char>(&insertionParams);
}

int InsertSlice::resetOutput()
{
    vtkImageData* accData = this->accBuffer;
    if (accBuffer == NULL)
    {
        qDebug() << "Accumulation buffer object is not created";
        return -1;
    }
    int accExtent[6];
    for (int i = 0; i < 6; i++)
    {
        accExtent[i] = this->OutputExtent[i];
    }

    accData->SetExtent(accExtent);
    accData->SetOrigin(this->OutputOrigin);
    accData->SetSpacing(this->OutputSpacing);
    accData->AllocateScalars(VTK_UNSIGNED_SHORT, 3);

    void *accPtr = accData->GetScalarPointerForExtent(accExtent);
    if (accPtr == NULL)
    {
        qDebug() << "Cannot allocate memory for accumulation image extent:" << accExtent[1] - accExtent[0] <<"x"<< accExtent[3]-accExtent[2] <<" x "<< accExtent[5]-accExtent[4];
    }
    else
    {
        memset(accPtr, 0,((accExtent[1]-accExtent[0]+1)*
                (accExtent[3]-accExtent[2]+1)*
                (accExtent[5]-accExtent[4]+1)*
                accData->GetScalarSize()*accData->GetNumberOfScalarComponents()));
    }

    vtkImageData* outData = this->ReconstructedVolume;
    if (outData == NULL)
    {
        qDebug() << "Output image object is not created";
        return -1;
    }

    int *outExtent = this->OutputExtent;
    outData->SetExtent(outExtent);
    outData->SetOrigin(this->OutputOrigin);
    outData->SetSpacing(this->OutputSpacing);
    outData->AllocateScalars(VTK_UNSIGNED_CHAR, 3);

    void *outPtr = outData->GetScalarPointerForExtent(outExtent);
    if (outPtr == NULL)
    {
        qDebug() << "Cannot allocate memory for output image extent:" << outExtent[1] - outExtent[0] <<"x"<< outExtent[3]-outExtent[2] <<" x "<< outExtent[5]-outExtent[4];
    }
    else
    {
        memset(outPtr, 0,((outExtent[1]-outExtent[0]+1)*
                (outExtent[3]-outExtent[2]+1)*
                (outExtent[5]-outExtent[4]+1)*
                outData->GetScalarSize()*outData->GetNumberOfScalarComponents()));
    }

    return 1;
}

void InsertSlice::setOutputExtent(int OutputExtent[])
{
    for (int i = 0; i < 6; i++)
    {
        this->OutputExtent[i] = OutputExtent[i];
    }
}

void InsertSlice::setOutputOrigin(double OutputOrigin[])
{
    this->OutputOrigin[0] = OutputOrigin[0];
    this->OutputOrigin[1] = OutputOrigin[1];
    this->OutputOrigin[2] = OutputOrigin[2];
}

void InsertSlice::setOutputSpacing(double OutputSpacing[3])
{
    this->OutputSpacing[0] = OutputSpacing[0];
    this->OutputSpacing[1] = OutputSpacing[1];
    this->OutputSpacing[2] = OutputSpacing[2];
}

vtkImageData* InsertSlice::getAccumulationBuffer()
{
    return this->accBuffer;
}

vtkImageData* InsertSlice::getReconstructedVolume()
{
    return this->ReconstructedVolume;
}
