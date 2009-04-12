

// -*- C++ -*-
//
// This file is part of the Hurricane Software.
// Copyright (c) BULL S.A. 2000-2004, All Rights Reserved
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
// |  Author      :                       Remy Escassut              |
// |  E-mail      :            Jean-Paul.Chaput@lip6.fr              |
// | =============================================================== |
// |  C++ Header  :       "./BasicLayer.h"                           |
// | *************************************************************** |
// |  U p d a t e s                                                  |
// |                                                                 |
// x-----------------------------------------------------------------x


# ifndef  __HURRICANE_BASIC_LAYER__
# define  __HURRICANE_BASIC_LAYER__

# include  "hurricane/Layer.h"
# include  "hurricane/BasicLayers.h"
# include  "hurricane/Box.h"


namespace Hurricane {


  class BasicLayer : public Layer {

    public:
    // Subclass: Material.
      class Material {
        // Enum: Code.
        public:
          enum Code { nWell =0
                    , pWell
                    , nImplant
                    , pImplant
                    , active
                    , poly
                    , cut
                    , metal
                    , blockage
                    , other
                    };
        // Constructors.
                             Material             ( const Code& code = other );
                             Material             ( const Material& material );
        // Methods.
                 Material&   operator=            ( const Material& material );
          inline             operator const Code& () const;
          inline const Code& getCode              () const;
          inline string      _getTypeName         () const;
                 string      _getString           () const;
                 Record*     _getRecord           () const;

        // Internal: Attributes.
        private:
                 Code        _code;
      };

    public:
    // Constructor.
      static  BasicLayer*     create                 ( Technology*     technology
                                                     , const Name&     name
                                                     , const Material& material
                                                     , unsigned        extractNumber
                                                     , const DbU::Unit&     minimalSize    = 0
                                                     , const DbU::Unit&     minimalSpacing = 0
                                                     );
    // Accessors.                                    
      inline  const Material& getMaterial            () const;
      inline  unsigned        getExtractNumber       () const;
      virtual BasicLayers     getBasicLayers         () const;
      virtual BasicLayer*     getConnectorLayer      () const;
      virtual BasicLayer*     getObstructionLayer    () const;
      inline  const Name&     getRealName            () const;
    // Updators                                      
      inline  void            setConnectorLayer      ( BasicLayer* layer);
      inline  void            setObstructionLayer    ( BasicLayer* layer);
      inline  void            setRealName            ( const char* realName);
    // Hurricane Managment.
      virtual BasicLayer*     _getSymbolicBasicLayer ();
      virtual string          _getTypeName           () const;
      virtual string          _getString             () const;
      virtual Record*         _getRecord             () const;

    private:
    // Internal: Attributes
              Material        _material;
              unsigned        _extractNumber;
              BasicLayer*     _connectorLayer;
              BasicLayer*     _obstructionLayer;
              Name            _realName;

    protected:
    // Internal: Constructors & Destructors.
                              BasicLayer             ( Technology*      technology
                                                     , const Name&      name
                                                     , const Material&  material
                                                     , unsigned         extractNumber
                                                     , const DbU::Unit& minimalSize    = 0
                                                     , const DbU::Unit& minimalSpacing = 0
                                                     );
     virtual void             _postCreate            ();
     virtual void             _preDestroy            ();
  };


// Inline Functions.
                  BasicLayer::Material::operator const Code& () const { return _code; }
  const BasicLayer::Material::Code&
                  BasicLayer::Material::getCode              () const { return _code; }
  string          BasicLayer::Material::_getTypeName         () const { return _TName("BasicLayer::Material"); }
  const BasicLayer::Material&
                  BasicLayer::getMaterial                    () const { return _material; }
  unsigned        BasicLayer::getExtractNumber               () const { return _extractNumber; }
  const Name&     BasicLayer::getRealName                    () const { return _realName; }
  void            BasicLayer::setConnectorLayer              ( BasicLayer* layer) { _connectorLayer = layer; }
  void            BasicLayer::setObstructionLayer            ( BasicLayer* layer) { _obstructionLayer = layer; }
  void            BasicLayer::setRealName                    ( const char* realName) { _realName = realName; }


} // End of Hurricane namespace.


// -------------------------------------------------------------------
// Inspector Support for  :  BasicLayer::Material::Code.


template<>
inline std::string  getString<const Hurricane::BasicLayer::Material::Code*>
                             ( const Hurricane::BasicLayer::Material::Code* object )
{
  switch ( *object ) {
    case Hurricane::BasicLayer::Material::nWell:    return "nWell";
    case Hurricane::BasicLayer::Material::pWell:    return "pWell";
    case Hurricane::BasicLayer::Material::nImplant: return "nImplant";
    case Hurricane::BasicLayer::Material::pImplant: return "pImplant";
    case Hurricane::BasicLayer::Material::active:   return "active";
    case Hurricane::BasicLayer::Material::poly:     return "poly";
    case Hurricane::BasicLayer::Material::cut:      return "cut";
    case Hurricane::BasicLayer::Material::metal:    return "metal";
    case Hurricane::BasicLayer::Material::blockage: return "blockage";
    case Hurricane::BasicLayer::Material::other:    return "other";
  }
  return "abnormal";
}


template<>
inline Hurricane::Record* getRecord<const Hurricane::BasicLayer::Material::Code*>
                                   ( const Hurricane::BasicLayer::Material::Code* object )
{
  Hurricane::Record* record = new Hurricane::Record(getString(object));
  record->add(getSlot("Code", (unsigned int*)object));
  return record;
}


INSPECTOR_P_SUPPORT(Hurricane::BasicLayer);
INSPECTOR_P_SUPPORT(Hurricane::BasicLayer::Material);
IOSTREAM_POINTER_SUPPORT(Hurricane::BasicLayer::Material::Code);


# endif
