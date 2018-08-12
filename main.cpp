#include <QCoreApplication>

#include "Element.h"
#include "LazyComputationManager.h"
#include <QDebug>

void useElem(Element &elem)
{
    elem.update();
    qDebug() << "use " << elem.str();
}

void dumpElem(Element &elem)
{
    qDebug() << "dump: " << elem.str();
}


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    LazyComputationManager::getInstance()->setMaxThread(5);

    qDebug() << ">>>>>>>>>> Test EasyEvaluation 1";
    Element elemSrc(28);
    dumpElem(elemSrc);
    elemSrc.computeEvolution(38);
    dumpElem(elemSrc);

    // Potentially doing other stuff
    Element elem2 = elemSrc.clone();

    elem2.computeEvolution(42);
    dumpElem(elem2);

    for (ushort i=0; i<5; ++i)
    {
        qDebug() << "Main Thread doing stuff...";
        QThread::sleep(1);
    }

    dumpElem(elem2);
    useElem(elem2);
    elem2.computeEvolution(35);
    elem2.computeEvolution(36);
    useElem(elem2);


    qDebug() << "\n\n\nTest creating/deleting 10 temporary Elements with a scheduled evolution";
    for (ushort i=0 ; i < 10 ; ++i)
    {
        Element elemTmp = elem2.clone();
        elemTmp.computeEvolution(42);
    }
    qDebug() << "<<<<<<<<<<< Test EasyEvaluation 1";
    LazyComputationManager::getInstance()->dumpStatistics(true);


    return a.exec();
}
