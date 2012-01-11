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


PRJ=..$/..

PRJNAME=stoc
TARGET=test_security
TARGETTYPE=CUI
LIBTARGET=NO
ENABLE_EXCEPTIONS=TRUE
NO_BSYMBOLIC=TRUE

# --- Settings -----------------------------------------------------

.INCLUDE :  settings.mk

# --- Application 1 ------------------------------------------------
APP1TARGET= $(TARGET)
APP1OBJS=   $(OBJ)$/test_security.obj 

APP1STDLIBS= \
		$(CPPULIB) 		\
		$(CPPUHELPERLIB) 	\
		$(SALHELPERLIB) 	\
		$(SALLIB)

.IF "$(GUI)"=="WNT"
APP1STDLIBS+= 
#		$(LIBCIMT) $(LIBCMT)
.ENDIF


# --- Target ------------------------------------------------

.INCLUDE :	target.mk

