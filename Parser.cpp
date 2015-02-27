#include "Parser.h"
#include <QDebug>

using namespace std;

Parser::Parser(QObject *parent) :
    QObject(parent)
{
    this->isFirst = true;
    this->i = 0;
    this->tx = 0;
    this->ty = 0;
    this->tz = 0;
    this->rx = 0;
    this->ry = 0;
    this->rz = 0;
}

void Parser::parseTSVFile(QString inputFile, vtkPoints* positions, vtkPoints* directions)
{
    QByteArray byteArray = inputFile.toUtf8();
    const char* cString = byteArray.constData();
    
    std::cout << std::setprecision(9);

    std::ifstream input(cString);
    if (!input.is_open()) return;

    positions->SetDataTypeToDouble();

    typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
    boost::char_separator<char> sep("\t");

    string line;
    while (getline(input, line))
    {
        if(this->isFirst == true)
        {
            this->isFirst = false;
            continue;
        }
        tokenizer tok(line,sep);
        for (tokenizer::iterator tok_iter = tok.begin(); tok_iter != tok.end(); ++tok_iter)
        {
            string str = *tok_iter;
            // For some reason this is taking ',' as decimal separator, locales issue
            //std::replace(str.begin(),str.end(),',','.');
            double w = atof(str.c_str());

            if (this->i == 5)
            {
                // Reading Rz
                //std::cout << "<" << w << "> ";
                this->rz = w;
            }
            if (this->i == 6)
            {
                // Reading Ry
                //std::cout << "<" << w << "> ";
                this->ry = w;
            }
            if (this->i == 7)
            {
                // Reading Rz
                //std::cout << "<" << w << "> ";
                this->rx = w;
            }
            if (this->i == 8)
            {
                // Reading Tx
                //std::cout << "<" << w << "> ";
                this->tx = w;
            }
            if (this->i == 9)
            {
                // Reading Ty
                //std::cout << "<" << w << "> ";
                this->ty = w;
            }
            if (this->i == 10)
            {
                // Reading Tz
                //std::cout << "<" << w << "> ";
                this->tz = w;
            }
            this->i++;
        }
        double point[3] = {this->tx, this->ty, this->tz};
        double angle[3] = {this->rx, this->ry, this->rz};

        positions->InsertNextPoint(point);
        qDebug() << "positions" << point[0] << point[1] << point[2];
        directions->InsertNextPoint(angle);
        qDebug() << "directions" << angle[0] << angle[1] << angle[2];
        //std::cout << "\n";
        this->i = 0;
    }
    input.close();
}
