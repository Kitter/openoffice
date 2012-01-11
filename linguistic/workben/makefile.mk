#**************************************************************
#  
#  Licensed to the Apache Software Foundation (ASF) under one
#  or more contributor license agreements.  See the NOTICE file
#  distributed with this work for additional information
#  regarding copyright ownership.  The ASF licenses this file
#  to you under the Apache License, Version 2.0 (the
#  "License"); you may not use this file except in compliance
#  with the License.  You may obtain a copy of the License at
#  
#    http://www.apache.org/licenses/LICENSE-2.0
#  
#  Unless required by applicable law or agreed to in writing,
#  software distributed under the License is distributed on an
#  "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
#  KIND, either express or implied.  See the License for the
#  specific language governing permissions and limitations
#  under the License.
#  
#**************************************************************



PRJ = ..

PRJNAME = linguistic
TARGET	= lex
ENABLE_EXCEPTIONS=TRUE
USE_DEFFILE=TRUE

#----- Settings ---------------------------------------------------------

.INCLUDE : settings.mk

# --- Files --------------------------------------------------------

UNOTYPES=\
	com.sun.star.linguistic2.DictionaryEvent\
	com.sun.star.linguistic2.DictionaryEventFlags\
	com.sun.star.linguistic2.DictionaryListEvent\
	com.sun.star.linguistic2.DictionaryListEventFlags\
	com.sun.star.linguistic2.DictionaryType\
	com.sun.star.linguistic2.LinguServiceEventFlags\
	com.sun.star.linguistic2.SpellFailure\
	com.sun.star.linguistic2.XDictionary\
	com.sun.star.linguistic2.XDictionary1\
	com.sun.star.linguistic2.XDictionaryEntry\
	com.sun.star.linguistic2.XDictionaryEventListener\
	com.sun.star.linguistic2.XDictionaryList\
	com.sun.star.linguistic2.XDictionaryListEventListener\
	com.sun.star.linguistic2.XHyphenatedWord\
	com.sun.star.linguistic2.XHyphenator\
	com.sun.star.linguistic2.XLinguServiceEventBroadcaster\
	com.sun.star.linguistic2.XLinguServiceEventListener\
	com.sun.star.linguistic2.XLinguServiceManager\
	com.sun.star.linguistic2.XMeaning\
	com.sun.star.linguistic2.XPossibleHyphens\
	com.sun.star.linguistic2.XSearchableDictionaryList\
	com.sun.star.linguistic2.XSpellAlternatives\
	com.sun.star.linguistic2.XSpellChecker\
	com.sun.star.linguistic2.XSupportedLocales\
	com.sun.star.linguistic2.XThesaurus


SLOFILES=	\
		$(SLO)$/sprophelp.obj\
		$(SLO)$/sreg.obj\
		$(SLO)$/sspellimp.obj


SHL1TARGET= $(TARGET)$(DLLPOSTFIX)

SHL1STDLIBS= \
		$(CPPULIB) 	 \
		$(CPPUHELPERLIB) 	 \
		$(VOSLIB)		\
		$(TOOLSLIB)		\
		$(SVTOOLLIB)	\
		$(SVLLIB)		\
		$(VCLLIB)		\
		$(SFXLIB)		\
		$(SALLIB)		\
		$(UCBHELPERLIB)	\
		$(UNOTOOLSLIB)	\
		$(LNGLIB)

# build DLL
SHL1LIBS=		$(SLB)$/$(TARGET).lib
SHL1IMPLIB=		i$(TARGET)
SHL1DEPN=		$(SHL1LIBS)
SHL1DEF=		$(MISC)$/$(SHL1TARGET).def
.IF "$(OS)"!="MACOSX"
SHL1VERSIONMAP=$(SOLARENV)/src/component.map
.ENDIF

# build DEF file
DEF1NAME	 =$(SHL1TARGET)
DEF1EXPORTFILE=	exports.dxp

# --- Targets ------------------------------------------------------

.INCLUDE : target.mk

