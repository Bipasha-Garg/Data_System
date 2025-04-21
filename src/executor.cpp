#include "global.h"

void executeCommand()
{

    switch (parsedQuery.queryType)
    {
    case CLEAR:
        executeCLEAR();
        break;
    case CROSS:
        executeCROSS();
        break;
    case DISTINCT:
        executeDISTINCT();
        break;
    case EXPORT:
        executeEXPORT();
        break;
    case EXPORT_MATRIX:
        executeMatrixEXPORT();
        break;
    case INDEX:
        executeINDEX();
        break;
    case JOIN:
        executeJOIN();
        break;
    case LIST:
        executeLIST();
        break;
    case LOAD:
        executeLOAD();
        break;
    case LOAD_MATRIX:
        executeMatrixLOAD();
        break;
    case PRINT:
        executePRINT();
        break;
    case PRINT_MATRIX:
        executeMatrixPRINT();
        break;
    case PROJECTION:
        executePROJECTION();
        break;
    case SELECTION:
        executeSELECTION();
        break;
    case SORT:
        executeSORT();
        break;
    case SOURCE:
        executeSOURCE();
        break;
    case CHECKSYMMETRY:
        executeMatrixCHECKSYMMETRY();
        break;
    case CHECKANTISYM:
        executeMatrixCHECKANTISYMMETRY();
        break;
    case ROTATE_MATRIX:
        executeROTATE();
        break;
    case CROSS_TRANSPOSE:
        executeCROSSTRANSPOSE();
        break;
    case INPLACE_SORT:
        executeInplaceSORT();
        break;
    case ORDER_BY:
        executeORDERBY();
        break;
    case GROUP_BY:
        executeGROUPBY();
        break;
    case SEARCH:
        executeSEARCH();
        break;
    case UPDATE:
        executeUPDATE();
        break;
    case INSERT:
        executeINSERT();
        break;
    case DELETE:
        executeDELETE();
        break;
    default:
        cout << "PARSING ERROR" << endl;
    }

    return;
}

void printRowCount(int rowCount)
{
    cout << "\n\nRow Count: " << rowCount << endl;
    return;
}