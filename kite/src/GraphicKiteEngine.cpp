// -*- C++ -*-
//
// This file is part of the Coriolis Software.
// Copyright (c) UPMC/LIP6 2008-2014, All Rights Reserved
//
// +-----------------------------------------------------------------+
// |                   C O R I O L I S                               |
// |      K i t e  -  D e t a i l e d   R o u t e r                  |
// |                                                                 |
// |  Author      :                    Jean-Paul Chaput              |
// |  E-mail      :            Jean-Paul.Chaput@lip6.fr              |
// | =============================================================== |
// |  C++ Header  :       "./GraphicKiteEngine.cpp"                  |
// +-----------------------------------------------------------------+


#include <boost/bind.hpp>
#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QApplication>
#include <hurricane/Warning.h>
#include <hurricane/Error.h>
#include <hurricane/Breakpoint.h>
#include <hurricane/DebugSession.h>
#include <hurricane/Go.h>
#include <hurricane/Net.h>
#include <hurricane/Cell.h>
#include <hurricane/UpdateSession.h>
#include <hurricane/viewer/Graphics.h>
#include <hurricane/viewer/CellWidget.h>
#include <hurricane/viewer/CellViewer.h>
#include <hurricane/viewer/ControllerWidget.h>
#include <hurricane/viewer/ExceptionWidget.h>
#include <crlcore/Utilities.h>
#include <crlcore/AllianceFramework.h>
#include <katabatic/GCell.h>
#include <katabatic/GCellGrid.h>
#include <knik/Edge.h>
#include <knik/Vertex.h>
#include <knik/KnikEngine.h>
#include <knik/GraphicKnikEngine.h>
#include <kite/GraphicKiteEngine.h>
//#include <kite/ConfigurationWidget.h>


namespace Kite {

  using namespace std;
  using Hurricane::Error;
  using Hurricane::Warning;
  using Hurricane::Exception;
  using Hurricane::Breakpoint;
  using Hurricane::DebugSession;
  using Hurricane::UpdateSession;
  using Hurricane::Point;
  using Hurricane::Net;
  using Hurricane::Graphics;
  using Hurricane::ColorScale;
  using Hurricane::ControllerWidget;
  using Hurricane::ExceptionWidget;
  using CRL::Catalog;
  using CRL::AllianceFramework;
  using Knik::KnikEngine;
  using Knik::GraphicKnikEngine;


  size_t             GraphicKiteEngine::_references = 0;
  GraphicKiteEngine* GraphicKiteEngine::_singleton  = NULL;


  void  GraphicKiteEngine::initKatabaticAc ( CellWidget* widget )
  { }


  void  GraphicKiteEngine::drawKatabaticAc ( CellWidget*           widget
                                           , const Go*             go
                                           , const BasicLayer*     basicLayer
                                           , const Box&            box
                                           , const Transformation& transformation
                                           )
  { }


  void  GraphicKiteEngine::initKatabaticGCell ( CellWidget* widget )
  {
    widget->getDrawingPlanes().setPen( Qt::NoPen );

    KiteEngine* kite = KiteEngine::get( widget->getCell() );
    if ( kite ) {
      kite->getGCellGrid()->setDensityMode( Katabatic::GCellGrid::MaxDensity );
    }
  }


  void  GraphicKiteEngine::drawKatabaticGCell ( CellWidget*           widget
                                              , const Go*             go
                                              , const BasicLayer*     basicLayer
                                              , const Box&            box
                                              , const Transformation& transformation
                                              )
  {
    const Katabatic::GCell* gcell = static_cast<const Katabatic::GCell*>(go);

    QPainter& painter = widget->getPainter();
    size_t    density = (size_t)( gcell->getDensity() * 255.0 );
    if (density > 255) density = 255;

    painter.setBrush
      ( Graphics::getColorScale(ColorScale::Fire).getBrush(density,widget->getDarkening()) );
    painter.drawRect
      ( widget->dbuToDisplayRect(gcell->getBoundingBox().inflate(0
                                                                ,0
                                                                ,gcell->getTopRightShrink()
                                                                ,gcell->getTopRightShrink())) );
  }


  KiteEngine* GraphicKiteEngine::createEngine ()
  {
    Cell* cell = getCell ();

    KiteEngine* kite = KiteEngine::get( cell );
    if (not kite) {
      kite = KiteEngine::create( cell );
      kite->setPostEventCb( boost::bind(&GraphicKiteEngine::postEvent,this) );
      kite->setViewer( _viewer );
    } else
      cerr << Warning( "%s already has a Kite engine.", getString(cell).c_str() ) << endl;

    return kite;
  }


  KiteEngine* GraphicKiteEngine::getForFramework ( unsigned int flags )
  {
  // Currently, only one framework is avalaible: Alliance.

    KiteEngine* kite = KiteEngine::get( getCell() );
    if (kite) return kite;

    if (flags & CreateEngine) {
      kite = createEngine();
      if (not kite) 
        throw Error( "Failed to create Kite engine on %s.", getString(getCell()).c_str() );
    } else {
      throw Error( "KiteEngine not created yet, run the global router first." );
    }

    return kite;
  }


  void  GraphicKiteEngine::_loadGlobalSolution ()
  {
    KiteEngine* kite = getForFramework( CreateEngine );
    kite->runGlobalRouter( KtLoadGlobalRouting );
  }


  void  GraphicKiteEngine::_saveGlobalSolution ()
  {
    KiteEngine* kite = KiteEngine::get( getCell() );
    if (kite) kite->saveGlobalSolution ();
  }


  void  GraphicKiteEngine::_globalRoute ()
  {
    KiteEngine* kite = getForFramework( CreateEngine );
    kite->runGlobalRouter( KtBuildGlobalRouting );
  }


  void  GraphicKiteEngine::_loadGlobalRouting ()
  {
    static KatabaticEngine::NetSet routingNets;

    KiteEngine* kite = getForFramework( NoFlags );
    if (cmess1.enabled()) kite->printConfiguration();

    _viewer->clearToolInterrupt();
    kite->loadGlobalRouting( Katabatic::EngineLoadGrByNet, routingNets );
  }


  void  GraphicKiteEngine::_balanceGlobalDensity ()
  {
    KiteEngine* kite = getForFramework( NoFlags );
    kite->balanceGlobalDensity ();
    kite->layerAssign ( Katabatic::EngineNoNetLayerAssign );
  }


  void  GraphicKiteEngine::_runNegociate ()
  {
    KiteEngine* kite = getForFramework( NoFlags );
    kite->runNegociate();
  }


  void  GraphicKiteEngine::_finalize ()
  {
    KiteEngine* kite = getForFramework( NoFlags );
    if (kite) {
      kite->finalizeLayout();
      kite->destroy();
    }
  }


  void  GraphicKiteEngine::_save ()
  {
    Cell*              cell = getCell();
    AllianceFramework* af   = AllianceFramework::get();

    string name = getString(cell->getName()) + "_kite";
    cell->setName( name );
    af->saveCell( cell, Catalog::State::Physical );
  }


  void  GraphicKiteEngine::globalRoute ()
  { ExceptionWidget::catchAllWrapper( std::bind(&GraphicKiteEngine::_globalRoute,this) ); }


  void  GraphicKiteEngine::loadGlobalSolution ()
  { ExceptionWidget::catchAllWrapper( std::bind(&GraphicKiteEngine::_loadGlobalSolution,this) ); }


  void  GraphicKiteEngine::saveGlobalSolution ()
  { ExceptionWidget::catchAllWrapper( std::bind(&GraphicKiteEngine::_saveGlobalSolution,this) ); }


  void  GraphicKiteEngine::detailRoute ()
  {
    ExceptionWidget::catchAllWrapper( std::bind(&GraphicKiteEngine::_loadGlobalRouting   ,this) );
    ExceptionWidget::catchAllWrapper( std::bind(&GraphicKiteEngine::_balanceGlobalDensity,this) );
    ExceptionWidget::catchAllWrapper( std::bind(&GraphicKiteEngine::_runNegociate        ,this) );
  }


  void  GraphicKiteEngine::finalize ()
  { ExceptionWidget::catchAllWrapper( std::bind(&GraphicKiteEngine::_finalize,this) ); }


  void  GraphicKiteEngine::save ()
  { ExceptionWidget::catchAllWrapper( std::bind(&GraphicKiteEngine::_save,this) ); }


  void  GraphicKiteEngine::route ()
  {
    globalRoute();
    detailRoute();
    finalize   ();
  }


  void  GraphicKiteEngine::dumpMeasures ()
  {
    KiteEngine* kite = getForFramework( NoFlags );
    if (kite) kite->dumpMeasures();
  }


  void  GraphicKiteEngine::postEvent ()
  {
    static unsigned int count = 0;

    if (not (count++ % 500)) {
      QApplication::processEvents();

      if (_viewer->isToolInterrupted()) {
        KiteEngine* kite = KiteEngine::get( getCell() );
        if (kite) kite->setInterrupt( true );
        _viewer->clearToolInterrupt();
      }
    }
  }


  void  GraphicKiteEngine::addToMenu ( CellViewer* viewer )
  {
    assert( _viewer == NULL );

    _viewer = viewer;

    QMenu* prMenu   = _viewer->findChild<QMenu*>("viewer.menuBar.placeAndRoute");
    QMenu* stepMenu = _viewer->findChild<QMenu*>("viewer.menuBar.placeAndRoute.stepByStep");
    if (prMenu == NULL) {
      QMenuBar* menuBar = _viewer->findChild<QMenuBar*>("viewer.menuBar");
      if (menuBar == NULL) {
        cerr << Warning( "GraphicKiteEngine::addToMenu() - No MenuBar in parent widget." ) << endl;
        return;
      }
      prMenu = menuBar->addMenu( tr("P&&R") );
      prMenu->setObjectName( "viewer.menuBar.placeAndRoute" );

      stepMenu = prMenu->addMenu( tr("&Step by Step") );
      stepMenu->setObjectName( "viewer.menuBar.placeAndRoute.stepByStep" );

      prMenu->addSeparator();
    }

    QAction* dRouteAction = _viewer->findChild<QAction*>("viewer.menuBar.placeAndRoute.detailedRoute");
    if (dRouteAction)
      cerr << Warning( "GraphicKiteEngine::addToMenu() - Kite detailed router already hooked in." ) << endl;
    else {
      stepMenu->addSeparator();

      QAction* gRouteAction = new QAction ( tr("Kite - &Global Route"), _viewer );
      gRouteAction->setObjectName( "viewer.menuBar.placeAndRoute.stepBystep.globalRoute" );
      gRouteAction->setStatusTip ( tr("Run the <b>Knik</b> global router") );
      gRouteAction->setVisible   ( true );
      stepMenu->addAction( gRouteAction );

      QAction* gLoadSolutionAction = new QAction ( tr("Kite - &Load Global Routing"), _viewer );
      gLoadSolutionAction->setObjectName( "viewer.menuBar.placeAndRoute.stepByStep.loadGlobalRouting" );
      gLoadSolutionAction->setStatusTip ( tr("Load a solution for the global routing (.kgr)") );
      gLoadSolutionAction->setVisible   ( true );
      stepMenu->addAction( gLoadSolutionAction );

      QAction* gSaveSolutionAction = new QAction ( tr("Kite - &Save Global Routing"), _viewer );
      gSaveSolutionAction->setObjectName( "viewer.menuBar.placeAndRoute.stepByStep.saveGlobalRouting" );
      gSaveSolutionAction->setStatusTip ( tr("Save a global router solution (.kgr)") );
      gSaveSolutionAction->setVisible   ( true );
      stepMenu->addAction( gSaveSolutionAction );

      dRouteAction = new QAction ( tr("Kite - &Detailed Route"), _viewer );
      dRouteAction->setObjectName( "viewer.menuBar.placeAndRoute.stepBystep.detailedRoute" );
      dRouteAction->setStatusTip ( tr("Run the <b>Kite</b> detailed router") );
      dRouteAction->setVisible   ( true );
      stepMenu->addAction( dRouteAction );

      QAction* dFinalizeAction = new QAction( tr("Kite - &Finalize Routing"), _viewer );
      dFinalizeAction->setObjectName( "viewer.menuBar.placeAndRoute.stepBystep.finalize" );
      dFinalizeAction->setStatusTip ( tr("Closing Routing") );
      dFinalizeAction->setVisible   ( true );
      stepMenu->addAction( dFinalizeAction );

      QAction* dDumpMeasuresAction = new QAction ( tr("Kite - Dump &Measures"), _viewer );
      dDumpMeasuresAction->setObjectName( "viewer.menuBar.placeAndRoute.stepBystep.dumpMeasures" );
      dDumpMeasuresAction->setStatusTip ( tr("Dumping Measurements on the disk") );
      dDumpMeasuresAction->setVisible   ( true );
      stepMenu->addAction( dDumpMeasuresAction );

      QAction* dSaveAction = new QAction ( tr("Kite - &Save Design"), _viewer );
      dSaveAction->setObjectName( "viewer.menuBar.placeAndRoute.stepBystep.save" );
      dSaveAction->setStatusTip ( tr("Save routed design (temporary hack)") );
      dSaveAction->setVisible   ( true );
      stepMenu->addAction( dSaveAction );

      QAction* routeAction = new QAction ( tr("Kite - &Route"), _viewer );
      routeAction->setObjectName( "viewer.menuBar.placeAndRoute.route" );
      routeAction->setStatusTip ( tr("Route the design (global & detailed)") );
      routeAction->setVisible   ( true );
      prMenu->addAction( routeAction );

      connect( gLoadSolutionAction, SIGNAL(triggered()), this, SLOT(loadGlobalSolution()) );
      connect( gSaveSolutionAction, SIGNAL(triggered()), this, SLOT(saveGlobalSolution()) );
      connect( gRouteAction       , SIGNAL(triggered()), this, SLOT(globalRoute       ()) );
      connect( dRouteAction       , SIGNAL(triggered()), this, SLOT(detailRoute       ()) );
      connect( dFinalizeAction    , SIGNAL(triggered()), this, SLOT(finalize          ()) );
      connect( dSaveAction        , SIGNAL(triggered()), this, SLOT(save              ()) );
      connect( dDumpMeasuresAction, SIGNAL(triggered()), this, SLOT(dumpMeasures      ()) );
      connect( routeAction        , SIGNAL(triggered()), this, SLOT(route             ()) );
    }
  }


  const Name& GraphicKiteEngine::getName () const
  { return KiteEngine::staticGetName(); }


  Cell* GraphicKiteEngine::getCell ()
  {
    if (_viewer == NULL) {
      throw Error( "<b>Kite:</b> GraphicKiteEngine not bound to any Viewer." );
      return NULL;
    }

    if (_viewer->getCell() == NULL) {
      throw Error( "<b>Kite:</b> No Cell is loaded into the Viewer." );
      return NULL;
    }

    return _viewer->getCell();
  }


  GraphicKiteEngine* GraphicKiteEngine::grab ()
  {
    if (not _references) {
      _singleton = new GraphicKiteEngine ();
    }
    _references++;

    return _singleton;
  }


  size_t  GraphicKiteEngine::release ()
  {
    --_references;
    if (not _references) {
      delete _singleton;
      _singleton = NULL;
    }
    return _references;
  }


  GraphicKiteEngine::GraphicKiteEngine ()
    : GraphicTool()
    , _viewer    (NULL)
  {
    addDrawGo( "Knik::Edge"      , GraphicKnikEngine::initKnikEdges , GraphicKnikEngine::drawKnikEdges  );
    addDrawGo( "Knik::Vertex"    , GraphicKnikEngine::initKnikVertex, GraphicKnikEngine::drawKnikVertex );
    addDrawGo( "Katabatic::Ac"   , initKatabaticAc   , drawKatabaticAc    );
    addDrawGo( "Katabatic::GCell", initKatabaticGCell, drawKatabaticGCell );
  }


  GraphicKiteEngine::~GraphicKiteEngine ()
  { }


}  // Kite namespace.
