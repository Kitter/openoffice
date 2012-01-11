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



PRJ=..$/..$/..

PRJNAME=formula
TARGET=uidlg
IMGLST_SRS=$(SRS)$/$(TARGET).srs
BMP_IN=$(PRJ)$/res

# --- Settings ----------------------------------

.INCLUDE : settings.mk
.INCLUDE :  $(PRJ)$/util$/makefile.pmk

# --- Files -------------------------------------

# ... resource files ............................

SRS1NAME=$(TARGET)
SRC1FILES =	\
		formdlgs.src	\
		parawin.src	

# ... object files ............................

EXCEPTIONSFILES=	\
		$(SLO)$/formula.obj					\
		$(SLO)$/FormulaHelper.obj			\
		$(SLO)$/parawin.obj				    \
		$(SLO)$/funcutl.obj			        \
		$(SLO)$/funcpage.obj			        \
		$(SLO)$/structpg.obj
		
SLOFILES=	\
		$(EXCEPTIONSFILES)


# --- Targets ----------------------------------

.INCLUDE : target.mk

