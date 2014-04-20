// ****************************************************************************************************
// File: ./Cell.cpp
// Authors: R. Escassut
// Copyright (c) BULL S.A. 2000-2009, All Rights Reserved
//
// This file is part of Hurricane.
//
// Hurricane is free software: you can redistribute it  and/or  modify it under the  terms  of the  GNU
// Lesser General Public License as published by the Free Software Foundation, either version 3 of  the
// License, or (at your option) any later version.
//
// Hurricane is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without  even
// the implied warranty of MERCHANTABILITY or FITNESS FOR A  PARTICULAR  PURPOSE. See  the  Lesser  GNU
// General Public License for more details.
//
// You should have received a copy of the Lesser GNU General Public License along  with  Hurricane.  If
// not, see <http://www.gnu.org/licenses/>.
// ****************************************************************************************************

//#define  TEST_INTRUSIVESET

#include "hurricane/SharedName.h"
#include "hurricane/Cell.h"
#include "hurricane/DataBase.h"
#include "hurricane/Library.h"
#include "hurricane/Instance.h"
#include "hurricane/Net.h"
#include "hurricane/Pin.h"
#include "hurricane/RoutingPad.h"
#include "hurricane/Layer.h"
#include "hurricane/Slice.h"
#include "hurricane/Rubber.h"
#include "hurricane/Marker.h"
#include "hurricane/Component.h"
#include "hurricane/UpdateSession.h"
#include "hurricane/Error.h"

namespace Hurricane {


  void  Cell::_insertSlice ( ExtensionSlice* slice )
  {
    ExtensionSliceMap::iterator islice = _extensionSlices.find ( slice->getName() );
    if ( islice != _extensionSlices.end() )
      throw Error ( "Attempt to re-create ExtensionSlice %s in Cell %s."
                  , getString(slice->getName()).c_str()
                  , getString(slice->getCell()->getName()).c_str()
                  );

    _extensionSlices.insert ( pair<Name,ExtensionSlice*>(slice->getName(),slice) );
  }


  void  Cell::_removeSlice ( ExtensionSlice* slice )
  {
    ExtensionSliceMap::iterator islice = _extensionSlices.find ( slice->getName() );
    if ( islice != _extensionSlices.end() ) {
      _extensionSlices.erase ( islice );
    }
  }


  ExtensionSlice* Cell::getExtensionSlice ( const Name& name ) const
  {
    ExtensionSliceMap::const_iterator islice = _extensionSlices.find ( name );
    if ( islice != _extensionSlices.end() )
      return islice->second;

    return NULL;
  }


  ExtensionSlice::Mask  Cell::getExtensionSliceMask ( const Name& name ) const
  {
    ExtensionSliceMap::const_iterator islice = _extensionSlices.find ( name );
    if ( islice != _extensionSlices.end() )
      return islice->second->getMask();

    return 0;
  }


// ****************************************************************************************************
// Cell implementation
// ****************************************************************************************************

Cell::Cell(Library* library, const Name& name)
// *******************************************
:    Inherit(),
    _library(library),
    _name(name),
    _instanceMap(),
    _quadTree(),
    _slaveInstanceSet(),
    _netMap(),
    _sliceMap(),
    _extensionSlices(),
    _markerSet(),
    //_viewSet(),
    _abutmentBox(),
    _boundingBox(),
    _isTerminal(true),
    _isPad(false),
    _nextOfLibraryCellMap(NULL),
    _nextOfSymbolCellSet(NULL),
    _slaveEntityMap(),
    _observers()
{
    if (!_library)
        throw Error("Can't create " + _TName("Cell") + " : null library");

    if (name.isEmpty())
        throw Error("Can't create " + _TName("Cell") + " : empty name");

    if (_library->getCell(_name))
        throw Error("Can't create " + _TName("Cell") + " " + getString(_name) + " : already exists");
}

Cell* Cell::create(Library* library, const Name& name)
// ***************************************************
{
    Cell* cell = new Cell(library, name);

    cell->_postCreate();

    return cell;
}

Box Cell::getBoundingBox() const
// *****************************
{
    if (_boundingBox.isEmpty()) {
        Box& boundingBox = (Box&)_boundingBox;
        boundingBox = _abutmentBox;
        boundingBox.merge(_quadTree.getBoundingBox());
        for_each_slice(slice, getSlices()) {
            boundingBox.merge(slice->getBoundingBox());
            end_for;
        }
    }
    
    return _boundingBox;
}

bool Cell::isLeaf() const
// **********************
{
    return _instanceMap.isEmpty();
}

bool Cell::isCalledBy(Cell* cell) const
// ************************************
{
    for_each_instance(instance, cell->getInstances()) {
        Cell* masterCell = instance->getMasterCell();
        if (masterCell == this) return true;
        if (isCalledBy(masterCell)) return true;
        end_for;
    }
    return false;
}

void Cell::setName(const Name& name)
// *********************************
{
    if (name != _name) {
        if (name.isEmpty())
            throw Error("Can't change " + _TName("Cell") + " name : empty name");

        if (_library->getCell(name))
            throw Error("Can't change " + _TName("Cell") + " name : already exists");

        _library->_getCellMap()._remove(this);
        _name = name;
        _library->_getCellMap()._insert(this);
    }
}

void Cell::setAbutmentBox(const Box& abutmentBox)
// **********************************************
{
    if (abutmentBox != _abutmentBox) {
        if (!_abutmentBox.isEmpty() &&
             (abutmentBox.isEmpty() || !abutmentBox.contains(_abutmentBox)))
            _unfit(_abutmentBox);
        _abutmentBox = abutmentBox;
        _fit(_abutmentBox);
    }
}

void Cell::flattenNets(unsigned int flags)
// ***************************************
{
  UpdateSession::open();

  forEach ( Occurrence, ioccurrence, getHyperNetRootNetOccurrences() ) {
    Net* net = static_cast<Net*>((*ioccurrence).getEntity());

    HyperNet  hyperNet ( *ioccurrence );
    if ( not (*ioccurrence).getPath().isEmpty() ) {
      DeepNet* deepNet = DeepNet::create( hyperNet );
      if (deepNet) deepNet->_createRoutingPads( flags );
    } else {
      RoutingPad* previousRp = NULL;
      RoutingPad* currentRp  = NULL;
      bool        buildRing  = false;

      if (net->isGlobal()) {
        if      ( (flags & Cell::BuildClockRings ) and net->isClock () ) buildRing = true;
        else if ( (flags & Cell::BuildSupplyRings) and net->isSupply() ) buildRing = true;
      } else {
        buildRing = flags & Cell::BuildRings;
      }

      forEach ( Component*, icomponent, net->getComponents() ) {
        Plug* primaryPlug = dynamic_cast<Plug*>( *icomponent );
        if (primaryPlug) {
          if ( !primaryPlug->getBodyHook()->getSlaveHooks().isEmpty() ) {
            cerr << "[ERROR] " << primaryPlug << "\n"
                 << "        has attached components, not managed yet." << endl;
          } else {
            primaryPlug->getBodyHook()->detach();
          }
        }
      }

      forEach ( Occurrence, iplugOccurrence, hyperNet.getLeafPlugOccurrences() ) {
        currentRp = RoutingPad::create( net, *iplugOccurrence, RoutingPad::BiggestArea );
        currentRp->materialize();

        if (flags & WarnOnUnplacedInstances)
          currentRp->isPlacedOccurrence( RoutingPad::ShowWarning );

        if (buildRing) {
          if (previousRp) {
            currentRp->getBodyHook()->attach( previousRp->getBodyHook() );
          }
          Plug* plug = static_cast<Plug*>( (*iplugOccurrence).getEntity() );
          if ( (*iplugOccurrence).getPath().isEmpty() ) {
            plug->getBodyHook()->attach( currentRp->getBodyHook() );
            plug->getBodyHook()->detach();
          }
          previousRp = currentRp;
        }
      }

      forEach ( Component*, icomponent, net->getComponents() ) {
        Pin* pin = dynamic_cast<Pin*>( *icomponent );
        if (pin) {
          currentRp = RoutingPad::create( pin );
          if (buildRing) {
            if (previousRp) {
              currentRp->getBodyHook()->attach( previousRp->getBodyHook() );
            }
            pin->getBodyHook()->attach( currentRp->getBodyHook() );
            pin->getBodyHook()->detach();
          }
          previousRp = currentRp;
        }
      }
    }
  }

  UpdateSession::close();
}

void Cell::materialize()
// *********************
{
  forEach ( Instance*, iinstance, getInstances() ) {
    if ( iinstance->getPlacementStatus() != Instance::PlacementStatus::UNPLACED )
      iinstance->materialize();
  }

  forEach ( Net*   , inet   , getNets   () ) inet   ->materialize();
  forEach ( Marker*, imarker, getMarkers() ) imarker->materialize();
}

void Cell::unmaterialize()
// ***********************
{
    for_each_instance(instance, getInstances()) instance->unmaterialize(); end_for;
    for_each_net(net, getNets()) net->unmaterialize(); end_for;
    for_each_marker(marker, getMarkers()) marker->unmaterialize(); end_for;
}

void Cell::_postCreate()
// *********************
{
    Inherit::_postCreate();

    _library->_getCellMap()._insert(this);
}

void Cell::_preDestroy()
// ********************
{
    while(_slaveEntityMap.size()) {
      _slaveEntityMap.begin()->second->destroy();
    }

    //for_each_view(view, getViews()) view->SetCell(NULL); end_for;
    for_each_marker(marker, getMarkers()) marker->destroy(); end_for;
    for_each_instance(slaveInstance, getSlaveInstances()) slaveInstance->destroy(); end_for;
    for_each_instance(instance, getInstances()) instance->destroy(); end_for;
    for_each_net(net, getNets()) net->destroy(); end_for;
    for_each_slice(slice, getSlices()) slice->_destroy(); end_for;
    while(!_extensionSlices.empty()) _removeSlice(_extensionSlices.begin()->second);

    _library->_getCellMap()._remove(this);

    Inherit::_preDestroy();
}

string Cell::_getString() const
// ****************************
{
    string s = Inherit::_getString();
    s.insert(s.length() - 1, " " + getString(_name));
    return s;
}

Record* Cell::_getRecord() const
// ***********************
{
    Record* record = Inherit::_getRecord();
    if (record) {
        record->add(getSlot("Library", _library));
        record->add(getSlot("Name", &_name));
        record->add(getSlot("Instances", &_instanceMap));
        record->add(getSlot("QuadTree", &_quadTree));
        record->add(getSlot("SlaveInstances", &_slaveInstanceSet));
        record->add(getSlot("Nets", &_netMap));
        record->add(getSlot("Pins", &_pinMap));
        record->add(getSlot("Slices", &_sliceMap));
        record->add(getSlot("Markers", &_markerSet));
        record->add(getSlot("SlaveEntityMap", &_slaveEntityMap));
        record->add(getSlot("AbutmentBox", &_abutmentBox));
        record->add(getSlot("BoundingBox", &_boundingBox));
        record->add(getSlot("isTerminal", &_isTerminal));
        record->add(getSlot("isFlattenLeaf", &_isFlattenLeaf));
    }
    return record;
}

void Cell::_fit(const Box& box)
// ****************************
{
    if (box.isEmpty()) return;
    if (_boundingBox.isEmpty()) return;
    if (_boundingBox.contains(box)) return;
    _boundingBox.merge(box);
    for_each_instance(instance, getSlaveInstances()) {
        instance->getCell()->_fit(instance->getTransformation().getBox(box));
        end_for;
    }
}

void Cell::_unfit(const Box& box)
// ******************************
{
    if (box.isEmpty()) return;
    if (_boundingBox.isEmpty()) return;
    if (!_boundingBox.isConstrainedBy(box)) return;
    _boundingBox.makeEmpty();
    for_each_instance(instance, getSlaveInstances()) {
        instance->getCell()->_unfit(instance->getTransformation().getBox(box));
        end_for;
    }
}

void Cell::_addSlaveEntity(Entity* entity, Entity* slaveEntity)
// ************************************************************************
{
  assert(entity->getCell() == this);

  _slaveEntityMap.insert(pair<Entity*,Entity*>(entity,slaveEntity));
}

void Cell::_removeSlaveEntity(Entity* entity, Entity* slaveEntity)
// ***************************************************************************
{
  assert(entity->getCell() == this);

  pair<SlaveEntityMap::iterator,SlaveEntityMap::iterator>
    bounds = _slaveEntityMap.equal_range(entity);
  SlaveEntityMap::iterator it = bounds.first;
  for(; it != bounds.second ; it++ ) {
    if (it->second == slaveEntity) {
      _slaveEntityMap.erase(it);
      break;
    }
  }
}

void Cell::_getSlaveEntities(SlaveEntityMap::iterator& begin, SlaveEntityMap::iterator& end)
// *********************************************************************************************************
{
  begin = _slaveEntityMap.begin();
  end   = _slaveEntityMap.end();
}

void Cell::_getSlaveEntities(Entity* entity, SlaveEntityMap::iterator& begin, SlaveEntityMap::iterator& end)
// *********************************************************************************************************
{
  begin = _slaveEntityMap.lower_bound(entity);
  end   = _slaveEntityMap.upper_bound(entity);
}

void Cell::addObserver(BaseObserver* observer)
// *******************************************
{
  _observers.addObserver(observer);
}

void Cell::removeObserver(BaseObserver* observer)
// **********************************************
{
  _observers.removeObserver(observer);
}

void Cell::notify(unsigned flags)
// ******************************
{
  _observers.notify(flags);
}

// ****************************************************************************************************
// Cell::InstanceMap implementation
// ****************************************************************************************************

Cell::InstanceMap::InstanceMap()
// *****************************
:    Inherit()
{
}

Name Cell::InstanceMap::_getKey(Instance* instance) const
// ******************************************************
{
    return instance->getName();
}

unsigned int  Cell::InstanceMap::_getHashValue(Name name) const
// *******************************************************
{
  return name._getSharedName()->getId() / 8;
}

Instance* Cell::InstanceMap::_getNextElement(Instance* instance) const
// *******************************************************************
{
    return instance->_getNextOfCellInstanceMap();
}

void Cell::InstanceMap::_setNextElement(Instance* instance, Instance* nextInstance) const
// **************************************************************************************
{
    instance->_setNextOfCellInstanceMap(nextInstance);
}



// ****************************************************************************************************
// Cell::SlaveInstanceSet implementation
// ****************************************************************************************************

Cell::SlaveInstanceSet::SlaveInstanceSet()
// ***************************************
:    Inherit()
{
}

unsigned Cell::SlaveInstanceSet::_getHashValue(Instance* slaveInstance) const
// **************************************************************************
{
  return slaveInstance->getId() / 8;
}

Instance* Cell::SlaveInstanceSet::_getNextElement(Instance* slaveInstance) const
// *****************************************************************************
{
    return slaveInstance->_getNextOfCellSlaveInstanceSet();
}

void Cell::SlaveInstanceSet::_setNextElement(Instance* slaveInstance, Instance* nextSlaveInstance) const
// ****************************************************************************************************
{
    slaveInstance->_setNextOfCellSlaveInstanceSet(nextSlaveInstance);
}



// ****************************************************************************************************
// Cell::NetMap implementation
// ****************************************************************************************************

Cell::NetMap::NetMap()
// *******************
:    Inherit()
{
}

Name Cell::NetMap::_getKey(Net* net) const
// ***************************************
{
    return net->getName();
}

unsigned Cell::NetMap::_getHashValue(Name name) const
// **************************************************
{
  return (unsigned int)name._getSharedName()->getId() / 8;
}

Net* Cell::NetMap::_getNextElement(Net* net) const
// ***********************************************
{
    return net->_getNextOfCellNetMap();
}

void Cell::NetMap::_setNextElement(Net* net, Net* nextNet) const
// *************************************************************
{
    net->_setNextOfCellNetMap(nextNet);
}


// ****************************************************************************************************
// Cell::PinMap implementation
// ****************************************************************************************************

Cell::PinMap::PinMap()
// *******************
:    Inherit()
{
}

Name Cell::PinMap::_getKey(Pin* pin) const
// ***************************************
{
    return pin->getName();
}

unsigned Cell::PinMap::_getHashValue(Name name) const
// **************************************************
{
  return (unsigned int)name._getSharedName()->getId() / 8;
}

Pin* Cell::PinMap::_getNextElement(Pin* pin) const
// ***********************************************
{
    return pin->_getNextOfCellPinMap();
}

void Cell::PinMap::_setNextElement(Pin* pin, Pin* nextPin) const
// *************************************************************
{
    pin->_setNextOfCellPinMap(nextPin);
}


// ****************************************************************************************************
// Cell::SliceMap implementation
// ****************************************************************************************************

Cell::SliceMap::SliceMap()
// ***********************
:    Inherit()
{
}

const Layer* Cell::SliceMap::_getKey(Slice* slice) const
// *****************************************************
{
    return slice->getLayer();
}

unsigned Cell::SliceMap::_getHashValue(const Layer* layer) const
// *************************************************************
{
  return (unsigned int)layer->getMask() / 8;
}

Slice* Cell::SliceMap::_getNextElement(Slice* slice) const
// *******************************************************
{
    return slice->_getNextOfCellSliceMap();
}

void Cell::SliceMap::_setNextElement(Slice* slice, Slice* nextSlice) const
// ***********************************************************************
{
    slice->_setNextOfCellSliceMap(nextSlice);
};



// ****************************************************************************************************
// Cell::MarkerSet implementation
// ****************************************************************************************************

Cell::MarkerSet::MarkerSet()
// *************************
:    Inherit()
{
}

unsigned Cell::MarkerSet::_getHashValue(Marker* marker) const
// **********************************************************
{
  return (unsigned int)marker->getId() / 8;
}

Marker* Cell::MarkerSet::_getNextElement(Marker* marker) const
// ***********************************************************
{
    return marker->_getNextOfCellMarkerSet();
}

void Cell::MarkerSet::_setNextElement(Marker* marker, Marker* nextMarker) const
// ****************************************************************************
{
    marker->_setNextOfCellMarkerSet(nextMarker);
}

} // End of Hurricane namespace.


// ****************************************************************************************************
// Copyright (c) BULL S.A. 2000-2009, All Rights Reserved
// ****************************************************************************************************
