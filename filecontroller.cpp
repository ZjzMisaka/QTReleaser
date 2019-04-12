#include "filecontroller.h"

FileController::FileController()
{
    needStopThread = false;
}

void FileController::findFileInPath(QString path, QString fileName)
{
    QDir d(path);        //此处修改遍历文件夹地址
    d.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks | QDir::AllDirs);//列出文件,列出隐藏文件（在Unix下就是以.开始的为文件）,不列出符号链接（不支持符号连接的操作系统会忽略）
    d.setSorting(QDir::Size | QDir::Reversed);//按文件大小排序，相反的排序顺序
    QFileInfoList list = d.entryInfoList();//返回这个目录中所有目录和文件的QFileInfo对象的列表
    while(!list.isEmpty() && !needStopThread)
    {
        QFileInfo tem= list.last();
        if(!tem.isDir())
        {
            QString filePath = tem.filePath();
            bool isNameSame;
            if(tem.fileName() == fileName)
            {
                isNameSame = true;
            }
            else
            {
                isNameSame = false;
            }
            emit setSchedule(filePath, isNameSame);
            list.removeLast();
        }
        else if(tem.fileName() != "." && tem.fileName() != "..")
        {
            QDir a(tem.filePath());
            list.removeLast();          //移除链表最后一项方便退回
            list.append(a.entryInfoList());
        }
        else
        {
            list.removeLast();
        }
    }
    emit finishAutoSet();
    needStopThread = false;
}

void FileController::stopThread()
{
    needStopThread = true;
}
