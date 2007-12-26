// ****************************************************************************************************
// File: QuadTree.h
// Authors: R. Escassut
// Copyright (c) BULL S.A. 2000-2004, All Rights Reserved
// ****************************************************************************************************

#ifndef HURRICANE_QUAD_TREE
#define HURRICANE_QUAD_TREE

#include "Box.h"
#include "Gos.h"
#include "IntrusiveSet.h"

namespace Hurricane {



// ****************************************************************************************************
// QuadTree declaration
// ****************************************************************************************************

class QuadTree {
// Types
// *****

	public: class GoSet : public IntrusiveSet<Go> {
	// *******************************************

		public: typedef IntrusiveSet<Go> Inherit;
	
		public: GoSet();

		public: virtual unsigned _GetHashValue(Go* go) const;
		public: virtual Go* _GetNextElement(Go* go) const;
		public: virtual void _SetNextElement(Go* go, Go* nextGo) const;

	};

// Attributes
// **********

	private: QuadTree* _parent;
	private: Unit _x;
	private: Unit _y;
	private: Box _boundingBox;
	private: unsigned _size;
	private: GoSet _goSet;
	private: QuadTree* _ulChild; // Upper Left Child
	private: QuadTree* _urChild; // Upper Right Child
	private: QuadTree* _llChild; // Lower Left Child
	private: QuadTree* _lrChild; // Lower Right Child

// Constructors
// ************

	public: QuadTree();

	private: QuadTree(QuadTree* parent);

	private: QuadTree(const QuadTree& quadTree); // not implemented to forbid copy construction

// Destructor
// **********

	public: ~QuadTree();

// Operators
// *********

	private: QuadTree& operator=(const QuadTree& quadTree); // not implemented to forbid assignment

// Accessors
// *********

	public: const Box& GetBoundingBox() const;
	public: Gos GetGos() const;
	public: Gos GetGosUnder(const Box& area) const;

// Predicates
// **********

	public: bool IsEmpty() const {return (_size == 0);};

// Updators
// ********

	public: void Insert(Go* go);
	public: void Remove(Go* go);

// Others
// ******

    public: string _GetTypeName() const { return _TName("QuadTree"); };
	public: string _GetString() const;
	public: Record* _GetRecord() const;

	public: GoSet& _GetGoSet() {return _goSet;};
	public: QuadTree* _GetDeepestChild(const Box& box);
	public: QuadTree* _GetFirstQuadTree() const;
	public: QuadTree* _GetFirstQuadTree(const Box& area) const;
	public: QuadTree* _GetNextQuadTree();
	public: QuadTree* _GetNextQuadTree(const Box& area);

	public: bool _HasBeenExploded() const {return (_ulChild != NULL);};

	public: void _Explode();
	public: void _Implode();

};


} // End of Hurricane namespace.

#endif // HURRICANE_QUAD_TREE

// ****************************************************************************************************
// Copyright (c) BULL S.A. 2000-2004, All Rights Reserved
// ****************************************************************************************************
