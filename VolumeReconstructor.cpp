#include "VolumeReconstructor.h"

VolumeReconstructor::VolumeReconstructor(QObject *parent) :
    QObject(parent)
{
    this->outputOrigin[0] = 0.0;
    this->outputOrigin[1] = 0.0;
    this->outputOrigin[2] = 0.0;

    this->outputSpacing[0] = 1.0;
    this->outputSpacing[1] = 1.0;
    this->outputSpacing[2] = 1.0;

    this->positions = vtkSmartPointer<vtkPoints>::New();
    this->directions = vtkSmartPointer<vtkPoints>::New();
    this->pngReader = vtkSmartPointer<vtkPNGReader>::New();
    //this->imageData = vtkSmartPointer<vtkImageData>::New();
    this->accBuffer = vtkSmartPointer<vtkImageData>::New();

    this->transformImageToReference = vtkSmartPointer<vtkMatrix4x4>::New();
    this->tVolumePixFromRef = vtkSmartPointer<vtkTransform>::New();
    this->tRefFromImage = vtkSmartPointer<vtkTransform>::New();
    this->tImageFromImagePix = vtkSmartPointer<vtkTransform>::New();
    this->tImagePixToVolumePix = vtkSmartPointer<vtkTransform>::New();
    this->mImagePixToVolumePix = vtkSmartPointer<vtkMatrix4x4>::New();

    this->ReconstructedVolume = vtkImageData::New();

    this->transformImageToReference->SetElement(0, 0, 0.999842744018339);
    this->transformImageToReference->SetElement(0, 1, 0.0151091633708696);
    this->transformImageToReference->SetElement(0, 2, 0.00928441791986296);
    this->transformImageToReference->SetElement(0, 3, 57.4219788466268);
    this->transformImageToReference->SetElement(1, 0, 0.0150774692679663);
    this->transformImageToReference->SetElement(1, 1, 0.999880292567433);
    this->transformImageToReference->SetElement(1, 2, 0.0034742561415262);
    this->transformImageToReference->SetElement(1, 3, 238.422002969038);
    this->transformImageToReference->SetElement(2, 0, -0.00933579960966547);
    this->transformImageToReference->SetElement(2, 1, -0.00333372426810844);
    this->transformImageToReference->SetElement(2, 2, 0.999950863356871);
    this->transformImageToReference->SetElement(2, 3, 295.934803148914);
    this->transformImageToReference->SetElement(3, 0, 0);
    this->transformImageToReference->SetElement(3, 1, 0);
    this->transformImageToReference->SetElement(3, 2, 0);
    this->transformImageToReference->SetElement(3, 3, 1);
}

int VolumeReconstructor::generateVolume(QString inputTSV)
{
    Parser *parser = new Parser(this);
    parser->parseTSVFile(inputTSV, this->positions, this->directions);

    readPNGImages("/media/datos/ETSIT/PFC/fotos_cortadas");

    setOutputExtent();

    for (int i = 48; i < 49; i++)
    {
        qDebug() << "Processing image number" << i;
        insertSlice(this->images.at(i));
    }
    saveOutputVolume();
    qDebug() << "Volumen guardado";
    return 1;
}

void VolumeReconstructor::readPNGImages(QString directory)
{
    for (int i = 0; i < this->positions->GetNumberOfPoints(); ++i)
    {
        QString fileName = QString("%1/crop%2.png").arg(directory).arg(QString::number(i+1));
        qDebug() << "Reading" <<fileName;
        QByteArray byteArray = fileName.toUtf8();
        const char* cString = byteArray.constData();

        this->pngReader->SetFileName(cString);
        this->pngReader->Update();

        this->imageData = vtkSmartPointer<vtkImageData>::New();
        this->imageData = this->pngReader->GetOutput();
        //this->imageData->SetOrigin(this->positions->GetPoint(i));

        this->images.push_back(this->imageData);
        this->imageData->Delete();
    }
    qDebug() << "Number of Images:" << this->images.size();
}

void VolumeReconstructor::setOutputExtent()
{
    double extent_Ref[6] =
    {
        VTK_DOUBLE_MAX, VTK_DOUBLE_MIN,
        VTK_DOUBLE_MAX, VTK_DOUBLE_MIN,
        VTK_DOUBLE_MAX, VTK_DOUBLE_MIN
    };

    for (int i = 0; i < this->images.size(); i++)
    {
        vtkImageData *currentImage = this->images.at(i);
        int *frameExtent = currentImage->GetExtent();
        std::vector<double*> corners_ImagePix;
        double minX = frameExtent[0];
        double maxX = frameExtent[1];
        double minY = frameExtent[2];
        double maxY = frameExtent[3];

        double c0[4] = {minX,minY,0,1};
        double c1[4] = {minX,maxY,0,1};
        double c2[4] = {maxX,minY,0,1};
        double c3[4] = {maxX,maxY,0,1};
        corners_ImagePix.push_back(c0);
        corners_ImagePix.push_back(c1);
        corners_ImagePix.push_back(c2);
        corners_ImagePix.push_back(c3);

        // transform the corners to Reference and expand the extent if needed
        for (unsigned int corner = 0; corner < corners_ImagePix.size(); ++corner)
        {
            double corner_Ref[ 4 ] = { 0, 0, 0, 1 }; // position of the corner in the Reference coordinate system
            this->transformImageToReference->MultiplyPoint(corners_ImagePix[corner], corner_Ref);

            for ( int axis = 0; axis < 3; axis ++ )
            {
                if ( corner_Ref[axis] < extent_Ref[axis*2] )
                {
                    // min extent along this coord axis has to be decreased
                    extent_Ref[axis*2]=corner_Ref[axis];
                }
                if ( corner_Ref[axis] > extent_Ref[axis*2+1] )
                {
                    // max extent along this coord axis has to be increased
                    extent_Ref[axis*2+1]=corner_Ref[axis];
                }
            }
        }
    }

    double* outputSpacing = this->ReconstructedVolume->GetSpacing();
    this->outputExtent[1] = 700;//int((extent_Ref[1] - extent_Ref[0]) / outputSpacing[0]);
    this->outputExtent[3] = 700;//int((extent_Ref[3] - extent_Ref[2]) / outputSpacing[1]);
    this->outputExtent[5] = 700;//int((extent_Ref[5] - extent_Ref[4]) / outputSpacing[2]);

    //this->outData->SetExtent(this->outputExtent);
    //this->outData->SetOrigin(extent_Ref[0], extent_Ref[2], extent_Ref[4]);
    vtkInformation *info = vtkInformation::New();
    this->ReconstructedVolume->SetScalarType(this->imageData->GetScalarType(), info);
    this->ReconstructedVolume->SetExtent(this->outputExtent);
    this->ReconstructedVolume->SetOrigin(this->outputOrigin);
    qDebug() << "Extent size: " << outputExtent[0] << outputExtent[1] << outputExtent[2] << outputExtent[3] << outputExtent[4] << outputExtent[5];
    qDebug() << "Extent size: " << extent_Ref[0] << extent_Ref[1] << extent_Ref[2] << extent_Ref[3] << extent_Ref[4] << extent_Ref[5];

    if (resetOutput() != 1)
    {
        qDebug() << "Error, failed to initialize output volume";
    }
}

int VolumeReconstructor::resetOutput()
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
        accExtent[i] = this->outputExtent[i];
    }

    accData->SetExtent(accExtent);
    accData->SetOrigin(this->outputOrigin);
    accData->SetSpacing(this->outputSpacing);
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

    int *outExtent = this->outputExtent;
    outData->SetExtent(outExtent);
    outData->SetOrigin(this->outputOrigin);
    outData->SetSpacing(this->outputSpacing);
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

int VolumeReconstructor::insertSlice(vtkImageData *image)
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
    void *outPtr = this->ReconstructedVolume->GetScalarPointerForExtent(outExt);

    void *accPtr = this->accBuffer->GetScalarPointerForExtent(outExt);

    this->tVolumePixFromRef->Translate(this->ReconstructedVolume->GetOrigin());
    this->tVolumePixFromRef->Scale(this->ReconstructedVolume->GetSpacing());
    this->tVolumePixFromRef->Inverse();

    this->tRefFromImage->SetMatrix(this->transformImageToReference);

    this->tImageFromImagePix->Scale(image->GetSpacing());

    this->tImagePixToVolumePix->Concatenate(tVolumePixFromRef);
    this->tImagePixToVolumePix->Concatenate(tRefFromImage);
    this->tImagePixToVolumePix->Concatenate(tImageFromImagePix);

    this->tImagePixToVolumePix->GetMatrix(mImagePixToVolumePix);

    unsigned int *accBufferErrors;
    vtkPasteSliceIntoVolumeInsertSliceParams insertionParams;
    insertionParams.accOverflowCount = accBufferErrors;
    insertionParams.accPtr = (unsigned short *)accPtr;
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

    qDebug() << "Image Scalar Type" << image->GetScalarTypeAsString();
    outputSliceTransformation<double, unsigned char>(&insertionParams);
}

void VolumeReconstructor::saveOutputVolume()
{
    qDebug() << "Guardando volumen";
    vtkSmartPointer<vtkImageExtractComponents> extract = vtkSmartPointer<vtkImageExtractComponents>::New();
    extract->SetComponents(0);
    extract->SetInputData(this->ReconstructedVolume);
    extract->Update();
    vtkSmartPointer<vtkImageData> extractionData = vtkSmartPointer<vtkImageData>::New();
    extractionData->DeepCopy(extract->GetOutput());
    vtkSmartPointer<vtkMetaImageWriter> writer = vtkSmartPointer<vtkMetaImageWriter>::New();
    writer->SetInputData(extractionData);
    writer->SetCompression(true);
    writer->SetFileName("outVol.mha");
    writer->SetRAWFileName("outVol.raw");
    writer->Write();
}
