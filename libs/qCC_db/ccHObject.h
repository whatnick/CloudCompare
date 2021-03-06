//##########################################################################
//#                                                                        #
//#                            CLOUDCOMPARE                                #
//#                                                                        #
//#  This program is free software; you can redistribute it and/or modify  #
//#  it under the terms of the GNU General Public License as published by  #
//#  the Free Software Foundation; version 2 of the License.               #
//#                                                                        #
//#  This program is distributed in the hope that it will be useful,       #
//#  but WITHOUT ANY WARRANTY; without even the implied warranty of        #
//#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         #
//#  GNU General Public License for more details.                          #
//#                                                                        #
//#          COPYRIGHT: EDF R&D / TELECOM ParisTech (ENST-TSI)             #
//#                                                                        #
//##########################################################################
//
//*********************** Last revision of this file ***********************
//$Author:: dgm                                                            $
//$Rev:: 2241                                                              $
//$LastChangedDate:: 2012-09-21 23:22:39 +0200 (ven., 21 sept. 2012)       $
//**************************************************************************
//

#ifndef CC_HIERARCHY_OBJECT_HEADER
#define CC_HIERARCHY_OBJECT_HEADER

//Local
#include "ccObject.h"
#include "ccDrawableObject.h"

//System
#include <vector>

//! Hierarchical CloudCompare Object
#ifdef QCC_DB_USE_AS_DLL
#include "qCC_db_dll.h"
class QCC_DB_DLL_API ccHObject : public ccObject, public ccDrawableObject
#else
class ccHObject : public ccObject, public ccDrawableObject
#endif
{
public:

    //! Default constructor
    /** \param name object name (optional)
    **/
    ccHObject(const char* name=0);

    //! Default destructor
    virtual ~ccHObject();

	//! Static factory
	/** Warning: objects depending on other structures (such as meshes 
		or polylines that should be linked with point clouds playing the
		role of vertices) are returned 'naked'.
		\param objectType object type (see CC_CLASS_ENUM)
		\param name object name (optional)
		\return instantiated object (if type is valid) or 0
	**/
	static ccHObject* New(unsigned objectType, const char* name=0);

    //! Returns class ID
    /** \return class unique ID
    **/
    virtual CC_CLASS_ENUM getClassID() const {return CC_HIERARCHY_OBJECT;};

    //! Returns parent object
    /** \return parent object (NULL if no parent)
    **/
    ccHObject* getParent() const;

    /*** children management ***/

    //! Adds a child to object's children list
    /** \param anObject child
        \param dependant specifies if the child object should be deleted with its parent
		\param insertIndex insertion index (if <0, item is simply appended to the children list)
    **/
    virtual void addChild(ccHObject* anObject, bool dependant=true, int insertIndex=-1);

    //! Returns the number of children
    /** \return children number
    **/
    unsigned getChildrenNumber() const;

    //! Returns the ith child
    /** \param childPos child position
        \return child object (or NULL if wrong position)
    **/
    ccHObject* getChild(unsigned childPos) const;

	//! Finds an entity in this object hierarchy
	/** \param uniqueID child unique ID
		\return child (or NULL if not found)
	**/
	ccHObject* find(int uniqueID);

	//! standard ccHObject container (for children, etc.)
	typedef std::vector<ccHObject*> Container;

    //! Collects the children corresponding to a certain pattern
    /** \param filteredChildren result container
        \param recursive specifies if the search should be recursive
        \param filter pattern for children selection
        \return number of collected children
    **/
    unsigned filterChildren(Container& filteredChildren, bool recursive=false, CC_CLASS_ENUM filter = CC_OBJECT) const;

    //! Removes a specific child
    void removeChild(const ccHObject* anObject);
    //! Removes a specific child given its index
    void removeChild(int pos);
    //! Removes all children
    void removeAllChildren();
    //! Returns child index
    int getChildIndex(const ccHObject* aChild) const;
	//! Swaps two children
	void swapChildren(unsigned firstChildIndex, unsigned secondChildIndex);
    //! Returns index relatively to its parent or -1 if no parent
    int getIndex() const;

	//! Detaches entity from parent
	void detachFromParent();

	//! Transfer a given child to another parent
	void transferChild(unsigned index, ccHObject& newParent);
	//! Transfer all children to another parent
	void transferChildren(ccHObject& newParent, bool forceFatherDependent = false);

    //! Shortcut: returns first child
    ccHObject* getFirstChild() const;
    //! Shortcut: returns last child
    ccHObject* getLastChild() const;

    //! Returns true if the current object is an ancestor of the specified one
    bool isAncestorOf(const ccHObject *anObject) const;

    //Inherited from ccDrawableObject
    virtual ccBBox getBB(bool relative=true, bool withGLfeatures=false, const ccGenericGLDisplay* window=NULL);
    virtual void draw(CC_DRAW_CONTEXT& context);
    virtual void prepareDisplayForRefresh_recursive();
    virtual void setDisplay_recursive(ccGenericGLDisplay* win);
	virtual void setSelected_recursive(bool state);
    virtual void removeFromDisplay_recursive(ccGenericGLDisplay* win);
    virtual void refreshDisplay_recursive();

    //! Applies the active OpenGL transformation to the entity (recursive)
    /** The input ccGLMatrix should be left to 0, unless you want to apply
        a pre-transformation.
        \param trans a ccGLMatrix structure (reference to)
    **/
    void applyGLTransformation_recursive(ccGLMatrix* trans = 0);

    //! Returns the bounding-box center
    /** \return bounding-box center
    **/
    virtual CCVector3 getCenter();

    //! Returns last modification time
    /** \return last modification time
    **/
    qint64 getLastModificationTime() const;

    //! Returns last modification time (recursive)
    /** \return last modification time
    **/
    qint64 getLastModificationTime_recursive() const;

    //! Updates modification time
    void updateModificationTime();

    //! Returns the entity bounding-box only
    /** Children bboxes are ignored.
        \return bounding-box
    **/
    virtual ccBBox getMyOwnBB();

    //! Returns the entity GL display bounding-box
    /** Children bboxes are ignored. The bounding-box
        should take into account entity geometrical data
        and any other 3D displayed elements.
        \return bounding-box
    **/
    virtual ccBBox getDisplayBB();

	//inherited from ccSerializableObject
	virtual bool isSerializable() const;
	virtual bool toFile(QFile& out) const;
	virtual bool fromFile(QFile& in, short dataVersion);

	//! Returns whether object is shareable or not
	/** If object is father dependent and 'shared', it won't
		be deleted but 'released' instead.
	**/
	virtual bool isShareable() const { return false; }

protected:

    //! Sets parent object
    virtual void setParent(ccHObject* anObject);

    //! Draws the entity only (not its children)
    virtual void drawMeOnly(CC_DRAW_CONTEXT& context);

    //! Applies a GL transformation to the entity
    /** this = rotMat*(this-rotCenter)+(rotCenter+trans)
        \param trans a ccGLMatrix structure
    **/
    virtual void applyGLTransformation(const ccGLMatrix& trans);

	//! Save own object data
	/** Called by 'toFile' (recursive scheme)
		To be overloaded (but still called;) by subclass.
	**/
	virtual bool toFile_MeOnly(QFile& out) const;

	//! Loads own object data
	/** Called by 'fromFile' (recursive scheme)
		To be overloaded (but still called;) by subclass.
	**/
	virtual bool fromFile_MeOnly(QFile& out, short dataVersion);

    //! Object's parent
    ccHObject* m_parent;

    //! Object's children
    Container m_children;

    //! Last modification time
    qint64 m_lastModificationTime;
};

#endif
