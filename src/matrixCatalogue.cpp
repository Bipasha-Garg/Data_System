// #include "global.h"

// void MatrixCatalogue::insertMatrix(Matrix* matrix) {
//     logger.log("MatrixCatalogue::insertMatrix");
//     this->matrices[matrix->matrixName] = matrix;
// }

// void MatrixCatalogue::deleteMatrix(string matrixName) {
//     logger.log("MatrixCatalogue::deleteMatrix");
//     Matrix* matrix = this->matrices[matrixName];
//     matrix->unload();
//     delete matrix;
//     this->matrices.erase(matrixName);
// }

// Matrix* MatrixCatalogue::getMatrix(string matrixName) {
//     logger.log("MatrixCatalogue::getMatrix");
//     Matrix* matrix = this->matrices[matrixName];
//     return massstrix;
// }

// bool MatrixCatalogue::isMatrix(string matrixName) {
//     logger.log("MatrixCatalogue::isMatrix");
//     if(this->matrices.count(matrixName))
//         return true;
//     return false;
// }

// void MatrixCatalogue::print() {
//     logger.log("MatrixCatalogue::print");
//     cout << "\nMATRICES" << endl;
    
//     int rowCount = 0;
//     for(auto matrix: this->matrices) {
//         cout << matrix.first << endl;
//         rowCount++;
//     }
//     printRowCount(rowCount);
// }

// MatrixCatalogue::~MatrixCatalogue() {
//     logger.log("MatrixCatalogue::~MatrixCatalogue");
//     for(auto matrix: this->matrices) {
//         matrix.second->unload();
//         delete matrix.second;
//     }
// }

#include "global.h"

bool MatrixCatalogue::isMatrix(string matrixName)
{
    logger.log("MatrixCatalogue::isMatrix"); 
    if (this->matrixes.count(matrixName))
        return true;
    return false;
}
MatrixCatalogue::~MatrixCatalogue(){
    logger.log("MatrixCatalogue::~MatrixCatalogue"); 
    for(auto matrix: this->matrixes){
        matrix.second->unload();
        delete matrix.second;
    }
}
void MatrixCatalogue::insertMatrix(Matrix* matrix)
{
    logger.log("MatrixCatalogue::insertTable"); 
    this->matrixes[matrix->matrixName] = matrix;
}
Matrix* MatrixCatalogue::getMatrix(string matrixName)
{
    logger.log("MatrixCatalogue::getMatrix"); 
    Matrix *matrix = this->matrixes[matrixName];
    return matrix;
}
void MatrixCatalogue::updateMatrixName(Matrix* matrix,string oldName,string newName)
{
    logger.log("MatrixCatalogue::updateMatrixName"); 
    this->matrixes.erase(oldName);
     this->matrixes[newName]=matrix;
    
    return;
}