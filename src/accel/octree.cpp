/*
   FileName:      octree.cpp

   Created Time:  2016-3-7

   Auther:        Cao Jiayin

   Email:         soraytrace@hotmail.com

   Location:      China, Shanghai

   Description:   SORT is short for Simple Open-source Ray Tracing. Anyone could checkout the source code from
                'sourceforge', https://soraytrace.svn.sourceforge.net/svnroot/soraytrace. And anyone is free to
                modify or publish the source code. It's cross platform. You could compile the source code in 
                linux and windows , g++ or visual studio 2008 is required.
*/

#include "octree.h"
#include "geometry/primitive.h"

IMPLEMENT_CREATOR( OcTree );

// default construction
OcTree::OcTree():m_uMaxTriInLeaf(16),m_uMaxDepthInOcTree(16),m_pRoot(0)
{
}

// destructor
OcTree::~OcTree()
{
	releaseOcTree( m_pRoot );
}

// Get the intersection between the ray and the primitive set
// @param r The ray
// @param intersect The intersection result
// @return 'true' if the ray pirece one of the primitve in the list
bool OcTree::GetIntersect( const Ray& r , Intersection* intersect ) const
{
	float fmax;
	float fmin = Intersect( r , m_BBox , &fmax );
	if( fmin < 0.0f )
		return false;

	if( traverseOcTree( m_pRoot , r , intersect , fmin , fmax ) )
	{
		if( intersect == 0 )
			return true;
		return intersect->primitive != 0;
	}
	return false;
}

// build the acceleration structure
void OcTree::Build()
{
	// handling empty mesh case
	if( m_primitives->size() == 0 )
		return ;

	// generate aabb
	_computeBBox();

	// initialize a triangle container
	NodeTriangleContainer* container = new NodeTriangleContainer();
	vector<Primitive*>::const_iterator it = m_primitives->begin();
	while( it != m_primitives->end() ){
		container->primitives.push_back( *it );
		++it;
	}
	
	// create root node
	m_pRoot = new OcTreeNode();
	m_pRoot->bb = m_BBox;

	// split octree node
	splitNode( m_pRoot , container , 0 );
}

// output log information
void OcTree::OutputLog() const
{
	LOG_HEADER( "Accelerator" );
	LOG<<"Accelerator Type :\tOcTree"<<ENDL;
}

// Release OcTree memory.
// @param node Sub-tree belongs to this node will be released recursively.
void OcTree::releaseOcTree( OcTreeNode* node ){
	// return for empty node
	if( node == 0 )
		return;

	// release all child nodes
	for( int i = 0 ; i < 8 ; ++i )
		releaseOcTree( node->child[i] );

	// delete current node
	delete node;
}

// Split current node into eight if criteria is not met. Otherwise, it will make it a leaf.\n
// This function invokes itself recursively, so the whole sub-tree will be built once it is called.
// @param node Node to be splitted.
// @param container Container holdes all triangle information in this node.
// @param bb Bounding box of this node.
// @param depth Current depth of this node.
void OcTree::splitNode( OcTreeNode* node , NodeTriangleContainer* container , unsigned depth )
{
	// make a leaf if there are not enough points
	if( container->primitives.size() < (int)m_uMaxTriInLeaf || depth > m_uMaxDepthInOcTree )
	{
		// make leaf
		makeLeaf( node , container );
		// no need to process any more
		return;
	}

	// container for child node
	NodeTriangleContainer* childcontainer[8];
	for( int i = 0; i < 8 ; ++i ){
		node->child[i] = new OcTreeNode();
		childcontainer[i] = new NodeTriangleContainer();
	}
	
	// get the center point of this tree node
	int offset = 0;
	Vector length = ( node->bb.m_Max - node->bb.m_Min ) * 0.5f;
	for( int i = 0 ; i < 2; ++i ){
		for( int j = 0 ; j < 2 ; ++j ){
			for( int k = 0 ; k < 2 ; ++k ){
				// setup the lower left bottom point
				node->child[offset]->bb.m_Min = node->bb.m_Min + Vector( (float)k , (float)j , (float)i ) * length;
				node->child[offset]->bb.m_Max = node->child[offset]->bb.m_Min + length;
				++offset;
			}
		}
	}
	
	// distribute triangles
	vector<const Primitive*>::const_iterator it = container->primitives.begin();
	while( it != container->primitives.end() ){
		for( int i = 0 ; i < 8 ; ++i ){
			// check for intersection
			if( (*it)->GetIntersect( node->child[i]->bb ) ){
				childcontainer[i]->primitives.push_back( *it );
			}
		}
		++it;
	}

	// There are cases where triangles lie along diagnonal direction and it will be 
	// extremely difficult, if not impossible, to separate them from different nodes.
	// In these very case, we need to stop immediately to avoid memory exploition.
	int total_child_tri = 0;
	for( int i = 0 ; i < 8 ; ++i )
		total_child_tri += (int)childcontainer[i]->primitives.size();
	if( total_child_tri > (int)(2 * container->primitives.size()) && depth > 8 ){
		// make leaf
		makeLeaf( node , container );

		// splitting plane information is no useful anymore.
		for( int i = 0 ; i < 8 ; ++i )
			delete childcontainer[i];

		// no need to process any more
		return;
	}
	
	// container for this level is no useful any more
	delete container;

	// split children node
	for( int i = 0 ; i < 8 ; ++i )
		splitNode( node->child[i] , childcontainer[i], depth + 1 );
}

// Making the current node as a leaf node.
// An new index buffer will be allocated in this node.
// @param node Node to be made as a leaf node.
// @param container Container holdes all triangle information in this node.
void OcTree::makeLeaf( OcTreeNode* node , NodeTriangleContainer* container )
{
	std::vector<const Primitive*>::const_iterator it = container->primitives.begin();
	while( it != container->primitives.end() ){
		node->primitives.push_back( *it );
		++it;
	}
	
	// delete the container
	delete container;
}

// Traverse OcTree recursively.
// @param node Sub-tree belongs to this node will be visited in a depth first manner.
// @param ray The input ray to be tested.
// @param fmin Current minimum value along the range.
// @param fmax Current maximum value along the range.
// @param ray_max Maximum value in along the range for optimization of shadow ray.
// @result Whether the ray intersects anything in the primitive set
bool OcTree::traverseOcTree( const OcTreeNode* node , const Ray& ray , Intersection* intersect , float fmin , float fmax ) const
{
	static const float	delta = 0.001f;
	bool inter = false;

	// Early rejections
	if( fmin > fmax )
		return false;
	if( intersect && intersect->t < fmin - delta )
		return intersect->t < fmax + delta;

	// Iterate if there is primitives in the node. Since it is not allowed to store primitives in non-leaf node, there is no need to proceed.
	if( node->child[0] == 0 )
	{
		vector<const Primitive*>::const_iterator it = node->primitives.begin();
		while( it != node->primitives.end() ){
			inter |= (*it)->GetIntersect( ray , intersect );
			if( intersect == 0 && inter )
				return true;
			++it;
		}
		return inter && ( intersect->t < ( fmax + delta ) && intersect->t > ( fmin - delta ) );
	}

	Point contact = ray(fmin);
	Point center = ( node->bb.m_Max + node->bb.m_Min ) * 0.5f;
	int node_index = ( contact.x > center.x ) + ( contact.y > center.y ) * 2 + ( contact.z > center.z ) * 4;

	float	_curt = fmin;
	int 	_dir[3];
	float	_delta[3],_next[3];
	for( int i = 0 ; i < 3 ; i++ ){
		_dir[i] = ( ray.m_Dir[i] > 0.0f ) ? 1 : -1;
		if( ray.m_Dir[i] != 0.0f )
			_delta[i] = fabs( node->bb.Delta(i) / ray.m_Dir[i] ) * 0.5f;
		else
			_delta[i] = FLT_MAX;
	}
	for( int i = 0 ; i < 3 ; i++ ){
		float target = node->child[node_index]->bb.m_Min[i] + ((_dir[i]+1)>>1) * node->bb.Delta(i) * 0.5f;
		_next[i] = ( target - ray.m_Ori[i] ) / ray.m_Dir[i];
		if( ray.m_Dir[i] == 0.0f )
			_next[i] = FLT_MAX;
	}

	// traverse the octree
	while( ( intersect && _curt < intersect->t ) || ( intersect == 0 ) )
	{
		// get the axis along which the ray leaves the node fastest.
		unsigned nextAxis = (_next[0] <= _next[1])?0:1;
		nextAxis = (_next[nextAxis] <= _next[2])?nextAxis:2;

		// chech if there is intersection in the current grid
		if( traverseOcTree( node->child[node_index] , ray , intersect , _curt , _next[nextAxis] ) )
			return true;

		// get to the next node based on distance
		node_index += ( 1 << nextAxis ) * _dir[nextAxis];

		// update next
		_curt = _next[nextAxis];
		_next[nextAxis] += _delta[nextAxis];

		// check for early rejection
		if( node_index < 0 || node_index > 7 )
			break;
		if( _curt > fmax + delta || _curt < fmin - delta )
			break;
	}

	return inter;
}