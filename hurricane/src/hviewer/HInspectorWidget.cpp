

#include <QFontMetrics>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QTableView>
#include <QHeaderView>
#include <QSortFilterProxyModel>
#include <QKeyEvent>
#include <QGroupBox>
#include <QVBoxLayout>

#include "hurricane/viewer/Graphics.h"
#include "hurricane/viewer/RecordModel.h"
#include "hurricane/viewer/HInspectorWidget.h"
#include "hurricane/Slot.h"


namespace Hurricane {


  HInspectorWidget::History::History ()
    : _depth(0)
    , _slots()
    , _comboBox(NULL)
  {
  }


  HInspectorWidget::History::~History ()
  {
    clear ( true );
  }


  void  HInspectorWidget::History::push ( Slot* slot )
  {
    if ( _depth < _slots.size()-1 ) {
      while ( _depth < _slots.size()-1 ) pop ();
    }

    _depth++;
    _slots.push_back ( slot->getClone() );
    _comboBox->addItem ( QString("%1: %2").arg(_depth).arg(_slots[_slots.size()-1]->getDataString().c_str()));
    _comboBox->setCurrentIndex ( _depth );

  }


  void  HInspectorWidget::History::pop ()
  {
    if ( _slots.size() > 1 ) {
      delete _slots.back ();
      _slots.pop_back ();
      _comboBox->removeItem ( _slots.size() );
      if ( _depth == _slots.size() )
        _comboBox->setCurrentIndex ( --_depth );
    }
  }


  void  HInspectorWidget::History::back ()
  {
    if ( _depth == 0 ) return;

    _comboBox->setCurrentIndex ( --_depth );
  }


  void  HInspectorWidget::History::goTo ( int depth )
  {
    if ( ( depth < 0 ) || ( depth >= (int)_slots.size() ) ) return;

    _depth = depth;
  }


  size_t  HInspectorWidget::History::getDepth () const
  {
    return _depth;
  }


  Slot* HInspectorWidget::History::getSlot () const
  {
    return _slots[_depth]->getClone();
  }


  void  HInspectorWidget::History::clear ( bool inDelete )
  {
    if ( !_slots.empty() ) {
    // Delete the rootRecord only if it'not the current viewed record.
      if ( _depth != 0 ) {
        delete _slots[0]->getDataRecord();
      }

      for ( size_t i=0 ; i < _slots.size() ; i++ )
        delete _slots[i];

      _slots.clear ();
      if ( !inDelete )
        _comboBox->clear ();
    }
  }


  void  HInspectorWidget::History::setComboBox ( QComboBox* comboBox )
  {
    assert ( comboBox  != NULL );

    _comboBox = comboBox;
  }


  void  HInspectorWidget::History::setRootRecord ( Record* rootRecord )
  {
    assert ( _comboBox  != NULL );
    assert ( rootRecord != NULL );

    clear ();

    Slot* rootSlot = ::getSlot ( "<TopLevelSlot>", rootRecord );
    _slots.push_back ( rootSlot );
    _comboBox->addItem ( QString("%1: %2").arg(_depth).arg(_slots[_slots.size()-1]->getDataString().c_str()));
    _depth = 0;
  }



  HInspectorWidget::HInspectorWidget ( Record* rootRecord, QWidget* parent )
      : QWidget(parent)
      , _recordModel(NULL)
      , _sortModel(NULL)
      , _historyComboBox(NULL)
      , _slotsView(NULL)
      , _rowHeight(20)
      , _rootRecord(rootRecord)
      , _history()
  {
    setAttribute ( Qt::WA_DeleteOnClose );

    _rowHeight = QFontMetrics(Graphics::getFixedFont()).height() + 4;

    _slotsView = new QTableView(this);
    _slotsView->setShowGrid(false);
    _slotsView->setAlternatingRowColors(true);
    _slotsView->setSelectionBehavior(QAbstractItemView::SelectRows);
    _slotsView->setSortingEnabled(true);

    QHeaderView* horizontalHeader = _slotsView->horizontalHeader ();
    horizontalHeader->setStretchLastSection ( true );
    horizontalHeader->setMinimumSectionSize ( 200 );

    QHeaderView* verticalHeader = _slotsView->verticalHeader ();
    verticalHeader->setVisible ( false );

    _historyComboBox = new QComboBox ( this );
    _history.setComboBox   ( _historyComboBox );
    _history.setRootRecord ( _rootRecord );

    _recordModel = new RecordModel ( _history.getSlot(), this );
    _sortModel   = new QSortFilterProxyModel ( this );
    _sortModel->setSourceModel       ( _recordModel );
    _sortModel->setDynamicSortFilter ( true );
    _sortModel->setFilterKeyColumn   ( 1 );

    _slotsView->setModel ( _sortModel );
    _slotsView->horizontalHeader()->setStretchLastSection ( true );
    _slotsView->resizeColumnToContents ( 0 );

    int rows = _sortModel->rowCount ();
    for ( rows-- ; rows >= 0 ; rows-- )
      _slotsView->setRowHeight ( rows, _rowHeight );
    _slotsView->selectRow ( 0 );

    _filterPatternLineEdit = new QLineEdit(this);
    QLabel* filterPatternLabel = new QLabel(tr("&Filter pattern:"), this);
    filterPatternLabel->setBuddy(_filterPatternLineEdit);

    QGridLayout* inspectorLayout = new QGridLayout();
    inspectorLayout->addWidget(_historyComboBox      , 0, 0, 1, 2);
    inspectorLayout->addWidget(_slotsView            , 1, 0, 1, 2);
    inspectorLayout->addWidget(filterPatternLabel    , 2, 0);
    inspectorLayout->addWidget(_filterPatternLineEdit, 2, 1);

    setLayout ( inspectorLayout );

    connect ( _historyComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(historyChanged(int)) );
    connect ( _filterPatternLineEdit, SIGNAL(textChanged(const QString &))
            , this                  , SLOT(textFilterChanged())
            );

    setWindowTitle(tr("Inspector"));
    resize(500, 300);
  }


  HInspectorWidget::~HInspectorWidget ()
  {
  //cerr << "HInspectorWidget::~HInspectorWidget()" << endl;
  //cerr << "Records: " << Record::getAllocateds()  << endl;
  //cerr << "Slots:   " << Slot::getAllocateds()  << endl;
  }


  void  HInspectorWidget::setRootRecord ( Record* record )
  {
    _history.setRootRecord ( record );
    _recordModel->setSlot ( _history.getSlot(), _history.getDepth() );
    _slotsView->selectRow ( 0 );
  }


  bool  HInspectorWidget::setSlot ()
  {
    bool change = true;

    change = _recordModel->setSlot ( _history.getSlot(), _history.getDepth() );
    if ( change ) {
      int rows = _sortModel->rowCount ();
      for ( rows-- ; rows >= 0 ; rows-- )
        _slotsView->setRowHeight ( rows, _rowHeight );
      _slotsView->selectRow ( 0 );
    }

    return change;
  }


  void  HInspectorWidget::pushSlot ( Slot* slot )
  {
    _history.push ( slot );
    if ( !setSlot() )
      _history.pop ();
  }


  void  HInspectorWidget::popSlot ()
  {
    _history.pop ();
    setSlot ();
  }


  void  HInspectorWidget::back ()
  {
    _history.back ();
    setSlot ();
  }


  void  HInspectorWidget::keyPressEvent(QKeyEvent *event)
  {
    if ( event->key() == Qt::Key_Right ) {
      QModelIndex index = _slotsView->currentIndex();
      if ( index.isValid() ) {
        Slot* slot = _recordModel->getRecord()->getSlot(_sortModel->mapToSource(index).row());

        if ( slot )
          pushSlot ( slot );
      }
    } else if ( event->key() == Qt::Key_Left ) {
      back ();
    } else if ( event->key() == Qt::Key_O ) {
      forkInspector ( _slotsView->currentIndex() );
    } else {
      event->ignore();
    }
  }


  void  HInspectorWidget::textFilterChanged ()
  {
    _sortModel->setFilterRegExp ( _filterPatternLineEdit->text() );
  }


  void  HInspectorWidget::historyChanged ( int depth )
  {
    _history.goTo ( depth );
    setSlot ();
  }


  void  HInspectorWidget::forkInspector ( const QModelIndex& index )
  {
    if ( index.isValid() ) {
      Slot*   slot   = _recordModel->getRecord()->getSlot(_sortModel->mapToSource(index).row());
      Record* record = slot->getDataRecord();

      if ( record ) {
        HInspectorWidget* fork = new HInspectorWidget ( record );
        fork->show ();
      }
    }
  }


} // End of Hurricane namespace.