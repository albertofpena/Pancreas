#include "VolumeReconstructor.h"

#define INDEX_PRUEBA 1700

VolumeReconstructor::VolumeReconstructor(QObject *parent) :
    QObject(parent)
{
    this->positions = vtkSmartPointer<vtkPoints>::New();
    this->directions = vtkSmartPointer<vtkPoints>::New();
    this->pngReader = vtkSmartPointer<vtkPNGReader>::New();
    //this->imageData = vtkSmartPointer<vtkImageData>::New();

    this->ReconstructedVolume = vtkImageData::New();
    this->ReconstructedVolume->SetSpacing(1.0,1.0,1.0);
    this->sliceAdder = new InsertSlice;

    this->translateAndRotate = vtkSmartPointer<vtkMatrix4x4>::New();
    this->transformImageToReference = vtkSmartPointer<vtkMatrix4x4>::New();
    this->finalTransform = vtkSmartPointer<vtkMatrix4x4>::New();
    this->finalTransform->SetElement(0, 0, 0.5315);
    this->finalTransform->SetElement(0, 1, -0.6281);
    this->finalTransform->SetElement(0, 2, -0.5683);
    this->finalTransform->SetElement(0, 3, -41.7975);
    this->finalTransform->SetElement(1, 0, 0.8329);
    this->finalTransform->SetElement(1, 1, 0.5097);
    this->finalTransform->SetElement(1, 2, 0.2156);
    this->finalTransform->SetElement(1, 3, 8.4493);
    this->finalTransform->SetElement(2, 0, 0.1542);
    this->finalTransform->SetElement(2, 1, -0.5879);
    this->finalTransform->SetElement(2, 2, 0.7941);
    this->finalTransform->SetElement(2, 3, -210.6896);
    this->finalTransform->SetElement(3, 0, 0);
    this->finalTransform->SetElement(3, 1, 0);
    this->finalTransform->SetElement(3, 2, 0);
    this->finalTransform->SetElement(3, 3, 1);
}

int VolumeReconstructor::generateVolume(QString inputTSV)
{
    Parser *parser = new Parser(this);
    parser->parseTSVFile(inputTSV, this->positions, this->directions);
    qDebug() << "VolumeReconstructor.cpp:" << __LINE__ << ": Number of positions readed" << this->positions->GetNumberOfPoints();

    setOutputExtent();

    for (int i = 0; i < INDEX_PRUEBA; ++i)
    {
        vtkSmartPointer<vtkImageData> currentImage = vtkSmartPointer<vtkImageData>::New();
        qDebug() << "Processing image number" << i;
        currentImage = readPNGImages("/media/datos/ETSIT/PFC/pseudotumor/VTS_01_1", i);
        this->sliceAdder->pasteSlice(currentImage,transformImageToReference);
    }
//    recalculateExtent();
//    this->sliceAdder->pasteSlice(this->sliceAdder->getReconstructedVolume(),this->transformImageToReference);
    this->ReconstructedVolume = this->sliceAdder->getReconstructedVolume();
    saveOutputVolume();
    qDebug() << "Volumen guardado";
    return 1;
}

vtkImageData *VolumeReconstructor::readPNGImages(QString directory, int index)
{
    vtkImageData *outImage = vtkImageData::New();
    QString fileName = QString("%1/crop%2.png").arg(directory).arg(QString::number(index+1));
    QByteArray byteArray = fileName.toUtf8();
    const char* cString = byteArray.constData();

    this->pngReader->SetFileName(cString);
    this->pngReader->Update();
    qDebug() << "Reading" << this->pngReader->GetFileName();
    outImage = this->pngReader->GetOutput();

    //outImage->SetOrigin(this->positions->GetPoint(index));

    double *pos = {this->positions->GetPoint(index)};
    double *rot = {this->directions->GetPoint(index)};

    vtkTransform *transform = vtkTransform::New();
    transform->Identity();
//    transform->SetMatrix(this->transformImageToReference);
    transform->PreMultiply();
    transform->Scale(0.1,0.1,0.1);
    transform->RotateZ(rot[0]);
    transform->RotateX(rot[1]);
    transform->RotateY(rot[2]);
    transform->PostMultiply();
    transform->Translate(pos[1]+55,pos[0]-2,pos[2]+135);
    transform->Update();
//    transform->Concatenate(this->transformImageToReference);
    transform->GetMatrix(this->transformImageToReference);
//    this->translateAndRotate->Multiply4x4(this->translateAndRotate,this->transformImageToReference,this->transformImageToReference);

//    qDebug() << "positions:" << pos[0]+36 << pos[1]+29 << pos[2]+225;
//    qDebug() << "directions:" << rot[0] << rot[1] << rot[2];
//    qDebug() << "Transform matrix:" << this->transformImageToReference->GetElement(0,0) << this->transformImageToReference->GetElement(0,1) << this->transformImageToReference->GetElement(0,2) << this->transformImageToReference->GetElement(0,3) << "\n"
//                                    << this->transformImageToReference->GetElement(1,0) << this->transformImageToReference->GetElement(1,1) << this->transformImageToReference->GetElement(1,2) << this->transformImageToReference->GetElement(1,3) << "\n"
//                                    << this->transformImageToReference->GetElement(2,0) << this->transformImageToReference->GetElement(2,1) << this->transformImageToReference->GetElement(2,2) << this->transformImageToReference->GetElement(2,3) << "\n"
//                                    << this->transformImageToReference->GetElement(3,0) << this->transformImageToReference->GetElement(3,1) << this->transformImageToReference->GetElement(3,2) << this->transformImageToReference->GetElement(3,3);
    transform->Delete();
    return outImage;
}

void VolumeReconstructor::setOutputExtent()
{
    double extent_Ref[6] =
    {
        VTK_DOUBLE_MAX, VTK_DOUBLE_MIN,
        VTK_DOUBLE_MAX, VTK_DOUBLE_MIN,
        VTK_DOUBLE_MAX, VTK_DOUBLE_MIN
    };

    for (int i = 0; i < INDEX_PRUEBA; ++i)
    {
        vtkSmartPointer<vtkImageData> currentImage = vtkSmartPointer<vtkImageData>::New();
        currentImage = readPNGImages("/media/datos/ETSIT/PFC/pseudotumor/VTS_01_1", i);
        int *frameExtent = currentImage->GetExtent();
        qDebug() << "Extent reslice" << frameExtent[0] << frameExtent[1] << frameExtent[2] << frameExtent[3] << frameExtent[4] << frameExtent[5];
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

        //currentImage->Delete();
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
    this->outputExtent[1] = int((extent_Ref[1] - extent_Ref[0]) / outputSpacing[0]);
    this->outputExtent[3] = int((extent_Ref[3] - extent_Ref[2]) / outputSpacing[1]);
    this->outputExtent[5] = int((extent_Ref[5] - extent_Ref[4]) / outputSpacing[2]);

    //this->outputExtent[0] = int(extent_Ref[0]);
    //this->outputExtent[2] = int(extent_Ref[2]);
    //this->outputExtent[4] = int(extent_Ref[4]);
    //this->outData->SetExtent(this->outputExtent);
    this->ReconstructedVolume->SetOrigin(extent_Ref[0], extent_Ref[2], extent_Ref[4]);
    double* outputOrigin = this->ReconstructedVolume->GetOrigin();

    this->sliceAdder->setOutputExtent(this->outputExtent);
    this->sliceAdder->setOutputOrigin(outputOrigin);
    this->sliceAdder->setOutputSpacing(outputSpacing);
    qDebug() << "Volume origin:" << extent_Ref[0] << extent_Ref[2] << extent_Ref[4];
    qDebug() << "Extent size: " << outputExtent[0] << outputExtent[1] << outputExtent[2] << outputExtent[3] << outputExtent[4] << outputExtent[5];
    qDebug() << "Extent size: " << extent_Ref[0] << extent_Ref[1] << extent_Ref[2] << extent_Ref[3] << extent_Ref[4] << extent_Ref[5];

    if (this->sliceAdder->resetOutput() != 1)
    {
        qDebug() << "Error, failed to initialize output volume";
    }
}

void VolumeReconstructor::recalculateExtent()
{
    double extent_Ref[6] =
    {
        VTK_DOUBLE_MAX, VTK_DOUBLE_MIN,
        VTK_DOUBLE_MAX, VTK_DOUBLE_MIN,
        VTK_DOUBLE_MAX, VTK_DOUBLE_MIN
    };

    int *frameExtent = this->sliceAdder->getReconstructedVolume()->GetExtent();
    qDebug() << "Extent reslice" << frameExtent[0] << frameExtent[1] << frameExtent[2] << frameExtent[3] << frameExtent[4] << frameExtent[5];
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

    //currentImage->Delete();
    // transform the corners to Reference and expand the extent if needed
    for (unsigned int corner = 0; corner < corners_ImagePix.size(); ++corner)
    {
        double corner_Ref[ 4 ] = { 0, 0, 0, 1 }; // position of the corner in the Reference coordinate system
        this->finalTransform->MultiplyPoint(corners_ImagePix[corner], corner_Ref);

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

    double* outputSpacing = this->ReconstructedVolume->GetSpacing();
    this->outputExtent[1] = int((extent_Ref[1] - extent_Ref[0]) / outputSpacing[0]);
    this->outputExtent[3] = int((extent_Ref[3] - extent_Ref[2]) / outputSpacing[1]);
    this->outputExtent[5] = int((extent_Ref[5] - extent_Ref[4]) / outputSpacing[2]);

    //this->outputExtent[0] = int(extent_Ref[0]);
    //this->outputExtent[2] = int(extent_Ref[2]);
    //this->outputExtent[4] = int(extent_Ref[4]);
    //this->outData->SetExtent(this->outputExtent);
    this->ReconstructedVolume->SetOrigin(extent_Ref[0], extent_Ref[2], extent_Ref[4]);
    double* outputOrigin = this->ReconstructedVolume->GetOrigin();

    this->sliceAdder->setOutputExtent(this->outputExtent);
    this->sliceAdder->setOutputOrigin(outputOrigin);
    this->sliceAdder->setOutputSpacing(outputSpacing);

    qDebug() << "Extent size: " << outputExtent[0] << outputExtent[1] << outputExtent[2] << outputExtent[3] << outputExtent[4] << outputExtent[5];
    qDebug() << "Extent size: " << extent_Ref[0] << extent_Ref[1] << extent_Ref[2] << extent_Ref[3] << extent_Ref[4] << extent_Ref[5];
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
    writer->SetFileName("outVolVID1.mha");
    //writer->SetRAWFileName("outVol.raw");
    writer->Write();
}
