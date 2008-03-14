
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
// ===================================================================
//
// $Id: PyDataBase.cpp,v 1.1 2008/02/07 19:09:57 xtof Exp $
//
// x-----------------------------------------------------------------x 
// |                                                                 |
// |                   C O R I O L I S                               |
// |    I s o b a r  -  Hurricane / Python Interface                 |
// |                                                                 |
// |  Author      :                    Jean-Paul CHAPUT              |
// |  E-mail      :       Jean-Paul.Chaput@asim.lip6.fr              |
// | =============================================================== |
// |  C++ Module  :       "./PyCataBase.cpp"                         |
// | *************************************************************** |
// |  U p d a t e s                                                  |
// |                                                                 |
// x-----------------------------------------------------------------x




# include  "PyDataBase.h"
# include  "PyTechnology.h"


namespace  Isobar {


USING_NAMESPACE_HURRICANE

extern "C" {


# define  METHOD_HEAD(function)   GENERIC_METHOD_HEAD(DataBase,db,function)


// x=================================================================x
// |             "PyDataBase" Python Module Code Part               |
// x=================================================================x

# if defined(__PYTHON_MODULE__)


  // x-------------------------------------------------------------x
  // |             "PyDataBase" Attribute Methods                 |
  // x-------------------------------------------------------------x




  // ---------------------------------------------------------------
  // Attribute Method  :  "PyDataBase_GetDataBase ()"

  PyObject* PyDataBase_GetDataBase ( PyObject* module ) {
    trace << "PyDataBase_GetDataBase()" << endl;

    DataBase* db = NULL;

    HTRY
    db = GetDataBase ();
    if ( db == NULL )
      PyErr_SetString ( HurricaneError, "DataBase has not been created yet" );
    HCATCH

    return PyDataBase_Link ( db );
  }

  // ---------------------------------------------------------------
  // Attribute Method  :  "PyDataBase_GetTechnology ()"

  PyObject* PyDataBase_GetTechnology ( PyDataBase* self ) {
    trace << "PyDataBase_GetTechnology()" << endl;

    Technology* techno = NULL;

    HTRY
    METHOD_HEAD("DataBase.GetTechnology()")

    techno = db->GetTechnology ();
    
    if ( techno == NULL )
        Py_RETURN_NONE;
    HCATCH

    return PyTechnology_Link ( techno );
  }

  
  // Standart Accessors (Attributes).


  // Standart Delete (Attribute).
  DBoDeleteAttribute(PyDataBase_Delete,PyDataBase)




  // ---------------------------------------------------------------
  // PyDataBase Attribute Method table.

  PyMethodDef PyDataBase_Methods[] =
    { { "GetTechnology", (PyCFunction)PyDataBase_GetTechnology, METH_NOARGS, "Return the Technology" }
    , { "Delete"       , (PyCFunction)PyDataBase_Delete       , METH_NOARGS
                       , "Delete associated hurricane object The python object remains." }
    , {NULL, NULL, 0, NULL}           /* sentinel */
    };




  // x-------------------------------------------------------------x
  // |               "PyDataBase" Object Methods                  |
  // x-------------------------------------------------------------x
  // ---------------------------------------------------------------
  // Attribute Method  :  "PyDataBase_new ()"

  static PyObject* PyDataBase_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
    trace << "PyDataBase_new()" << endl;

    DataBase* db = NULL;
    
    HTRY
    db = DataBase::Create ();
    HCATCH

    return PyDataBase_Link(db);
  }

  DBoDeleteMethod(DataBase)
  PyTypeObjectLinkPyType(DataBase)
  PyTypeObjectConstructor(DataBase)

#else  // End of Python Module Code Part.


// x=================================================================x
// |               "PyCell" Shared Library Code Part                 |
// x=================================================================x



  // Link/Creation Method.

  DBoLinkCreateMethod(PyDataBase_Link,PyDataBase,PyTypeDataBase,DataBase)

  // ---------------------------------------------------------------
  // PyDataBase Object Definitions.
      
  PyTypeRootObjectDefinitions(DataBase)

#endif  // End of Shared Library Code Part.


}  // End of extern "C".




}  // End of Isobar namespace.
 