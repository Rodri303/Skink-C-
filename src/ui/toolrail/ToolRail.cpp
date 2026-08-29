#include "ui/toolrail/ToolRail.hpp"
#include <QPushButton>
#include <QVBoxLayout>
namespace Skink::Ui::ToolRail {
ToolRail::ToolRail(QWidget* parent):QWidget(parent) { setObjectName("toolStrip"); setFixedSize(42,173); auto* layout=new QVBoxLayout(this); layout->setContentsMargins(4,6,4,6); layout->setSpacing(3); const struct { const char* text; const char* tip; } specs[]={{"B","Pincel"},{"S","Selección"},{"P","Mover"},{"T","Transformar"}}; for(int i=0;i<4;++i){auto* b=new QPushButton(specs[i].text,this);b->setObjectName(i==0?"stripButtonActive":"stripButton");b->setToolTip(specs[i].tip);b->setFixedSize(34,38); layout->addWidget(b); if(i==0)connect(b,&QPushButton::clicked,this,&ToolRail::brushRequested); if(i==1)connect(b,&QPushButton::clicked,this,&ToolRail::selectionRequested); if(i==2)connect(b,&QPushButton::clicked,this,&ToolRail::panRequested); if(i==3)connect(b,&QPushButton::clicked,this,&ToolRail::transformRequested);}}
}
