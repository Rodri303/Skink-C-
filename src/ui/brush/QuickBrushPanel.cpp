#include "ui/brush/QuickBrushPanel.hpp"
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
namespace Skink::Ui::Brush { QuickBrushPanel::QuickBrushPanel(QWidget* parent):QFrame(parent){setObjectName("floatingPanel");setFixedSize(330,254);auto* layout=new QVBoxLayout(this);layout->setContentsMargins(0,0,0,0);layout->setSpacing(0);auto* header=new QWidget(this);header->setObjectName("panelHeader");header->setFixedHeight(46);auto* h=new QHBoxLayout(header);h->setContentsMargins(20,0,20,0);auto* title=new QLabel("PINCEL RAPIDO",header);title->setObjectName("panelTitle");h->addWidget(title);h->addStretch();layout->addWidget(header);auto* host=new QWidget(this);auto* grid=new QGridLayout(host);grid->setContentsMargins(12,12,12,12);grid->setSpacing(8);const QString names[]={"Tinta transparente","Marcador","Lapiz","Aerografo"};for(int i=0;i<4;++i){auto* card=new QPushButton(names[i]+"\n\n----------",host);card->setObjectName(i==1?"quickCardSelected":"quickCard");card->setCheckable(true);card->setChecked(i==1);card->setMinimumHeight(92);grid->addWidget(card,i/2,i%2);connect(card,&QPushButton::clicked,this,[this,names,i]{emit presetSelected(names[i]);});}layout->addWidget(host,1);} }
