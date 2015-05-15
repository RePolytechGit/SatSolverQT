#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include <fstream>
#include <QString>
#include <vector>
#include <sstream>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
       solution=new Sat_Solver;
    ui->setupUi(this);
    ui->label_2->setText("Result");
    ui->label_3->setText("CALCULATING");
    ui->textEdit->setText(QString::fromLocal8Bit("Введите выражение в формате  (x1+x2)*(!x2+x3)"));
    ui->label_3->hide();
    ui->label_2->hide();
    ui->pushButton_2->hide();
    ui->textEdit_2->hide();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    if(   ui->checkBox->isChecked())
    {
       QString file = QFileDialog::getOpenFileName(this,QString::fromLocal8Bit("Выберите файл"),"","Dimacs files(*.cnf)");
       if(file.isEmpty()){
           QMessageBox::warning(this,"error",QString::fromLocal8Bit("Файл не выбран!"));
           return;
       }
       std::string filename=file.toLocal8Bit().constData();
       std::ifstream fin(filename.c_str());
       if (fin.is_open())
       {
           int per = 0, mnoj = 0, a;
           char b;
           string start;
           while (!fin.eof() && (start != "p cnf"))
           {
               fin.get(b);
               start.push_back(b);
               if (b == '\n')
                   start.clear();
           }

           fin >> per >> mnoj;
           solution->variables_size = per;
           solution->function_size = mnoj;
           solution->variables = new variable[solution->variables_size];
           for (int j = 0; j < per; j++)
               solution->variables[j].determined = false;
           solution->function = new clauses[solution->function_size];
           for (int j = 0; j < mnoj; j++)
           {
               solution->function[j].checker = false;
           }
           int i = 0;
           clauses* supp = NULL;
           while (fin >> a)
           {
               if (a != 0)
               {
                   if (a > 0)
                   {
                       solution->function[i].posliterals.push_back(a - 1);
                       supp = &solution->function[i];
                       solution->variables[a - 1].poslist.push_back(supp);
                   }
                   else
                   {
                       a *= (-1);
                       solution->function[i].negliterals.push_back(a - 1);
                       supp = &solution->function[i];
                       solution->variables[a - 1].neglist.push_back(supp);
                   }
               }
               else
                   i += 1;
           }
       }
    }
    else{

        QString func=ui->textEdit->toPlainText();
        if(func.isEmpty())
        {
            QMessageBox::warning(this,"error",QString::fromLocal8Bit("Функция не введена!"));
            return;
        }
        int number_of_var=0;
        std::vector<vector<int>> brackets;
        std::vector<int> bracket;
        std::string supp=func.toLocal8Bit().constData();
       int a=0;
       int k,i=0;
       while(i<supp.size())
       {
           if('0'<=supp[i]&&supp[i]<='9')
           {
               k=i;
           while('0'<=supp[i]&&supp[i]<='9')
           {
               a*=10;
               a+=supp[i]-'0';
               i++;
           }

           if(supp[k-2]=='!')
               bracket.push_back((-1)*a);
           else
               bracket.push_back(a);

        if(a>number_of_var)
            number_of_var=a;
           a=0;
           }
           if(supp[i]=='*'){
               brackets.push_back(bracket);
               bracket.clear();
           }
           i++;
        }
       if(bracket.size())
       brackets.push_back(bracket);
       solution->variables_size = number_of_var;
       solution->function_size = brackets.size();
       solution->variables = new variable[solution->variables_size];
       for (int j = 0; j < number_of_var; j++)
       solution->variables[j].determined = false;
       solution->function = new clauses[solution->function_size];
       for (int j = 0; j < solution->function_size; j++)
       {
           solution->function[j].checker = false;
       }

       clauses* support = NULL;

       k=0;

       for(int i = 0;i<brackets.size();i++){
           for(int a=0; a<brackets[i].size();a++)
           {
       if (brackets[i][a] > 0)
       {
           k=brackets[i][a];
           solution->function[i].posliterals.push_back(k - 1);
           support = &solution->function[i];
           solution->variables[k - 1].poslist.push_back(support);
       }
       else
       {
           k=brackets[i][a];
           k *= (-1);
           solution->function[i].negliterals.push_back(k - 1);
           support = &solution->function[i];
           solution->variables[k - 1].neglist.push_back(support);
       }
           }
       }
    }
    ui->textEdit->hide();
    ui->checkBox->hide();
    ui->pushButton->hide();
    ui->label->hide();
    ui->label_3->show();
   if( solution->solver())
   {
       QString text;

       for(int i=0;i<solution->variables_size;i++)
       {
           if(solution->variables[i].determined)
             if(  ! solution->variables[i].value)

           {
               text.append(" !x");
               text.append(QString::number(i+1));
           }
           else
           {
               text.append(" x");
               text.append(QString::number(i+1));
           }
       }
       ui->label_3->hide();
      ui->textEdit_2->setText(text);
     ui->label_2->show();
     ui->pushButton_2->show();
     ui->textEdit_2->show();
   }
   else
   {
       ui->label_3->hide();
      ui->textEdit_2->setText("UNSATISFIABLE");
     ui->label_2->show();
     ui->pushButton_2->show();
     ui->textEdit_2->show();
   }


}



void MainWindow::on_textEdit_selectionChanged()
{
    ui->textEdit->clear();
}

void MainWindow::on_pushButton_2_clicked()
{
    ui->label_2->hide();
    ui->pushButton_2->hide();
    ui->textEdit_2->hide();
    ui->label->show();
    ui->pushButton->show();
    ui->textEdit->show();
    ui->checkBox->show();
    ui->textEdit->setText(QString::fromLocal8Bit("Введите выражение в формате  (x1+x2)*(!x2+x3)"));

}
