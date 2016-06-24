#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "define.h"
#include <QFileDialog>
#include <stdio.h>
#include <iostream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_TilesViewScale(TILESVIEWSCALE),
    m_MapViewScale(MAPVIEWSCALE),
    m_SelectedTile(nullptr),
    m_Config(new Config())
{
    ui->setupUi(this);
    CleanButtons();
    handleLevel1();

    m_TileSet = new TileSet(m_SelectedTile);
    m_Map = new Map(m_TileSet, m_Config);
    if (!m_TileSet->SetTileSetImg("tiles.png"))
        ui->statusBar->showMessage("Error: Loading TileSet Img");


    ui->m_GraphicsViewTiles->setScene(m_TileSet);
    ui->m_GraphicsViewTiles->scale(m_TilesViewScale, m_TilesViewScale);
    if (!m_TileSet->LoadTileToScene())
        ui->statusBar->showMessage("Error: Loading Tiles to scene");
    m_TileSet->DrawGrid();

    ui->m_GraphicsViewMap->setScene(m_Map);
    ui->m_GraphicsViewMap->scale(m_MapViewScale, m_MapViewScale);
    m_Map->SetXMap(ui->m_XSizeField->value());
    m_Map->SetYMap(ui->m_YSizeField->value());
    m_Map->DrawGrid();

    // Connect button signal to appropriate slot
    connect(ui->m_TileSetButtonAdd, SIGNAL (clicked()), this, SLOT (handleButtonAddTileSet()));
    connect(ui->m_TileSetButtonSub, SIGNAL (clicked()), this, SLOT (handleButtonSubTileSet()));
    connect(ui->m_ButtonAddMap, SIGNAL (clicked()), this, SLOT (handleButtonAddMap()));
    connect(ui->m_ButtonSubMap, SIGNAL (clicked()), this, SLOT (handleButtonSubMap()));
    connect(ui->m_ButtonAddOpMap, SIGNAL (clicked()), this, SLOT (handleButtonAddOpMap()));
    connect(ui->m_ButtonSubOpMap, SIGNAL (clicked()), this, SLOT (handleButtonSubOpMap()));
    connect(ui->m_Level1, SIGNAL (clicked()), this, SLOT (handleLevel1()));
    connect(ui->m_Level2, SIGNAL (clicked()), this, SLOT (handleLevel2()));
    connect(ui->m_Level3, SIGNAL (clicked()), this, SLOT (handleLevel3()));
    connect(ui->m_Level4, SIGNAL (clicked()), this, SLOT (handleLevel4()));
    connect(ui->m_YSizeField,SIGNAL(valueChanged(int)),this,SLOT(setYMap(int)));
    connect(ui->m_XSizeField,SIGNAL(valueChanged(int)),this,SLOT(setXMap(int)));
    connect(ui->m_YSizeField,SIGNAL(valueChanged(int)),this,SLOT(setYMap(int)));
    connect(ui->actionExporterMap, SIGNAL(triggered()), this, SLOT(exportMap()));

    ui->statusBar->showMessage("Loading Success");
}

void MainWindow::setXMap(int p_X)
{
    m_Map->SetXMap(p_X);
    m_Map->DrawGrid();
}

void MainWindow::setYMap(int p_Y)
{
    m_Map->SetYMap(p_Y);
    m_Map->DrawGrid();
}


void MainWindow::handleButtonAddTileSet()
{
    if (m_TilesViewScale + 0.2f >= 4.0f)
        return;

    m_TilesViewScale = m_TilesViewScale + 0.2f;
    ui->m_GraphicsViewTiles->setTransform(QTransform::fromScale(m_TilesViewScale, m_TilesViewScale));
    ui->statusBar->showMessage("Resize TileSet *" + QString::number(m_TilesViewScale));
}

void MainWindow::handleButtonSubTileSet()
{
    if (m_TilesViewScale - 0.2f <= 0.4f)
        return;

    m_TilesViewScale = m_TilesViewScale - 0.2f;
    ui->m_GraphicsViewTiles->setTransform(QTransform::fromScale(m_TilesViewScale, m_TilesViewScale));
    ui->statusBar->showMessage("Resize TileSet *" + QString::number(m_TilesViewScale));
}

void MainWindow::handleButtonAddMap()
{
    if (m_MapViewScale + 0.2f >= 4.0f)
        return;

    m_MapViewScale = m_MapViewScale + 0.2f;
    ui->m_GraphicsViewMap->setTransform(QTransform::fromScale(m_MapViewScale, m_MapViewScale));
    ui->statusBar->showMessage("Resize Map *" + QString::number(m_MapViewScale));
}

void MainWindow::handleButtonSubMap()
{
    if (m_MapViewScale - 0.2f <= 0.2f)
        return;

    m_MapViewScale = m_MapViewScale - 0.2f;
    ui->m_GraphicsViewMap->setTransform(QTransform::fromScale(m_MapViewScale, m_MapViewScale));
    ui->statusBar->showMessage("Resize Map *" + QString::number(m_MapViewScale));
}

void MainWindow::handleButtonAddOpMap()
{
    float l_Opacity = m_Map->GetOpacityGrid();

    if (l_Opacity + 0.1f > 1.0f)
        return;

    m_Map->SetOpacityGrid(l_Opacity + 0.1f);
}

void MainWindow::handleButtonSubOpMap()
{
    float l_Opacity = m_Map->GetOpacityGrid();

    if (l_Opacity - 0.1f < 0.0f)
        return;

    m_Map->SetOpacityGrid(l_Opacity - 0.1f);
}


void MainWindow::CleanButtons()
{
    ui->m_Level1->setStyleSheet("background-color: #FFF");
    ui->m_Level2->setStyleSheet("background-color: #FFF");
    ui->m_Level3->setStyleSheet("background-color: #FFF");
    ui->m_Level4->setStyleSheet("background-color: #FFF");
}

void MainWindow::handleLevel1()
{
    m_Config->SetTileLevel(1);
    CleanButtons();
    ui->m_Level1->setStyleSheet("background-color: #00ff00");
}


void MainWindow::handleLevel2()
{
    m_Config->SetTileLevel(2);
    CleanButtons();
    ui->m_Level2->setStyleSheet("background-color: #00ff00;");
}

void MainWindow::handleLevel3()
{
    m_Config->SetTileLevel(3);
    CleanButtons();
    ui->m_Level3->setStyleSheet("background-color: #00ff00;");
}

void MainWindow::handleLevel4()
{
    m_Config->SetTileLevel(4);
    CleanButtons();
    ui->m_Level4->setStyleSheet("background-color: #00ff00;");
}

void MainWindow::exportMap()
{
     const char* l_FileName = QFileDialog::getSaveFileName(this, tr("Save Map"), "/map.map", tr("Map (*.map)")).toStdString().c_str();

     FILE * pFile;
     pFile = fopen (l_FileName, "wb");

     for (int i = 0; i < m_Map->GetMapSize(); ++i)
     {
         Case* l_Case = m_Map->GetCase(i);
         int l_TabTileNb[l_Case->GetMaxTileLevel()];

         t_Case l_FluxCase;
         for (int j = 0; j < l_Case->GetMaxTileLevel(); ++j)
         {
            Tile* l_Tile = l_Case->GetTile(j);
            if (l_Tile == nullptr)
                l_FluxCase.l_TabTileNb[j] = -1;
            else
                l_FluxCase.l_TabTileNb[j] = l_Tile->GetID();
         }
         l_FluxCase.l_Block = false;
        fwrite(&l_FluxCase, sizeof(l_FluxCase), 1, pFile);
     }

     fclose (pFile);
}

MainWindow::~MainWindow()
{
    delete ui;
}
