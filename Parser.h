/*
 * Explicar
 * 
 * */

#ifndef __Parser_h
#define __Parser_h

#include <QObject>
#include <iterator>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string.h>
#include <iomanip>
#include <vtkPoints.h>
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>


class Parser : public QObject
{
    Q_OBJECT
public:
    explicit Parser(QObject *parent = 0);

private:
    double tx;
    double ty;
    double tz;
    double rx;
    double ry;
    double rz;
    bool isFirst;
    int i;

signals:

public slots:
    void parseTSVFile(QString inputTSV, vtkPoints *positions, vtkPoints *directions);
};


#endif