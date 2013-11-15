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

#include <com/sun/star/uno/Sequence.h>
#include <com/sun/star/accessibility/AccessibleStateType.hpp>
#include <com/sun/star/accessibility/AccessibleRole.hpp>
#include <com/sun/star/accessibility/XAccessibleValue.hpp>
#include <com/sun/star/accessibility/XAccessibleComponent.hpp>
#include <com/sun/star/accessibility/XAccessibleText.hpp>

#include <stdlib.h>
#include <memory.h>
#include <stdio.h>
#include <memory.h>
#include <algorithm>
#include <assert.h>

#include "AccObject.hxx"
#include "AccEventListener.hxx"
#include "UAccCOM_i.c"
#include "AccResource.hxx"

#include "act.hxx"

using namespace std;
using namespace com::sun::star::uno;
using namespace com::sun::star::accessibility;
using namespace com::sun::star::accessibility::AccessibleRole;
using namespace com::sun::star::accessibility::AccessibleStateType;


/**
   * Constructor.
   * @param pXAcc Uno XAccessible interface of control.
   * @param Agent The agent kept in all listeners,it's the sole interface by which
   *              listener communicate with windows manager.
   * @param listener listener that registers in UNO system.
   * @return.
   */
AccObject::AccObject(XAccessible* pAcc,AccObjectManagerAgent* pAgent ,AccEventListener* listener) :
        m_pIMAcc	(NULL),
        m_resID		(NULL),
        m_pParantID	(NULL),
        m_pParentObj(NULL),
        m_accListener (listener),
        m_bShouldDestroy(sal_False),
        m_xAccRef( pAcc )
{
    sal_Bool bRet = ImplInitilizeCreateObj();

    m_xAccContextRef = m_xAccRef->getAccessibleContext();
    m_xAccActionRef = Reference< XAccessibleAction > (m_xAccContextRef,UNO_QUERY);
    m_accRole = m_xAccContextRef -> getAccessibleRole();
    if( m_pIMAcc )
    {
        m_pIMAcc->SetXAccessible((long) m_xAccRef.get());
        m_pIMAcc->Put_XAccAgent((long)pAgent);
        m_pIMAcc->SetDefaultAction((long)m_xAccActionRef.get());
    }
}
/**
   * Destructor.
   * @param
   * @return
   */
AccObject::~AccObject()
{
    m_pIMAcc = NULL;
    m_xAccRef = NULL;
    m_xAccActionRef = NULL;
    m_xAccContextRef = NULL;
}



/**
   * Insert a child element.
   * @param pChild Child element that should be inserted into child list.
   * @param pos Insert postion.
   * @return
   */
void AccObject::InsertChild( AccObject* pChild,short pos )
{

    std::vector<AccObject*>::iterator iter;
    iter = std::find(m_childrenList.begin(),m_childrenList.end(),pChild);
    if(iter!=m_childrenList.end())
        return;
    if(LAST_CHILD==pos)
    {
        m_childrenList.push_back(pChild);
    }
    else
    {
        std::vector<AccObject*>::iterator iter=m_childrenList.begin()+pos;
        m_childrenList.insert(iter,pChild);
    }

    pChild->SetParentObj(this);
}

/**
   * Delete a child element
   * @param pChild Child element that should be inserted into child list.
   * @param pos Insert postion.
   * @return
   */
void AccObject::DeleteChild( AccObject* pChild )
{
    std::vector<AccObject*>::iterator iter;
    iter = std::find(m_childrenList.begin(),m_childrenList.end(),pChild);
    if(iter!=m_childrenList.end())
    {
        m_childrenList.erase(iter);
        if(m_pIMAcc)
            pChild->SetParentObj(NULL);
    }
}

/**
   * In order to windows API WindowFromAccessibleObject,we sometimes to set a pure
   * top window accessible object created by windows system as top ancestor.
   * @param.
   * @return
   */
void AccObject::UpdateValidWindow()
{
    if(m_pIMAcc)
        m_pIMAcc->Put_XAccWindowHandle(m_pParantID);
}

/**
   * Translate all UNO basic information into MSAA com information.
   * @param
   * @return If the method is correctly processed.
   */
sal_Bool AccObject::ImplInitilizeCreateObj()
{
	ActivateActContext();
	HRESULT hr = CoCreateInstance( CLSID_MAccessible, NULL, CLSCTX_ALL ,
                                   IID_IMAccessible,
                                   (void **)&m_pIMAcc);
	DeactivateActContext();

    if ( S_OK != hr )
    {
        return sal_False;
    }

    return sal_True;
}

/**
   * Update name property to com object.
   * @param
   * @return
   */
void  AccObject::UpdateName( )
{
    if (!m_pIMAcc)
    {
        return;
    }

    if( ( TEXT_FRAME == m_accRole   ) && ( m_pParentObj !=NULL )&& ( SCROLL_PANE == m_pParentObj -> m_accRole ) )
        m_pIMAcc->Put_XAccName( m_pParentObj->m_xAccContextRef->getAccessibleName().getStr() );
	//IAccessibility2 Implementation 2009-----
    if ( PARAGRAPH == m_accRole)
    {
    	::rtl::OUString emptyStr = ::rtl::OUString::createFromAscii("");
    	m_pIMAcc->Put_XAccName(emptyStr.getStr());
    }
	//-----IAccessibility2 Implementation 2009
    else
        m_pIMAcc->Put_XAccName(m_xAccContextRef->getAccessibleName().getStr());

    return ;
}
/**
   * Update description property to com object.
   * no content for update description
   * @param
   * @return
   */
void AccObject::UpdateDescription()
{
    if (!m_pIMAcc)
    {
        return;
    }

    m_pIMAcc->Put_XAccDescription(m_xAccContextRef->getAccessibleDescription().getStr());
    return ;
}

/**
   * Update default action property to com object.
   * @param
   * @return
   */
void  AccObject::UpdateAction()
{
    m_xAccActionRef = Reference< XAccessibleAction > (m_xAccContextRef,UNO_QUERY);

    if( m_xAccActionRef.is() && m_pIMAcc )
    {
        if( m_xAccActionRef->getAccessibleActionCount() > 0 )
        {
            UpdateDefaultAction( );
            m_pIMAcc->SetDefaultAction((long)m_xAccActionRef.get());
        }
    }
}

/**
   * Update value property to com object.
   * @param
   * @return
   */
void AccObject::UpdateValue()
{
    if( NULL == m_pIMAcc  || !m_xAccContextRef.is() )
    {
        assert(false);
        return ;
    }

    Reference< XAccessibleValue > pRValue(m_xAccContextRef.get(),UNO_QUERY);
    Any pAny;
    if( pRValue.is() )
    {
        pAny = pRValue->getCurrentValue();
    }

    SetValue( pAny );
}

/**
   * Set special default action description string via UNO role.
   * @param Role UNO role
   * @return
   */
void AccObject::UpdateDefaultAction( )
{
    if(!m_xAccActionRef.is())
        return ;

    switch(m_accRole)
    {
    case PUSH_BUTTON:
    case TOGGLE_BUTTON:
    case RADIO_BUTTON:
    case MENU_ITEM:
    case RADIO_MENU_ITEM:
    case CHECK_MENU_ITEM:
    case LIST_ITEM:
    case CHECK_BOX:
    case TREE_ITEM:
    case BUTTON_DROPDOWN:
        m_pIMAcc->Put_ActionDescription( m_xAccActionRef->getAccessibleActionDescription((sal_Int32)0).getStr() );
        return;
    }
}

/**
   * Set value property via pAny.
   * @param pAny New value.
   * @return
   */
void  AccObject::SetValue( Any pAny )
{
    unsigned short pUNumberString[100];
    memset( pUNumberString, 0 , sizeof( pUNumberString) );

    if( NULL == m_pIMAcc || !m_xAccContextRef.is() )
    {
        assert(false);
        return ;
    }
    Reference< XAccessibleText > pRText(m_xAccContextRef,UNO_QUERY);
    ::rtl::OUString val;
    int index = 0 ;
    switch(m_accRole)
    {
    case SPIN_BOX:
        // 3. date editor's msaa value should be the same as spinbox
    case DATE_EDITOR:
    case TEXT:
    case PARAGRAPH:
    case HEADING:

        if(pRText.get())
        {
            val = pRText->getText();
        }
        m_pIMAcc->Put_XAccValue( val.getStr() );
        break;
    case TREE_ITEM:
	//IAccessibility2 Implementation 2009-----
    //case CHECK_BOX:	//Commented by Li Xing to disable the value for general checkbox
    case COMBO_BOX:
    case TABLE_CELL:
    case NOTE:
    case SCROLL_BAR:
        m_pIMAcc->Put_XAccValue( GetMAccessibleValueFromAny(pAny).getStr() );
        break ;
	// Added by Li Xing, only the checkbox in tree should have the value. 
	case CHECK_BOX:
		if( ( m_pParentObj !=NULL ) && (TREE == m_pParentObj->m_accRole || TREE_ITEM == m_pParentObj->m_accRole ))
	        m_pIMAcc->Put_XAccValue( GetMAccessibleValueFromAny(pAny).getStr() );
	//-----IAccessibility2 Implementation 2009
		break;
    default:
        break;
    }

    return;


}
::rtl::OUString AccObject::GetMAccessibleValueFromAny(Any pAny)
{
    ::rtl::OUString strValue;

    if(NULL == m_pIMAcc)
        return strValue;

    if(pAny.getValueType() == getCppuType( (sal_uInt16 *)0 ) )
    {
        sal_uInt16 val;
        if (pAny >>= val)
        {
            strValue=::rtl::OUString::valueOf((sal_Int32)val);

        }
    }
    else if(pAny.getValueType() == getCppuType( (::rtl::OUString *)0 ) )
    {

        pAny >>= strValue ;

    }
    else if(pAny.getValueType() == getCppuType( (Sequence< ::rtl::OUString > *)0 ) )
    {
        Sequence< ::rtl::OUString > val;
        if (pAny >>= val)
        {

            int count = val.getLength();

            for( int iIndex = 0;iIndex < count;iIndex++ )
            {
                strValue += val[iIndex];
            }

        }
    }
    else if(pAny.getValueType() == getCppuType( (double *)0 ) )
    {
        double val;
        if (pAny >>= val)
        {
            strValue=::rtl::OUString::valueOf(val);
        }
    }
    else if(pAny.getValueType() == getCppuType( (sal_Int32 *)0 ) )
    {
        sal_Int32 val;
        if (pAny >>= val)
        {
            strValue=::rtl::OUString::valueOf(val);
        }
    }
    else if (pAny.getValueType() == getCppuType( (com::sun::star::accessibility::TextSegment *)0 ) )
    {
        com::sun::star::accessibility::TextSegment val;
        if (pAny >>= val)
        {
            ::rtl::OUString realVal(val.SegmentText);
            strValue = realVal;

        }
    }

    return strValue;
}
/**
   * Set name property via pAny.
   * @param pAny New accessible name.
   * @return
   */
void  AccObject::SetName( Any pAny)
{
    if( NULL == m_pIMAcc )
        return ;

    m_pIMAcc->Put_XAccName( GetMAccessibleValueFromAny(pAny).getStr() );

}

/**
   * Set description property via pAny.
   * @param pAny New accessible description.
   * @return
   */
void  AccObject::SetDescription( Any pAny )
{
    if( NULL == m_pIMAcc )
        return ;
    m_pIMAcc->Put_XAccDescription( GetMAccessibleValueFromAny(pAny).getStr() );
}

/**
   * Set role property via pAny
   * @param Role New accessible role.
   * @return
   */
void  AccObject::SetRole( short Role )
{
    if( NULL == m_pIMAcc )
        return ;
    m_pIMAcc->Put_XAccRole( Role );
}

/**
* Get role property via pAny
* @param
* @return accessible role
*/
short AccObject::GetRole() const
{
    return m_accRole;
}

/**
   * Get MSAA state from UNO state
   * @Role xState UNO state.
   * @return
   */
DWORD AccObject::GetMSAAStateFromUNO(short xState)
{
    DWORD IState = UNO_MSAA_UNMAPPING;

    if( !m_xAccContextRef.is() )
    {
        assert(false);
        return IState;
    }
    short Role = m_accRole;

    switch( xState )
    {
    case  BUSY:
        IState = STATE_SYSTEM_BUSY;
        break;
    case  CHECKED:
        if( Role == PUSH_BUTTON || Role == TOGGLE_BUTTON )
        {
            IState = STATE_SYSTEM_PRESSED;
        }
        else
            IState = STATE_SYSTEM_CHECKED;
        break;
    case  DEFUNC:
        IState = STATE_SYSTEM_UNAVAILABLE;
        break;
    case  EXPANDED:
        IState = STATE_SYSTEM_EXPANDED;
        break;
    case  FOCUSABLE:
        IState = STATE_SYSTEM_FOCUSABLE;
        break;
    case  FOCUSED:
        IState = STATE_SYSTEM_FOCUSED;
        break;
    case  INDETERMINATE:
        IState = STATE_SYSTEM_MIXED;
        break;
    case  MULTI_SELECTABLE:
        IState = STATE_SYSTEM_MULTISELECTABLE;
        break;
    case  PRESSED:
        IState = STATE_SYSTEM_PRESSED;
        break;
    case  RESIZABLE:
        IState = STATE_SYSTEM_SIZEABLE;
        break;
    case  SELECTABLE:
        if( m_accRole == MENU || m_accRole == MENU_ITEM)
        {
            IState = UNO_MSAA_UNMAPPING;
        }
        else
        {
            IState = STATE_SYSTEM_SELECTABLE;
        }
        break;
    case  SELECTED:
        if( m_accRole == MENU || m_accRole == MENU_ITEM )
        {
            IState = UNO_MSAA_UNMAPPING;
        }
        else
        {
            IState = STATE_SYSTEM_SELECTED;
        }
        break;
    case  ARMED:
        IState = STATE_SYSTEM_FOCUSED;
        break;
    case  EXPANDABLE:
        {
            sal_Bool isExpanded = sal_True;
            sal_Bool isExpandable = sal_True;
            if( Role == PUSH_BUTTON || Role == TOGGLE_BUTTON  || BUTTON_DROPDOWN == Role )
            {
                IState = STATE_SYSTEM_HASPOPUP;
            }
            else
            {
                GetExpandedState(&isExpandable,&isExpanded);
                if(!isExpanded)
                    IState = STATE_SYSTEM_COLLAPSED;
            }
        }
        break;
	//Remove the SENSITIVE state mapping. There is no corresponding MSAA state. 
    //case  SENSITIVE:
    //    IState = STATE_SYSTEM_PROTECTED;
    case EDITABLE:
        if( m_pIMAcc )
        {
            m_pIMAcc->DecreaseState( STATE_SYSTEM_READONLY );
        }
        break;
    case OFFSCREEN:
        IState = STATE_SYSTEM_OFFSCREEN;
        break;
    case MOVEABLE:
        IState = STATE_SYSTEM_MOVEABLE;
        break;
    case COLLAPSE:
        IState = STATE_SYSTEM_COLLAPSED;
        break;
    case DEFAULT:
        IState = STATE_SYSTEM_DEFAULT;
        break;
    default:
        break;
    }

    return IState;
}

/**
   * Decrease state of com object
   * @param xState The lost state.
   * @return
   */
void  AccObject::DecreaseState( short xState )
{
    if( NULL == m_pIMAcc )
    {
        return;
    }

    if( xState == FOCUSABLE)
    {
        short Role = m_accRole ;
        if(Role == MENU_ITEM
                || Role == RADIO_MENU_ITEM
                || Role == CHECK_MENU_ITEM)
            return;
        else
        {
            if (Role == TOGGLE_BUTTON || Role == PUSH_BUTTON || BUTTON_DROPDOWN == Role)
            {
                if( ( m_pParentObj !=NULL ) && (TOOL_BAR == m_pParentObj->m_accRole ) )
                    return;
            }
        }
    }

    else if( xState == AccessibleStateType::VISIBLE  )
    {
        m_pIMAcc->IncreaseState( STATE_SYSTEM_INVISIBLE );
    }
    else if( xState == AccessibleStateType::SHOWING )
    {
        m_pIMAcc->IncreaseState( STATE_SYSTEM_OFFSCREEN );
    }

    DWORD msState = GetMSAAStateFromUNO(xState);
    if(msState!=UNO_MSAA_UNMAPPING)
        m_pIMAcc->DecreaseState(msState);
}

/**
   * Increase state of com object
   * @param xState The new state.
   * @return
   */
void AccObject::IncreaseState( short xState )
{
    if( NULL == m_pIMAcc )
    {
        assert(false);
        return;
    }


    if( xState == AccessibleStateType::VISIBLE  )
    {
        m_pIMAcc->DecreaseState( STATE_SYSTEM_INVISIBLE );
    }
    else if( xState == AccessibleStateType::SHOWING )
    {
        m_pIMAcc->DecreaseState( STATE_SYSTEM_OFFSCREEN );
    }


    DWORD msState = GetMSAAStateFromUNO(xState);
    if(msState!=UNO_MSAA_UNMAPPING)
        m_pIMAcc->IncreaseState( msState );
}

/**
   * Get next child element
   * @param
   * @return AccObject Object interface.
   */
AccObject* AccObject::NextChild()
{
    IAccChildList::iterator pInd = m_childrenList.begin();
    if( pInd != m_childrenList.end() )
        return 	*pInd;
    return NULL;
}
/**
   * update action desciption desc
   * @param
   * @return 
   */
void AccObject::UpdateActionDesc()
{
    if (!m_pIMAcc)
    {
        return;
    }

    ::rtl::OUString	pXString = m_xAccContextRef->getAccessibleDescription();
    m_pIMAcc->Put_XAccDescription(pXString.getStr());
    long Role = m_accRole;

    if(  Role == PUSH_BUTTON || Role == RADIO_BUTTON || Role == MENU_ITEM ||
            Role == LIST_ITEM || Role == CHECK_BOX || Role == TREE_ITEM ||
            Role == CHECK_MENU_ITEM || Role == RADIO_MENU_ITEM )
    {
        UpdateDefaultAction(  );
    }
    else
    {

        if( m_xAccActionRef.is() )
        {
            if( m_xAccActionRef->getAccessibleActionCount() > 0 )
            {
				if (!(Role == SPIN_BOX || Role == COMBO_BOX || Role == DATE_EDITOR ||
					  Role == EDIT_BAR || Role == PASSWORD_TEXT || Role == TEXT))
				{
					pXString = m_xAccActionRef->getAccessibleActionDescription( 0 );
					//Solution:If string length is more than zero,action will will be set.
					if( pXString.getLength() > 0)
						m_pIMAcc->Put_ActionDescription( pXString.getStr() );
				}                
            }
        }
    }

}
/**
   * update role information from uno to com
   * @param
   * @return 
   */
void AccObject::UpdateRole()
{
    if (!m_pIMAcc)
    {
        return;
    }

    XAccessibleContext* pContext  = m_xAccContextRef.get();
    m_pIMAcc->Put_XAccRole( ROLE_SYSTEM_WINDOW  );
    short iRoleIndex = pContext->getAccessibleRole();
    if (( 0 <= iRoleIndex) && ( iRoleIndex <= (sizeof(ROLE_TABLE)/(sizeof(short)*2))))
    {
        short iIA2Role = ROLE_TABLE[iRoleIndex][1] ;
        m_pIMAcc->Put_XAccRole( iIA2Role  );
    }

}
/**
   * update state information from uno to com
   * @param
   * @return 
   */
void AccObject::UpdateState()
{
    if (!m_pIMAcc)
    {
        return;
    }

    XAccessibleContext* pContext  = m_xAccContextRef.get();
    Reference< XAccessibleStateSet > pRState = pContext->getAccessibleStateSet();
    if( !pRState.is() )
    {
        assert(false);
        return ;
    }

    m_pIMAcc->SetState(0L);

    if ( m_accRole == POPUP_MENU )
    {
        return;
    }

    Sequence<short> pStates = pRState->getStates();
    int count = pStates.getLength();

    sal_Bool isEnable = sal_False;
    sal_Bool isShowing = sal_False;
    sal_Bool isEditable = sal_False;
    sal_Bool isVisible = sal_False;
    sal_Bool isFocusable = sal_False;

    for( int iIndex = 0;iIndex < count;iIndex++ )
    {
        if( pStates[iIndex] == ENABLED )
            isEnable = sal_True;
        else if( pStates[iIndex] == SHOWING)
            isShowing = sal_True;
        else if( pStates[iIndex] == VISIBLE)
            isVisible = sal_True;
        else if( pStates[iIndex] == EDITABLE )
            isEditable = sal_True;
        else if (pStates[iIndex] == FOCUSABLE)
            isFocusable = sal_True;
        IncreaseState( pStates[iIndex]);
    }
    sal_Bool bIsMenuItem = m_accRole == MENU_ITEM || m_accRole == RADIO_MENU_ITEM || m_accRole == CHECK_MENU_ITEM;

    if(bIsMenuItem)
    {
        if(!(isShowing && isVisible) )
        {
            m_pIMAcc->IncreaseState( STATE_SYSTEM_INVISIBLE );
            m_pIMAcc->DecreaseState( STATE_SYSTEM_FOCUSABLE );
        }
    }
    else
    {
        if(!(isShowing || isVisible) )
            m_pIMAcc->IncreaseState( STATE_SYSTEM_INVISIBLE );
    }

    short Role = m_accRole;

    if( m_pIMAcc )
    {
        switch(m_accRole)
        {
        case LABEL:
            m_pIMAcc->IncreaseState( STATE_SYSTEM_READONLY );
            break;
        case TEXT:
            // 2. editable combobox -> readonly ------ bridge
        case EMBEDDED_OBJECT:
        case END_NOTE:
        case FOOTER:
        case FOOTNOTE:
        case GRAPHIC:
        case HEADER:
        case HEADING:

            //Image Map
        case PARAGRAPH:
        case PASSWORD_TEXT:
        case SHAPE:
        case SPIN_BOX:
        case TABLE:
        case TABLE_CELL:
        case TEXT_FRAME:
        case DATE_EDITOR:
        case DOCUMENT:
        case COLUMN_HEADER:
            {
                if(!isEditable)
                    m_pIMAcc->IncreaseState( STATE_SYSTEM_READONLY );
            }
            break;
        default:
            break;
        }
    }

    if( isEnable )
    {

        if(!(Role == FILLER || Role == END_NOTE || Role == FOOTER || Role == FOOTNOTE || Role == GROUP_BOX || Role == RULER
                || Role == HEADER || Role == ICON || Role == INTERNAL_FRAME || Role == LABEL || Role == LAYERED_PANE
                || Role == SCROLL_BAR || Role == SCROLL_PANE || Role == SPLIT_PANE || Role == STATUS_BAR || Role == TOOL_TIP))
        {
            if( SEPARATOR == Role  )
            {
                if( ( m_pParentObj != NULL ) && ( MENU == m_pParentObj->m_accRole  || POPUP_MENU == m_pParentObj->m_accRole ))
                    IncreaseState( FOCUSABLE );
            }

            else if (TABLE_CELL == Role || TABLE == Role || PANEL == Role || OPTION_PANE == Role ||
                     COLUMN_HEADER == Role)
            {
                if (isFocusable)
                    IncreaseState( FOCUSABLE );
            }
            else
            {
                if(bIsMenuItem)
                {
                    if ( isShowing && isVisible)
                    {
                        IncreaseState( FOCUSABLE );
                    }
                }
                else
                {
                    IncreaseState( FOCUSABLE );
                }
            }
        }
    }
    else
    {
        m_pIMAcc->IncreaseState( STATE_SYSTEM_UNAVAILABLE );
        sal_Bool isDecreaseFocusable = sal_False;
        if( !((Role == MENU_ITEM) ||
                (Role == RADIO_MENU_ITEM) ||
                (Role == CHECK_MENU_ITEM)) )
        {
            if  ( Role == TOGGLE_BUTTON || Role == PUSH_BUTTON || BUTTON_DROPDOWN == Role)
            {
                if(( m_pParentObj != NULL )&& (TOOL_BAR ==  m_pParentObj->m_accRole ) )
                    IncreaseState( FOCUSABLE );
                else
                    DecreaseState( FOCUSABLE );
            }
            else
                DecreaseState( FOCUSABLE );
        }
        else if( isShowing || isVisible )
        {
            IncreaseState( FOCUSABLE );
        }
    }

    if( m_pIMAcc )
    {
        switch(m_accRole)
        {
        case POPUP_MENU:
        case MENU:
            if( pContext->getAccessibleChildCount() > 0 )
                m_pIMAcc->IncreaseState( STATE_SYSTEM_HASPOPUP );
            break;
        case PASSWORD_TEXT:
            m_pIMAcc->IncreaseState( STATE_SYSTEM_PROTECTED );
            break;
        default:
            break;
        }
    }

}
/**
   * update location information from uno to com
   * @param
   * @return 
   */
void AccObject::UpdateLocation()
{
    if (!m_pIMAcc)
    {
        return;
    }
    XAccessibleContext* pContext  = m_xAccContextRef.get();

    Reference< XAccessibleComponent > pRComponent(pContext,UNO_QUERY);
    if( pRComponent.is() )
    {
        ::com::sun::star::awt::Point pCPoint = pRComponent->getLocationOnScreen();
        ::com::sun::star::awt::Size pCSize = pRComponent->getSize();
        Location tempLocation;
        tempLocation.m_dLeft = pCPoint.X;
        tempLocation.m_dTop =  pCPoint.Y;
        tempLocation.m_dWidth = pCSize.Width;
        tempLocation.m_dHeight = pCSize.Height;
        m_pIMAcc->Put_XAccLocation( tempLocation );
    }

}


/**
   * Public method to mapping information between MSAA and UNO.
   * @param
   * @return If the method is correctly processed.
   */
sal_Bool AccObject:: UpdateAccessibleInfoFromUnoToMSAA ( )
{
    if( NULL == m_pIMAcc || !m_xAccContextRef.is()  )
    {
        assert(false);
        return sal_False;
    }

    UpdateName();

    UpdateValue();

    UpdateActionDesc();

    UpdateRole();

    UpdateLocation();

    UpdateState();

    return sal_True;
}

/*
   * Add a child selected element.
   * @param pAccObj Child object pointer.
   * @return
   */
void AccObject::AddSelect( long index, AccObject* accObj)
{
    m_selectionList.insert(IAccSelectionList::value_type(index,accObj));
}

IAccSelectionList& AccObject::GetSelection()
{
    return m_selectionList;
}


/**
   * Set self to focus object in parant child list
   * @param
   * @return
   */
void AccObject::setFocus()
{
    if(m_pIMAcc)
    {
        IncreaseState(FOCUSED);
        m_pIMAcc->Put_XAccFocus(CHILDID_SELF);

        UpdateRole();
    }
}

/**
   * Unset self from focus object in parant child list.
   * @param
   * @return
   */
void AccObject::unsetFocus()
{
    if(m_pIMAcc)
    {
        DecreaseState( FOCUSED );
        m_pIMAcc->Put_XAccFocus(UACC_NO_FOCUS);
    }
}

void AccObject::GetExpandedState( sal_Bool* isExpandable, sal_Bool* isExpanded)
{
    *isExpanded = sal_False;
    *isExpandable = sal_False;

    if( !m_xAccContextRef.is() )
    {
        return;
    }
    Reference< XAccessibleStateSet > pRState = m_xAccContextRef->getAccessibleStateSet();
    if( !pRState.is() )
    {
        return;
    }

    Sequence<short> pStates = pRState->getStates();
    int count = pStates.getLength();

    for( int iIndex = 0;iIndex < count;iIndex++ )
    {
        if( EXPANDED == pStates[iIndex]  )
        {
            *isExpanded = sal_True;
        }
        else if( EXPANDABLE == pStates[iIndex]  )
        {
            *isExpandable = sal_True;
        }
    }
}

void AccObject::NotifyDestroy(sal_Bool ifDelete)
{
    m_bShouldDestroy=ifDelete;
    if(m_pIMAcc)
        m_pIMAcc->NotifyDestroy(m_bShouldDestroy);
}

void AccObject::SetParentObj(AccObject* pParentAccObj)
{
    m_pParentObj = pParentAccObj;

    if(m_pIMAcc)
    {
        if(m_pParentObj)
        {
            m_pIMAcc->Put_XAccParent(m_pParentObj->GetIMAccessible());
        }
        else
        {
            m_pIMAcc->Put_XAccParent(NULL);
        }
    }
}
//ResID means ChildID in MSAA
void AccObject::SetResID(long id)
{
    m_resID = id;
    if(m_pIMAcc)
        m_pIMAcc->Put_XAccChildID(m_resID);
}
//return COM interface in acc object
IMAccessible*  AccObject::GetIMAccessible()
{
    return m_pIMAcc;
}

Reference < XAccessible > AccObject::GetXAccessible()
{
    return m_xAccRef;
}

void AccObject::SetParentHWND(HWND hWnd)
{
    m_pParantID = hWnd;
}
void AccObject::SetListener( AccEventListener* Listener )
{
    m_accListener = Listener;
}
AccEventListener* AccObject::getListener()
{
    return m_accListener;
}

long AccObject::GetResID()
{
    return m_resID;
}

HWND AccObject::GetParentHWND()
{
    return m_pParantID;
}

AccObject* AccObject::GetParentObj()
{
    return m_pParentObj;
}
sal_Bool  AccObject::ifShouldDestroy()
{
    return m_bShouldDestroy;
}
