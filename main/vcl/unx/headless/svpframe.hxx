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


 
#ifndef _SVP_SVPFRAME_HXX

#include <vcl/sysdata.hxx>

#include <salframe.hxx>
#include "svpelement.hxx"

#include <list>

class SvpSalInstance;
class SvpSalGraphics;

class SvpSalFrame : public SalFrame, public SvpElement
{
    SvpSalInstance*                     m_pInstance;
    SvpSalFrame*	                m_pParent;       // pointer to parent frame
    std::list< SvpSalFrame* >           m_aChildren;     // List of child frames
    sal_uLong                         m_nStyle;
    bool                                m_bVisible;
    long                                m_nMinWidth;
    long                                m_nMinHeight;
    long                                m_nMaxWidth;
    long                                m_nMaxHeight;

    SystemEnvData                       m_aSystemChildData;
    
    basebmp::BitmapDeviceSharedPtr      m_aFrame;
    std::list< SvpSalGraphics* >        m_aGraphics;
    
    static SvpSalFrame*       s_pFocusFrame;
public:
	SvpSalFrame( SvpSalInstance* pInstance,
                 SalFrame* pParent,
                 sal_uLong nSalFrameStyle,
                 SystemParentData* pSystemParent = NULL );
	virtual ~SvpSalFrame();
    
    void GetFocus();
    void LoseFocus();
    void PostPaint() const;
    
    // SvpElement
    virtual const basebmp::BitmapDeviceSharedPtr& getDevice() const { return m_aFrame; }

    // SalFrame
    virtual SalGraphics*		GetGraphics();
    virtual void				ReleaseGraphics( SalGraphics* pGraphics );

    virtual sal_Bool				PostEvent( void* pData );

    virtual void				SetTitle( const XubString& rTitle );
    virtual void				SetIcon( sal_uInt16 nIcon );
    virtual void				SetMenu( SalMenu* pMenu );
    virtual void                                DrawMenuBar();

    virtual void                SetExtendedFrameStyle( SalExtStyle nExtStyle );
    virtual void				Show( sal_Bool bVisible, sal_Bool bNoActivate = sal_False );
    virtual void				Enable( sal_Bool bEnable );
    virtual void                SetMinClientSize( long nWidth, long nHeight );
    virtual void                SetMaxClientSize( long nWidth, long nHeight );
    virtual void				SetPosSize( long nX, long nY, long nWidth, long nHeight, sal_uInt16 nFlags );
    virtual void				GetClientSize( long& rWidth, long& rHeight );
    virtual void				GetWorkArea( Rectangle& rRect );
    virtual SalFrame*			GetParent() const;
    virtual void				SetWindowState( const SalFrameState* pState );
    virtual sal_Bool				GetWindowState( SalFrameState* pState );
    virtual void				ShowFullScreen( sal_Bool bFullScreen, sal_Int32 nDisplay );
    virtual void				StartPresentation( sal_Bool bStart );
    virtual void				SetAlwaysOnTop( sal_Bool bOnTop );
    virtual void				ToTop( sal_uInt16 nFlags );
    virtual void				SetPointer( PointerStyle ePointerStyle );
    virtual void				CaptureMouse( sal_Bool bMouse );
    virtual void				SetPointerPos( long nX, long nY );
    using SalFrame::Flush;
    virtual void				Flush();
    virtual void				Sync();
    virtual void				SetInputContext( SalInputContext* pContext );
    virtual void				EndExtTextInput( sal_uInt16 nFlags );
    virtual String				GetKeyName( sal_uInt16 nKeyCode );
    virtual String				GetSymbolKeyName( const XubString& rFontName, sal_uInt16 nKeyCode );
    virtual sal_Bool                MapUnicodeToKeyCode( sal_Unicode aUnicode, LanguageType aLangType, KeyCode& rKeyCode );
    virtual LanguageType		GetInputLanguage();
    virtual SalBitmap*			SnapShot();
    virtual void				UpdateSettings( AllSettings& rSettings );
    virtual void				Beep( SoundType eSoundType );
    virtual const SystemEnvData*	GetSystemData() const;
    virtual SalPointerState     GetPointerState();
    virtual void				SetParent( SalFrame* pNewParent );
    virtual bool				SetPluginParent( SystemParentData* pNewParent );
    virtual void                SetBackgroundBitmap( SalBitmap* pBitmap );
	virtual void				ResetClipRegion();
	virtual void				BeginSetClipRegion( sal_uLong nRects );
	virtual void				UnionClipRegion( long nX, long nY, long nWidth, long nHeight );
	virtual void				EndSetClipRegion();

    /*TODO: functional implementation */
    virtual void                SetScreenNumber( unsigned int nScreen ) { (void)nScreen; }
};
#endif // _SVP_SVPFRAME_HXX
