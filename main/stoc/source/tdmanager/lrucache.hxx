/**************************************************************
 * 
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 * 
 *************************************************************/


#ifndef _LRU_CACHE_HXX_
#define _LRU_CACHE_HXX_

// __CACHE_DIAGNOSE forces cache size to 4 and works only for OUString keys
//  #define __CACHE_DIAGNOSE 1

#include <osl/mutex.hxx>
#include "rtl/ustring.hxx"

#include <hash_map>


/** Implementation of a least recently used (lru) cache.
	<br>
	@author Daniel Boelzle
*/
template< class t_Key, class t_Val, class t_KeyHash, class t_KeyEqual >
class LRU_Cache
{
	struct CacheEntry
	{
		t_Key				aKey;
		t_Val				aVal;
		CacheEntry *		pPred;
		CacheEntry *		pSucc;
	};
	typedef ::std::hash_map< t_Key, CacheEntry *, t_KeyHash, t_KeyEqual > t_Key2Element;
	
	mutable ::osl::Mutex		_aCacheMutex;
	sal_Int32					_nCachedElements;
	t_Key2Element				_aKey2Element;
	
	CacheEntry *				_pBlock;
	mutable CacheEntry *		_pHead;
	mutable CacheEntry *		_pTail;
	inline void toFront( CacheEntry * pEntry ) const;
	
public:
	/** Constructor:
		<br>
		@param nCachedElements number of elements to be cached; default param set to 128
	*/
	inline LRU_Cache( sal_Int32 nCachedElements = 128 );
	/** Destructor: releases all cached elements and keys.
		<br>
	*/
	inline ~LRU_Cache();
	
	/** Retrieves a value from the cache. Returns default constructed value,
		if none was found.
		<br>
		@param rKey a key
		@return value
	*/
	inline t_Val getValue( t_Key const & rKey ) const;
	/** Sets a value to be cached for given key.
		<br>
		@param rKey a key
		@param rValue a value
	*/
	inline void setValue( t_Key const & rKey, t_Val const & rValue );
	/** Tests whether a value is cached for given key.
		<br>
		@param rKey a key
		@return true, if value is cached
	*/
	inline sal_Bool hasValue( t_Key const & rKey ) const;
	/** Clears the cache, thus releasing all cached elements and keys.
		<br>
	*/
	inline void clear();
};
//__________________________________________________________________________________________________
template< class t_Key, class t_Val, class t_KeyHash, class t_KeyEqual >
inline LRU_Cache< t_Key, t_Val, t_KeyHash, t_KeyEqual >::LRU_Cache( sal_Int32 nCachedElements )
#ifdef __CACHE_DIAGNOSE
	: _nCachedElements( 4 )
#else
	: _nCachedElements( nCachedElements )
#endif
	, _pBlock( 0 )
{
	if (_nCachedElements > 0)
	{
		_pBlock = new CacheEntry[_nCachedElements];
		_pHead	= _pBlock;
		_pTail	= _pBlock + _nCachedElements -1;
		for ( sal_Int32 nPos = _nCachedElements; nPos--; )
		{
			_pBlock[nPos].pPred = _pBlock + nPos -1;
			_pBlock[nPos].pSucc = _pBlock + nPos +1;
		}
	}
}
//__________________________________________________________________________________________________
template< class t_Key, class t_Val, class t_KeyHash, class t_KeyEqual >
inline LRU_Cache< t_Key, t_Val, t_KeyHash, t_KeyEqual >::~LRU_Cache()
{
	delete [] _pBlock;
}
//__________________________________________________________________________________________________
template< class t_Key, class t_Val, class t_KeyHash, class t_KeyEqual >
inline void LRU_Cache< t_Key, t_Val, t_KeyHash, t_KeyEqual >::toFront(
    CacheEntry * pEntry ) const
{
	if (pEntry != _pHead)
	{
		// cut out element
		if (pEntry == _pTail)
		{
			_pTail = pEntry->pPred;
		}
		else
		{
			pEntry->pSucc->pPred = pEntry->pPred;
			pEntry->pPred->pSucc = pEntry->pSucc;
		}
		// push to front
		_pHead->pPred = pEntry;
		pEntry->pSucc = _pHead;
		_pHead		  = pEntry;
	}
}
//__________________________________________________________________________________________________
template< class t_Key, class t_Val, class t_KeyHash, class t_KeyEqual >
inline sal_Bool LRU_Cache< t_Key, t_Val, t_KeyHash, t_KeyEqual >::hasValue(
    t_Key const & rKey ) const
{
	::osl::MutexGuard aGuard( _aCacheMutex );
	typename t_Key2Element::const_iterator const iFind( _aKey2Element.find( rKey ) );
	return (iFind != _aKey2Element.end());
}
//__________________________________________________________________________________________________
template< class t_Key, class t_Val, class t_KeyHash, class t_KeyEqual >
inline t_Val LRU_Cache< t_Key, t_Val, t_KeyHash, t_KeyEqual >::getValue(
    t_Key const & rKey ) const
{
	::osl::MutexGuard aGuard( _aCacheMutex );
	const typename t_Key2Element::const_iterator iFind( _aKey2Element.find( rKey ) );
	if (iFind != _aKey2Element.end())
	{
		CacheEntry * pEntry = (*iFind).second;
		toFront( pEntry );
#ifdef __CACHE_DIAGNOSE
		OSL_TRACE( "> retrieved element \"" );
		OSL_TRACE( ::rtl::OUStringToOString( pEntry->aKey, RTL_TEXTENCODING_ASCII_US ).getStr() );
		OSL_TRACE( "\" from cache <\n" );
#endif
		return pEntry->aVal;
	}
	return t_Val();
}
//__________________________________________________________________________________________________
template< class t_Key, class t_Val, class t_KeyHash, class t_KeyEqual >
inline void LRU_Cache< t_Key, t_Val, t_KeyHash, t_KeyEqual >::setValue(
	t_Key const & rKey, t_Val const & rValue )
{
	if (_nCachedElements > 0)
	{
		::osl::MutexGuard aGuard( _aCacheMutex );
		typename t_Key2Element::const_iterator const iFind( _aKey2Element.find( rKey ) );
		
		CacheEntry * pEntry;
		if (iFind == _aKey2Element.end())
		{
			pEntry = _pTail; // erase last element
#ifdef __CACHE_DIAGNOSE
			if (pEntry->aKey.getLength())
			{
				OSL_TRACE( "> kicking element \"" );
				OSL_TRACE( ::rtl::OUStringToOString( pEntry->aKey, RTL_TEXTENCODING_ASCII_US ).getStr() );
				OSL_TRACE( "\" from cache <\n" );
			}
#endif
			_aKey2Element.erase( pEntry->aKey );
			_aKey2Element[ pEntry->aKey = rKey ] = pEntry;
		}
		else
		{
			pEntry = (*iFind).second;
#ifdef __CACHE_DIAGNOSE
			OSL_TRACE( "> replacing element \"" );
			OSL_TRACE( ::rtl::OUStringToOString( pEntry->aKey, RTL_TEXTENCODING_ASCII_US ).getStr() );
			OSL_TRACE( "\" in cache <\n" );
#endif
		}
		pEntry->aVal = rValue;
		toFront( pEntry );
	}
}
//__________________________________________________________________________________________________
template< class t_Key, class t_Val, class t_KeyHash, class t_KeyEqual >
inline void LRU_Cache< t_Key, t_Val, t_KeyHash, t_KeyEqual >::clear()
{
	::osl::MutexGuard aGuard( _aCacheMutex );
	_aKey2Element.clear();
	for ( sal_Int32 nPos = _nCachedElements; nPos--; )
	{
		_pBlock[nPos].aKey = t_Key();
		_pBlock[nPos].aVal = t_Val();
	}
#ifdef __CACHE_DIAGNOSE
	OSL_TRACE( "> cleared cache <\n" );
#endif
}

//==================================================================================================
struct FctHashOUString : public ::std::unary_function< ::rtl::OUString const &, size_t >
{
	size_t operator()( ::rtl::OUString const & rKey ) const
		{ return (size_t)rKey.hashCode(); }
};

/** Template instance for OUString keys, Any values.<br>
*/
typedef LRU_Cache< ::rtl::OUString, ::com::sun::star::uno::Any,
				   FctHashOUString, ::std::equal_to< ::rtl::OUString > >
	LRU_CacheAnyByOUString;


#endif
