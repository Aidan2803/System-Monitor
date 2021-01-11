#include <QVBoxLayout>
#include <QPushButton>
#include <QListWidget>
#include "topnesting.h"

Layouts::Layouts(QWidget *parent)
    : QWidget(parent) {

  QVBoxLayout *vbox = new QVBoxLayout();
  QHBoxLayout *hbox = new QHBoxLayout(this);

  QListWidget *lw = new QListWidget(this);
  lw->addItem("The Omen");
  lw->addItem("The Exorcist");
  lw->addItem("Notes on a scandal");
  lw->addItem("Fargo");
  lw->addItem("Capote");

  QPushButton *add = new QPushButton("Add", this);
  QPushButton *rename = new QPushButton("Rename", this);
  QPushButton *remove = new QPushButton("Remove", this);
  QPushButton *removeall = new QPushButton("Remove All", this);

  vbox->setSpacing(3);
  vbox->addStretch(1);
  vbox->addWidget(add);
  vbox->addWidget(rename);
  vbox->addWidget(remove);
  vbox->addWidget(removeall);
  vbox->addStretch(1);

  hbox->addWidget(lw);
  hbox->addSpacing(15);
  hbox->addLayout(vbox);

  setLayout(hbox);
}
