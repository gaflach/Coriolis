

// -*- C++ -*-
//
// This file is part of the Coriolis Project.
// Copyright (C) Laboratoire LIP6 - Departement ASIM
// Universite Pierre et Marie Curie
//
// Main contributors :
//        Christophe Alexandre   <Christophe.Alexandre@lip6.fr>
//        Sophie Belloeil             <Sophie.Belloeil@lip6.fr>
//        Hugo Cl�ment                   <Hugo.Clement@lip6.fr>
//        Jean-Paul Chaput           <Jean-Paul.Chaput@lip6.fr>
//        Damien Dupuis                 <Damien.Dupuis@lip6.fr>
//        Christian Masson           <Christian.Masson@lip6.fr>
//        Marek Sroka                     <Marek.Sroka@lip6.fr>
// 
// The  Coriolis Project  is  free software;  you  can redistribute it
// and/or modify it under the  terms of the GNU General Public License
// as published by  the Free Software Foundation; either  version 2 of
// the License, or (at your option) any later version.
// 
// The  Coriolis Project is  distributed in  the hope that it  will be
// useful, but WITHOUT ANY WARRANTY; without even the implied warranty
// of MERCHANTABILITY  or FITNESS FOR  A PARTICULAR PURPOSE.   See the
// GNU General Public License for more details.
// 
// You should have  received a copy of the  GNU General Public License
// along with the Coriolis Project; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA
//
// License-Tag
// Authors-Tag
//
// ===================================================================
//
// $Id$
//
// x-----------------------------------------------------------------x
// |                                                                 |
// |                  H U R R I C A N E                              |
// |     V L S I   B a c k e n d   D a t a - B a s e                 |
// |                                                                 |
// |  Author      :                    Jean-Paul Chaput              |
// |  E-mail      :            Jean-Paul.Chaput@lip6.fr              |
// | =============================================================== |
// |  C++ Header  :       "./TransistorLayer.h"                      |
// | *************************************************************** |
// |  U p d a t e s                                                  |
// |                                                                 |
// x-----------------------------------------------------------------x


# ifndef  __HURRICANE_TRANSISTOR_LAYER__
# define  __HURRICANE_TRANSISTOR_LAYER__

# include  <vector>

# include  "hurricane/Layer.h"
# include  "hurricane/TransistorLayers.h"


namespace Hurricane {


  class TransistorLayer : public Layer {

    public:
    // Constructor.
      static  TransistorLayer*     create            ( Technology* technology
                                                     , const Name& name
                                                     , BasicLayer* gateLayer
                                                     , BasicLayer* activeLayer
                                                     , BasicLayer* diffusionLayer
                                                     , BasicLayer* wellLayer
                                                     );
    // Accessors.
      virtual BasicLayers          getBasicLayers    () const;
      virtual Unit                 getExtentionCap   () const;
      virtual Unit                 getExtentionWidth () const;
      virtual Unit                 getExtentionCap   ( const BasicLayer* layer ) const;
      virtual Unit                 getExtentionWidth ( const BasicLayer* layer ) const;
    // Updators.
      virtual void                 setExtentionCap   ( const BasicLayer* layer, Unit cap );
      virtual void                 setExtentionWidth ( const BasicLayer* layer, Unit width );
    // Hurricane Managment.
      virtual string               _getTypeName      () const;
      virtual string               _getString        () const;
      virtual Record*              _getRecord        () const;

    private:
    // Internal: Attributes
              vector<BasicLayer*>  _basicLayers;
              vector<Unit>         _extentionCaps;
              vector<Unit>         _extentionWidths;
              Unit                 _maximalExtentionCap;
              Unit                 _maximalExtentionWidth;

    protected:
    // Internal: Constructors & Destructors.
                                   TransistorLayer   ( Technology* technology
                                                     , const Name& name
                                                     , BasicLayer* gateLayer
                                                     , BasicLayer* activeLayer
                                                     , BasicLayer* diffusionLayer
                                                     , BasicLayer* wellLayer
                                                     );
  };


} // End of Hurricane namespace.


SetNestedSlotAdapter(Hurricane::TransistorLayer)


# endif
