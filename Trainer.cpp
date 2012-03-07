#include "Trainer.h"

Trainer::Trainer(ControlPanel *parent, bool simple) :
    QWidget(0)
{
    trainerModerator = new Moderator();
    simpleLayout = new QVBoxLayout();
    this->goButton = new QPushButton("GO");
    this->numTrialsLabel = new QLabel("Number of trials:");
    this->numTrialsInput = new QLineEdit();
    simpleLayout->addWidget(numTrialsLabel);
    simpleLayout->addWidget(numTrialsInput);
    simpleLayout->addWidget(goButton);
    this->setLayout(simpleLayout);
    this->show();
}

Trainer::~Trainer()
{

}
void Trainer::closeEvent(QCloseEvent *event){
    this->stop();
}
void Trainer::stop(){
    // kill ALL of the children processes!
}
void Trainer::pause(){
    //pause running those trials
}
void Trainer::resume(){
    //resume running those trials
}
void Trainer::start(){

}
